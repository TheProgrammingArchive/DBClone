#ifndef BTREE_H
#define BTREE_H

typedef enum{
    NODE_LEAF,
    NODE_INTERNAL
} NodeType;

typedef struct{
    int key;
    struct Row* assoc_row; // NULL for internal node
    int assoc_child_page; // -1 for leaf node
} Pair;

int data_space(NodeType node_type); // Space for actual data excluding metadata

// Way to read and write to constants from file
int is_root(void* page);
void set_is_root(void* page, int is_root);

NodeType node_type(void* page);
void set_node_type(void* page, NodeType node_type);

int parent_pointer(void* page);
void set_parent_pointer(void* page, int parent_page);

// Leaf node specific
int num_cells(void* page);
void set_num_cells(void* page, int num_cells);

// Internal node specific
int num_keys(void* page);
void set_num_keys(void* page, int num_keys);

void* left_most_child_pointer(void* page);
void set_left_most_child_poiner(void* page, int left_most_child_page);

// KVC pairs
void* get_key(void* page, int cell_num, int row_size);
void* get_assoc_row(void* page, int cell_num);
void* get_assoc_ptr(void* page, int cell_num);

#endif