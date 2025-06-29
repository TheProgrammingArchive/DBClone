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

    //mem_clear(&tree, tree.root);
}