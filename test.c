#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define PAGE_SIZE 4096
#define MAX_PAGES 100
#define VARCHAR_MAX 255 

typedef enum{
    DB3_INT,
    DB3_FLOAT,
    DB3_VARCHAR,
} DataType;

typedef struct{
    DataType type;
    void* data;
    size_t data_size;
} Column;

typedef struct{ 
    Column* columns;
    int column_count;
} Row;


int row_size(Row* row){
    int size = sizeof(row->column_count);
    for (int i = 0; i < row->column_count; i++){
        size += sizeof(DataType);
        size += row->columns[i].data_size;
        size += sizeof(size_t);
    }
    return size;
}

void serialize_row(Row* src, void* dest){
    unsigned long pos = 0;
    memcpy(dest + pos, &(src->column_count), sizeof(int));
    pos += sizeof(int);
    for (int i = 0; i < src->column_count; i++){
        memcpy(dest + pos, &(src->columns[i].type), sizeof(DataType));
        pos += sizeof(DataType);
        memcpy(dest + pos, &(src->columns[i].data_size), sizeof(size_t));
        pos += sizeof(size_t);
        memcpy(dest + pos, src->columns[i].data, src->columns[i].data_size);
        pos += src->columns[i].data_size;
    }
}

void deserialize_row(Row* dest, void* src){
    unsigned long pos = 0;
    memcpy(&(dest->column_count), src + pos, sizeof(int));
    pos += sizeof(int);

    dest->columns = (Column*)malloc(dest->column_count*sizeof(Column));
    for (int i = 0; i < dest->column_count; i++){
        memcpy(&(dest->columns[i].data), src + pos, sizeof(DataType));
        pos += sizeof(DataType);
    }
}

void free_data(Row* row){
    for (int i = 0; i < row->column_count; i++){
        free(row->columns[i].data);
    }
    free(row->columns);
}

int main(){
    Column col1, col2, col3;
    col1.type = DB3_INT;
    col1.data_size = sizeof(int);
    col1.data = malloc(sizeof(int));
    *(int*)col1.data = 3;

    col2.type = DB3_FLOAT;
    col2.data_size = sizeof(float);
    col2.data = malloc(sizeof(float));
    *(float*)col2.data = 444.4;

    col3.type = DB3_VARCHAR;
    col3.data_size = VARCHAR_MAX*sizeof(char);
    col3.data = malloc(col2.data_size);
    strcpy((char*)col3.data, "hello");

    Row new_row;
    new_row.column_count = 3;
    new_row.columns = (Column*)malloc(new_row.column_count*sizeof(Column));
    new_row.columns[0] = col1, new_row.columns[1] = col2, new_row.columns[2] = col3;

    void* dest = malloc(1000);
    serialize_row(&new_row, dest);

    Row test;
    deserialize_row(&test, dest);

    printf("%d", test.column_count);

    free_data(&new_row);
    free(dest);
}