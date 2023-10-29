#include "tree.h"

#define NUM 1000

std::uniform_int_distribution<int> u(MINNUM, MAXNUM); 
std::default_random_engine e(1);

int *generate_array() {
    int *A = new int[NUM];

    for(int i=0; i < NUM; i++) {
        A[i] = u(e);
    }

    return A;
}

void insertion_sort(int A[], int n) {
    int key;
    for(int j=n - 50; j < n; j++) {
        key = A[j];
        int i = j - 1;
        while(i >= n - 50 && A[i] > key) {
            A[i + 1] = A[i];
            i--;
        }
        A[i + 1] = key;
    }
}

void print_array(int *A, bool sort=false) {
    if(sort) 
        insertion_sort(A, NUM);
    for(int i= NUM - 50; i < NUM; i++) {
        std::cout << A[i] << ' ';
    }
    std::cout << '\n';
}

void inorder_walk(bin_tree_node* T) {
    if(T) {
        inorder_walk(T->lchild);
        std::cout << T->element << ' ';
        inorder_walk(T->rchild); 
    }
}

int main() {
    int *A = generate_array();
    print_array(A);

/*========== Binary Search tree ==============*/ 

    // bin_tree T;
    // T.initialize();

    // // auto start = std::chrono::high_resolution_clock::now();
    // for(int i=0; i < NUM; i++) {
    //     T.insert(A[i]);
    // }
    // // auto stop = std::chrono::high_resolution_clock::now();
    // // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // // std::cout << "Time taken by Insertion: "
    // //     << duration.count() << " microseconds" << std::endl;

    // // start = std::chrono::high_resolution_clock::now();
    // for(int i=0; i < NUM; i++) {
    //     T.search(A[i]);
    // }
    // // stop = std::chrono::high_resolution_clock::now();
    // // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // // std::cout << "Time taken by Search: "
    // //     << duration.count() << " microseconds" << std::endl;

    // // start = std::chrono::high_resolution_clock::now();
    // for(int i=0; i < NUM - 50; i++) {
    //     T.search_delete(A[i]);
    // }
    // // stop = std::chrono::high_resolution_clock::now();
    // // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // // std::cout << "Time taken by Deletion: "
    // //     << duration.count() << " microseconds" << std::endl;

    // T.print_tree(T.root, 0);

    // std::cout << "Inorder walk is:\n";
    // inorder_walk(T.root);
    // std::cout << std::endl;
    
/*========== Red Black tree ==============*/ 

    // RBtree T;
    // T.initialize();

    // // auto start = std::chrono::high_resolution_clock::now();
    // for(int i=0; i < NUM; i++) {
    //     T.insert(A[i]);
    // }
    // // auto stop = std::chrono::high_resolution_clock::now();
    // // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // // std::cout << "Time taken by Insertion: "
    // //     << duration.count() << " microseconds" << std::endl;
    
    // // start = std::chrono::high_resolution_clock::now();
    // for(int i=0; i < NUM; i++) {
    //     T.search(A[i]);
    // }
    // // stop = std::chrono::high_resolution_clock::now();
    // // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // // std::cout << "Time taken by Search: "
    // //     << duration.count() << " microseconds" << std::endl;

    // // start = std::chrono::high_resolution_clock::now();
    // for(int i=0; i < NUM - 50; i++) {
    //     T.search_delete(A[i]);
    // }
    // // stop = std::chrono::high_resolution_clock::now();
    // // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // // std::cout << "Time taken by Deletion: "
    // //     << duration.count() << " microseconds" << std::endl;

    // T.print_tree(T.root, 0);
    // std::cout << "Inorder walk is:\n";
    // T.inorder(T.root);
    // std::cout << std::endl;

/*========== AVL tree ==============*/ 

    // AVLtree T;
    // T.initialize();

    // // auto start = std::chrono::high_resolution_clock::now();
    // for(int i=0; i < 20; i++) {
    //     T.root = T.insert(T.root, A[i]);
    // }
    // // auto stop = std::chrono::high_resolution_clock::now();
    // // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // // std::cout << "Time taken by Insertion: "
    // //     << duration.count() << " microseconds" << std::endl;

    // // start = std::chrono::high_resolution_clock::now();
    // // for(int i=0; i < NUM; i++) {
    // //     T.search(A[i]);
    // // }
    // // stop = std::chrono::high_resolution_clock::now();
    // // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // // std::cout << "Time taken by Search: "
    // //     << duration.count() << " microseconds" << std::endl;

    // // start = std::chrono::high_resolution_clock::now();
    // // for(int i=0; i < NUM - 50; i++) {
    // //     T.root = T.delete_node(T.root, A[i]);
    // // }
    // // stop = std::chrono::high_resolution_clock::now();
    // // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // // std::cout << "Time taken by Deletion: "
    // //     << duration.count() << " microseconds" << std::endl;    

    // T.print_tree(T.root, 0);
    // std::cout << "Inorder walk is:\n";
    // inorder_walk(T.root);
    // std::cout << std::endl;
    
/*========== B tree ==============*/ 

    // Btree T;

    // // auto start = std::chrono::high_resolution_clock::now();
    // for(int i=0; i < NUM; i++) {
    //     T.insert(A[i]);
    // }

    // // auto stop = std::chrono::high_resolution_clock::now();
    // // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // // std::cout << "Time taken by Insertion: "
    // //     << duration.count() << " microseconds" << std::endl;

    // // start = std::chrono::high_resolution_clock::now();
    // for(int i=0; i < NUM; i++) {
    //     T.search(A[i]);
    // }
    // // stop = std::chrono::high_resolution_clock::now();
    // // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // // std::cout << "Time taken by Search: "
    // //     << duration.count() << " microseconds" << std::endl;

    // // start = std::chrono::high_resolution_clock::now();
    // for(int i=0; i < NUM - 50; i++) {
    //     T.delete_node(A[i]);
    // }
    // // stop = std::chrono::high_resolution_clock::now();
    // // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // // std::cout << "Time taken by Deletion: "
    // //     << duration.count() << " microseconds" << std::endl;    

    // T.print_tree(T.root, 0);
    // std::cout << "Inorder walk is:\n";
    // T.inorder(T.root);
    // std::cout << std::endl;

/*========== B plus tree ==============*/ 

    BPtree T;

    // auto start = std::chrono::high_resolution_clock::now();
    for(int i=0; i < NUM; i++) {
        T.insert(A[i]);
    }
    // auto stop = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // std::cout << "Time taken by Insertion: "
    //     << duration.count() << " microseconds" << std::endl;

    // start = std::chrono::high_resolution_clock::now();
    // for(int i=0; i < NUM; i++) {
    //     T.search(A[i]);
    // }
    // stop = std::chrono::high_resolution_clock::now();
    // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // std::cout << "Time taken by Search: "
    //     << duration.count() << " microseconds" << std::endl;    

    // start = std::chrono::high_resolution_clock::now();
    for(int i=0; i < NUM - 50; i++) {
        T.delete_node(A[i]);
    }
    // stop = std::chrono::high_resolution_clock::now();
    // duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // std::cout << "Time taken by Deletion: "
    //     << duration.count() << " microseconds" << std::endl;

    T.print_tree(T.root, 0);

    std::cout << "Inorder walk is:\n";
    T.inorder(T.root);
    std::cout << std::endl;



    std::cout << "sorted array is:\n";
    print_array(A, true);
}