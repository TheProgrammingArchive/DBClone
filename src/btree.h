#ifndef BTREE_H
#define BTREE_H
#include "database.h"

typedef enum{
    NODE_LEAF,
    NODE_INTERNAL
} NodeType;

typedef struct{
    int key;
    Row* assoc_row; // NULL for internal node
    int assoc_child_page; // -1 for leaf node
} Pair;

// Way to read and write to constants from file
int is_root(int page_num);
void set_is_root(int page_num, int is_root);

int node_type(int page_num);
void set_node_type(int page_num, NodeType node_type);

int parent_pointer(int page_num);
void set_parent_pointer(int page_num);

// Leaf node specific
int num_cells(int page_num);
void set_num_cells(int page_num, int num_cells);

// Internal node specific
int num_keys(int page_num);
void set_num_keys(int page_num, int num_keys);

#endif