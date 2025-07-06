#include <stdio.h>
#include <stdlib.h>
#include "database.h"
#include "string.h"

int main(){
    DataType cd[3] = {0, 1, 2};

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

    Cursor* cursor = start_connection("database.db", 3, cd, row_size(&new_row, 3));
    
    close_connection(cursor);

    // void* dest = malloc(4096);
    // serialize_row(&new_row, 3, dest);

    // Row final;
    // deserialize_row(&final, 3, dest);

    for (int i = 0; i < 3; i++)
        free(new_row.columns[i].data);
    free(new_row.columns);
}
