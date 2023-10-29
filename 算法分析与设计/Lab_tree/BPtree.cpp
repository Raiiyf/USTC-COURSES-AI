#include "tree.h"

void BPtree::search(int x) {
  if (root == NULL) {
    std::cout << "Tree is empty\n";
  } else {
    BPNode *cursor = root;
    while (cursor->IS_LEAF == false) {
      for (int i = 0; i < cursor->num; i++) {
        if (x < cursor->key[i]) {
          cursor = cursor->ptr[i];
          break;
        }
        // x is larger than all the keys
        if (i == cursor->num - 1) {
          cursor = cursor->ptr[i + 1];
          break;
        }
      }
    }
    for (int i = 0; i < cursor->num; i++) {
      if (cursor->key[i] == x) {
        // std::cout << "Found\n";
        return;
      }
    }
    std::cout << "Not found\n";
  }
}

void BPtree::insert(int x) {
  if (root == nullptr) {
    root = new BPNode;
    root->key[0] = x;
    root->IS_LEAF = true;
    root->num = 1;
  } 
  else {
    BPNode *cursor = root;
    BPNode *parent;
    // Find the leaf to insert
    while (cursor->IS_LEAF == false) {
      parent = cursor;
      for (int i = 0; i < cursor->num; i++) {
        if (x <= cursor->key[i]) { //
          cursor = cursor->ptr[i];
          break;
        }
        if (i == cursor->num - 1) {
          cursor = cursor->ptr[i + 1];
          break;
        }
      }
    }
    if (cursor->num < MAXSIZE) {
      // Current leaf has room for a new item
      int i = 0;
      while (x > cursor->key[i] && i < cursor->num)
        i++;
      for (int j = cursor->num; j > i; j--) {
        cursor->key[j] = cursor->key[j - 1];
      }
      cursor->key[i] = x;
      cursor->num++;
      cursor->ptr[cursor->num] = cursor->ptr[cursor->num - 1]; // Set new leaf ointer
      cursor->ptr[cursor->num - 1] = nullptr; // Delete the previous leaf pointer
    } 
    else {
      // Current leaf has no room for a new item
      BPNode *newLeaf = new BPNode;
      int virtualNode[MAXSIZE + 1];
      // copy current keys into virtual node
      for (int i = 0; i < MAXSIZE; i++) {
        virtualNode[i] = cursor->key[i];
      }

      int i = 0, j;
      // insert x into virtual node
      while (x > virtualNode[i] && i < MAXSIZE)
        i++;
      for (int j = MAXSIZE; j > i; j--) {
        virtualNode[j] = virtualNode[j - 1];
      }
      virtualNode[i] = x;

      // Maintain leaf attributes
      newLeaf->IS_LEAF = true;
      cursor->num = (MAXSIZE + 1) / 2;
      newLeaf->num = MAXSIZE + 1 - (MAXSIZE + 1) / 2;
      cursor->ptr[cursor->num] = newLeaf;
      newLeaf->ptr[newLeaf->num] = cursor->ptr[MAXSIZE];
      cursor->ptr[MAXSIZE] = nullptr;

      // Distribute keys into two leaves 
      // Keys in newleaf is larger
      for (i = 0; i < cursor->num; i++) {
        cursor->key[i] = virtualNode[i];
      }
      for (i = 0, j = cursor->num; i < newLeaf->num; i++, j++) {
        newLeaf->key[i] = virtualNode[j];
      }

      // Add a new root node to link two leaves
      if (cursor == root) {
        BPNode *newRoot = new BPNode;
        newRoot->key[0] = newLeaf->key[0]; // The median
        newRoot->ptr[0] = cursor;
        newRoot->ptr[1] = newLeaf;
        newRoot->IS_LEAF = false;
        newRoot->num = 1;
        root = newRoot;
      } 
      else {
        // Insert a new key into parent
        insertInternal(newLeaf->key[0], parent, newLeaf);
      }
    }
  }
}

// Insert a new key into an internal node cursor with right child.
void BPtree::insertInternal(int x, BPNode *cursor, BPNode *child) {
  if (cursor->num < MAXSIZE) {
    // Current node has room
    int i = 0;
    while (x > cursor->key[i] && i < cursor->num) 
      i++;

    // Shifting array from key[i] and ptr[i + 1] rightside
    for (int j = cursor->num; j > i; j--) {
      cursor->key[j] = cursor->key[j - 1];
    }
    for (int j = cursor->num + 1; j > i + 1; j--) {
      cursor->ptr[j] = cursor->ptr[j - 1];
    }
    cursor->key[i] = x;
    cursor->num++;
    cursor->ptr[i + 1] = child;
  } 
  else {
    // Current node has no room
    BPNode *newInternal = new BPNode;
    // Insert into a virutal node and then split into two part
    int virtualKey[MAXSIZE + 1];
    BPNode *virtualPtr[MAXSIZE + 2];
    // Copy cursor
    for (int i = 0; i < MAXSIZE; i++) {
      virtualKey[i] = cursor->key[i];
    }
    for (int i = 0; i < MAXSIZE + 1; i++) {
      virtualPtr[i] = cursor->ptr[i];
    }
    // Find pos to insert
    int i = 0, j;
    while (x > virtualKey[i] && i < MAXSIZE) 
      i++;
    for (int j = MAXSIZE; j > i; j--) {
      virtualKey[j] = virtualKey[j - 1];
    }
    virtualKey[i] = x;
    for (int j = MAXSIZE + 1; j > i + 1; j--) {
      virtualPtr[j] = virtualPtr[j - 1];
    }
    virtualPtr[i + 1] = child;
    // Spliting virtual node
    newInternal->IS_LEAF = false;
    cursor->num = (MAXSIZE + 1) / 2;
    newInternal->num = MAXSIZE - (MAXSIZE + 1) / 2;
    for (i = 0; i < cursor->num; i++) {
      cursor->key[i] = virtualKey[i];
    }
    for (i = 0; i < cursor->num + 1; i++) {
      cursor->ptr[i] = virtualPtr[i];
    }
    for (i = 0, j = cursor->num + 1; i < newInternal->num; i++, j++) {
      newInternal->key[i] = virtualKey[j];
    }
    for (i = 0, j = cursor->num + 1; i < newInternal->num + 1; i++, j++) {
      newInternal->ptr[i] = virtualPtr[j];
    }

    if (cursor == root) {
      BPNode *newRoot = new BPNode;
      newRoot->key[0] = virtualKey[cursor->num];
      newRoot->ptr[0] = cursor;
      newRoot->ptr[1] = newInternal;
      newRoot->IS_LEAF = false;
      newRoot->num = 1;
      root = newRoot;
    } else {
      insertInternal(virtualKey[cursor->num], findParent(root, cursor), newInternal);
    }
  }
}

// Find parent of child in tree rooted by cursor
BPNode *BPtree::findParent(BPNode *cursor, BPNode *child) {
  BPNode *parent;
  if (cursor->IS_LEAF) { //|| (cursor->ptr[0])->IS_LEAF
    return nullptr;
  }
  for (int i = 0; i < cursor->num + 1; i++) {
    if (cursor->ptr[i] == child) {
      parent = cursor;
      return parent;
    } 
    else {
      parent = findParent(cursor->ptr[i], child);
      if (parent != nullptr)
        return parent;
    }
  }
  return parent;
}

void BPtree::delete_keys(BPNode *cursor, int key, int new_key) {
  BPNode *parent = findParent(root, cursor);
  if(parent == nullptr) 
    return;

  for(int i=parent->num - 1; i >= 0; i--) {
    if(parent->key[i] == key) {
      parent->key[i] = new_key;
      return;
    }
  }
  delete_keys(parent, key, new_key);
}

void BPtree::delete_node(int x) {
  if (root == nullptr) {
    std::cout << "Tree empty\n";
  } else {
    BPNode *cursor = root;
    BPNode *parent;
    int leftSibling, rightSibling;
    while (cursor->IS_LEAF == false) {
      for (int i = 0; i < cursor->num; i++) {
        parent = cursor;
        leftSibling = i - 1;
        rightSibling = i + 1;
        if (x < cursor->key[i]) {
          cursor = cursor->ptr[i];
          break;
        }
        if (i == cursor->num - 1) {
          leftSibling = i;
          rightSibling = i + 2;
          cursor = cursor->ptr[i + 1];
          break;
        }
      }
    }
    bool found = false;
    int pos;
    for (pos = 0; pos < cursor->num; pos++) {
      if (cursor->key[pos] == x) {
        found = true;
        break;
      }
    }
    if (!found) {
      // std::cout << x << " Not found\n";
      return;
    }
    for (int i = pos; i < cursor->num; i++) {
      // Overwrite the key to be deleted
      cursor->key[i] = cursor->key[i + 1];
    }

    if(pos == 0) delete_keys(cursor, x, cursor->key[pos]);

    cursor->num--;
    if (cursor == root) {
      for (int i = 0; i < MAXSIZE + 1; i++) {
        cursor->ptr[i] = nullptr;
      }
      if (cursor->num == 0) {
        std::cout << "Tree died\n";
        // delete[] cursor->key;
        // delete[] cursor->ptr;
        delete cursor;
        root = nullptr;
      }
      return;
    }
    cursor->ptr[cursor->num] = cursor->ptr[cursor->num + 1];
    cursor->ptr[cursor->num + 1] = nullptr;
    
    if (cursor->num >= MINSIZE) 
      return;
    
    if (leftSibling >= 0) {
      BPNode *leftNode = parent->ptr[leftSibling];
      if (leftNode->num >= MINSIZE + 1) {
        // Borrow from left brother
        for (int i = cursor->num; i > 0; i--) {
          cursor->key[i] = cursor->key[i - 1];
        }
        cursor->num++;
        cursor->ptr[cursor->num] = cursor->ptr[cursor->num - 1];
        cursor->ptr[cursor->num - 1] = nullptr;
        cursor->key[0] = leftNode->key[leftNode->num - 1];
        leftNode->num--;
        leftNode->ptr[leftNode->num] = cursor;
        leftNode->ptr[leftNode->num + 1] = nullptr;
        // Setting new key
        parent->key[leftSibling] = cursor->key[0];
        return;
      }
    }
    if (rightSibling <= parent->num) {
      BPNode *rightNode = parent->ptr[rightSibling];
      if (rightNode->num >= MINSIZE + 1) {
        // Borrow from right brother
        cursor->num++;
        cursor->ptr[cursor->num] = cursor->ptr[cursor->num - 1];
        cursor->ptr[cursor->num - 1] = nullptr;
        cursor->key[cursor->num - 1] = rightNode->key[0];
        rightNode->num--;
        rightNode->ptr[rightNode->num] = rightNode->ptr[rightNode->num + 1];
        rightNode->ptr[rightNode->num + 1] = nullptr;
        for (int i = 0; i < rightNode->num; i++) {
          rightNode->key[i] = rightNode->key[i + 1];
        }
        parent->key[rightSibling - 1] = rightNode->key[0];
        return;
      }
    }
    if (leftSibling >= 0) {
      BPNode *leftNode = parent->ptr[leftSibling];
      // Moving item to leftnode
      for (int i = leftNode->num, j = 0; j < cursor->num; i++, j++) {
        leftNode->key[i] = cursor->key[j];
      }
      leftNode->ptr[leftNode->num] = nullptr;
      leftNode->num += cursor->num;
      leftNode->ptr[leftNode->num] = cursor->ptr[cursor->num];
      // std::cout << "Merging two leaf nodes\n";

      removeInternal(parent->key[leftSibling], parent, cursor);
      delete cursor;
    } else if (rightSibling <= parent->num) {
      BPNode *rightNode = parent->ptr[rightSibling];
      // Moving item to cursor
      for (int i = cursor->num, j = 0; j < rightNode->num; i++, j++) {
        cursor->key[i] = rightNode->key[j];
      }
      cursor->ptr[cursor->num] = nullptr;
      cursor->num += rightNode->num;
      cursor->ptr[cursor->num] = rightNode->ptr[rightNode->num];
      // std::cout << "Merging two leaf nodes\n";

      removeInternal(parent->key[rightSibling - 1], parent, rightNode);
      delete rightNode;
    }
  }
}

// delete a key x and ptr child at internal node cursor
void BPtree::removeInternal(int x, BPNode *cursor, BPNode *child) {
  if (cursor == root) {
    if (cursor->num == 1) {
      if (cursor->ptr[1] == child) {
        // delete child;
        root = cursor->ptr[0];
        delete cursor;
        // std::cout << "Changed root node\n";
        return;
      } else if (cursor->ptr[0] == child) {
        // delete child;
        root = cursor->ptr[1];
        delete cursor;
        // std::cout << "Changed root node\n";
        return;
      }
    }
  }
  int pos;
  // Overwrite key
  for (pos = 0; pos < cursor->num; pos++) {
    if (cursor->key[pos] == x) {
      break;
    }
  }
  for (int i = pos; i < cursor->num; i++) {
    cursor->key[i] = cursor->key[i + 1];
  }
  // Overwrite child
  for (pos = 0; pos < cursor->num + 1; pos++) {
    if (cursor->ptr[pos] == child) {
      break;
    }
  }
  for (int i = pos; i < cursor->num + 1; i++) {
    cursor->ptr[i] = cursor->ptr[i + 1];
  }

  cursor->num--;
  if (cursor->num >= MINSIZE) {
    // for its a internal node, we don't need to maintain keys at other node
    return;
  }
  if (cursor == root)
    return;
  
  BPNode *parent = findParent(root, cursor);
  int leftSibling, rightSibling;
  for (pos = 0; pos < parent->num + 1; pos++) {
    if (parent->ptr[pos] == cursor) {
      leftSibling = pos - 1;
      rightSibling = pos + 1;
      break;
    }
  }
  if (leftSibling >= 0) {
    BPNode *leftNode = parent->ptr[leftSibling];
    if (leftNode->num > MINSIZE) {
      // borrow from left brother
      for (int i = cursor->num; i > 0; i--) {
        cursor->key[i] = cursor->key[i - 1];
      }
      // Resemble right rotation
      cursor->key[0] = parent->key[leftSibling];
      parent->key[leftSibling] = leftNode->key[leftNode->num - 1]; 
      for (int i = cursor->num + 1; i > 0; i--) {
        cursor->ptr[i] = cursor->ptr[i - 1];
      }
      cursor->ptr[0] = leftNode->ptr[leftNode->num];
      cursor->num++;
      leftNode->num--;
      return;
    }
  }
  if (rightSibling <= parent->num) {
    BPNode *rightNode = parent->ptr[rightSibling];
    if (rightNode->num > MINSIZE) {
      // borrow from right brother
      cursor->key[cursor->num] = parent->key[pos];
      parent->key[pos] = rightNode->key[0];
      for (int i = 0; i < rightNode->num - 1; i++) {
        rightNode->key[i] = rightNode->key[i + 1];
      }
      cursor->ptr[cursor->num + 1] = rightNode->ptr[0];
      for (int i = 0; i < rightNode->num; ++i) {
        rightNode->ptr[i] = rightNode->ptr[i + 1];
      }
      cursor->num++;
      rightNode->num--;
      return;
    }
  }
  if (leftSibling >= 0) {
    BPNode *leftNode = parent->ptr[leftSibling];
    // Merging into left brother
    leftNode->key[leftNode->num] = parent->key[leftSibling];
    for (int i = leftNode->num + 1, j = 0; j < cursor->num; j++) {
      leftNode->key[i + j] = cursor->key[j];
    }
    for (int i = leftNode->num + 1, j = 0; j < cursor->num + 1; j++) {
      leftNode->ptr[i + j] = cursor->ptr[j];
      cursor->ptr[j] = nullptr;
    }
    leftNode->num += cursor->num + 1;
    cursor->num = 0;
    removeInternal(parent->key[leftSibling], parent, cursor);
  } 
  else if (rightSibling <= parent->num) {
    BPNode *rightNode = parent->ptr[rightSibling];
    // Merging into cursor
    cursor->key[cursor->num] = parent->key[rightSibling - 1];
    for (int i = cursor->num + 1, j = 0; j < rightNode->num; j++) {
      cursor->key[i + j] = rightNode->key[j];
    }
    for (int i = cursor->num + 1, j = 0; j < rightNode->num + 1; j++) {
      cursor->ptr[i + j] = rightNode->ptr[j];
      rightNode->ptr[j] = nullptr;
    }
    cursor->num += rightNode->num + 1;
    rightNode->num = 0;
    removeInternal(parent->key[rightSibling - 1], parent, rightNode);
  }
}

void BPtree::print_tree(BPNode *x, int h) {
    if(x == nullptr) return;
    for(int i=0; i < x->num; i++) {
        if(!x->IS_LEAF)
            print_tree(x->ptr[i], h+1);
        for(int i=0; i < h; i++) std::cout << "  ";
        std::cout << x->key[i] << '\n';
    }
    if(!x->IS_LEAF)
        print_tree(x->ptr[x->num], h+1);
}

void BPtree::inorder(BPNode *x) {
    if(x == nullptr) return;
    for(int i=0; i < x->num; i++) {
        if(!x->IS_LEAF)
            inorder(x->ptr[i]);
        if(x->IS_LEAF)
          std::cout << x->key[i] << ' ';
    }
    if(!x->IS_LEAF)
        inorder(x->ptr[x->num]);
}

