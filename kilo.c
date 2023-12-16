#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
struct termios orig_termios;

#define CTRL_KEY(k)((k)& 0x1f)
//CTRL_KEY macro bitwishe-ANDs a character with 00011111 in binary
void die(const char *s){
	perror(s);
	exit(1);
}
void disableRawMode(){
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios)==-1)
		die("tcsetattr");
}

void enableRawMode(){
	if(tcgetattr(STDIN_FILENO, &orig_termios)==-1) die("tcsgetattr");
        atexit(disableRawMode); //executes automatically when the program exits <stdlib.h>

	struct termios raw = orig_termios;
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


char editorReadKey(){
	int nread;
	char c;
	while((nread=read(STDIN_FILENO, &c, 1))!=1){
		if (nread== -1 && errno !=EAGAIN) die("read");
	}
	return c;
}

void editorProcessKeypress(){
	char c = editorReadKey();

	switch(c){
		case CTRL_KEY('q'):
			exit(0);
			break;
	}
}

void editorRefreshScreen(){
	write(STDOUT_FILENO, "\x1b[2J",4);
}

int main(){

	enableRawMode();
	
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
