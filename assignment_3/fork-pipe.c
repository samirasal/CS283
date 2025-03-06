#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024

void execute_command(const char *cmd) {
    char buffer[BUFFER_SIZE];
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        perror("popen failed");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    pclose(fp);
}

int main() {
    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {  // Child process
        close(pipefd[0]);  // Close unused read end
        dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipe
        close(pipefd[1]);  // Close write end after duplicating

        // Execute a command
        execlp("ls", "ls", NULL);
        perror("execlp failed");
        exit(EXIT_FAILURE);
    } else {  // Parent process
        close(pipefd[1]);  // Close unused write end

        // Read from pipe
        char buffer[BUFFER_SIZE];
        while (read(pipefd[0], buffer, sizeof(buffer)) > 0) {
            printf("%s", buffer);
        }
        close(pipefd[0]);  // Close read end
        wait(NULL);  // Wait for child process to finish
    }

    return 0;
}
