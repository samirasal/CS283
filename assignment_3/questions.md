1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  fgets() is a good choice for this application because it safely reads a line of input from the user, including spaces, and allows you to specify the maximum number of characters to read

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Using malloc() helps manage memory more efficiently in cases where the buffer size is unknown or variable. That said, for this specific assignment, a fixed-size array is sufficient because the maximum input size is defined by SH_CMD_MAX


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trailing spaces could interfere with parsing, especially when splitting commands by pipes (|). If spaces are not trimmed, the shell might fail to execute commands correctly.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  Output Redirection (>)
Example: ls > output.txt
This redirects the output of the ls command to a file called output.txt instead of printing it to the terminal.
Challenge: Implementing this requires opening the file, redirecting STDOUT to the file, and ensuring proper error handling if the file cannot be opened.

Input Redirection (<)
Example: sort < input.txt
This reads input from input.txt instead of waiting for user input.
Challenge: The shell must open the file, redirect STDIN to read from the file, and handle cases where the file does not exist or cannot be read.

Appending Output (>>)
Example: echo "new line" >> output.txt
This appends the output of the echo command to the end of output.txt instead of overwriting it.
Challenge: The shell must open the file in append mode and ensure that the file is not truncated.


- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**: 
     Redirection is used to send the output of a command to a file or read input from a file. It changes the source or destination of input/output streams (STDIN, STDOUT, STDERR).
    Example: ls > output.txt (redirects STDOUT to a file).

    Piping is used to send the output of one command as input to another command. It connects the STDOUT of one command to the STDIN of another.
    Example: ls | grep .txt (sends the output of ls to grep).

The key difference is that redirection involves files, while piping involves connecting commands directly.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:
    Error Handling: STDERR is specifically for error messages, while STDOUT is for regular output. Separating them allows users to distinguish between normal output and errors.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Separating STDOUT and STDERR: By default, STDOUT and STDERR should remain separate so that users can distinguish between regular output and error messages.
    or
    Providing an Option to Merge: We could implement a feature to merge STDOUT and STDERR using the 2>&1 syntax. For example, command > output.txt 2>&1 would redirect both STDOUT and STDERR to the same file.