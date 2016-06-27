#ifndef _BST_HPP_
#define _BST_HPP_

//#include <iterator>
#include <queue>

// TreeNode<T> definition
template<typename T>
class TreeNode {
public:
    T val;
    TreeNode<T> *left;
    TreeNode<T> *right;
    TreeNode<T> *parent;
    // constructor
    TreeNode() : left(nullptr), right(nullptr), parent(nullptr){};
    TreeNode(T v) : val(v), left(nullptr), right(nullptr), parent(nullptr){};
    TreeNode(T v, TreeNode<T> *l, TreeNode *r, TreeNode *p = nullptr) : val(v), left(l), right(r),parent(p){};
};


// Binary search Tree declaration
template<typename T>
class BST {
public:
    // default constructor
    explicit BST();
    // copy constructor
    BST(const BST &other);
    // assignment constructor
    const BST<T>& operator=(const BST &other);
    // destructor
    ~BST();

    // STL-style iterator
    class iterator {
    public:
        friend class BST;
        explicit iterator();
        const iterator& operator=(const iterator &other); // assignment constructor
        iterator& operator++(); // prefix increment
        iterator operator++(int); // postfix increment
        T& operator*() const;
        bool operator!=(const BST<T>::iterator &other) const;
        bool operator==(const BST<T>::iterator &other) const;
        // for iterator_traits to refer
        typedef std::output_iterator_tag iterator_category;
        typedef T value_type;
        typedef std::ptrdiff_t difference_type;
        typedef T* pointer;
        typedef T& reference;
    private:
        iterator(TreeNode<T>* n);
        TreeNode<T>* node;
        TreeNode<T>* lastNode;
    };

    // iterator begin() and end()
    iterator begin() const;
    iterator end() const;

    // insert an element into BST
    void insert(T v);

    // find an element
    iterator find(const T& v) const;
    //const_iterator find(const T& v) const;

    // remove one element
    void erase(iterator itr);

    // return the size of BST
    const std::size_t size() const;

    // check whether the BST is nullptr
    const bool empty() const;

private:
    int cnt;
    TreeNode<T> *root;

    // private insert function
    void insert(T v, TreeNode<T> *&r, TreeNode<T> * const &p = nullptr);
    // private : replace node in parent
    void replace_node_in_parent(TreeNode<T> *node, TreeNode<T> *newNode = nullptr);
    // private : find the max value node
    TreeNode<T> *findMax(TreeNode<T> *node);
};

// BST<T>::iterator
// default constructor
template<typename T>
BST<T>::iterator::iterator() {
    node = nullptr;
    lastNode = nullptr;
}

// one argument constructor
template<typename T>
BST<T>::iterator::iterator(TreeNode<T>* n) {
    node = n;
    lastNode = nullptr;
}

// assignment constructor
template<typename T>
const typename BST<T>::iterator& BST<T>::iterator::operator=(const iterator &other) {
    this->node = other.node;
    return *this;
}

template<typename T>
T& BST<T>::iterator::operator*() const {
    return this->node->val;
}

// overload operator ==
template<typename T>
bool BST<T>::iterator::operator==(const BST<T>::iterator &other) const {
    return this->node == other.node;
}

// overload operator !=
template<typename T>
bool BST<T>::iterator::operator!=(const BST<T>::iterator &other ) const {
    return this->node != other.node;
}

// overload prefix ++
template<typename T>
typename BST<T>::iterator& BST<T>::iterator::operator++() {
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
typename BST<T>::iterator BST<T>::iterator::operator++(int) {
    // if current node is root , we could get the lastNode
    if( node->parent == nullptr ) {
        lastNode = node;
        while( lastNode->right )
            lastNode = lastNode->right;
    }
    TreeNode<T> *p = this->node;
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
BST<T>::BST() {
    cnt = 0;
    root = nullptr;
}

// copy constructor -- deep copy every element of the other BST
template<typename T>
BST<T>::BST(const BST &other) {
    cnt = other.cnt;
    if( cnt == 0 )
        return;
    std::queue<std::pair<TreeNode<T>*, TreeNode<T>*> > q;
    q.push(std::make_pair(root(other.root->val),other.root));
    while( !q.empty() ) {
        TreeNode<T> *node1 = q.front().first;
        TreeNode<T> *node2 = q.front().second;
        q.pop();
        if( node2->left ) {
            node1->left = new TreeNode<T>(node2->left->val);
            node1->left->parent = node1;
            q.push(make_pair(node1->left, node2->left));
        }
        if( node2->right ) {
            node1->right = new TreeNode<T>(node2->right->val);
            node1->irght->parent = node1;
            q.push(make_pair(node1->right, node2->right));
        }
    }
}

// assignment constructor
template<typename T>
const BST<T>& BST<T>::operator=(const BST &other) {
    cnt = other.cnt;
    if( cnt == 0 )
        return *this;
    std::queue<std::pair<TreeNode<T>*, TreeNode<T>*> > q;
    q.push(std::make_pair(root(other.root->val),other.root));
    while( !q.empty() ) {
        TreeNode<T> *node1 = q.front().first;
        TreeNode<T> *node2 = q.front().second;
        q.pop();
        if( node2->left ) {
            node1->left = new TreeNode<T>(node2->left->val);
            node1->left->parent = node1;
            q.push(make_pair(node1->left, node2->left));
        }
        if( node2->right ) {
            node1->right = new TreeNode<T>(node2->right->val);
            node1->right->parent = node1;
            q.push(make_pair(node1->right, node2->right));
        }
    }
    return *this;
}

// destructor
template<typename T>
BST<T>::~BST() {
    if( cnt == 0 )
        return ;
    cnt = 0;
    std::queue<TreeNode<T>*> q;
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

// insert an element into BST
template<typename T>
void BST<T>::insert(T v) {
    insert(v,root);
    // increase cnt
    ++cnt;
}

// return the size of BST
template<typename T>
const std::size_t BST<T>::size() const {
    return cnt;
}

// check whether the BST is nullptr
template<typename T>
const bool BST<T>::empty() const {
    return cnt == 0;
}

// iterator begin() and end()
template<typename T>
typename BST<T>::iterator BST<T>::begin() const {
    TreeNode<T>* node = root;
    while( node && node->left )
        node = node->left;
    return iterator(node);
}

template<typename T>
typename BST<T>::iterator BST<T>::end() const {
    return iterator();
}

// find an element
template<typename T>
typename BST<T>::iterator BST<T>::find(const T& v) const {
    TreeNode<T> *p = root;
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
void BST<T>::erase(iterator itr) {
    --cnt;
    if( cnt == 0 ) {
        delete root;
        root = nullptr;
        return;
    }
    // if left & right child both exist
    if( (itr.node)->left && (itr.node)->right ) {
        TreeNode<T> *predecessor = findMax((itr.node)->left);
        (itr.node)->val = predecessor->val;
        erase(iterator(predecessor));
    // if only left child exist
    } else {
        TreeNode<T> *child = (itr.node)->left ? (itr.node)->left : (itr.node)->right;
        replace_node_in_parent(itr.node, child);
        delete itr.node;
    }
}

template<typename T>
void BST<T>::replace_node_in_parent(TreeNode<T> *node, TreeNode<T> *newNode) {
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
TreeNode<T> *BST<T>::findMax(TreeNode<T> *node) {
    while( node && node->right )
        node = node->right;
    return node;
}

// private : insert an element into BST
template<typename T>
void BST<T>::insert(T v, TreeNode<T> *&r, TreeNode<T> * const &p) {
    if( r == nullptr )
        r = new TreeNode<T>(v, nullptr, nullptr, p);
    else if( v < r->val )
        insert(v, r->left, r);
    else
        insert(v, r->right,r);
}

#endif