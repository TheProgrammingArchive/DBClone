#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#define MAX_INPUT_SZ 128

typedef struct{
    char* buffer;
    size_t buffer_size;
} InputBuffer;

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

MetaCommandResult execute_meta_command(InputBuffer* buffer){
    if (strcmp(buffer->buffer, ".exit") == 0){
        delete_buffer(buffer);
        exit(EXIT_SUCCESS);
    }

    return META_COMMAND_UNRECOGNIZED;
}


int main(){
    InputBuffer* buffer = new_buffer();

    if (!buffer)
        exit(EXIT_FAILURE);

    while (1){
        printf("db> ");
        if (!read_input(buffer, MAX_INPUT_SZ)){
            delete_buffer(buffer);
            exit(EXIT_FAILURE);
        }
    
        if (buffer->buffer[0] == '.'){
            MetaCommandResult result = execute_meta_command(buffer);

            if (result == META_COMMAND_UNRECOGNIZED)
                printf("Unrecognized meta command\n");
        }
        else{

        }
    }
}