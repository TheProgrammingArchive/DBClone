#include "btree.h"
#include <stdlib.h>

// Header constants
// Common constants
const int is_root_size = sizeof(int);
const int is_root_offset = 0;
const NodeType node_type_size = sizeof(NodeType);
const int node_type_offset = is_root_offset + node_type_size;
const int parent_pointer_size = sizeof(int);
const int parent_pointer_offset = node_type_offset + parent_pointer_size;

// Leaf node constants
const int num_cells_size = sizeof(int);
const int num_cell_size_offset = parent_pointer_offset + num_cells_size;
const int space_for_cells = PAGE_SIZE - (is_root_size + node_type_size + parent_pointer_size + num_cells_size);

// Internal node constants
const int num_keys_size = sizeof(int);
const int num_keys_offset = parent_pointer_offset + num_keys_size;
const int left_most_child_page_size = sizeof(int);
const int left_most_child_page_size_offset = num_keys_offset + left_most_child_page_size;

int leaf_node_to_insert(int key){
    
}