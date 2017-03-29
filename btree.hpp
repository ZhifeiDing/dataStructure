#ifndef _BPlusTree_HPP_
#define _BPlusTree_HPP_

#include <algorithm>
#include <cassert>
#include <vector>

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
    void erase(const _key& val);

    // return the size of BPlusTree
    const std::size_t size() const;

    // check whether the BPlusTree is nullptr
    const bool empty() const;

    // debug usage
    // output leaf items
    std::vector<std::pair<_key, _data> > dumpTree() const {
        std::vector<std::pair<_key, _data> > res;
        leafNode* n = head_;
        while( n ) {
            for(int i = 0; i < n->size_; ++i) {
                res.push_back(std::make_pair(n->key_[i], n->data_[i]));
            }
            n = n->next_;
        }
        return res;
    }

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

    // result flags of deletion
    enum result_flag {
        // deletion successful and no fix-ups necessary
        btree_ok = 0,
        // deletion not successful because key was not found
        btree_not_found = 1,
        // deletion successful, the last key was updated so parent need updates
        btree_update_lastkey = 2,
        // deletion successful, children nodes were merged and the parent
        // needs to remove the empty node
        btree_fixmerge = 4
    };

    // record the delet result status
    struct result_t {
        // result flag
        result_flag flags_;
        // the key to be updated at the parent's key
        _key lastKey_;
        // constructor of a result with specific flag, this can also
        // be used as for implicit conversion
        inline result_t(result_flag f = btree_ok) : flags_(f), lastKey_() {}
        inline result_t(result_flag f, const _key& k) : flags_(f), lastKey_(k) {}
        // test if result has a given flag set
        inline bool has(result_flag f) const {
            return (flags_ &f) != 0;
        }
        // operator overload
        inline result_t& operator|=(const result_t& other) {
            flags_ = result_flag(flags_ | other.flags_);
            if( other.has(btree_update_lastkey) )
                lastKey_ = other.lastKey_;
            return *this;
        }
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
        innerNode(unsigned int l) : node(l) {}

        // innernode only store keys and pointers to children
        _key key_[INNER_MAX_];
        node* child_[INNER_MAX_+1]; // child is one more than the key

        inline bool isFull() const {
            return node::size_ == INNER_MAX_;
        }
        inline bool isUnderflow() const {
            return node::size_ < INNER_MIN_;
        }
        inline bool isFew() const {
            return node::size_ >= INNER_MIN_;
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
        inline bool isFew() const {
            return node::size_ >= LEAF_MIN_;
        }
        inline void setData(unsigned int idx, std::pair<_key, _data> val) {
            assert( idx < LEAF_MAX_ );
            key_[idx] = val.first;
            key_[idx] = val.second;
        }
    };

    void split_innernode(innerNode* n, node* &splitNode, _key& splitKey, unsigned int idx);
    void split_leafnode(leafNode* n, node* &splitNode, _key& splitKey);

    bool insert(node* node_, const _key& key, const _data& data, node* &splitNode, _key& splitKey);

    result_t erase(const _key& val,node* n,node* left,node* right,innerNode* leftParent,innerNode* rightParent,innerNode* parent,unsigned int idxParent);
    result_t shift_left_leaf(leafNode *left, leafNode *right, innerNode *parent, unsigned int idxParent);
    void shift_left_inner(innerNode *left, innerNode *right, innerNode *parent, unsigned int idxParent);
    void shift_right_leaf(leafNode *left, leafNode *right, innerNode *parent, unsigned int idxParent);
    void shift_right_inner(innerNode *left, innerNode *right, innerNode *parent, unsigned int idxParent);
    result_t merge_inner(innerNode* left, innerNode* right, innerNode* parent, unsigned int idxParent);
    result_t merge_leaves(leafNode* left, leafNode* right, innerNode* parent);
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
        return std::make_pair(false, std::make_pair(_key(), _data()) );
    while( !n->isLeafNode() ) {
        innerNode* tmp = static_cast<innerNode*>(n);
        int idx = find(tmp, key);
        n = tmp->child_[idx];
    }

    leafNode* leaf = static_cast<leafNode*>(n);
    const unsigned int idx = find(leaf, key);
    if( idx < leaf->size_ && leaf->key_[idx] == key )
        return std::make_pair(true, std::make_pair(leaf->key_[idx], leaf->data_[idx]) );

    return std::make_pair(false, std::make_pair(_key(), _data()) );
}

// remove one element
template<typename _key, typename _data, int _M, int _L>
void BPlusTree<_key,_data,_M,_L>::erase(const _key& val) {
    if( !root_ )
        return;
    result_t res = erase(val, root_, nullptr, nullptr,nullptr,nullptr,nullptr, 0);
    if( !res.has(btree_not_found) )
        --stats_.itemCount_;
}

// descends down the tree for searching the key
// and remove it after found
template<typename _key, typename _data, int _M, int _L>
typename BPlusTree<_key,_data,_M,_L>::result_t BPlusTree<_key,_data,_M,_L>::erase(const _key& val,
                                        node* n,
                                        node* left,
                                        node* right,
                                        innerNode* leftParent,
                                        innerNode* rightParent,
                                        innerNode* parent,
                                        unsigned int idxParent
                                        ) {
    if( n->isLeafNode() ) {

        leafNode* leaf = static_cast<leafNode*>(n);
        leafNode* leftLeaf = static_cast<leafNode*>(left);
        leafNode* rightLeaf = static_cast<leafNode*>(right);

        int idx = find(leaf, val);

        if( idx >= leaf->size_ || leaf->key_[idx] != val )
            return btree_not_found;
        // if found , then erase the key
        std::copy(leaf->key_ + idx + 1, leaf->key_ + leaf->size_, leaf->key_ + idx);
        std::copy(leaf->data_ + idx + 1, leaf->data_ + leaf->size_, leaf->data_ + idx);
        --leaf->size_;

        result_t res = btree_ok;
        // if the key is the last one
        if( idx == leaf->size_ ) {
            if( parent && idxParent < parent->size_ )
                parent->key_[idxParent] = leaf->key_[leaf->size_ - 1];
            else {
                if( leaf->size_ >= 1 ) {
                    res |= result_t(btree_update_lastkey, leaf->key_[leaf->size_-1]);
                }
            }
        }

        if( leaf->isUnderflow() && !( leaf == root_ && leaf->size_ >= 1 ) ) {
            // case1 : if leaf == root, then reset to null
            if( leftLeaf == nullptr && rightLeaf == nullptr ) {
                root_ = nullptr;
                leaf = nullptr;
                head_ = tail_ = nullptr;
                return btree_ok;

            }
            // case2 : if both left and right leaves would underflow because of
            // shift, then merge them
            else if( (leftLeaf == nullptr || leftLeaf->isFew()) && (rightLeaf == nullptr || rightLeaf->isFew()) ) {
                if( leftParent == parent )
                    res |= merge_leaves(leftLeaf, leaf, parent);
                else
                    res |= merge_leaves(leaf, rightLeaf, parent);
            }
            // case3: the right leaf has extra data, balance it
            else if( (leftLeaf && leftLeaf->isFew()) && (rightLeaf && !rightLeaf->isFew()) ) {
                if( rightParent == parent )
                    res |= shift_left_leaf(leaf, rightLeaf, rightParent, idxParent);
                else
                    res |= merge_leaves(leftLeaf, leaf, leftParent);
            }
            // case4 : the left leaf has extra data, so balance left
            else if( (leftLeaf && !leftLeaf->isFew()) && (rightLeaf && rightLeaf->isFew()) ) {
                if( leftParent == parent )
                    shift_right_leaf(leftLeaf, leaf, leftParent, idxParent-1);
                else
                    res |= merge_leaves(leaf, rightLeaf, rightParent);
            }
            // case5 : both leaves have extra data, choose more data leaf
            else if( leftParent == rightParent ) {
                if( leftLeaf->size_ <= rightLeaf->size_ )
                    res |= shift_left_leaf(leaf, rightLeaf, rightParent, idxParent);
                else
                    shift_right_leaf(leftLeaf, leaf, leftParent, idxParent-1);
            } else {
                if( leftParent == parent )
                    shift_right_leaf(leftLeaf, leaf, leftParent, idxParent-1);
                else
                    res |= shift_left_leaf(leaf, rightLeaf, rightParent, idxParent);
            }

        }
        return res;

    } else {

        innerNode* inner = static_cast<innerNode*>(n);
        innerNode* leftInner = static_cast<innerNode*>(left);
        innerNode* rightInner = static_cast<innerNode*>(right);

        node* myleft, *myright;
        innerNode* myleftParent, *myrightParent;

        int idx = find(inner, val);
        if( idx == 0 ) {
            myleft = left ? (static_cast<innerNode*>(left))->child_[left->size_-1] : nullptr;
            myleftParent = leftParent;
        } else {
            myleft = inner->child_[idx-1];
            myleftParent = inner;
        }
        if( idx == inner->size_ ) {
            myright = right ? (static_cast<innerNode*>(right))->child_[0] : nullptr;
            myrightParent = rightParent;
        } else {
            myright = inner->child_[idx+1];
            myrightParent = inner;
        }

        result_t res = erase(val, inner->child_[idx], myleft, myright,
                         myleftParent, myrightParent, inner, idx);
        result_t myRes = btree_ok;
        if( res.has(btree_not_found) )
            return res;
        if( res.has(btree_update_lastkey) ) {
            if( parent && idxParent < parent->size_ )
                parent->key_[idxParent] = res.lastKey_;
            else
                myRes |= result_t(btree_update_lastkey, res.lastKey_);
        }

        if( res.has(btree_fixmerge) ) {
            // either current node or the next is empty and should be removed
            if( inner->child_[idx]->size_ != 0 )
                ++idx;
            std::copy(inner->key_+idx, inner->key_+inner->size_, inner->key_+idx-1);
            std::copy(inner->child_ +idx+1, inner->child_ +inner->size_+1, inner->child_+idx);
            --inner->size_;

            if( inner->level_ == 1 ) {
                --idx;
                leafNode* child = static_cast<leafNode*>(inner->child_[idx]);
                inner->key_[idx] = child->key_[child->size_-1];
            }
        }

        if( inner->isUnderflow() && !(inner == root_ && inner->size_ >= 1) ) {
            // case1 : the inner node is the root and has just one child, the
            // child becomes the new root
            if( leftInner == nullptr && rightInner == nullptr ) {
                root_ = inner->child_[0];
                inner->size_ = 0;
                return btree_ok;
            }
            // case2 : if both left and right leaves would underflow if shift,
            // then merge them
            else if( (!leftInner || leftInner->isFew()) && (!rightInner || rightInner->isFew()) )  {
                if( leftParent == parent )
                    myRes |= merge_inner(leftInner, inner, leftParent, idxParent-1);
                else
                    myRes |= merge_inner(inner, rightInner, rightParent, idxParent);
            }
            // case3 : the right leaf has extra data, so balance right with
            // current
            else if( (leftInner && leftInner->isFew()) && (rightInner && !rightInner->isFew()) ) {
                if( rightParent == parent )
                    shift_left_inner(inner, rightInner, rightParent, idxParent);
                else
                    myRes |= merge_inner(leftInner, inner, leftParent, idxParent-1);
            }
            // case4 : the left leaf has extra data, so balance left
            else if( (leftInner && !leftInner->isFew()) && (rightInner && rightInner->isFew()) ) {
                if( leftParent == parent )
                    shift_right_inner(leftInner, inner, leftParent, idxParent-1);
                else
                    myRes |= merge_inner(inner, rightInner, rightParent, idxParent);
            }
            // case5 : both the leaf and right leaves have extra data
            else if( leftParent == rightParent ) {
                if( leftInner->size_ <= rightInner->size_ )
                    shift_left_inner(inner, rightInner, rightParent, idxParent);
                else
                    shift_right_inner(leftInner, inner, leftParent, idxParent-1);
            } else {
                if( leftParent == parent )
                    shift_right_inner(leftInner, inner, leftParent, idxParent-1);
                else
                    shift_left_inner(inner, rightInner, rightParent, idxParent);
            }
        }
        return myRes;
    }
}

// merge two leaf nodes
template<typename _key, typename _data, int _M, int _L>
typename BPlusTree<_key,_data,_M,_L>::result_t BPlusTree<_key,_data,_M,_L>::merge_leaves(leafNode* left, leafNode* right, innerNode* parent) {

    std::copy(right->key_, right->key_ + right->size_,
              left->key_ + left->size_);
    std::copy(right->data_, right->data_ + right->size_,
              left->data_ + left->size_);

    left->size_ += right->size_;

    left->next_ = right->next_;
    if (left->next_)
         left->next_->prev_ = left;
    else
         tail_ = left;

    right->size_ = 0;

    return btree_fixmerge;
}

// Merge two inner nodes.
template<typename _key, typename _data, int _M, int _L>
typename BPlusTree<_key,_data,_M,_L>::result_t BPlusTree<_key,_data,_M,_L>::merge_inner(innerNode* left, innerNode* right, innerNode* parent, unsigned int idxParent) {
    // retrieve the decision key from parent
    left->key_[left->size_] = parent->key_[idxParent];
    ++left->size_;

    // copy over keys and children from right
    std::copy(right->key_, right->key_ + right->size_,
              left->key_ + left->size_);
    std::copy(right->child_, right->child_ + right->size_+1,
              left->child_ + left->size_);

    left->size_ += right->size_;
    right->size_ = 0;

    return btree_fixmerge;
}

/// Balance two leaf nodes. The function moves key/data pairs from right to
/// left so that both nodes are equally filled. The parent node is updated
/// if possible.
template<typename _key, typename _data, int _M, int _L>
typename BPlusTree<_key,_data,_M,_L>::result_t BPlusTree<_key,_data,_M,_L>::shift_left_leaf(leafNode *left, leafNode *right, innerNode *parent, unsigned int idxParent) {
    unsigned int shiftnum = (right->size_ - left->size_) >> 1;


    // copy the first items from the right node to the last slot in the left node.

    std::copy(right->key_, right->key_ + shiftnum,
              left->key_ + left->size_);
    std::copy(right->data_, right->data_ + shiftnum,
              left->data_ + left->size_);

    left->size_ += shiftnum;

    // shift all slots in the right node to the left

    std::copy(right->key_ + shiftnum, right->key_ + right->size_,
              right->key_);
    std::copy(right->data_ + shiftnum, right->data_ + right->size_,
              right->data_);

    right->size_ -= shiftnum;

    // fixup parent
    if (idxParent < parent->size_) {
        parent->key_[idxParent] = left->key_[left->size_ - 1];
        return btree_ok;
    }
    else { // the update is further up the tree
        return result_t(btree_update_lastkey, left->key_[left->size_ - 1]);
    }
}

/// Balance two inner nodes. The function moves key/data pairs from right
/// to left so that both nodes are equally filled. The parent node is
/// updated if possible.
template<typename _key, typename _data, int _M, int _L>
void BPlusTree<_key,_data,_M,_L>::shift_left_inner(innerNode *left, innerNode *right, innerNode *parent, unsigned int idxParent) {
    unsigned int shiftnum = (right->size_ - left->size_) >> 1;


    // copy the parent's decision key_ and child_ to the first new key on the left
    left->key_[left->size_] = parent->key_[idxParent];
    left->size_++;

    // copy the other items from the right node to the last slots in the left node.

    std::copy(right->key_, right->key_ + shiftnum-1,
              left->key_ + left->size_);
    std::copy(right->child_, right->child_ + shiftnum,
              left->child_ + left->size_);

    left->size_ += shiftnum - 1;

    // fixup parent
    parent->key_[idxParent] = right->key_[shiftnum - 1];

    // shift all slots in the right node

    std::copy(right->key_ + shiftnum, right->key_ + right->size_,
              right->key_);
    std::copy(right->child_ + shiftnum, right->child_ + right->size_+1,
              right->child_);

    right->size_ -= shiftnum;
}

/// Balance two leaf nodes. The function moves key/data pairs from left to
/// right so that both nodes are equally filled. The parent node is updated
/// if possible.
template<typename _key, typename _data, int _M, int _L>
void BPlusTree<_key,_data,_M,_L>::shift_right_leaf(leafNode *left, leafNode *right, innerNode *parent, unsigned int idxParent) {

    unsigned int shiftnum = (left->size_ - right->size_) >> 1;


    std::copy_backward(right->key_, right->key_ + right->size_,
                       right->key_ + right->size_ + shiftnum);
    std::copy_backward(right->data_, right->data_ + right->size_,
                       right->data_ + right->size_ + shiftnum);

    right->size_ += shiftnum;

    // copy the last items from the left node to the first slot in the right node.
    std::copy(left->key_ + left->size_ - shiftnum, left->key_ + left->size_,
              right->key_);
    std::copy(left->data_ + left->size_ - shiftnum, left->data_ + left->size_,
              right->data_);

    left->size_ -= shiftnum;

    parent->key_[idxParent] = left->key_[left->size_-1];
}

/// Balance two inner nodes. The function moves key/data pairs from left to
/// right so that both nodes are equally filled. The parent node is updated
/// if possible.
template<typename _key, typename _data, int _M, int _L>
void BPlusTree<_key,_data,_M,_L>::shift_right_inner(innerNode *left, innerNode *right, innerNode *parent, unsigned int idxParent) {

    unsigned int shiftnum = (left->size_ - right->size_) >> 1;


    std::copy_backward(right->key_, right->key_ + right->size_,
                       right->key_ + right->size_ + shiftnum);
    std::copy_backward(right->child_, right->child_ + right->size_+1,
                       right->child_ + right->size_+1 + shiftnum);

    right->size_ += shiftnum;

    // copy the parent's decision key_ and child_ to the last new key on the right
    right->key_[shiftnum - 1] = parent->key_[idxParent];

    // copy the remaining last items from the left node to the first slot in the right node.
    std::copy(left->key_ + left->size_ - shiftnum+1, left->key_ + left->size_,
              right->key_);
    std::copy(left->child_ + left->size_ - shiftnum+1, left->child_ + left->size_+1,
              right->child_);

    // copy the first to-be-removed key from the left node to the parent's decision slot
    parent->key_[idxParent] = left->key_[left->size_ - shiftnum];

    left->size_ -= shiftnum;
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
    bool res = insert(root_, key, data, newChild, newKey);

    // if newChild is not null, we must put the shiftup key into root
    if( newChild ) {
        innerNode* newRoot = newInner(root_->level_ + 1);

        newRoot->key_[0] = newKey;

        newRoot->child_[0] = root_;
        newRoot->child_[1] = newChild;

        newRoot->size_ = 1;
        root_ = newRoot;
    }

    // after insertion, update the item count
    if( res )
        ++stats_.itemCount_;
}

// insert helper function
// descent down to leaf and insert key/pair
// if the node overflows, then split the node and shiftup until root
template<typename _key, typename _data, int _M, int _L>
bool BPlusTree<_key,_data,_M,_L>::insert(node* node_, const _key& key,
    const _data& data, node* &splitNode, _key& splitKey) {

    if( node_->isLeafNode() ) {
        leafNode* n = static_cast<leafNode*>(node_);
        int idx = find(n, key);
        // if key has already existed, just return
        if( idx < n->size_ && n->key_[idx] == key )
            return false;
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

        return true;

    } else {
        innerNode* n = static_cast<innerNode*>(node_);

        int idx = find(n, key);
        // then descent down to child node
        node* newChild = nullptr;
        _key newKey;
        bool res = insert(n->child_[idx], key, data, newChild, newKey);

        // if newChild is not null, we must put the shiftup key into root
        if( newChild ) {
            if( n->isFull() ) {
                split_innernode(n, splitNode, splitKey, idx);
                // if the insrt key is in the splitNode
                if( idx >= n->size_ + 1 ) {
                    idx -= n->size_ + 1;
                    n = static_cast<innerNode*>(splitNode);
                } else if( idx == n->size_ + 1 && n->size_ < splitNode->size_ ) {
                    innerNode* tmp = static_cast<innerNode*>(splitNode);
                    // move the split key/child into the left node
                    n->key_[n->size_] = splitKey;
                    n->child_[n->size_+1] = tmp->child_[0];
                    ++n->size_;

                    tmp->child_[0] = newChild;
                    splitKey = newKey;

                    return res;

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
        return res;
    }
}

// splits a leaf node into two equal size leaves
template<typename _key, typename _data, int _M, int _L>
void BPlusTree<_key,_data,_M,_L>::split_leafnode(leafNode* n, node* &splitNode, _key& splitKey) {
    unsigned int m = n->size_ / 2;

    leafNode* leaf = newLeaf();
    leaf->size_ = n->size_ - m;

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
    splitKey = n->key_[n->size_-1];
    splitNode = leaf;
}

// splits a inner node into two equal size leaves
template<typename _key, typename _data, int _M, int _L>
void BPlusTree<_key,_data,_M,_L>::split_innernode(innerNode* n, node* &splitNode, _key& splitKey, unsigned int idx) {
    unsigned int m = n->size_ / 2;

    // TODO
    innerNode* inner = newInner(n->level_);

    inner->size_ = n->size_ - m - 1;
    std::copy(n->key_ + m + 1, n->key_ + n->size_, inner->key_);
    std::copy(n->child_ + m + 1, n->child_ + n->size_ + 1, inner->child_);

    n->size_ = m;

    splitKey = n->key_[m];
    splitNode = inner;
}

#endif