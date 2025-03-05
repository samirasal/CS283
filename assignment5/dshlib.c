#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>  // Required for chdir()
#include <errno.h>  
#include <sys/types.h>
#include <sys/wait.h>


#include "dshlib.h"




/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */

// Helper function to trim leading and trailing spaces
void trim_spaces(char *str)
{
    if (str == NULL || *str == '\0')
        return;

    // Trim leading spaces
    char *start = str;
    while (*start == SPACE_CHAR)
        start++;
    
    // Move trimmed string to the beginning
    if (start != str)
        memmove(str, start, strlen(start) + 1);
    
        // Trim trailing spaces
    char *end = str + strlen(str) - 1;
    while (end > str && *end == SPACE_CHAR)
        *end-- = '\0';
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (cmd->argc == 0) {
        return BI_NOT_BI;  // No command entered
    }

    if (strcmp(cmd->argv[0], "exit") == 0) {
        return BI_CMD_EXIT;  // Indicate that we should exit
    }

    if (strcmp(cmd->argv[0], "cd") == 0) {
        exec_cd(cmd);  // Execute the `cd` command
        return BI_CMD_CD;  // Indicate that a built-in command was executed
    }

    return BI_NOT_BI;  // Not a built-in command
}


void exec_cd(cmd_buff_t *cmd) {
    if (cmd->argc < 2) {
        return;  // Do nothing if no directory is provided
    }
    
    if (chdir(cmd->argv[1]) != 0) {
        perror("cd failed");
    }
}



void parse_input(char *input, cmd_buff_t *cmd) {
    cmd->argc = 0;
    memset(cmd->argv, 0, sizeof(cmd->argv));

    char *ptr = input;
    char *arg_start = NULL;
    int in_quotes = 0;

    while (*ptr) {
        if (*ptr == '"') {
            in_quotes = !in_quotes;  // Toggle quote state
            if (!in_quotes) {
                *ptr = '\0';  // Close quoted argument
            }
        } else if (isspace((unsigned char)*ptr) && !in_quotes) {
            if (arg_start) {
                *ptr = '\0';  // End the argument
                cmd->argv[cmd->argc++] = arg_start;
                arg_start = NULL;
            }
        } else {
            if (!arg_start) {
                arg_start = (*ptr == '"') ? ptr + 1 : ptr;  // Skip leading quote
            }
        }
        ptr++;
    }

    if (arg_start) {
        cmd->argv[cmd->argc++] = arg_start;
    }

    cmd->argv[cmd->argc] = NULL;  // Null-terminate the argument list
}


void exec_local_cmd_loop() {
    char *line = NULL;
    size_t len = 0;
    command_list_t cmd_list;

    while (1) {
        printf("dsh3> ");
        if (getline(&line, &len, stdin) == -1) {
            break;  // Exit loop on EOF
        }

        trim_spaces(line);  // Clean input

        // Build the command list (parses pipes)
        if (build_cmd_list(line, &cmd_list) != OK) {
            fprintf(stderr, "Error: Invalid command\n");
            continue;
        }

        if (cmd_list.num == 1) {  // Single command (no pipes)
            cmd_buff_t *cmd = &cmd_list.commands[0];

            Built_In_Cmds result = exec_built_in_cmd(cmd);
            if (result == BI_CMD_EXIT) {
                break;  // Exit shell
            } else if (result == BI_NOT_BI) {
                exec_external_cmd(cmd);  // Execute external command
            }
        } else {  // Multiple piped commands
            execute_pipeline(&cmd_list);
        } 
        printf("dsh3> ");  // Print shell prompt after command execution
    }
    
    printf("cmdloopreturned0\n"); 
}



void exec_external_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {
        // Ensure PATH is set before calling execvp
        setenv("PATH", "/usr/bin:/bin", 1);

        // Child process executes the command
        execvp(cmd->argv[0], cmd->argv);

        // If execvp fails, print error and exit
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else {
        // Parent process waits for the child to complete
        int status;
        waitpid(pid, &status, 0);
    }
}

void execute_pipeline(command_list_t *clist) {
    int num_cmds = clist->num;
    int pipes[CMD_MAX - 1][2];  // Pipes between commands
    pid_t pids[CMD_MAX];

    // Create pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }
    }

    // Loop through commands and fork
    for (int i = 0; i < num_cmds; i++) {
        pids[i] = fork();

        if (pids[i] == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) {  // Child process
            // Redirect input if not the first command
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            // Redirect output if not the last command
            if (i < num_cmds - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close all pipes in the child
            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute command
            cmd_buff_t *cmd = &clist->commands[i];
            printf("Executing command %d: %s\n", i, cmd->argv[0]);  // Debugging
            execvp(cmd->argv[0], cmd->argv);
            perror("execvp failed");  // Only runs if execvp fails
            exit(EXIT_FAILURE);
        }
    }

    // Close all pipes in the parent
    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all child processes
    for (int i = 0; i < num_cmds; i++) {
        waitpid(pids[i], NULL, 0);
    }
}




int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    if (cmd_line == NULL || strlen(cmd_line) == 0)
    {
        return WARN_NO_CMDS;
    }

    memset(clist, 0, sizeof(command_list_t));

    char *cmd_copy = strdup(cmd_line);
    if (cmd_copy == NULL)
    {
        return ERR_MEMORY;
    }

    char *token = strtok(cmd_copy, PIPE_STRING);
    int cmd_count = 0;

    while (token != NULL)
    {
        if (cmd_count >= CMD_MAX)
        {
            free(cmd_copy);
            return ERR_TOO_MANY_COMMANDS;
        }

        trim_spaces(token);

        if (strlen(token) == 0)
        {
            free(cmd_copy);
            return WARN_NO_CMDS;
        }

        cmd_buff_t *cmd = &clist->commands[cmd_count]; // Use cmd_buff_t
        cmd->_cmd_buffer = strdup(token);  // Store the full command string

        if (cmd->_cmd_buffer == NULL)
        {
            free(cmd_copy);
            return ERR_MEMORY;
        }

        // Parse arguments into cmd_buff_t
        cmd->argc = 0;
        char *arg = strtok(cmd->_cmd_buffer, " ");
        while (arg != NULL && cmd->argc < CMD_ARGV_MAX - 1)
        {
            cmd->argv[cmd->argc++] = arg;
            arg = strtok(NULL, " ");
        }
        cmd->argv[cmd->argc] = NULL;  // Null-terminate

        cmd_count++;
        token = strtok(NULL, PIPE_STRING);
    }

    clist->num = cmd_count;
    free(cmd_copy);
    return OK;
}
