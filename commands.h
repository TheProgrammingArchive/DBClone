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

typedef enum{
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_SYNTAX_ERROR
} PrepareResult;

typedef enum{
    EXECUTE_SUCCESS,
    EXECUTE_FAILURE
} ExecuteResult;

#endif