#include <stdio.h>
#include <string.h>
#include "database.h"

int main(){
    Row new_row;
    Column col1, col2;
    col1.column_type = DB3_INTEGER;
    col1.data = malloc(sizeof(int));
    *(int*)col1.data = 5;
    col1.data_size = sizeof(int);

    col2.column_type = DB3_VARCHAR;
    col2.data = (char*)malloc(6*sizeof(char));
    strcpy((char*)col2.data, "tests");
    col2.data_size = strlen("tests") + 1;

    new_row.columns = (Column*)malloc(2*sizeof(Column));
    new_row.columns[0] = col1;
    new_row.columns[1] = col2;
    new_row.col_count = 2;

    int* att_sz = sizeof_attributes(&new_row);
    printf("%d ", sizeof(float));
}