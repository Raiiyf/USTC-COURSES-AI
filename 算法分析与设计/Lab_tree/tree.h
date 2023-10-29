#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <random>
#include <string>
#include <chrono>
#include "windows.h"

#define MINNUM 1
#define MAXNUM 1000

#define M 5
#define MAXSIZE (M - 1)
#define MINSIZE (M / 2)

enum color_t {BLACK, RED};

class bin_tree_node {
public:
    int element;
    bin_tree_node *lchild;
    bin_tree_node *rchild;
    bin_tree_node *parent;

    color_t color;
    int height;
};

class bin_tree {
public:
    bin_tree_node* root;
    int num_nodes;
    bin_tree_node *nil;

    void initialize() {
        num_nodes = 0;
        root = nullptr;
        nil = nullptr;
    }
    // search.cpp
    void print_tree(bin_tree_node* n, int h);
    bin_tree_node *search(int k);
    void insert(int k);
    void transplant(bin_tree_node *u, bin_tree_node *v);
    void delete_node(bin_tree_node *z);
    void search_delete(int k);
};

class RBtree : public bin_tree {
public:
    void initialize() {
        num_nodes = 0;
        nil = new bin_tree_node;
        nil->color = BLACK;
        nil->element = -1;
        nil->parent = nil;
        root = nil;
    }

    // RBtree.cpp
    void print_tree(bin_tree_node* n, int h);
    void left_rotate(bin_tree_node *x);
    void right_rotate(bin_tree_node *y);
    void insert_fixup(bin_tree_node *z);
    void insert(int k);
    void transplant(bin_tree_node *u, bin_tree_node *v);
    void delete_fixup(bin_tree_node *x);
    void delete_node(bin_tree_node *z);
    void search_delete(int k);
    void inorder(bin_tree_node *x);
};

class AVLtree : public bin_tree {
public:
    int height(bin_tree_node *x) {
        if (x == nullptr) return 0;
        return x->height;
    }

    int BF(bin_tree_node *x) {
        if (x == nullptr) return 0;
        return height(x->lchild) - height(x->rchild);
    }

    inline int max(int a, int b) {
        return (a > b)? a : b;
    }

    // AVLtree.cpp
    void print_tree(bin_tree_node* n, int h);
    bin_tree_node *right_rotate(bin_tree_node *y);
    bin_tree_node *left_rotate(bin_tree_node *x);
    bin_tree_node *insert(bin_tree_node *node, int k);
    bin_tree_node *delete_node(bin_tree_node *node, int k);
};


typedef struct BNode {
    int num;
    BNode *parent;
    int key[M + 1]; // key[0] is sentiel and key[M] is an extra room
    BNode *children[M + 1]; // children[i] stores the interval between key[i] and key[i + 1]

}BNode;

typedef struct Result{
    BNode *node;
    int index;
    bool tag;
}Result;

class Btree {
public:
    BNode *root;
    int num_keys;

    Btree(){
        root = nullptr;
        num_keys = 0;
    }
    // Btree.cpp
    void print_tree(BNode*x, int h);
    void inorder(BNode *);
    Result search(int k);
    bool insert(int k);
    bool delete_node(int k);
};


class BPNode {
public:
    bool IS_LEAF;
    int key[MAXSIZE]; // int[MAXSIZE]
    int num;
    BPNode *ptr[MAXSIZE + 1]; // ptr[i] points to array smaller than key[i]. Noted that, in leaf, ptr[num] links to success leaf
};

class BPtree {
public:
    BPNode *root;

    //BPtree.cpp
    void print_tree(BPNode *, int);
    void inorder(BPNode *);
    void insertInternal(int, BPNode *, BPNode *);
    void removeInternal(int, BPNode *, BPNode *);
    BPNode *findParent(BPNode *, BPNode *);
    void search(int);
    void insert(int);
    void delete_node(int);
    void delete_keys(BPNode *, int, int);

    BPtree() {
        root = nullptr;
    }
};

#endif
