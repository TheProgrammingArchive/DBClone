#include <stdio.h>
#include <stdlib.h>
#include "database.h"
#include "string.h"

int main(){
    Table new_table;
    new_table.column_count = 3;
    new_table.column_descriptor = (DataType*)malloc(sizeof(DataType)*3);

    new_table.column_descriptor[0] = DB3_INT;
    new_table.column_descriptor[1] = DB3_FLOAT;
    new_table.column_descriptor[2] = DB3_STRING;

    Row new_row;
    new_row.columns = (Column*)malloc(sizeof(Column)*3);

    new_row.columns[0].data_type = DB3_INT;
    new_row.columns[0].data_size = sizeof(int);
    int x = 3;
    new_row.columns[0].data = malloc(sizeof(int));
    *(int*)new_row.columns[0].data = x;

    new_row.columns[1].data_type = DB3_FLOAT;
    new_row.columns[1].data_size = sizeof(float);
    new_row.columns[1].data = malloc(sizeof(float));
    float y = 9.3;
    *(float*)new_row.columns[1].data = y;

    new_row.columns[2].data_type = DB3_STRING;
    new_row.columns[2].data_size = sizeof(char)*10;
    char test[10] = "the one";
    new_row.columns[2].data = malloc(new_row.columns[2].data_size);
    strcpy((char*)new_row.columns[2].data, test);

    void* dest = malloc(4096);
    serialize_row(&new_row, 3, dest);

    Row final;
    deserialize_row(&final, 3, dest);

    printf("%s ", (char*)final.columns[2].data);

    // free
    free(dest);
    free(new_table.column_descriptor);
    
    for (int i = 0; i < new_table.column_count; i++){
        free(final.columns[i].data);
        free(new_row.columns[i].data); 
    }

    free(new_row.columns);
    free(final.columns);
}
