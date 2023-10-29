#include "tree.h"


void bin_tree::print_tree(bin_tree_node* T, int h) {
    if(T) {
        print_tree(T->rchild, h + 1);
        for(int i=0; i < h; i++) std::cout << "  ";
        std::cout << T->element << '\n';
        print_tree(T->lchild, h + 1); 
    }
}

bin_tree_node *bin_tree::search(int k) {
    bin_tree_node *x = root;
    while(x != nil && k != x->element) {
        if(k < x->element) x = x->lchild;
        else x = x->rchild;
    }
    return x;
}

void bin_tree::insert(int k) {
    bin_tree_node *y = nullptr;
    bin_tree_node *x = root;

    bin_tree_node *z = new bin_tree_node;
    z->element = k;
    z->lchild = z->rchild = nullptr;

    while(x) {
        y = x;
        if(z->element < x->element) x = x->lchild;
        else x = x->rchild;
    }

    z->parent = y;
    if(y == nullptr) root = z;
    else if(z->element < y->element) y->lchild = z;
         else y->rchild = z;

    num_nodes++;
}

void bin_tree::transplant(bin_tree_node *u, bin_tree_node *v) {
    // replace u with v
    if(u->parent == nullptr) root = v;

    else {
        if(u == u->parent->lchild) u->parent->lchild = v;
        else  u->parent->rchild = v;
    }

    if(v) v->parent = u->parent;
}

void bin_tree::delete_node(bin_tree_node *z) {
    if(z->lchild == nullptr) transplant(z, z->rchild);
    else {
        if(z->rchild == nullptr) transplant(z, z->lchild);
        else {
            // y is the direct successor of z
            bin_tree_node *y = z->rchild;
            while(y->lchild) y = y->lchild;

            if(y->parent != z) {
                // y is not the rchild of z
                // Need to handle y's rchild first
                transplant(y, y->rchild);
                y->rchild = z->rchild;
                y->rchild->parent = y;
            }

            transplant(z, y);
            y->lchild = z->lchild;
            y->lchild->parent = y;
        }
    }

    delete z;
    num_nodes--;
}

void bin_tree::search_delete(int k) {
    bin_tree_node *z = search(k);
    if(z != nil) delete_node(z);
}


