#ifndef _BPlusTree_HPP_
#define _BPlusTree_HPP_

#include <algorithm>
#include <cassert>

// BPlus Tree declaration
// the size of disk block
// default is 1024kb on linux
static const int BLOCK_SIZE = 1024;
// @param :
// _M : denote the maximum number key of innernode
// _L : denote the maximum number key of leafnode
template<typename _key,
         typename _data,
         // let innernode and leafnode contains allocated in a block default
         int _M = ( BLOCK_SIZE - sizeof(void*) ) / ( sizeof(_key) + sizeof(void*) ),
         int _L = BLOCK_SIZE / ( sizeof(_key) + sizeof(_data) )>
class BPlusTree {
public:
    // default constructor
    explicit BPlusTree();
    // no assignment constructor & copy constructor
    BPlusTree(const BPlusTree &other) = delete;
    BPlusTree(BPlusTree &&) = delete;
    const BPlusTree<_key,_data,_M,_L>& operator=(const BPlusTree &other) = delete;
    // destructor
    ~BPlusTree();

    // insert an element into BPlusTree
    // current we don't support identical key
    void insert(const _key &key, const _data &data);

    // find an element, if true return key/data pair
    // else return false pair
    std::pair<bool, std::pair<_key, _data> > find(const _key& key) const;

    // remove one element
    void erase(_key val);

    // return the size of BPlusTree
    const std::size_t size() const;

    // check whether the BPlusTree is nullptr
    const bool empty() const;

private:

    class node;
    class innerNode;
    class leafNode;

    /*
     * template find function to search the key which is not less
     * than specified key
     */
    template<typename nodeType>
    inline int find(nodeType *n, const _key& key) const {
        unsigned int i;
        for(i = 0; i < n->size_ && n->key_[i] < key; ++i);
        return i;
    }

    /*
     * const variables to hold the size limit of node
     */
    // the number of inner node which may be different with the
    // number of leaf node
    static const int INNER_MAX_ = _M;
    // the minimum number of inner node
    static const int INNER_MIN_ = INNER_MAX_ / 2;
    // the number of leaf node
    static const int LEAF_MAX_ = _L;
    // the minimum number of leaf node
    static const int LEAF_MIN_ = LEAF_MAX_ / 2;

    inline leafNode* newLeaf() {
        leafNode* n = new leafNode();
        ++stats_.leaves_;
        return n;
    }

    inline innerNode* newInner(unsigned int l) {
        innerNode* n = new innerNode(l);
        ++stats_.inners_;
        return n;
    }

    struct tree_stats {
        std::size_t itemCount_; // number of items in btree
        std::size_t leaves_;    // number of leaf nodes
        std::size_t inners_;    // number of inner nodes

        // default constructor
        tree_stats() : itemCount_(0), leaves_(0), inners_(0) {};
    };

    // the root of b+tree
    node *root_;
    // the head which points to the first leaf node
    leafNode* head_;
    // the head which points to the last leaf node
    leafNode* tail_;
    // record the statistical information of tree
    tree_stats stats_;

    // private insert helper function

    enum nodeType {leaf, inner};
    // TreeNode definition
    // base class of node
    class node {
    public:
        // no argument constructor
        node() : level_(0), size_(0) {};
        // constructor with one argument
        node(unsigned int l) : level_(l), size_(0) {};
        // record the current node's level, if 0, it's leaf node
        unsigned int level_;
        // record current node's size
        unsigned int size_;

        // method to check whether it's leaf or inner node
        inline bool isLeafNode() const {
            return level_ == 0;
        }

    };

    // innernode can only store the key and pointer to child node
    class innerNode : public node {
    public:
        // no argument constructor
        innerNode() {};
        // one argument constructor
        innerNode(unsigned int l) {
            node::node(l);
        }

        // innernode only store keys and pointers to children
        _key key_[INNER_MAX_];
        node* child_[INNER_MAX_+1]; // child is one more than the key

        inline bool isFull() const {
            return node::size_ == INNER_MAX_;
        }
        inline bool isUnderflow() const {
            return node::size_ < INNER_MIN_;
        }
    };

    // leaf node can store key/data pair
    class leafNode : public node {
    public:
        // no argument constructor
        leafNode() : prev_(nullptr), next_(nullptr) {};

        // pointer to previous leafnode
        leafNode* prev_;
        // pointer to next leafnode
        leafNode* next_;
        // leafnode must store keys and data
        _key key_[LEAF_MAX_];
        _data data_[LEAF_MAX_];

        inline bool isFull() const {
            return node::size_ == LEAF_MAX_;
        }
        inline bool isUnderflow() const {
            return node::size_ < LEAF_MIN_;
        }
        inline void setData(unsigned int idx, std::pair<_key, _data> val) {
            assert( idx < LEAF_MAX_ );
            key_[idx] = val.first;
            key_[idx] = val.second;
        }
    };

    void split_innernode(innerNode* n, node* &splitNode, _key& splitKey, unsigned int idx);
    void split_leafnode(leafNode* n, node* &splitNode, _key& splitKey);

    void insert(node* n, const _key& key, const _data& data, node* &splitNode, _key& splitKey);
};


// default constructor -- only initialize the private variable
template<typename _key, typename _data, int _M, int _L>
BPlusTree<_key,_data,_M,_L>::BPlusTree() {
    root_ = nullptr;
    head_ = nullptr;
    tail_ = nullptr;
}

// return the number of key/data pairs in BPlusTree
template<typename _key, typename _data, int _M, int _L>
const std::size_t BPlusTree<_key,_data,_M,_L>::size() const {
    return stats_.itemCount_;
}

// check whether the BPlusTree contains at least one key/data pair
template<typename _key, typename _data, int _M, int _L>
const bool BPlusTree<_key,_data,_M,_L>::empty() const {
    return size() == 0;
}

// destructor
template<typename _key, typename _data, int _M, int _L>
BPlusTree<_key,_data,_M,_L>::~BPlusTree() {
}


// find an element, if true return key/data pair
// else return false pair
template<typename _key, typename _data, int _M, int _L>
std::pair<bool, std::pair<_key, _data> >
BPlusTree<_key,_data,_M,_L>::find(const _key& key) const {
    node* n = root_;
    if( !n )
        return std::make_pair<false, std::make_pair<_key(), _data()> >;
    while( !n->isLeafNode() ) {
    }

    const unsigned int idx = find(n, key);
    if( idx < n->size() && n->key_[idx] == key )
        return std::make_pair<true, std::make_pair<n->key_[idx], n->data_[idx]> >;

    return std::make_pair<false, std::make_pair<_key(), _data()> >;
}

// remove one element
template<typename _key, typename _data, int _M, int _L>
void BPlusTree<_key,_data,_M,_L>::erase(_key val) {
}

// insert an element into BPlusTree
// current we don't support identical key
template<typename _key, typename _data, int _M, int _L>
void BPlusTree<_key,_data,_M,_L>::insert(const _key& key, const _data& data) {
    // if root_ is nullptr, first create the root node
    if( root_ == nullptr )
        root_ = head_ = tail_ = newLeaf();

    // now we can descent down from the root
    // if inner node has been split, we must handle if split propagates to root
    node* newChild = nullptr;
    _key newKey;
    insert(root_, key, data, newChild, newKey);

    // if newChild is not null, we must put the shiftup key into root
    if( newChild ) {
        innerNode* newRoot = newInner(root_.level_ + 1);

        newRoot.key_[0] = newKey;

        newRoot.child_[0] = root_;
        newRoot.child_[1] = newChild;

        newRoot.size = 1;
        root_ = newRoot;
    }

    // after insertion, update the item count
    ++stats_.itemCount_;
}

// insert helper function
// descent down to leaf and insert key/pair
// if the node overflows, then split the node and shiftup until root
template<typename _key, typename _data, int _M, int _L>
void BPlusTree<_key,_data,_M,_L>::insert(node* n, const _key& key,
    const _data& data, node* &splitNode, _key& splitKey) {

    if( n->isLeafNode() ) {
        n = static_cast<leafNode*>(n);
        int idx = find(n, key);
        // if key has already existed, just return
        if( idx < n->size && n->key_[idx] == key )
            return;
        if( n->isFull() ) {
            split_leafnode(n, splitNode, splitKey);

            // if the inserting key shold be in the new leaf node
            if( idx >= n->size_ ) {
                idx -= n->size_;
                n = static_cast<leafNode*>(splitNode);
            }
        }
        // insert key/data into node
        std::copy_backward(n->key_ + idx, n->key_ + n->size_,
                           n->key_ + n->size_ + 1);
        std::copy_backward(n->data_ + idx, n->data_ + n->size_,
                           n->data_ + n->size_ + 1);
        n->key_[idx] = key;
        n->data_[idx] = data;
        ++n->size_;

        // if the inserted key/data is the first of leafnode
        // we should update splitKey
        if( splitNode && n == splitNode && idx == 0 )
            splitKey = key;

    } else {
        n = static_cast<innerNode*>(n);

        int idx = find(n, key);
        // then descent down to child node
        node* newChild = nullptr;
        _key newKey;
        insert(n->child_[idx], key, data, newChild, newKey);

        // if newChild is not null, we must put the shiftup key into root
        if( newChild ) {
            if( n->isFull() ) {
                split_innernode(n, splitNode, splitKey, idx);
                if( idx >= n->size_ ) {
                    idx -= n->size_;
                    n = static_cast<innerNode*>(splitNode);
                } else if( idx == n->size_ && n->size_ < splitNode->size_ ) {
                    splitNode = static_cast<innerNode*>(splitNode);
                    // move the split key/child into the left node
                    n->key_[n->size_] = splitKey;
                    n->child_[n->size_+1] = splitNode->child_[0];
                    ++n->size_;

                    splitNode->child_[0] = newChild;
                    splitKey = newKey;

                }
            }

            std::copy_backward(n->key_ + idx, n->key_ + n->size_,
                               n->key_ + n->size_ + 1);
            std::copy_backward(n->child_ + idx, n->child_ + n->size_ + 1,
                               n->child_ + n->size_ + 2);

            n->key_[idx] = newKey;
            n->child_[idx+1] = newChild;
            ++n->size_;
        }
    }
}

// splits a leaf node into two equal size leaves
template<typename _key, typename _data, int _M, int _L>
void BPlusTree<_key,_data,_M,_L>::split_leafnode(leafNode* n, node* &splitNode, _key& splitKey) {
    unsigned int m = n->size / 2;

    leafNode* leaf = newLeaf();
    leaf->size = n->size_ - m;

    leaf->next_ = n->next_;

    if( leaf->next_ == nullptr )
        tail_ = leaf;
    else
        leaf->next_->prev_ = leaf;

    // move the half key/data to new leaf node
    std::copy(n->key_ + m, n->key_ + n->size_, leaf->key_);
    std::copy(n->data_ + m, n->data_ + n->size_, leaf->data_);

    // set the original leafnode chain to new leaf node
    n->size_ = m;
    n->next_ = leaf;
    leaf->prev_ = n;

    // shift up the key to parent
    splitKey = leaf[0];
    splitNode = leaf;
}

// splits a inner node into two equal size leaves
template<typename _key, typename _data, int _M, int _L>
void BPlusTree<_key,_data,_M,_L>::split_innernode(innerNode* n, node* &splitNode, _key& splitKey, unsigned int idx) {
    unsigned int m = n->size_ / 2;

    // TODO
    innerNode* inner = newInner(n->level_);

    inner->size_ = n->size_ - m;
    std::copy(n->key_ + m, n->key_ + n->size_, inner->key_);
    std::copy(n->child_ + m, n->child_ + n->size_, inner->child_);

    n->size_ = m;

    splitKey = inner->key_[0];
    splitNode = inner;
}

#endif