#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
    while (*str == SPACE_CHAR)
        str++;

    // Trim trailing spaces
    char *end = str + strlen(str) - 1;
    while (end > str && *end == SPACE_CHAR)
        *end-- = '\0';
}




 int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    if (cmd_line == NULL || strlen(cmd_line) == 0)
    {
        return WARN_NO_CMDS; 
    }

    memset(clist, 0, sizeof(command_list_t)); 

    char *token = strtok(cmd_line, PIPE_STRING); 
    int cmd_count = 0;

    while (token != NULL)
    {
        if (cmd_count >= CMD_MAX)
        {
            return ERR_TOO_MANY_COMMANDS; 
        }
        trim_spaces(token);
        
        // while (*token == SPACE_CHAR) token++;
        // char *end = token + strlen(token) - 1;
        // while (end > token && *end == SPACE_CHAR) *end-- = '\0';

      
        char *cmd_name = strtok(token, " ");
        if (cmd_name == NULL)
            return WARN_NO_CMDS; 

        strncpy(clist->commands[cmd_count].exe, cmd_name, EXE_MAX - 1);
        clist->commands[cmd_count].exe[EXE_MAX - 1] = '\0';
        
        char *args = strtok(NULL, ""); 
        if (args)
        {
            strncpy(clist->commands[cmd_count].args, args, ARG_MAX - 1);
            clist->commands[cmd_count].args[ARG_MAX - 1] = '\0';
        }
        
        cmd_count++;
        token = strtok(NULL, PIPE_STRING); 
    }

    clist->num = cmd_count;
    return OK; 
}