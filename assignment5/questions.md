1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_answer here_:the child processes would become "zombie processes" (defunct processes) after they complete. These zombie processes would remain in the process table until the parent process (the shell) terminates, consuming system resources

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_answer here_: If a pipe end is left open, the process reading from the pipe might block indefinitely, waiting for input that will never arrive because the writing end is still open in another process.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_answer here_: The cd command changes the current working directory of the shell process itself. If cd were implemented as an external command (using execvp()), it would change the working directory of the child process but not the parent shell process. Challenges:The shell's working directory would not change, making the cd command ineffective.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_answer here_:Instead of using a fixed-size array for pipes and commands, dynamically allocate memory for the pipes and command structures using malloc() or realloc().
Trade-offs:
Dynamic memory allocation is slower than using a fixed-size array, but it allows for more flexibility.
Managing dynamic memory adds complexity to the code, as you need to handle allocation, reallocation, and deallocation carefully to avoid memory leaks or crashes.