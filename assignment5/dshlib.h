#ifndef __DSHLIB_H__
#define __DSHLIB_H__

// Constants for command structure sizes
#define EXE_MAX 64
#define ARG_MAX 256
#define CMD_MAX 8
#define CMD_ARGV_MAX (CMD_MAX + 1)
// Longest command that can be read from the shell
#define SH_CMD_MAX EXE_MAX + ARG_MAX

// Command structure
typedef struct command {
    char exe[EXE_MAX];
    char args[ARG_MAX];
} command_t;

// Command buffer structure
typedef struct cmd_buff {
    int argc;
    char *argv[CMD_ARGV_MAX];
    char *_cmd_buffer;
} cmd_buff_t;

// Command list structure for pipelines
typedef struct command_list {
    int num;
    cmd_buff_t commands[CMD_MAX];
} command_list_t;

// Special character defines
#define SPACE_CHAR ' '
#define PIPE_CHAR '|'
#define PIPE_STRING "|"

#define SH_PROMPT "dsh3> "
#define EXIT_CMD "exit"
#define EXIT_SC 99

// Standard Return Codes
#define OK 0
#define WARN_NO_CMDS -1
#define ERR_TOO_MANY_COMMANDS -2
#define ERR_CMD_OR_ARGS_TOO_BIG -3
#define ERR_CMD_ARGS_BAD -4  // For extra credit
#define ERR_MEMORY -5
#define ERR_EXEC_CMD -6
#define OK_EXIT -7

// Built-in command enumeration
typedef enum {
    BI_CMD_EXIT,
    BI_CMD_DRAGON,
    BI_CMD_CD,
    BI_NOT_BI,
    BI_EXECUTED,
    BI_RC,
} Built_In_Cmds;

// Built-in command function prototypes
Built_In_Cmds match_command(const char *input);
Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd);
void exec_cd(cmd_buff_t *cmd);

// Main execution functions
void exec_local_cmd_loop();
int exec_cmd(cmd_buff_t *cmd);
void exec_external_cmd(cmd_buff_t *cmd);
void execute_pipeline(command_list_t *clist);

// Command buffer management
int alloc_cmd_buff(cmd_buff_t *cmd_buff);
int free_cmd_buff(cmd_buff_t *cmd_buff);
int clear_cmd_buff(cmd_buff_t *cmd_buff);
int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff);
int close_cmd_buff(cmd_buff_t *cmd_buff);
int build_cmd_list(char *cmd_line, command_list_t *clist);
int free_cmd_list(command_list_t *cmd_lst);

// Output constants
#define CMD_OK_HEADER "PARSED COMMAND LINE - TOTAL COMMANDS %d\n"
#define CMD_WARN_NO_CMD "warning: no commands provided\n"
#define CMD_ERR_PIPE_LIMIT "error: piping limited to %d commands\n"

// Starter code placeholders
#define M_NOT_IMPL "The requested operation is not implemented yet!\n"
#define EXIT_NOT_IMPL 3
#define NOT_IMPLEMENTED_YET 0

#endif // __DSHLIB_H__
