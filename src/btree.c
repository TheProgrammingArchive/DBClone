#include "btree.h"
#include "database.h"
#include <stdlib.h>
#define PAGE_SIZE 4096

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

    int pair_size = sizeof(int) + row_size;
    NodeType type = node_type(page);
    if (type == NODE_LEAF){
        int start = num_cells_offset + num_cells_size;

        return (page + start + pair_size*cell_num);
    }
    else{
        int start = left_most_child_page_offset + left_most_child_page_size;

        return (page + start + pair_size*cell_num);
    }
}