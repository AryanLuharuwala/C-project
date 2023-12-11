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
 
