#ifndef _RBT_HPP_
#define _RBT_HPP_

//#include <iterator>
#include <queue>


// Binary search Tree declaration
template<typename T>
class RBT {
public:
    // default constructor
    explicit RBT();
    // copy constructor
    RBT(const RBT &other);
    // assignment constructor
    const RBT<T>& operator=(const RBT &other);
    // destructor
    ~RBT();

    enum Color { RED, BLACK };
    // TreeNode definition
    class TreeNode {
    public:
        friend class RBT;
        T val;
        TreeNode *left;
        TreeNode *right;
        TreeNode *parent;
        // indicate this node's color
        Color color;
        // constructor
        TreeNode() : left(nullptr), right(nullptr), parent(nullptr), color(BLACK){};
        TreeNode(T v) : val(v), left(nullptr), right(nullptr), parent(nullptr), color(RED){};
        TreeNode(T v, TreeNode *l, TreeNode *r, TreeNode *p = nullptr, Color c = RED) : val(v), left(l), right(r),parent(p), color(c){};
    };


    // STL-style iterator
    class iterator {
    public:
        friend class RBT;
        explicit iterator();
        const iterator& operator=(const iterator &other); // assignment constructor
        iterator& operator++(); // prefix increment
        iterator operator++(int); // postfix increment
        T& operator*() const; // derefence the pointer
        bool operator!=(const RBT<T>::iterator &other) const;
        bool operator==(const RBT<T>::iterator &other) const;
        // for iterator_traits to refer
        typedef std::output_iterator_tag iterator_category;
        typedef T value_type;
        typedef std::ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T& reference;
    private:
        iterator(TreeNode* n);
        TreeNode* node;
        TreeNode* lastNode;
    };

    // iterator begin() and end()
    iterator begin() const;
    iterator end() const;

    // insert an element into RBT
    void insert(T v);

    // find an element
    iterator find(const T& v) const;
    //const_iterator find(const T& v) const;

    // remove one element
    void erase(iterator itr);

    // return the size of RBT
    const std::size_t size() const;

    // check whether the RBT is nullptr
    const bool empty() const;

private:

    int cnt;
    TreeNode *root;

    // private insert helper function
    void insert(T v, TreeNode *&r, TreeNode * const &p = nullptr);
    void insert_case1(TreeNode *r);
    void insert_case2(TreeNode *r);
    void insert_case3(TreeNode *r);
    void insert_case4(TreeNode *r);
    void insert_case5(TreeNode *r);
    TreeNode* grandParent(TreeNode * const &r) const;
    TreeNode* uncle( TreeNode * const &r) const;
    TreeNode* sibling( TreeNode * const &r) const;
    // private : replace node in parent
    void replace_node_in_parent(TreeNode *node, TreeNode *newNode = nullptr);

    // delete helper functions
    void delete_case1(TreeNode *node);
    void delete_case2(TreeNode *node);
    void delete_case3(TreeNode *node);
    void delete_case4(TreeNode *node);
    void delete_case5(TreeNode *node);
    void delete_case6(TreeNode *node);

    // rotate the node
    void rotate_right(TreeNode *r);
    void rotate_left(TreeNode *r);
    // private : find the max value node
    TreeNode *findMax(TreeNode *node);
};

// RBT<T>::iterator
// default constructor
template<typename T>
RBT<T>::iterator::iterator() {
    node = nullptr;
    lastNode = nullptr;
}

// one argument constructor
template<typename T>
RBT<T>::iterator::iterator(TreeNode* n) {
    node = n;
    lastNode = nullptr;
}

// assignment constructor
template<typename T>
const typename RBT<T>::iterator& RBT<T>::iterator::operator=(const iterator &other) {
    this->node = other.node;
    return *this;
}

template<typename T>
T& RBT<T>::iterator::operator*() const {
    return this->node->val;
}

// overload operator ==
template<typename T>
bool RBT<T>::iterator::operator==(const RBT<T>::iterator &other) const {
    return this->node == other.node;
}

// overload operator !=
template<typename T>
bool RBT<T>::iterator::operator!=(const RBT<T>::iterator &other ) const {
    return this->node != other.node;
}

// overload prefix ++
template<typename T>
typename RBT<T>::iterator& RBT<T>::iterator::operator++() {
    // if current node is root , we could get the lastNode
    if( node->parent == nullptr ) {
        lastNode = node;
        while( lastNode->right )
            lastNode = lastNode->right;
    }
    if( node->right != nullptr ) {
        node = node->right;
        while( node->left )
            node = node->left;
    } else if( node == lastNode ){
        node = nullptr;
    } else if( node == node->parent->left ) {
        node = node->parent;
    } else if( node == node->parent->right ) {
        while( node->parent->val <= node->val )
            node = node->parent;
        node = node->parent;
    }
    return *this;
}

// overload postfix ++
template<typename T>
typename RBT<T>::iterator RBT<T>::iterator::operator++(int) {
    // if current node is root , we could get the lastNode
    if( node->parent == nullptr ) {
        lastNode = node;
        while( lastNode->right )
            lastNode = lastNode->right;
    }
    TreeNode *p = this->node;
    if( node->right != nullptr ) {
        node = node->right;
        while( node->left )
            node = node->left;
    } else if( node == lastNode ){
        node = nullptr;
    } else if( node == node->parent->left ) {
        node = node->parent;
    } else if( node == node->parent->right ) {
        while( node->parent->val <= node->val )
            node = node->parent;
        node = node->parent;
    }
    return iterator(p);
}

// default constructor -- only initialize the private variable
template<typename T>
RBT<T>::RBT() {
    cnt = 0;
    root = nullptr;
}

// copy constructor -- deep copy every element of the other RBT
template<typename T>
RBT<T>::RBT(const RBT &other) {
    cnt = other.cnt;
    if( cnt == 0 )
        return;
    std::queue<std::pair<TreeNode*, TreeNode*> > q;
    q.push(std::make_pair(root(other.root->val),other.root));
    while( !q.empty() ) {
        TreeNode *node1 = q.front().first;
        TreeNode *node2 = q.front().second;
        q.pop();
        if( node2->left ) {
            node1->left = new TreeNode(node2->left->val);
            node1->left->parent = node1;
            q.push(make_pair(node1->left, node2->left));
        }
        if( node2->right ) {
            node1->right = new TreeNode(node2->right->val);
            node1->irght->parent = node1;
            q.push(make_pair(node1->right, node2->right));
        }
    }
}

// assignment constructor
template<typename T>
const RBT<T>& RBT<T>::operator=(const RBT &other) {
    cnt = other.cnt;
    if( cnt == 0 )
        return *this;
    std::queue<std::pair<TreeNode*, TreeNode*> > q;
    q.push(std::make_pair(root(other.root->val),other.root));
    while( !q.empty() ) {
        TreeNode *node1 = q.front().first;
        TreeNode *node2 = q.front().second;
        q.pop();
        if( node2->left ) {
            node1->left = new TreeNode(node2->left->val);
            node1->left->parent = node1;
            q.push(make_pair(node1->left, node2->left));
        }
        if( node2->right ) {
            node1->right = new TreeNode(node2->right->val);
            node1->right->parent = node1;
            q.push(make_pair(node1->right, node2->right));
        }
    }
    return *this;
}

// destructor
template<typename T>
RBT<T>::~RBT() {
    if( cnt == 0 )
        return ;
    cnt = 0;
    std::queue<TreeNode*> q;
    q.push(root);
    while( !q.empty() ) {
        root = q.front();
        q.pop();
        if( root->left ) {
            q.push(root->left);
        }
        if( root->right ) {
            q.push(root->right);
        }
        delete root;
    }
}

// insert an element into RBT
template<typename T>
void RBT<T>::insert(T v) {
    insert(v,root);
    // increase cnt
    ++cnt;
}

// return the size of RBT
template<typename T>
const std::size_t RBT<T>::size() const {
    return cnt;
}

// check whether the RBT is nullptr
template<typename T>
const bool RBT<T>::empty() const {
    return cnt == 0;
}

// iterator begin() and end()
template<typename T>
typename RBT<T>::iterator RBT<T>::begin() const {
    TreeNode* node = root;
    while( node && node->left )
        node = node->left;
    return iterator(node);
}

template<typename T>
typename RBT<T>::iterator RBT<T>::end() const {
    return iterator();
}

// find an element
template<typename T>
typename RBT<T>::iterator RBT<T>::find(const T& v) const {
    TreeNode *p = root;
    while( p ) {
        if( p->val == v )
            return iterator(p);
        if( p->val < v )
            p = p->right;
        else
            p = p->left;
    }
    return end();
}

//template<typename T>
//const_iterator find(const T& v) const {
//}

// remove one element
template<typename T>
void RBT<T>::erase(iterator itr) {
    --cnt;
    if( cnt == 0 ) {
        delete root;
        root = nullptr;
        return;
    }
    // if left & right child both exist
    if( (itr.node)->left && (itr.node)->right ) {
        TreeNode *predecessor = findMax((itr.node)->left);
        (itr.node)->val = predecessor->val;
        erase(iterator(predecessor));
    } else {
        TreeNode *child = (itr.node)->left ? (itr.node)->left : (itr.node)->right;
        // here difference with bst, we need rebalance rbt to satisfy rbt rules
        if( (itr.node)->color == BLACK ) {
            // when current node is black and its child is RED, we can
            // simply repaint child to black
            if( child && child->color == RED )
                child->color = BLACK;
            // or the deleted node is the leaf cell, we should rebalance it
            else
                delete_case1(itr.node);
        }
        replace_node_in_parent(itr.node, child);
        delete itr.node;
    }
}

template<typename T>
void RBT<T>::delete_case1(TreeNode *node) {
    // when current node is the new root , it's okay
    if( node->parent != nullptr )
        delete_case2(node);
}

template<typename T>
void RBT<T>::delete_case2(TreeNode *node) {
    // in this case when current node's sibling is RED, we should replace its
    // parent's color with its sibling, and rotate the parent
    TreeNode *s = sibling(node);
    if( s->color == RED ) {
        node->parent->color = RED;
        s->color = BLACK;
        if( node == node->parent->left )
            rotate_left(node->parent);
        else
            rotate_right(node->parent);
    }
    delete_case3(node);
}

template<typename T>
void RBT<T>::delete_case3(TreeNode *node) {
    // in this case current node and its parent, its sibling and its sibling's
    // two children are both black. we just repaint its sibling to red and
    // rebalance the tree for its parent
    TreeNode *s = sibling(node);
    if( node->parent->color == BLACK && s->color == BLACK &&
        (s->left == nullptr || s->left->color == BLACK) && (s->right == nullptr || s->right->color == BLACK) ) {
        s->color = RED;
        delete_case1(node->parent);
    } else {
        delete_case4(node);
    }
}

template<typename T>
void RBT<T>::delete_case4(TreeNode *node) {
    // in this case current node and its sibling and its sibling's two children
    // are all black but the parent is red, we can just exchange the parent and
    // its sibling's color

    TreeNode *s = sibling(node);
    if( node->parent->color == RED && s->color == BLACK &&
        (s->left == nullptr || s->left->color == BLACK) && (s->right == nullptr || s->right->color == BLACK) ) {
        s->color = RED;
        node->parent->color = BLACK;
    } else {
        delete_case5(node);
    }
}

template<typename T>
void RBT<T>::delete_case5(TreeNode *node) {
    // in this case current node's sibling has a left red child and right black
    // child, we can perform a right rotation on the sibling and exchange the
    // color with its red child
    TreeNode *s = sibling(node);
    if( s->color == BLACK ) {
        if( node == node->parent->left && (s->left && s->left->color == RED) &&
            (s->right == nullptr || s->right->color == BLACK) ) {
            s->color = RED;
            s->left->color = BLACK;
            rotate_right(s);
        } else if( node == node->parent->right && (s->right && s->right->color == RED) &&
                    (s->left == nullptr || s->left->color == BLACK) ) {
            s->color = RED;
            s->right->color = BLACK;
            rotate_left(s);
        }
    }
    delete_case6(node);
}

template<typename T>
void RBT<T>::delete_case6(TreeNode *node) {
    // in this case current node's sibling has a right red child
    // we can perform rotate on current node's parent and exchange the color
    // with sibling, and repaint the sibling's right red child to black
    TreeNode *s = sibling(node);
    s->color = node->parent->color;
    node->parent->color = BLACK;
    if( node == node->parent->left ) {
        s->right->color = BLACK;
        rotate_left(node->parent);
    } else {
        s->left->color = BLACK;
        rotate_right(node->parent);
    }
}

template<typename T>
void RBT<T>::replace_node_in_parent(TreeNode *node, TreeNode *newNode) {
    if( node->parent ) {
        if( node->parent->left == node )
            node->parent->left = newNode;
        else
            node->parent->right = newNode;
    } else {
        root = newNode;
    }
    if( newNode )
        newNode->parent = node->parent;
}

template<typename T>
typename RBT<T>::TreeNode *RBT<T>::findMax(TreeNode *node) {
    while( node && node->right )
        node = node->right;
    return node;
}

// private : insert an element into RBT
template<typename T>
void RBT<T>::insert(T v, TreeNode *&r, TreeNode * const &p) {
    if( r == nullptr ) {
        r = new TreeNode(v, nullptr, nullptr, p);
        // here we need to remove the violation of RBT if exists
        insert_case1(r);
    } else if( v < r->val )
        insert(v, r->left, r);
    else
        insert(v, r->right,r);
}

// this function handle case that the current node is the root
template<typename T>
void RBT<T>::insert_case1(TreeNode *r) {
    // in this case we just paint the node to black
    if( r->parent == nullptr )
        r->color = BLACK;
    else
        insert_case2(r);
}

// this handles when the node's parent is black
template<typename T>
void RBT<T>::insert_case2(TreeNode *r) {
    // in this case we just return
    if( r->parent->color == BLACK )
        return;
    else
        insert_case3(r);
}

// get current node's grandparent
template<typename T>
typename RBT<T>::TreeNode* RBT<T>::grandParent(TreeNode * const &r) const {
    if( r && r->parent )
        return r->parent->parent;
    else
        return nullptr;
}

// get current node's uncle
template<typename T>
typename RBT<T>::TreeNode* RBT<T>::uncle(TreeNode * const &r) const {
    TreeNode *g = grandParent(r);
    // if no grandParent then no uncle
    if( g == nullptr )
        return nullptr;
    if( r->parent == g->left )
        return g->right;
    else
        return g->left;
}

// get current node's silbing
template<typename T>
typename RBT<T>::TreeNode* RBT<T>::sibling(TreeNode * const &r) const {
    if( r == nullptr || r->parent == nullptr )
        return nullptr;
    if( r->parent->left == r )
        return r->parent->right;
    else
        return r->parent->left;
}

// this handles when both the parent and the uncle are red
template<typename T>
void RBT<T>::insert_case3(TreeNode *r) {
    // when both the parent and the uncle are red, we can paint the parent and
    // uncle to black and the grandParent to red
    // this will remain all RBT rules but the grandParent may violate rule
    TreeNode* u = uncle(r);
    if( u && u->color == RED ) {
        r->parent->color = BLACK;
        u->color = BLACK;
        u->parent->color = RED;
        insert_case1(u->parent);
    } else
        insert_case4(r);
}

// this handles when current node's parent is red but the uncle is black
template<typename T>
void RBT<T>::insert_case4(TreeNode *r) {
    TreeNode *g = grandParent(r);
    // when current node is the right child and parent is the left child of
    // grandParent , we rotate left
    if( r == r->parent->right && r->parent == g->left ) {
        rotate_left(r->parent);
        r = r->left;
    // in this case we rotate right
    } else if( r == r->parent->left && r->parent == g->right ) {
        rotate_right(r->parent);
        r = r->right;
    }
    insert_case5(r);
}

// this handles that the parent is red but uncle is black
template<typename T>
void RBT<T>::insert_case5(TreeNode *r) {
    TreeNode *g = grandParent(r);
    // in this case we just paint the parent to BLACK and grandParent to RED
    // then rotate left or right
    g->color = RED;
    r->parent->color = BLACK;
    if( r == r->parent->left )
        rotate_right(g);
    else
        rotate_left(g);
}

// this rotate the current node to the left
template<typename T>
void RBT<T>::rotate_left(TreeNode *r) {
    TreeNode *saved_right_left = r->right->left, *child = r->right;
    if( r->parent ) {
        if( r == r->parent->left )
            r->parent->left = child;
        else
            r->parent->right = child;
    // if current node is the root we need update the root node also
    } else {
        root = child;
    }
    child->left = r;
    child->parent = r->parent;
    r->parent = child;
    r->right = saved_right_left;
    if( saved_right_left )
        saved_right_left->parent = r;
}
// thi rotate the current node to the right
template<typename T>
void RBT<T>::rotate_right(TreeNode *r) {
    TreeNode *saved_left_right = r->left->right, *child = r->left;
    if( r->parent ) {
        if( r == r->parent->left )
            r->parent->left = child;
        else
            r->parent->right = child;
    // if current node is the root we need update the root node also
    } else {
        root = child;
    }
    child->right = r;
    child->parent = r->parent;
    r->parent = child;
    r->left = saved_left_right;
    if( saved_left_right )
        saved_left_right->parent = r;
}
#endif