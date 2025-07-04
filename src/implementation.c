#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "database.h"

size_t row_size(Row* row, int column_count){
    size_t sz = 0;
    for (int i = 0; i < column_count; i++){
        sz += sizeof(row->columns[i].data_type); // enum object size
        sz += sizeof(row->columns[i].data_size); // size of data size type
        sz += row->columns[i].data_size; // Actual data size
    }
    return sz;
}

void serialize_row(Row* row, int column_count, void* dest){
    size_t pos = 0;
    for (int i = 0; i < column_count; i++){
        memcpy(dest + pos, &(row->columns[i].data_type), sizeof(DataType));
        pos += sizeof(DataType);
        memcpy(dest + pos, &(row->columns[i].data_size), sizeof(size_t));
        pos += sizeof(size_t);
        // Writing acutal data
        switch(row->columns[i].data_type){
            case DB3_INT:
                int data_int = *(int*)(row->columns[i].data);
                memcpy(dest + pos, &(data_int), row->columns[i].data_size);
                pos += row->columns[i].data_size;
                break;

            case DB3_FLOAT:
                float data_float = *(float*)(row->columns[i].data);
                memcpy(dest + pos, &(data_float), row->columns[i].data_size);
                pos += row->columns[i].data_size;
                break;

            case DB3_STRING:
                char* data_str = (char*)(row->columns[i].data);
                memcpy(dest + pos, data_str, row->columns[i].data_size);
                pos += row->columns[i].data_size;
                break;
        }
    }
}

void deserialize_row(Row* row, int column_count, void* src){
    size_t pos = 0;
    row->columns = (Column*)malloc(sizeof(Column)*column_count);
    for (int i = 0; i < column_count; i++){
        memcpy(&(row->columns[i].data_type), src + pos, sizeof(DataType));
        pos += sizeof(DataType);
        memcpy(&(row->columns[i].data_size), src + pos, sizeof(size_t));
        pos += sizeof(size_t);

        void* data = malloc(row->columns[i].data_size);
        memcpy(data, src + pos, row->columns[i].data_size);
        row->columns[i].data = data;

        pos += row->columns[i].data_size;
    }
}   