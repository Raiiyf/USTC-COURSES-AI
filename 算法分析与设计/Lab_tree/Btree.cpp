# include"tree.h"

void Btree::print_tree(BNode *x, int h) {
    if(x == nullptr) return;
    for(int i=1; i < x->num + 1; i++) {
        print_tree(x->children[i - 1], h+1);
        for(int i=0; i < h; i++) std::cout << "  ";
        std::cout << x->key[i] << '\n';
    }
    print_tree(x->children[x->num], h+1);
}

void Btree::inorder(BNode *x) {
    if(x == nullptr) return;
    for(int i=1; i < x->num + 1; i++) {
        inorder(x->children[i - 1]);
        std::cout << x->key[i] << ' ';
    }
    inorder(x->children[x->num]);
}

// search k at current B tree
// return its node and index
// if not find, k belongs to interval (node->key[index], node->key[index + 1])
// which means, node->key[index] is the last key less than k in array key.
Result Btree::search(int k) {
    Result res = {nullptr, -1, false};
    BNode *p = root;
    int i;
    while(p != nullptr) {
        // Set a sentiel at position 0
        p->key[0] = k;
        i = p->num;
        while(i>=0 && k < p->key[i]) 
            i--;
        res.node = p;
        res.index = i;
        if(i>0 && p->key[i] == k) {
            res.tag = true;
            break;
        }
        // children[i] stores the interval between key[i] and key[i + 1]
        p = p->children[i];
    }
    return res;
}

// Build a root node upon given left and right nodes
BNode *Makeroot(int k, BNode *left, BNode *right) {
    BNode *n = new BNode;
    n->num = 1;
    n->parent = nullptr;
    n->key[1] = k;
    n->children[0] = left;
    if(left) left->parent = n;
    n->children[1] = right;
    if(right) right->parent = n;
    return n;
}

// Insert an item k at key[pos + 1] with new child added at RHS.
// Which means keys in new added child are larger than k.
void Insert_item(BNode *ptr, int pos, int k, BNode *right) {
    for(int i=ptr->num; i>pos; i--) {
        ptr->key[i + 1] = ptr->key[i];
        ptr->children[i + 1] = ptr->children[i];
    }
    ptr->key[pos + 1] = k;
    ptr->children[pos + 1] = right;
    ptr->num++;
}

// Move keys from ptr->key[pos + 1] to rightmost to new node s->key
// ptr->key[pos + 1] (median) will be extract out for it is stored in s->key[0]
// Eg. ptr->num = 5 and pos = 2
// - Before: ptr->key = {sen, 1, 2, 3, 4, 5}; s->ptr = {}
// - After: ptr->key = {sen ,1, 2}; s->key = {3, 4, 5}
int Move_item(BNode *s, BNode *ptr, int pos) {
    for(int i = 0, j = pos + 1; j <= ptr->num; i++, j++) {
        s->key[i] = ptr->key[j];
        s->children[i] = ptr->children[j];
        if(s->children[i]) s->children[i]->parent = s;
    }
    s->num = MINSIZE;
    ptr->num = MINSIZE;
    s->parent = ptr->parent;
    return s->key[0];
}

// Splice the node into two part with both num is MINSIZE
// New node `s` residents at RHS
// Insert median key into its parent
BNode *Splice(BNode* ptr) {
    BNode *s = new BNode;
    // item is the median to be uploaded.
    int item = Move_item(s, ptr, MINSIZE);
    if(ptr->parent == nullptr)
        return Makeroot(item, ptr, s);

    BNode *pa = ptr->parent;
    int pos = pa->num;
    pa->key[0] = item;
    while(pos > 0 && item < pa->key[pos]) 
        pos--;
    Insert_item(pa, pos, item, s);

    if(pa->num > MAXSIZE)
        return Splice(pa);
    else return nullptr;
}

// Insert k into B tree
bool Btree::insert(int k) {
    if(root == nullptr) {
        root = Makeroot(k, nullptr, nullptr);
        num_keys = 1;
        return true;
    }

    BNode *ptr;
    int pos;
    BNode *p = root;
    int i;
    // Find the leaf node to insert
    while(p != nullptr) {
        // Set a sentiel at position 0
        p->key[0] = k;
        i = p->num;
        while(i>=0 && k < p->key[i]) 
            i--;
        ptr = p;
        pos = i;
        p = p->children[i];
    }

    // For there are one more slot in node->key
    // We can always insert a new key into it
    Insert_item(ptr, pos, k, nullptr);

    // node too full
    if(ptr->num > MAXSIZE) {
        BNode *newroot = Splice(ptr);
        if(newroot) root = newroot;
    }
    num_keys++;
    return true;
}


// Find the direct prev for item ptr->key[pos]
BNode *Findprev(BNode *ptr, int pos) {
    BNode *p = ptr->children[pos - 1];
    while(p && p->children[p->num]) 
        p = p->children[p->num];
    return p;
}

// Find the direct next for item ptr->key[pos]
BNode *Findnext(BNode *ptr, int pos) {
    BNode *p = ptr->children[pos];
    while(p && p->children[0]) 
        p = p->children[0];
    return p;
}

// Delete the key[pos] item in a LEAF node ptr
bool Delete_Leaf(BNode *ptr, int pos) {
    for(int i=pos; i < ptr->num; i++) {
        // For ptr is a leaf, no need to bother with children
        ptr->key[i] = ptr->key[i + 1];
    }
    ptr->num--;
    return true;
}

BNode *Merge(BNode *ptr) {
    BNode *par = ptr->parent;
    if(par == nullptr) {
        // ptr is root with no nodes
        if(ptr->num == 0) {
            BNode *newroot = ptr->children[0];
            newroot->parent = nullptr;
            delete ptr;
            return newroot;
        }
        else return nullptr;
    }
    
    int flag = 0;
    for (flag; flag <= par->num; flag++) 
        // find ptr's pos in its parent
        if(par->children[flag] == ptr) break;
    
    // If parent is not null, it will have at least two children
    if(flag < par->num) {
        flag++;
        // Right brother
        BNode *right = par->children[flag];
        if(right->num > MINSIZE) {
            // Resemble left rotation
            ptr->key[ptr->num + 1] = par->key[flag];
            ptr->children[ptr->num + 1] = right->children[0];
            if(right->children[0])
                right->children[0]->parent = ptr;
            par->key[flag] = right->key[1];
            // shrink right bro
            for(int j = 1; j <= right->num; j++) {
                right->key[j - 1] = right->key[j];
                right->children[j - 1] = right->children[j];
            }
            right->num--;
            ptr->num++;
            return nullptr;
        }
        flag--;
    }
    if(flag > 0) {
        // left brother
        BNode *left = par->children[flag - 1];
        if(left->num > MINSIZE) {
            for(int i = ptr->num; i >= 0; i--) {
                ptr->key[i + 1] = ptr->key[i];
                ptr->children[i + 1] = ptr->children[i];
            }
            ptr->key[1] = par->key[flag];
            ptr->children[0] = left->children[left->num];
            if(left->children[left->num])
                left->children[left->num]->parent = ptr;
            par->key[flag] = left->key[left->num];
            left->num--;
            ptr->num++;
            return nullptr;
        }
    }
    if(flag < par->num) {
        // Borrow from parent
        flag++;
        BNode *right = par->children[flag];
        ptr->key[ptr->num + 1] = par->key[flag];
        ptr->children[ptr->num + 1] = right->children[0];
        if(right->children[0])
            right->children[0]->parent = ptr;
        ptr->num++;

        for(int i=1, j=ptr->num; i <= right->num; i++) {
            ptr->key[j + i] = right->key[i];
            ptr->children[j + i] = right->children[i];
            if(right->children[i]) 
                right->children[i]->parent = ptr;
            ptr->num++;
        }
        delete right;

        for(int i=flag; i<par->num; i++) {
            par->key[i] = par->key[i + 1];
            par->children[i] = par->children[i + 1];
        }
        par->num--;
        if(par->num < MINSIZE) 
            return Merge(par);
    }
    else {
        // Shifting ptr left
        BNode *right = ptr;
        ptr = par->children[flag - 1];

        ptr->key[ptr->num + 1] = par->key[flag];
        ptr->children[ptr->num + 1] = right->children[0];
        if(right->children[0])
            right->children[0]->parent = ptr;
        ptr->num++;

        for(int i=1, j=ptr->num; i <= right->num; i++) {
            ptr->key[j + i] = right->key[i];
            ptr->children[j + i] = right->children[i];
            if(right->children[i]) 
                right->children[i]->parent = ptr;
            ptr->num++;
        }
        delete right;

        for(int i=flag; i<par->num; i++) {
            par->key[i] = par->key[i + 1];
            par->children[i] = par->children[i + 1];
        }
        par->num--;
        if(par->num < MINSIZE) 
            return Merge(par);
    }
    return nullptr;
}


bool Btree::delete_node(int k) {
    Result res = search(k);
    // Do not find item with key k
    if(res.node == nullptr || res.tag == false) return false;

    BNode *ptr = res.node;
    int pos = res.index;
    BNode *pre = Findprev(ptr, pos);
    BNode *nt = Findnext(ptr, pos);

    if(pre && pre->num > MINSIZE) {
        // Case 1
        // direct prev is the last item in pre->key
        ptr->key[pos] = pre->key[pre->num];
        // Make ptr & pos to be direct prev
        ptr = pre;
        pos = pre->num;
    }
    else if(nt && nt->num > MINSIZE) {
        // Case 2
        ptr->key[pos] = nt->key[1];
        ptr = nt;
        pos = 1;
    }
    else if(pre) {
        // Case 3
        ptr->key[pos] = pre->key[pre->num];
        ptr = pre;
        pos = pre->num;
    }
    Delete_Leaf(ptr, pos);
    if(ptr->parent == nullptr && ptr->num == 0) {
        delete ptr;
        root = nullptr;
    }
    else if(ptr->num < MINSIZE) {
        BNode *newroot = Merge(ptr);
        if(newroot) root = newroot;
    }
    num_keys--;
    return true;
}
