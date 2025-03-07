#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>  // Required for chdir()
#include <errno.h>  
#include <sys/types.h>
#include <sys/wait.h>

#include "dshlib.h"  // Include the header file for necessary definitions
#include <cstdlib>

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by splitting
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
    // Check for multiple commands and execute them using pipes
    char *line = NULL;
    size_t len = 0;
    cmd_buff_t cmd;

    while (1) {
        printf("dsh> ");
        if (getline(&line, &len, stdin) == -1) {
            break;  // Exit loop on EOF
        }

        trim_spaces(line);  // Clean input
        parse_input(line, &cmd);

        if (cmd.argc > 0) {
            Built_In_Cmds result = exec_built_in_cmd(&cmd);

            if (result == BI_CMD_EXIT) {
                break;  // Exit the shell
            } else if (result == BI_NOT_BI) {
                exec_piped_cmds(&cmd);  // Handle piped commands
            }
        }
    }
    free(line);
}

void exec_piped_cmds(cmd_buff_t *cmd) {
    int pipefd[2];
    pid_t pid;
    int i;

    for (i = 0; i < cmd->argc; i++) {
        // Create a pipe for each command except the last one
        if (i < cmd->argc - 1) {
            pipe(pipefd);
        }

        pid = fork();
        if (pid < 0) {
            perror("fork failed");
            return;
        }

        if (pid == 0) {  // Child process
            if (i > 0) {  // If not the first command, get input from the previous pipe
                dup2(pipefd[0], STDIN_FILENO);
            }
            if (i < cmd->argc - 1) {  // If not the last command, output to the next pipe
                dup2(pipefd[1], STDOUT_FILENO);
            }

            // Close pipe file descriptors
            close(pipefd[0]);
            close(pipefd[1]);

            // Execute the command
            execvp(cmd->argv[i], cmd->argv);
            perror("execvp failed");
            exit(EXIT_FAILURE);
        } else {  // Parent process
            // Close the write end of the pipe in the parent
            if (i < cmd->argc - 1) {
                close(pipefd[1]);
            }
            // Close the read end of the pipe in the parent
            if (i > 0) {
                close(pipefd[0]);
            }

            // Wait for the child process to complete
            waitpid(pid, NULL, 0);
        }
    }
}

int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    if (cmd_line == NULL || strlen(cmd_line) == 0)
    {
        return WARN_NO_CMDS;
    }

    // Check for "dragon" command
    if (strcmp(cmd_line, "dragon") == 0) {
        print_dragon();
        return OK;
    }

    // Initialize command list
    clist->num = 0;
    memset(clist->commands, 0, sizeof(clist->commands));

    char *token;
    char *rest = cmd_line;
    int cmd_index = 0;

    // Parse commands separated by '|'
    while ((token = strtok_r(rest, PIPE_STRING, &rest))) {
        if (cmd_index >= CMD_MAX) {
            printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
            return ERR_TOO_MANY_COMMANDS;
        }

        // Trim leading and trailing spaces
        while (*token == SPACE_CHAR) {
            token++;
        }

        if (strlen(token) >= EXE_MAX + ARG_MAX) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }


        strncpy(clist->commands[cmd_count]._cmd_buffer, cmd_name, EXE_MAX);
        clist->commands[cmd_count]._cmd_buffer[EXE_MAX - 1] = '\0';

        char *args = strtok(NULL, ""); 
        if (args)
        {
            trim_spaces(args); 
        strncpy(clist->commands[cmd_count].argv[1], args, ARG_MAX);
        clist->commands[cmd_count].argv[1][ARG_MAX - 1] = '\0';


        // Extract executable and arguments
        char *space_pos = strchr(token, SPACE_CHAR);
        if (space_pos != NULL) {
            *space_pos = '\0';  // Split exe and args
            strncpy(clist->commands[cmd_index].exe, token, EXE_MAX - 1);
            strncpy(clist->commands[cmd_index].args, space_pos + 1, ARG_MAX - 1);
        } else {
            strncpy(clist->commands[cmd_index].exe, token, EXE_MAX - 1);
            clist->commands[cmd_index].args[0] = '\0';  // No arguments

        }

        clist->num++;
        cmd_index++;
    }

    printf(CMD_OK_HEADER, clist->num);
    for (int i = 0; i < clist->num; i++) {
        if (strlen(clist->commands[i].args) > 0) {
            printf("<%d> %s [%s]\n", i + 1, clist->commands[i].exe, clist->commands[i].args);
        } else {
            printf("<%d> %s\n", i + 1, clist->commands[i].exe);
        }
    }

    return OK;
}
}