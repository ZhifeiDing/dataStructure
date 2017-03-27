#ifndef _BPlusTree_HPP_
#define _BPlusTree_HPP_


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
        int i;
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
            node(l);
        }

        // innernode only store keys and pointers to children
        _key key_[INNER_MAX_];
        node* child_[INNER_MAX_+1]; // child is one more than the key

        inline bool isFull() const {
            return size_ == INNER_MAX_;
        }
        inline bool isUnderflow() const {
            return size_ < INNER_MIN_;
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
        _key key[LEAF_MAX_];
        _data data[LEAF_MAX_];

        inline bool isFull() const {
            return size_ == LEAF_MAX_;
        }
        inline bool isUnderflow() const {
            return size_ < LEAF_MIN_;
        }
        inline void setData(unsigned int idx, std::pair<_key, _data> val) {
            assert( idx < LEAF_MAX_ );
            key[idx] = val.first;
            key[idx] = val.second;
        }
    };
};


// default constructor -- only initialize the private variable
template<typename _key, typename _data, int _M, int _L>
BPlusTree<_key,_data,_M,_L>::BPlusTree() {
    root_ = nullptr;
    prev_ = nullptr;
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

    int idx = find(n, key);
    if( idx < n->size() && n->key_[idx] == key )
        return std::make_pair<true, std::make_pair<n->key_[idx], n->data_[idx]> >;

    return std::make_pair<false, std::make_pair<_key(), _data()> >;
}

// remove one element
template<typename _key, typename _data, int _M, int _L>
void BPlusTree<_key,_data,_M,_L>::erase(iterator itr) {
}

// insert an element into BPlusTree
// current we don't support identical key
template<typename _key, typename _data, int _M, int _L>
void BPlusTree<_key,_data,_M,_L>::insert(const _key& key, const _data& data) {
}


#endif