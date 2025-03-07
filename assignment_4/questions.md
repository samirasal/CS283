1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**: `fork()` creates a child process before calling `execvp()`. This keeps the shell running while the child process runs the new program. If we used `execvp()` directly, it would replace the shell itself.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**: If `fork()` fails, it means there are not enough system resources. My code checks if `fork()` returns `-1`, prints an error message, and returns an error code.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**: `execvp()` looks for the command in directories listed in the `PATH` environment variable. If it finds the command, it runs it. If not, it returns an error.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**: `wait()` makes sure the parent process waits for the child process to finish. Without `wait()`, the child process could become a zombie, wasting system resources.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**: `WEXITSTATUS(status)` gets the exit code of the child process. This helps check if the command ran successfully or had an error.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**: It treats quoted arguments as a single unit, keeping spaces inside quotes. This is important because commands like `echo "hello world"` should keep "hello world" as one argument.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    >**Answer**: I improved the parser to handle quoted arguments correctly. The biggest challenge was making sure spaces inside quotes were not removed while still trimming extra spaces outside.

8. For this question, you need to do some research on Linux signals.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    >**Answer**: Signals are a simple way for processes to communicate asynchronously. Unlike message queues or pipes, signals don’t require shared memory or data exchange. They just notify a process of an event.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:
    - **SIGKILL (9):** Instantly stops a process. It cannot be ignored.
    - **SIGTERM (15):** Asks a process to stop. The process can handle it and clean up before exiting.
    - **SIGINT (2):** Happens when you press `Ctrl+C`. It tells the process to stop but can be ignored.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**: `SIGSTOP` pauses a process. It **cannot** be ignored or handled. It is useful for debugging (e.g., `kill -STOP <pid>`)