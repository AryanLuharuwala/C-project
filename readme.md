we use read stdin_filno to take input but the terminal takes input and send to the file only when the user presses enter which is fine for the program but for text editor we need every keypress to be reflected and that needs a lot of flags to be removed

tcgetattr() to read the current attributes into a struct 

modifying the struct by hand

passing the modified struct to tcsetattr() to write the new terminal attributes back out.
try turning of ECHO
terminal attributes can be read intoa termios struct using tcgetattr(), after modifying them using tcsetattr() the TCSAFLUSH argument specifies when to apply the change: in this case it waits for all pending output to be written to the terminal
the c_lflag field is for local flags like c_oflag c_iflag output flags an input flags
ECHO is a bit flag as 00000000000000000000000000001000 we then bitwise-not ~ to match with 
1111111111111111111111111110111 we use bitwise AND to make the 4th bit 0 


disable raw mode at exit

tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);

we restore the original terminal attributes for the user
we save a copy of termios struct in its original state and use setattr to apply it to terminal


escape sequence is made of 3 bytes \x1b followed by 3 bytes of [ J 
write() and STDOUT_FILENO come from <unistd.h>

the 4 in our write() call means we are writing 4 bytes out to the terminal, the first byte is \x1b which is the escape character or 27 in decimal, the other three bytes are [2J

Escape sequence always start with an escape character 27 followed by [ character. Escape sequences instruct the terminal to do various text formatting tasks, coloring text moving the cursor around, clearing parts of the screen.

we use J command Erase in Display 
ESC [ Ps J 
default 0 - erases some or all of characters in the display 
1 - erase form the start of the screen to active position inclusive
2- erase all of the display- all lines are erased, changed to single-width and cursor does not move.


