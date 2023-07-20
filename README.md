# Cshell
To compile, navigate to the directory containing cshell.c and the Makefile and execute the make command. Run the compiled shell with ./cshell for interactive mode or ./cshell <filename> for script mode.

----------------------------
# Key Features
cshell recognizes and supports environment variables and a history of executed commands.
Commands in the form of <command> <arg0> <arg1> … <argN>, where <command> is a name of a built-in command, are supported.
Non-built-in commands are executed in the child process with output displayed on the terminal.
Two modes: interactive mode (activated without command line arguments) and script mode (activated with command line arguments pointing to a script file).
The shell also supports inner environment variables.
Built-in commands supported include exit, print, theme, and log. <br>


# Built-In Commands
exit: terminates the shell with a message “Bye!”
print: takes arguments <arg0> <arg1> … <argN> and just prints them.
log: prints the history of executed commands with time and return code.
theme: changes the color of shell's output. Only red, green, and blue are supported. Any other color will display a message “unsupported theme”.
Getting Started
Please check the manual pages for fork(), execvp(), wait(), dup2(), open(), read(), fgets(), write(), exit(), malloc(), realloc(), strtok(), strdup() system calls to get familiar with their usage.
