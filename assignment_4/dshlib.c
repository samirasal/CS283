#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "dshlib.h"  

/* Helper function: Trim leading and trailing whitespace */
void trim_whitespace(char *str) {
    if (str == NULL)
        return;
    
    // Trim leading spaces
    char *start = str;
    while (*start && isspace((unsigned char)*start))
        start++;
    
    if (start != str)
        memmove(str, start, strlen(start) + 1);
    
    // Trim trailing spaces
    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
}

/* Tokenize a single command string into a cmd_buff_t structure.
   It handles tokens enclosed in double quotes as a single argument.
   Returns OK if tokens were extracted, or WARN_NO_CMDS / ERR_TOO_MANY_COMMANDS on error. */
int tokenize_command(char *command_str, cmd_buff_t *cmd) {
    cmd->argc = 0;
    memset(cmd->argv, 0, sizeof(cmd->argv));
    
    char *p = command_str;
    while (*p) {
       
        while (*p && isspace((unsigned char)*p))
            p++;
        if (*p == '\0')
            break;
        
        char *token = NULL;
        if (*p == '"') {

            p++;
            token = p;
            while (*p && *p != '"')
                p++;
            if (*p == '"') {
                *p = '\0';
                p++;
            }
        } else {
            token = p;
            while (*p && !isspace((unsigned char)*p))
                p++;
            if (*p) {
                *p = '\0';
                p++;
            }
        }
        if (cmd->argc < CMD_ARGV_MAX - 1) {
            cmd->argv[cmd->argc++] = token;
        } else {
            return ERR_TOO_MANY_COMMANDS;
        }
    }
    cmd->argv[cmd->argc] = NULL;
    return (cmd->argc > 0) ? OK : WARN_NO_CMDS;
}

/* Execute a single (non-piped) command by forking and executing it.
   Uses fork/execvp to execute external commands.
   In the child process, execvp() is used to run the command;
   the parent process waits for the child to finish. */
void execute_single(cmd_buff_t *cmd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork error");
        return;
    }
    if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
        perror("execution error");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        // For extra credit, one might store and handle the return code here.
    }
}

/* Main command loop.
   Prompts the user, reads input, and then:
     - Trims whitespace and checks for empty input.
     - Processes built-in commands ("exit" and "cd").
     - Parses the input into a cmd_buff_t.
     - Executes external commands using the fork/exec pattern.
   As declared in the header, this function returns void.
*/
int exec_local_cmd_loop() {
    char *line = NULL;
    size_t bufsize = 0;
    
    while (1) {
        printf("dsh2> ");
        if (getline(&line, &bufsize, stdin) == -1) {
            printf("\n");
            break;
        }
        
        // Remove the trailing newline and trim input
        line[strcspn(line, "\n")] = '\0';
        trim_whitespace(line);
        if (strlen(line) == 0) {
            fprintf(stderr, "Warning: no command entered\n");
            continue;
        }
        
        // Built-in: exit command
        if (strcmp(line, "exit") == 0) {
            printf("exiting...\n");
            break;
        }
        
        // Built-in: cd command
        if (strncmp(line, "cd", 2) == 0) {
            cmd_buff_t cd_cmd;
            if (tokenize_command(line, &cd_cmd) == OK && cd_cmd.argc >= 2) {
                if (chdir(cd_cmd.argv[1]) != 0) {
                    perror("cd error");
                }
            }
            continue;
        }
        
        // This function processes as a single command (external command)
        cmd_buff_t single_cmd;
        if (tokenize_command(line, &single_cmd) != OK) {
            fprintf(stderr, "Error: invalid command\n");
            continue;
        }
        execute_single(&single_cmd);
    }
    
    free(line);
    return OK;
}
