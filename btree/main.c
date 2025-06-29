#include "tree.h"
#include <stdio.h>

int main(){
    Btree tree;
    tree.root = NULL;
    tree.order = 3;

    insert(&tree, 5, "hello");
    insert(&tree, 12, "hi");
    insert(&tree, 1, "yo mama");
    insert(&tree, 2, "d");
    insert(&tree, 18, "erer");
    insert(&tree, 21, "test");

    //printf("%d ", tree.root->left_most_child->cell_count);
    mem_clear(&tree, tree.root);
}