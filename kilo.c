#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

struct termios orig_termios;

void disableRawMode(){
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode(){
	tcgetattr(STDIN_FILENO, &orig_termios);
        atexit(disableRawMode); //executes automatically when the program exits <stdlib.h>

	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ECHO | ICANON | ISIG);
	raw.c_iflag &= ~(IXON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

}
//IXON-Ctrl-S,Q Software flow control XON,XOFF
//ISIG-SIGINT, SIGTSTP Ctrl-C,Z
//ICANON-Canonical mode
//ECHO-print in terminal
int main(){

	enableRawMode();

	char c;
	while(read(STDIN_FILENO, &c, 1)==1 && c!='q'){
	  if (iscntrl(c)){
	    printf("%d\n",c);
	  } else{
	    printf("%d ( '%c')\n", c, c);
	  }	
	}
 	return 0;
}
