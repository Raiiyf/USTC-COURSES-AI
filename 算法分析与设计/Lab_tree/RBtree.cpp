#include "tree.h"

void RBtree::print_tree(bin_tree_node* n, int h) {
    if(n != nil) {
        print_tree(n->rchild, h + 1);
        for(int i=0; i < h; i++) std::cout << "  ";
        // if(n->color == RED) 
        //     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        // else 
        //     SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY);
        std::cout << n->element << ' ' << n->color << '\n';
        print_tree(n->lchild, h + 1); 
    }
}

void RBtree::left_rotate(bin_tree_node *x) {
    bin_tree_node *y = x->rchild;
    x->rchild = y->lchild;

    if(y->lchild != nil) 
        y->lchild->parent = x;

    y->parent = x->parent;
    if(x->parent == nil) 
        root = y;
    else {
        if(x == x->parent->lchild)
            x->parent->lchild = y;
        else x->parent->rchild = y;
    }

    y->lchild = x;
    x->parent = y;
}

void RBtree::right_rotate(bin_tree_node *y) {
    bin_tree_node *x = y->lchild;
    y->lchild = x->rchild;

    if(x->rchild != nil)
        x->rchild->parent = y;

    x->parent = y->parent;
    if(y->parent == nil)
        root = x;
    else {
        if(y == y->parent->lchild)
            y->parent->lchild = x;
        else y->parent->rchild = x;
    }

    x->rchild = y;
    y->parent = x;
}

void RBtree::insert_fixup(bin_tree_node *z) {
    bin_tree_node *y;
    while(z->parent->color == RED) {
        if(z->parent == z->parent->parent->lchild) {
            // uncle node
            y = z->parent->parent->rchild;
            if(y->color == RED) {
                // case 1: both parent and uncle are RED, we can infer grandparent is BLACK
                // Therefore, we inverse the color of them to achieve iterative solution.
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if(z == z->parent->rchild) {
                    // case 2: uncle node ia BLACK and z is parent's right child
                    // Using left rotation to convert to case 3.
                    z = z->parent;
                    left_rotate(z);
                }
                // case 3: uncle node ia BLACK and z is parent's left child
                // Using single right rotation to solve the problem.
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(z->parent->parent);
            }
        }
        else {
            y = z->parent->parent->lchild;
            if(y->color == RED) {
                // case 4
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if(z == z->parent->lchild) {
                    // case 5
                    z = z->parent;
                    right_rotate(z);
                }
                // case 6
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(z->parent->parent);
            }
        }
    }
    root->color = BLACK;
}

void RBtree::insert(int k) {
    bin_tree_node *z = new bin_tree_node;
    bin_tree_node *y = nil;
    bin_tree_node *x = root;
    z->element = k;

    // find the insert location (parent)
    while(x != nil) {
        y = x;
        if(z->element < x->element) 
            x = x->lchild;
        else x = x->rchild;
    }
    
    // add node z into tree
    z->parent = y;
    if(y == nil)
        root = z;
    else {
        if(z->element < y->element) 
            y->lchild = z;
        else y->rchild = z;
    }

    z->lchild = nil;
    z->rchild = nil;
    z->color = RED;
    num_nodes++;

    insert_fixup(z);
}

void RBtree::transplant(bin_tree_node *u, bin_tree_node *v) {
    if(u->parent == nil)
        root = v;
    else {
        if(u == u->parent->lchild)
            u->parent->lchild = v;
        else u->parent->rchild = v;
    }
    v->parent = u->parent;
}

void RBtree::delete_fixup(bin_tree_node *x) {
    bin_tree_node *w;
    while(x != root && x->color == BLACK) {
        if(x == x->parent->lchild) {
            w = x->parent->rchild;
            if(w->color == RED) {
                // case 1
                w->color = BLACK;
                x->parent->color = RED;
                left_rotate(x->parent);
                w = x->parent->rchild;
            }
            if(w->lchild->color == BLACK && w->rchild->color == BLACK) {
                // case 2
                w->color = RED;
                x = x->parent;
            }
            else {
                if(w->rchild->color == BLACK) {
                    // case 3
                    // w's left child is RED
                    w->lchild->color = BLACK;
                    w->color = RED;
                    right_rotate(w);
                    w = x->parent->rchild;
                }
                // case 4
                // w's right child is RED
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->rchild->color = BLACK;
                left_rotate(x->parent);
                x = root;
            }
        }
        else {
            w = x->parent->lchild;
            if(w->color == RED) {
                // case 5
                w->color = BLACK;
                x->parent->color = RED;
                right_rotate(x->parent);
                w = x->parent->lchild;
            }
            if(w->lchild->color == BLACK && w->rchild->color == BLACK) {
                // case 6
                w->color = RED;
                x = x->parent;
            }
            else {
                if(w->lchild->color == BLACK) {
                    // case 7
                    // w's right child is RED
                    w->rchild->color = BLACK;
                    w->color = RED;
                    left_rotate(w);
                    w = x->parent->lchild;
                }
                // case 4
                // w's left child is RED
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->lchild->color = BLACK;
                right_rotate(x->parent);
                x = root;
            }
        }
    }
    x->color = BLACK;
}

void RBtree::delete_node(bin_tree_node *z) {
    bin_tree_node *x;
    bin_tree_node *y = z;
    color_t y_orginal_color = y->color;
    if(z->lchild == nil) {
        x = z->rchild;
        transplant(z, z->rchild);
    }
    else {
        if(z->rchild == nil) {
            x = z->lchild;
            transplant(z, z->lchild);
        }
        else {
            // y is the direct successor of z
            y = z->rchild;
            while(y->lchild != nil) y = y->lchild;

            y_orginal_color = y->color;
            x = y->rchild;
            if(y->parent == z) x->parent = y; // doubt
            else {
                transplant(y, y->rchild);
                y->rchild = z->rchild;
                y->rchild->parent = y;
            }
            transplant(z, y);
            y->lchild = z->lchild;
            y->lchild->parent = y;
            y->color = z->color;
        }
    }
    delete z;
    num_nodes--;
    if(y_orginal_color == BLACK)
        delete_fixup(x);
}

void RBtree::search_delete(int k) {
    bin_tree_node *z = search(k);
    if(z != nil) delete_node(z);
    else std::cout << "Not Found\n";
}

void RBtree::inorder(bin_tree_node* T) {
    if(T != nil) {
        inorder(T->lchild);
        std::cout << T->element << ' ';
        inorder(T->rchild); 
    }
}
