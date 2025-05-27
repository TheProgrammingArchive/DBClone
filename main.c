#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include "database.h"
#define MAX_INPUT_SZ 128

typedef struct{
    char* buffer;
    size_t buffer_size;
} InputBuffer;

typedef struct{
    StatementType type;
    Row row_to_insert;
} Statement;

InputBuffer* new_buffer(){
    InputBuffer* buff = (InputBuffer*)malloc(sizeof(InputBuffer));

    if (!buff)
        return NULL;

    buff->buffer = NULL;
    buff->buffer_size = 0;

    return buff;
}

void delete_buffer(InputBuffer* buffer){
    free(buffer->buffer);
    free(buffer);
}

Table* new_table(){
    Table* table = (Table*)malloc(sizeof(Table));
    table->rows = 0;
    for (int i = 0; i < TABLE_MAX_PAGES; i++){
        table->pages[i] = NULL;
    }
    return table;
}

void free_table(Table* table){
    for (int i = 0; table->pages[i]; i++)
        free(table->pages[i]);
    free(table);
}

void serialize_row(Row* src, void* dest){
    memcpy(dest + ID_OFFSET, &(src->id), ID_SIZE);
    memcpy(dest + USERNAME_OFFSET, (src->username), USERNAME_SIZE);
    memcpy(dest + EMAIL_OFFSET, (src->email), EMAIL_SIZE);
}

void deserialize_row(Row* dest, void* src){
    memcpy(&(dest->id), src + ID_OFFSET, ID_SIZE);
    memcpy((dest->username), src + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy((dest->email), src + EMAIL_OFFSET, EMAIL_SIZE);
}

void* row_slot(Table* table, int row_num){
    int page_num = row_num / rows_per_page;
    void* page = table->pages[page_num];

    if (page == NULL)
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    
    int row_offset = row_num % rows_per_page;
    int byte_offset = row_offset*ROW_SIZE;

    return page + byte_offset;
}

ExecuteResult execute_insert(Statement* statement, Table* table){
    if (table->rows == TABLE_MAX_ROWS)
        return EXECUTE_FAILURE;

    Row* row = &(statement->row_to_insert);
    serialize_row(row, row_slot(table, table->rows));
    table->rows += 1;

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Statement* statement, Table* table){
    Row row_read;
    for (int i = 0; i < table->rows; i++){
        deserialize_row(&row_read, row_slot(table, i));
        printf("%d %s %s\n", row_read.id, row_read.username, row_read.email);
    }

    return EXECUTE_SUCCESS;
}

char* read_input(InputBuffer* buffer, size_t inp_max_size){
    if (buffer->buffer){
        free(buffer->buffer); 
        buffer->buffer = NULL;
    }

    buffer->buffer = (char*)malloc(inp_max_size*sizeof(char));

    if (fgets(buffer->buffer, inp_max_size, stdin) != NULL){
        buffer->buffer[strcspn(buffer->buffer, "\n")] = 0;
        buffer->buffer_size = strlen(buffer->buffer);
        return buffer->buffer;
    }
    
    return NULL;
}

MetaCommandResult execute_meta_command(InputBuffer* buffer, Table* table){
    if (strcmp(buffer->buffer, ".exit") == 0){
        delete_buffer(buffer);
        free_table(table);
        exit(EXIT_SUCCESS);
    }

    return META_COMMAND_UNRECOGNIZED;
}

ExecuteResult execute_statement(Statement* statement, Table* table){
    switch (statement->type){
        case STATEMENT_INSERT:
            return execute_insert(statement, table);
        case STATEMENT_SELECT:
            return execute_select(statement, table);
    }
    return EXECUTE_FAILURE;
}

PrepareResult prepare_result(InputBuffer* buffer, Statement* statement){
    if (strncmp(buffer->buffer, "insert", 6) == 0){
        statement->type = STATEMENT_INSERT;
        Row row_to_insert;

        strtok(buffer->buffer, " ");
        char* id = strtok(NULL, " ");
        char* username = strtok(NULL, " ");
        char* email = strtok(NULL, " ");

        if (id == NULL || username == NULL || email == NULL)
            return PREPARE_SYNTAX_ERROR;

        row_to_insert.id = atoi(id);    
        strcpy(row_to_insert.username, username);
        strcpy(row_to_insert.email, email);

        statement->row_to_insert = row_to_insert;

        return PREPARE_SUCCESS;
    }
    else if (strncmp(buffer->buffer, "select", 6) == 0){
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

int main(){
    InputBuffer* buffer = new_buffer();
    Table* table = new_table();

    if (!buffer)
        exit(EXIT_FAILURE);

    while (1){
        printf("db> ");
        if (!read_input(buffer, MAX_INPUT_SZ)){
            delete_buffer(buffer);
            exit(EXIT_FAILURE);
        }
        
        if (buffer->buffer[0] == '.'){
            switch (execute_meta_command(buffer, table)){
                case META_COMMAND_SUCCESS:
                    continue;
                case META_COMMAND_UNRECOGNIZED:
                    printf("Unrecognized meta command\n");
                    continue;
            }
        }
        else{
            Statement statement;
            switch (prepare_result(buffer, &statement)){
                case PREPARE_SUCCESS:
                    break;
                case PREPARE_UNRECOGNIZED_STATEMENT:
                    printf("Unrecognized statement\n");
                    continue;
                case PREPARE_SYNTAX_ERROR:
                    printf("Syntax Error\n");
                    continue;
            }

            switch(execute_statement(&statement, table)){
                case EXECUTE_SUCCESS:
                    printf("Successful execution\n");
                    break;
                case EXIT_FAILURE:
                    printf("Failed to execute\n");
                    break;
            }
        }
    }
}