#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dshlib.h"

// Store ASCII Art in a character array
const char *dragon_data[] = {
    "                                                                         @%%%%",                       
    "                                                                     %%%%%%",                         
    "                                                                    %%%%%%",                          
    "                                                                 % %%%%%%%           @",              
    "                                                                %%%%%%%%%%        %%%%%%%",           
    "                                       %%%%%%%  %%%%@         %%%%%%%%%%%%@    %%%%%%  @%%%%",        
    "                                  %%%%%%%%%%%%%%%%%%%%%%      %%%%%%%%%%%%%%%%%%%%%%%%%%%%",          
    "                                %%%%%%%%%%%%%%%%%%%%%%%%%%   %%%%%%%%%%%% %%%%%%%%%%%%%%%",           
    "                               %%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%     %%%",            
    "                             %%%%%%%%%%%%%%%%%%%%%%%%%%%%@ @%%%%%%%%%%%%%%%%%%        %%",            
    "                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%",                
    "                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",              
    "                            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%@%%%%%%@",              
    "      %%%%%%%%@           %%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%%%%%%%%%%%%%      %%",                
    "    %%%%%%%%%%%%%         %%@%%%%%%%%%%%%           %%%%%%%%%%% %%%%%%%%%%%%      @%",                
    "  %%%%%%%%%%   %%%        %%%%%%%%%%%%%%            %%%%%%%%%%%%%%%%%%%%%%%%",                        
    " %%%%%%%%%       %         %%%%%%%%%%%%%             %%%%%%%%%%%%@%%%%%%%%%%%",                       
    "%%%%%%%%%@                % %%%%%%%%%%%%%            @%%%%%%%%%%%%%%%%%%%%%%%%%",                     
    "%%%%%%%%@                 %%@%%%%%%%%%%%%            @%%%%%%%%%%%%%%%%%%%%%%%%%%%%",                  
    "%%%%%%%@                   %%%%%%%%%%%%%%%           %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%",              
    "%%%%%%%%%%                  %%%%%%%%%%%%%%%          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      %%%%",  
    "%%%%%%%%%@                   @%%%%%%%%%%%%%%         %%%%%%%%%%%%@ %%%% %%%%%%%%%%%%%%%%%   %%%%%%%%",
    "%%%%%%%%%%                  %%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%      %%%%%%%%%%%%%%%%%% %%%%%%%%%",
    "%%%%%%%%%@%%@                %%%%%%%%%%%%%%%%@       %%%%%%%%%%%%%%     %%%%%%%%%%%%%%%%%%%%%%%%  %%",
    " %%%%%%%%%%                  % %%%%%%%%%%%%%%@        %%%%%%%%%%%%%%   %%%%%%%%%%%%%%%%%%%%%%%%%% %%",
    "  %%%%%%%%%%%%  @           %%%%%%%%%%%%%%%%%%        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  %%%",
    "   %%%%%%%%%%%%% %%  %  %@ %%%%%%%%%%%%%%%%%%          %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%%",
    "    %%%%%%%%%%%%%%%%%% %%%%%%%%%%%%%%%%%%%%%%           @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    %%%%%%%",
    "     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%              %%%%%%%%%%%%%%%%%%%%%%%%%%%%        %%%",
    "      @%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                  %%%%%%%%%%%%%%%%%%%%%%%%%",
    "        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                      %%%%%%%%%%%%%%%%%%%  %%%%%%%",
    "           %%%%%%%%%%%%%%%%%%%%%%%%%%                           %%%%%%%%%%%%%%%  @%%%%%%%%%",
    "              %%%%%%%%%%%%%%%%%%%%           @%@%                  @%%%%%%%%%%%%%%%%%%   %%%",
    "                  %%%%%%%%%%%%%%%        %%%%%%%%%%                    %%%%%%%%%%%%%%%    %",
    "                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%                      %%%%%%%%%%%%%%",
    "                %%%%%%%%%%%%%%%%%%%%%%%%%%  %%%% %%%                      %%%%%%%%%%  %%%@",
    "                     %%%%%%%%%%%%%%%%%%% %%%%%% %%                          %%%%%%%%%%%%%@",
    "                                                                                 %%%%%%%@",
    NULL
};

// Function to print the ASCII dragon
void print_dragon() {
    for (int i = 0; dragon_data[i] != NULL; i++) {
        printf("%s\n", dragon_data[i]);
    }
}


int build_cmd_list(char *cmd_line, command_list_t *clist) {
    if (cmd_line == NULL || clist == NULL || strlen(cmd_line) == 0) {
        printf(CMD_WARN_NO_CMD);
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

