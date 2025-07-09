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

// Header constants
const int key_size = sizeof(int);
// Common constants
const int is_root_size = sizeof(int);
const int is_root_offset = 0;
const NodeType node_type_size = sizeof(NodeType);
const int node_type_offset = is_root_offset + is_root_size;
const int parent_pointer_size = sizeof(int);
const int parent_pointer_offset = node_type_offset + node_type_size;

// Leaf node constants
const int num_cells_size = sizeof(int);
const int num_cells_offset = parent_pointer_offset + parent_pointer_size;
const int space_for_kv_pairs = PAGE_SIZE - (is_root_size + node_type_size + parent_pointer_size + num_cells_size);

// Internal node constants
const int num_keys_size = sizeof(int);
const int num_keys_offset = parent_pointer_offset + parent_pointer_size;
const int left_most_child_page_size = sizeof(int);
const int left_most_child_page_offset = num_keys_offset + num_keys_size;
const int space_for_kp_pairs = PAGE_SIZE - (is_root_size + node_type_size + parent_pointer_size + num_keys_size + left_most_child_page_size);

int data_space(NodeType node_type){
    if (node_type == NODE_INTERNAL)
        return space_for_kp_pairs;
    return space_for_kv_pairs;
}

int is_root(void* page){
    return *(int*)(page + is_root_offset);
}
void set_is_root(void* page, int is_root){
    *(int*)(page + is_root_offset) = is_root;
}

NodeType node_type(void* page){
    return *(NodeType*)(page + node_type_offset);
}

void set_node_type(void* page, NodeType node_type){
    *(NodeType*)(page + node_type_offset) = node_type;
}

int parent_pointer(void* page){
    return *(int*)(page + parent_pointer_offset);
}

void set_parent_pointer(void* page, int parent_page){
    *(int*)(page + parent_pointer_offset) = parent_page;
}

// Leaf node
int num_cells(void* page){
    return *(int*)(page + num_cells_offset);
}

void set_num_cells(void* page, int num_cells){
    *(int*)(page + num_cells_offset) = num_cells;
}

// Internal node
int num_keys(void* page){
    return *(int*)(page + num_keys_offset);
}

void set_num_keys(void* page, int num_keys){
    *(int*)(page + num_keys_offset) = num_keys;
}

void* left_most_child_pointer(void* page){
    return (page + left_most_child_page_offset);
}

void set_left_most_child_poiner(void* page, int left_most_child_page){
    *(int*)(page + left_most_child_page_offset) = left_most_child_page;
}

// KV Pair handlers
void* get_key(void* page, int cell_num, int row_size){
    int curr_cells = num_cells(page);
    if (cell_num >= curr_cells)
        exit(EXIT_FAILURE);

    NodeType type = node_type(page);
    if (type == NODE_LEAF){
        int pair_size = sizeof(int) + row_size; // Key + row
        int start = num_cells_offset + num_cells_size;

        return (page + start + pair_size*cell_num);
    }
    else{
        int pair_size = sizeof(int) + sizeof(int); // Key + child ptr
        int start = left_most_child_page_offset + left_most_child_page_size;

        return (page + start + pair_size*cell_num);
    }
}

void* get_assoc_row(void* page, int cell_num, int row_size){
    if (node_type(page) == NODE_INTERNAL){
        printf("Internal nodes do not contain rows");
        exit(EXIT_FAILURE);
    }

    void* key_loc = get_key(page, cell_num, row_size);
    return key_loc + row_size;
}

void* get_assoc_ptr(void* page, int cell_num, int row_size){
    if (node_type(page) == NODE_LEAF){
        printf("Leaf nodes do not contain child pointers");
        exit(EXIT_FAILURE);
    }

    void* key_loc = get_key(page, cell_num, row_size);
    return key_loc + sizeof(int);
}