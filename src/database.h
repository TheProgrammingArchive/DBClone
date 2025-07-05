#ifndef DATABASE_H
#define DATABASE_H
#define TABLE_MAX_PAGES 200
#include "objects.h"
#include <stdlib.h>

typedef struct{
    int file_descriptor;
    size_t file_length;
    int num_pages;
    void* pages[TABLE_MAX_PAGES];
} Pager;

typedef struct{
    Pager* pager;
    int root_page_num;

    // MetaData
    int column_count;
    DataType* column_descriptor;
    size_t row_size;
} Table;

size_t row_size(Row* row, int column_count);

void serialize_row(Row* row, int column_count, void* dest);

void deserialize_row(Row* row, int column_count, void* src);

#endif