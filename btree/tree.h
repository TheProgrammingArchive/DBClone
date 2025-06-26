#ifndef TREE_H
#define TREE_H

typedef enum{
    NODE_INTERNAL, 
    NODE_LEAF
} NodeType;

typedef struct Pair{
    int key;
    char* value; // NULL for internal node
    struct Node* assoc_child; // NULL for leaf node
} Pair;

typedef struct Node{
    struct Node* parent; // NULL for root
    int cell_count, is_root;
    NodeType node_type;

    Pair* kv_pairs;
    struct Node* left_most_child; // NULL for leaf node
} Node;

typedef struct{
    Node* root;
    int order;
} Btree;

Node* find_leaf_to_insert(Btree* btree, Node* curr, int key);

void insert_into_leaf(Btree* btree, Node* ins_leaf_node, int key, char* value);

void insert_into_internal(Btree* btree, Node* ins_internal_node, int key, Node* assoc_child);

void split_insert_into_leaf(Btree* btree, Node* node_to_split, int key, char* value);

Node* new_root(Btree* btree, Node* left_most_child, int is_leaf);

void insert(Btree* btree, int key, char* value);

// void split_insert_into_internal(Btree* btree, Node* node_to_split, int key);

#endif