#include "tree.h"

void AVLtree::print_tree(bin_tree_node* T, int h) {
    if(T) {
        print_tree(T->rchild, h + 1);
        for(int i=0; i < h; i++) std::cout << "  ";
        std::cout << T->element << ' ' << BF(T) << '\n';
        print_tree(T->lchild, h + 1); 
    }
}

// perform right rotation at current root and return new root
bin_tree_node *AVLtree::right_rotate(bin_tree_node *y) {
    bin_tree_node *x = y->lchild;
    y->lchild = x->rchild;
    x->rchild = y;

    y->height = max(height(y->lchild), height(y->rchild)) + 1;
    x->height = max(height(x->lchild), height(x->rchild)) + 1;
    return x;
}

// perform left rotation at current root and return new root
bin_tree_node *AVLtree::left_rotate(bin_tree_node *x) {
    bin_tree_node *y = x->rchild;
    x->rchild = y->lchild;
    y->lchild = x;

    x->height = max(height(x->lchild), height(x->rchild)) + 1;
    y->height = max(height(y->lchild), height(y->rchild)) + 1;
    return y;
}

// Insert a new node into the AVL tree whose root is node and return the new root
bin_tree_node* AVLtree::insert(bin_tree_node* node, int k) {
    if (node == nullptr) {
        node = new bin_tree_node;
        node->element = k;
        node->lchild = nullptr;
        node->rchild = nullptr;
        node->height = 0;
        num_nodes++;
    } else if (k < node->element) {
        // Insert to its left child
        node->lchild = insert(node->lchild, k);
        if (BF(node) == 2) { // current node is imbalanced
            if (k < node->lchild->element) {
                // LL case -> right rotate
                node = right_rotate(node);
            } else {
                // LR case -> left rotate then right rotate
                node->lchild = left_rotate(node->lchild);
                node = right_rotate(node);
            }
        }
    } else if (k >= node->element) {
        // Duplicated node are allowed
        node->rchild = insert(node->rchild, k);
        if (BF(node) == -2) {
            if (k >= node->rchild->element) {
                // RR case -> left rotate
                node = left_rotate(node);
            } else {
                // RL case -> right rotate then left rotate
                node->rchild = right_rotate(node->rchild);
                node = left_rotate(node);
            }
        }
    }
    node->height = max(height(node->lchild), height(node->rchild)) + 1;
    return node;
}

// Deletes a given key from an AVL tree and returns the new root of the tree after deletion
bin_tree_node* AVLtree::delete_node(bin_tree_node* node, int k) {
    if (node == nullptr) return node;

    if (k < node->element) node->lchild = delete_node(node->lchild, k);
    else if (k > node->element) node->rchild = delete_node(node->rchild, k);
    else {
        if ((node->lchild == nullptr) || (node->rchild == nullptr)) {
            // Node to delete don't have two children
            bin_tree_node *temp = node->lchild ? node->lchild : node->rchild;

            if (temp == nullptr) {
                // Node to delete is a leave node
                temp = node;
                node = nullptr;
            } else *node = *temp;

            delete temp;
            num_nodes--;
        } else {
            // Delete the direct successor of node
            bin_tree_node* temp = node->rchild;
            while(temp -> lchild) temp = temp->lchild;

            node->element = temp->element;
            node->rchild = delete_node(node->rchild, temp->element);
        }
    }
    // After deletion
    if (node == nullptr) return node;

    node->height = 1 + max(height(node->lchild), height(node->rchild));

    int balance = BF(node);
    // LL
    if (balance > 1 && BF(node->lchild) >= 0) return right_rotate(node);
    // LR
    if (balance > 1 && BF(node->lchild) < 0) {
        node->lchild = left_rotate(node->lchild);
        return right_rotate(node);
    }
    // RR
    if (balance < -1 && BF(node->rchild) <= 0) return left_rotate(node);
    // RL
    if (balance < -1 && BF(node->rchild) > 0) {
        node->rchild = right_rotate(node->rchild);
        return left_rotate(node);
    }

    return node;
}
