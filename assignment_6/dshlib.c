#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "dshlib.h" 

#define INPUT_BUFFER 1024


void trim_whitespace(char *str) {
    if (str == NULL)
        return;
    
    
    char *start = str;
    while (*start && isspace((unsigned char)*start))
        start++;
    
    if (start != str)
        memmove(str, start, strlen(start) + 1);
    
  
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

/* Build a list of commands by splitting the input on pipe characters.
   Each segment is trimmed and tokenized.
   Returns OK on success. */
int build_command_list(char *input, command_list_t *cmd_list) {
    if (input == NULL || strlen(input) == 0)
        return WARN_NO_CMDS;
    
    memset(cmd_list, 0, sizeof(command_list_t));
    int count = 0;
    
    // Use strtok to break input by the pipe delimiter
    char *segment = strtok(input, PIPE_STRING);
    while (segment != NULL && count < CMD_MAX) {
        trim_whitespace(segment);
        if (tokenize_command(segment, &cmd_list->commands[count]) != OK) {
            return WARN_NO_CMDS;
        }
        count++;
        segment = strtok(NULL, PIPE_STRING);
    }
    
    cmd_list->num = count;
    return OK;
}

/* Execute commands connected via pipes.
   For each command, a new fork is created.
   The function returns OK if all children are spawned and waited upon successfully. */
int execute_pipeline(command_list_t *cmd_list) {
    int num_cmds = cmd_list->num;
    int prev_read_fd = -1;  
    pid_t pids[CMD_MAX];
    
    for (int i = 0; i < num_cmds; i++) {
        int pipefd[2] = { -1, -1 };
        if (i < num_cmds - 1) {
            if (pipe(pipefd) < 0) {
                perror("pipe error");
                return ERR_EXEC_CMD;
            }
        }
        
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork error");
            return ERR_EXEC_CMD;
        }
        
        if (pids[i] == 0) {  
            if (prev_read_fd != -1) {
                dup2(prev_read_fd, STDIN_FILENO);
                close(prev_read_fd);
            }
            if (i < num_cmds - 1) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            }
            execvp(cmd_list->commands[i].argv[0], cmd_list->commands[i].argv);
            printf("\n");
            perror("execution error");
            exit(EXIT_FAILURE);
        } else {  
            if (prev_read_fd != -1)
                close(prev_read_fd);
            if (i < num_cmds - 1) {
                close(pipefd[1]);
                prev_read_fd = pipefd[0];
            }
        }
    }
    
    
    for (int i = 0; i < num_cmds; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }
    
    return OK;
}

/* Execute a single (non-piped) command by forking and executing it.
   This function does not return a status code. */
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
    }
}

/* Main command loop.
   Prompts the user, reads input, and then:
     - Trims whitespace and checks for empty input.
     - Processes built-in commands ("exit" and "cd").
     - Determines whether the command contains pipes.
     - Executes the commands accordingly.
   Returns 0 upon exit. */
int exec_local_cmd_loop() {
    char *line = NULL;
    size_t bufsize = 0;
    
    fflush(stdout);
    printf("local mode\n");
 
    
    while (1) {
        
        printf("%s", SH_PROMPT);
         

        if (getline(&line, &bufsize, stdin) == -1) {
            printf("\n");
            break;
        }
        
       
        line[strcspn(line, "\n")] = '\0';
        trim_whitespace(line);
        if (strlen(line) == 0) {
            fprintf(stderr, "Warning: no command entered\n");
            continue;
        }
        
        // Built-in: exit command
        if (strcmp(line, "exit") == 0) {
           // printf("exiting...\n");
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
        
        // If the input contains a pipe character, process as a pipeline
        if (strchr(line, PIPE_CHAR) != NULL) {
            char *line_dup = strdup(line);
            if (!line_dup) {
                perror("memory allocation error");
                continue;
            }
            command_list_t cmd_list;
            if (build_command_list(line_dup, &cmd_list) != OK) {
                fprintf(stderr, "Error: could not parse piped command\n");
                free(line_dup);
                continue;
            }
            if (execute_pipeline(&cmd_list) != OK) {
                fprintf(stderr, "Pipeline execution failed\n");
            }
            free(line_dup);
        } else {
            // Process as a single command
            cmd_buff_t single_cmd;
            if (tokenize_command(line, &single_cmd) != OK) {
                fprintf(stderr, "Error: invalid command\n");
                continue;
            }
            execute_single(&single_cmd);
            
           
            printf("%s", SH_PROMPT);  
            
        fflush(stdout);
        }
    }
    
    free(line);

    return 0;
}
