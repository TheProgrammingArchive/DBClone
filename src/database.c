#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "database.h"


// Row Operations
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

// File operations
Pager* pager_open(const char* fname){
    int fd = open(fname, O_RDWR | O_CREAT, 0600);
    if (fd == -1){
        exit(EXIT_FAILURE);
    }

    off_t file_length = lseek(fd, 0, SEEK_END);
    Pager* pager = (Pager*)malloc(sizeof(Pager));
    pager->file_descriptor = fd;
    pager->file_length = file_length;
    pager->num_pages = file_length / PAGE_SIZE;
    
    for (int i = 0; i < TABLE_MAX_PAGES; i++)
        pager->pages[i] = NULL;

    return pager;
}

void pager_flush(Pager* pager, int page_num){
    if (!pager->pages[page_num])
        exit(EXIT_FAILURE);

    off_t advance = lseek(pager->file_descriptor, page_num*PAGE_SIZE, SEEK_SET);
    if (advance == -1)
        exit(EXIT_FAILURE);

    size_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], PAGE_SIZE);
    if (bytes_written == -1)    
        exit(EXIT_FAILURE);
}

void* get_page(Pager* pager, int page_num){
    if (!pager->pages[page_num]){
        pager->pages[page_num] = malloc(PAGE_SIZE);
        
        off_t advance = lseek(pager->file_descriptor, page_num*PAGE_SIZE, SEEK_SET);
        if (advance == -1)
            exit(EXIT_FAILURE);
        
        size_t bytes_read = read(pager->file_descriptor, pager->pages[page_num], PAGE_SIZE);
        if (bytes_read == -1)
            exit(EXIT_FAILURE);
        
    }
    return pager->pages[page_num];
}

// Database connections
Cursor* start_connection(const char* fname, int column_count, DataType* column_descriptor, size_t row_size){
    Table* table = (Table*)malloc(sizeof(Table));
    Cursor* cursor = (Cursor*)malloc(sizeof(Cursor));

    table->pager = pager_open(fname);
    table->column_count = column_count;
    table->column_descriptor = column_descriptor;
    table->root_page_num = 0;
    table->row_size = row_size;

    cursor->table = table;
    cursor->page_num = 0;
    cursor->cell_num = 0;

    return cursor;
}

void close_connection(Cursor* cursor){
    Table* table = cursor->table;
    Pager* pager = table->pager;

    int cls = close(pager->file_descriptor);
    if (cls == -1)
        exit(EXIT_FAILURE);
    for (int i = 0; i < pager->num_pages; i++)
        free(pager->pages[i]);

    free(pager);
    free(table);
    free(cursor);
}

// Btree