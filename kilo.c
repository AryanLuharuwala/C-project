#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>

struct termios orig_termios;

struct editorConfig{
	int screenrows;
	int screencols;
	struct termios orig_termios;

};

struct editorConfig E;

#define CTRL_KEY(k)((k)& 0x1f)
//CTRL_KEY macro bitwishe-ANDs a character with 00011111 in binary
void die(const char *s){

	write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
	perror(s);
	exit(1);
}
void disableRawMode(){
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios)==-1)
		die("tcsetattr");
}

void enableRawMode(){
	if(tcgetattr(STDIN_FILENO, &E.orig_termios)==-1) die("tcsgetattr");
        atexit(disableRawMode); //executes automatically when the program exits <stdlib.h>

	struct termios raw = E.orig_termios;
	raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag &= ~(CS8);
	raw.c_cc[VMIN] = 1;
	raw.c_cc[VTIME] = 1;
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw)==-1) die("tcsetattr");

}
//VMIN sets min number of bytes needed before read() can return
//VTIME value sets the maximum amount of time to wait before read() returns
//BRKINT causes a SIGINT signal to be sent like Ctrl C
////INPCK enables partity cehcking
////ISTRIP casues 8th bit to be stripped
////CS8 is a bit mask with multiple bits, which we set using bitwise or operator, sets the character size CS to 8 bits per byte
//IXON-Ctrl-S,Q Software flow control XON,XOFF
//ISIG-SIGINT, SIGTSTP Ctrl-C,Z
//ICANON-Canonical mode
//ECHO-print in terminal


//[H places the cursor at the row number and column number at whcih to position the cursor, so 80*24 size terminal and want to cursor in the centre [12;40H. 
//default for both is 1 first row first column, start at 1 not 0
char editorReadKey(){
	int nread;
	char c;
	while((nread=read(STDIN_FILENO, &c, 1))!=1){
		if (nread== -1 && errno !=EAGAIN) die("read");
	}
	return c;
}
		
void editorDrawRows(){
	int y;
	for (y=0; y<E.screenrows; y++){
		write(STDOUT_FILENO, "~", 1);
		if (y<E.screenrows -1){
			write(STDOUT_FILENO, "\r\n", 2);
		}
	}
}
 
int getCursorPosition(int *rows, int *cols){
	char buf[32];
	unsigned int i =0;

	if (write(STDOUT_FILENO, "\x1b[6n", 4) !=4) return -1;

	while ( i<sizeof(buf) -1){
		if (read(STDIN_FILENO, &buf[i], 1)!=1)break;
		if(buf[i] =='R')break;
		i++;
	}
	buf[i]='\0';
	
	if (buf[0] != '\x1b' || buf[1] != '[') return -1;
	if (sscanf(&buf[2], "%d;%d", rows, cols) !=2) return -1;
	return 0;
}

			
int getWindowSize(int *rows, int *cols){
	struct winsize ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
		if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12)!= 12) return -1;
		return getCursorPosition(rows,cols);
	} else {
		 *cols = ws.ws_col;
		 *rows = ws.ws_row;
		 return 0;
	}
}

void editorRefreshScreen(){
	write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);

	editorDrawRows();

	write(STDOUT_FILENO, "\x1b[H", 3);
}

void editorProcessKeypress(){
	char c = editorReadKey();

	switch(c){
		case CTRL_KEY('q'):
			write(STDOUT_FILENO, "\x1b[2J", 4);
        		write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
			break;
	}
}

void initEditor(){
	if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

int main(){

	enableRawMode();
	initEditor();	

	while(1){
	editorRefreshScreen();
	editorProcessKeypress();	  
	}
 	return 0;
}
//[2J is escape sequence
//errno and EAGAIN come from <errno.h>
//tcsetattr(), tcgetattr() and read() all return -1 on failure, and set errno value to indicate the error
//in cygwin when read() times out it returns -1 with an errno of EAGAIN instead of 0, 
