#include "tree.h"
#include <stdio.h>

int main(){
    Btree tree;
    tree.root = NULL;
    tree.order = 3;

    insert(&tree, 0, "hello");
    insert(&tree, 1, "hi");
    insert(&tree, 2, "yo mama");
}