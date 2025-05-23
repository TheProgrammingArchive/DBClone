#ifndef COMMANDS_H
#define COMMANDS_H

typedef enum{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED
} MetaCommandResult;

typedef enum{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

#endif