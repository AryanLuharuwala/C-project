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
	raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
	raw.c_oflag &= ~(OPOST);
	raw.c_cflag &= ~(CS8);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

}
//BRKINT causes a SIGINT signal to be sent like Ctrl C
////INPCK enables partity cehcking
////ISTRIP casues 8th bit to be stripped
////CS8 is a bit mask with multiple bits, which we set using bitwise or operator, sets the character size CS to 8 bits per byte
//IXON-Ctrl-S,Q Software flow control XON,XOFF
//ISIG-SIGINT, SIGTSTP Ctrl-C,Z
//ICANON-Canonical mode
//ECHO-print in terminal
int main(){

	enableRawMode();
	
	while(1){
	char c;
	read(STDIN_FILENO, &c, 1);
	  if (iscntrl(c)){
	    printf("%d\r\n",c);
	  } else{
	    printf("%d ( '%c')\r\n", c, c);
	  }
  	if (c=='q') break;	  
	}
 	return 0;
}
