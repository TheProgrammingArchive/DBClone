#ifndef DATABASE_H
#define DATABASE_H
#define TABLE_MAX_PAGES 200
#define PAGE_SIZE 4096
#include "objects.h"
#include <stdlib.h>

// Related to Pager and Table

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

typedef struct{
    Table* table;
    int page_num;
    int cell_num;
} Cursor;


// Row operations
size_t row_size(Row* row, int column_count);

void serialize_row(Row* row, int column_count, void* dest);

void deserialize_row(Row* row, int column_count, void* src);

// File operations
Pager* pager_open(const char* fname);

void pager_flush(Pager* pager, int page_num);

void* get_page(Pager* pager, int page_num);

// Database connection handlers
Cursor* start_connection(const char* fname, int column_count, DataType* column_descriptor, size_t row_size); // Store column count and column descriptor metadata in a json file

void close_connection(Cursor* cursor);

#endif