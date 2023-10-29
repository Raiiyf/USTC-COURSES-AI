#include <iostream>
#include <cstdlib>
#include <chrono>
#include <math.h>
#include <random>

#define MAXNUM n
#define MINNUM 1
#define PARENT(x) x>>1
#define LEFT(x) x<<1
#define RIGHT(x) (x<<1)+1
#define FETCHDIGIT(a, d) (a / d) % 10

class link
{
    public:
        int element;
        link* next;
        link* prev;
};


void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void insertion_sort(int A[], int n) {
    int key;
    for(int j=0; j < n; j++) {
        key = A[j];
        int i = j - 1;
        while(i >= 0 && A[i] > key) {
            A[i + 1] = A[i];
            i--;
        }
        A[i + 1] = key;
    }
}

void merge(int A[], int p, int q, int r) {
    // Merge operation
    // merge the subarray A[p...q] and A[q+1...r]
    int n_1 = q - p + 1;
    int n_2 = r - q;
    // int L[n_1 + 1] = {0};
    // int R[n_2 + 1] = {0};
    int *L = new int[n_1 + 1]();
    int *R = new int[n_2 + 1]();
    int i, j;
    // A[q] is placed in L
    for(i = 0; i < n_1; i++){
        L[i] = A[p + i];
    }
    for(j = 0; j < n_2; j++){
        R[j] = A[j + q + 1];
    }
    // Sentinel
    L[n_1] = 1e4;
    R[n_2] = 1e4;
    
    i = 0; j = 0;
    for(int k=p; k < r + 1; k++) {
        if(L[i] <= R[j]) {
            A[k] = L[i];
            i++;
        }
        else {
            A[k] = R[j];
            j++;
        }
    }
}

void merge_sort(int A[], int p, int r) {
    // Merge sort
    // Using merge sort to sort array A[p...r]
    if(p < r) {
        int q = (p + r) / 2;
        merge_sort(A, p, q);
        merge_sort(A, q + 1, r);
        merge(A, p, q, r);
    }
}

// Iterative version
void max_heapify(int A[], int i, int heapsize) {
    // In this algorithm, we array starts from 1 logically
    int largest;
    int l, r;
    while(i < heapsize) {
        l = LEFT(i);
        if(l <= heapsize && A[l - 1] > A[i - 1]) 
            largest = l;
        else
            largest = i;
        r = RIGHT(i);
        if(r <= heapsize && A[r - 1] > A[largest - 1])
            largest = r;

        if(largest != i) {
            swap(&A[i - 1], &A[largest - 1]);
            i = largest;
        }
        else return;
    }
}

// Recursive version
// void max_heapify(int A[], int i, int heapsize) {
//     // In this algorithm, we array starts from 1 logically
//     int largest;
//     int l, r;
//     l = LEFT(i);
//     if(l <= heapsize && A[l - 1] > A[i - 1]) 
//         largest = l;
//     else
//         largest = i;
//     r = RIGHT(i);
//     if(r <= heapsize && A[r - 1] > A[largest - 1])
//         largest = r;

//     if(largest != i) {
//         swap(&A[i - 1], &A[largest - 1]);
//         max_heapify(A, largest, heapsize);
//     }
// }

void build_max_heap(int A[], int heapsize) {
    for(int i=heapsize/2; i > 0; i--) {
        max_heapify(A, i, heapsize);
    }
}

void heapsort(int A[], int n) {
    build_max_heap(A, n);
    int temp;
    for(int i=n; i >= 2; i--) {
        swap(&A[1 - 1], &A[n - 1]);
        n--;
        max_heapify(A, 1, n);
    }
}

int partition(int A[], int p, int r) {
    int pivot = A[r];
    int i = p - 1;
    for(int j=p; j < r; j++) {
        if(A[j] <= pivot) {
            i++;
            if(i != j) swap(&A[i], &A[j]);
        }
    }
    swap(&A[i+1], &A[r]);
    return i+1;
}

void quicksort(int A[], int p, int r) {
    if(r > p) {
        int q = partition(A, p, r);
        quicksort(A, p, q - 1);
        quicksort(A, q + 1, r);
    }
}

void quicksort_opt(int A[], int p, int r) {
    while (p < r) {
        int q = partition(A, p, r);
        if (q - p < r - q) {
            // sort the former part
            quicksort_opt(A, p, q - 1);
            p = q + 1;
        }
        else {
            // sort the latter part
            quicksort_opt(A, q + 1, r);
            r = q - 1;
        }
    }
}

void counting_sort(int A[], int k, int n, int d) {
    // perform counting sort on array's (d + 1)th digit
    // change distribution to MIN - 1 to MAX - 1
    //int B[n] = {0};
    int *B = new int[n];
    int C[k] = {0};
    int i;
    d = int(pow(10,d));
    for(i=0; i < n; i++ ) {
        C[FETCHDIGIT((A[i] - 1), d)]++;
    }
    for(i=1; i < k; i++) {
        C[i] += C[i - 1];
    }
    for(i=n - 1; i >= 0; i--) {
        B[C[FETCHDIGIT((A[i] - 1), d)] - 1] = A[i];
        C[FETCHDIGIT((A[i] - 1), d)]--;
    }
    memcpy(A, B, n * sizeof(int));
}

void radix_sort(int A[], int d, int n) {
    for(int i=0; i < d; i++) {
        counting_sort(A, 10, n, i);
    }
}

// void bucket_sort(int A[], int n) {
//     int NUMBUCKET = n / 10;
//     int LENBUCKET = MAXNUM / NUMBUCKET;
//     int bucket[NUMBUCKET][10 * n / NUMBUCKET];
//     int count[NUMBUCKET] = {0};

//     int b;
//     for(int i=0; i < n; i++) {
//         // change distribution to MIN - 1 to MAX - 1
//         b = (A[i] - 1) / LENBUCKET;
//         bucket[b][count[b]] = A[i];
//         count[b]++;
//     }

//     for(int i=0; i < NUMBUCKET; i++) 
//         insertion_sort(bucket[i], count[i]);

//     int sum = 0;
//     for(int i=0; i < NUMBUCKET; i++) {
//         for(int j=0; j < count[i]; j++) {
//             A[sum + j] = bucket[i][j];
//         }
//         sum += count[i];
//     }
// }

void link_sort(link *A) {
    link *p = A->next;
    link *q, *r;
    for(;p;p = p->next) {
        for(q=p->prev; q->element > p->element; q=q->prev) {}
        if(q != p->prev) {
            if(p->next) {
                p->next->prev = p->prev;
            }
            p->prev->next = p->next;

            p->next = q->next;
            q->next->prev = p;
            q->next = p;
            p->prev = q;
        }

    }
}

void bucket_sort_linkver(int A[], int n) {
    int NUMBUCKET = n / 10;
    int LENBUCKET = MAXNUM / NUMBUCKET;
    link *bucket[NUMBUCKET];
   
    for(int i=0; i < NUMBUCKET; i++) {
        bucket[i] = new link;
        bucket[i]->element = -1;
        bucket[i]->next = nullptr;
        bucket[i]->prev = NULL;
    }

    int b;
    for(int i=0; i < n; i++) {
        // change distribution to MIN - 1 to MAX - 1
        b = (A[i] - 1) / LENBUCKET;
        link* p = new link;
        p->element = A[i];
        p->next = NULL;
        p->prev = NULL;
        link* q = bucket[b]->next;
        bucket[b]->next = p;
        p->prev = bucket[b];
        if(q) {
            p->next = q;
            q->prev = p;
        }
    }

    for(int i=0; i < NUMBUCKET; i++) {
        if(bucket[i]->next) {
            link_sort(bucket[i]);
        }
    }
    
    int sum=0;
    for(int i=0; i < NUMBUCKET; i++) {
        for(link* p=bucket[i]->next; p; p=p->next) {
            A[sum] = p->element;
            sum++;
        }
    }
}


void check_seq(int A[], int B[], int n) {
    for(int i=0; i < n; i++) {
        if (A[i] != B[i]) {
            std::cout << "\tInconsistance" << std::endl;
            return;
        }
    }
    std::cout << "\tConsistence" << std::endl;
}

void exp_1(int n=20, bool print_seq = false) {
    // Perform each sort algorithm and compare with insertion sort
    int Seq[n];

    // Generate random sequence
    std::uniform_int_distribution<int> u(MINNUM, MAXNUM); 
    std::default_random_engine e(0);
    
    for (int i = 0; i < n; i++) {
        Seq[i] = u(e);
        if (print_seq) std::cout << Seq[i] << " ";
    }
    if (print_seq) std::cout << std::endl;

    // Insertion sort
    std::cout << "Performing Insertion sort..." << std::endl;
    int Seq_insert[n];
    memcpy(Seq_insert, Seq, sizeof(Seq));
    auto start = std::chrono::high_resolution_clock::now();
    insertion_sort(Seq_insert, n);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken by Insertion sort: "
        << duration.count() << " microseconds" << std::endl;
    if (print_seq) {
        std::cout << "Results: ";
        for(int i = 0; i < n; ++i)
            std::cout << Seq_insert[i] << " ";
        std::cout << std::endl;
    }

    // Merge sort
    std::cout << "Performing Merge sort..." << std::endl;
    int Seq_merge[n];
    memcpy(Seq_merge, Seq, sizeof(Seq));
    // if (print_seq) {
    //     for (int i = 0; i < n; i++) {
    //         std::cout << Seq_merge[i] << " ";
    //     }
    //     std::cout << std::endl;
    // }
    start = std::chrono::high_resolution_clock::now();
    merge_sort(Seq_merge, 0, n - 1);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken by Merge sort: "
         << duration.count() << " microseconds" << std::endl;
    if (print_seq) {
        std::cout << "Results: ";
        for(int i = 0; i < n; ++i)
            std::cout << Seq_merge[i] << " ";
        std::cout << std::endl;
    }
    check_seq(Seq_insert, Seq_merge, n);


    // Quick sort
    std::cout << "Performing Quick sort..." << std::endl;
    int Seq_quick[n];
    memcpy(Seq_quick, Seq, sizeof(Seq));
    start = std::chrono::high_resolution_clock::now();
    quicksort_opt(Seq_quick, 0, n - 1);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken by Quick sort: "
         << duration.count() << " microseconds" << std::endl;
    if (print_seq) {
        std::cout << "Results: ";
        for(int i = 0; i < n; ++i)
            std::cout << Seq_quick[i] << " ";
        std::cout << std::endl;
    }
    check_seq(Seq_insert, Seq_quick, n);


    // Heap sort
    std::cout << "Performing Heap sort..." << std::endl;
    int Seq_heap[n];
    memcpy(Seq_heap, Seq, sizeof(Seq));
    start = std::chrono::high_resolution_clock::now();
    heapsort(Seq_heap, n);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken by Heap sort: "
         << duration.count() << " microseconds" << std::endl;
    if (print_seq) {
        std::cout << "Results: ";
        for(int i = 0; i < n; ++i)
            std::cout << Seq_heap[i] << " ";
        std::cout << std::endl;
    }
    check_seq(Seq_insert, Seq_heap, n);


    // Radix sort
    std::cout << "Performing Radix sort..." << std::endl;
    int Seq_radix[n];
    memcpy(Seq_radix, Seq, sizeof(Seq));
    start = std::chrono::high_resolution_clock::now();
    radix_sort(Seq_radix, 3, n);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken by Radix sort: "
         << duration.count() << " microseconds" << std::endl;
    if (print_seq) {
        std::cout << "Results: ";
        for(int i = 0; i < n; ++i)
            std::cout << Seq_radix[i] << " ";
        std::cout << std::endl;
    }
    check_seq(Seq_insert, Seq_radix, n);


    // Bucket sort
    std::cout << "Performing Bucket sort..." << std::endl;
    start = std::chrono::high_resolution_clock::now();
    bucket_sort_linkver(Seq, n);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    std::cout << "Time taken by Bucket sort: "
         << duration.count() << " microseconds" << std::endl;
    if (print_seq) {
        std::cout << "Results: ";
        for(int i = 0; i < n; ++i)
            std::cout << Seq[i] << " ";
        std::cout << std::endl;
    }
    check_seq(Seq_insert, Seq, n);
}

void exp_2() {
    // Perform each sort algorithm on a large n
    int n = 500000;
    int *Seq = new int[n];

    // Generate random sequence
    std::uniform_int_distribution<int> u(MINNUM, MAXNUM); 
    std::default_random_engine e(1);
    for (int i = 0; i < n; i++) {
        Seq[i] = u(e);
    }

    // Perform sort
    std::cout << "Performing sort..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    //insertion_sort(Seq, n);
    //merge_sort(Seq, 0, n - 1);
    //quicksort(Seq, 0, n - 1);
    //heapsort(Seq, n);
    //radix_sort(Seq, 6, n);
    bucket_sort_linkver(Seq, n);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken by sort: "
        << duration.count() << " milliseconds" << std::endl;
}

void exp_3(float degree) {
    int n = 100000;
    int *Seq_ordered = new int[n];
    for(int i=0; i < n; i++) {
        Seq_ordered[i] = i + 1;
    }

    std::default_random_engine e(2);
    int *Seq_spoiled = new int[n];
    memcpy(Seq_spoiled, Seq_ordered, n * sizeof(int));
    if(degree > 0)
        for(int i=0; i < n; i++) {
            std::uniform_int_distribution<int> u(i, n - 1); 
            swap(&Seq_spoiled[i], &Seq_spoiled[u(e)]);
        }
    if(degree < 0)
        for(int i=0; i < n / 2; i++) {
            swap(&Seq_spoiled[i], &Seq_spoiled[n - i - 1]);
        }

    int sum = 0;
    for(int i=0; i < n; i++) 
        if(Seq_ordered[i] == Seq_spoiled[i]) sum++;
    std::cout << "ordered ratio:" << float(sum) / n << std::endl;
    
    // Perform sort
    std::cout << "Performing sort..." << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    //insertion_sort(Seq_spoiled, n);
    //merge_sort(Seq_spoiled, 0, n - 1);
    //quicksort_opt(Seq_spoiled, 0, n - 1);
    heapsort(Seq_spoiled, n);
    //radix_sort(Seq_spoiled, 5, n);
    //bucket_sort_linkver(Seq_spoiled, n);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken by sort: "
        << duration.count() << " milliseconds" << std::endl;    
}

int main() {
    //exp_1(1000, false);
    exp_2();
    //exp_3(0);
    return 0;
}
