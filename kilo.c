#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void enableRawMode(){
	struct termios raw;

	tcgetattr(STDIN_FILENO, &raw);

	raw.c_lflag &= ~(ECHO);

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

}
//the CHO feature causes each key to be printed to the terminal, useful in canonical mode, gets in way when we want to render userinterface in raw mode.
//this program just doesnt print waht we are typing (writing a password in terminal) 
int main(){

	enableRawMode();
	char c;
	while(read(STDIN_FILENO, &c, 1)==1 && c!='q');
 	return 0;
}
