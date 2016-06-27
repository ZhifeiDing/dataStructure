#ifndef _KDT_HPP_
#define _KDT_HPP_

//#include <iterator>
#include <queue>
#include <iostream>
#include <vector>

// TreeNode<T> definition
template<typename T>
class TreeNode {
public:
    std::vector<T> val;
    TreeNode<T> *left;
    TreeNode<T> *right;
    // constructor
    TreeNode() : left(nullptr), right(nullptr) {};
    TreeNode(std::vector<T> &v) : val(v), left(nullptr), right(nullptr){};
    TreeNode(std::vector<T> &v, TreeNode<T> *l, TreeNode *r) : val(v), left(l), right(r){};
};


// K-D Tree declaration
template<typename T>
class KDT {
public:
    // default constructor
    explicit KDT(const int k);
    // copy constructor
    KDT(const KDT &other)=delete;
    // assignment constructor
    const KDT<T>& operator=(const KDT &other)=delete;
    // destructor
    ~KDT();

    // insert an element into KDT
    void insert(std::vector<T> &v);

    // print an element
    void printRange(const std::vector<T>& low, const std::vector<T> &high) const;
    // return the size of KDT
    const std::size_t size() const;

    // check whether the KDT is nullptr
    const bool empty() const;

private:
    int cnt;
    const int K;
    TreeNode<T> *root;

    // private insert function
    void insert(std::vector<T> &v, TreeNode<T> *&r, int idx = 0);

    void printRange(const std::vector<T>& low, const std::vector<T> &high, TreeNode<T> *r, int idx) const;
};

// default constructor -- only initialize the private variable
template<typename T>
KDT<T>::KDT(const int k) : K(k) {
    cnt = 0;
    root = nullptr;
}

// destructor
template<typename T>
KDT<T>::~KDT() {
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

// insert an element into KDT
template<typename T>
void KDT<T>::insert(std::vector<T> &v) {
    insert(v,root,0);
    // increase cnt
    ++cnt;
}

// return the size of KDT
template<typename T>
const std::size_t KDT<T>::size() const {
    return cnt;
}

// check whether the KDT is nullptr
template<typename T>
const bool KDT<T>::empty() const {
    return cnt == 0;
}

//  print the node that in the given range
template<typename T>
void KDT<T>::printRange(const std::vector<T> &low, const std::vector<T> &high) const {
    printRange(low,high,root,0);
}

// private : print the node that in the given range
template<typename T>
void KDT<T>::printRange(const std::vector<T> &low, const std::vector<T> &high, TreeNode<T> *r, int idx) const {
    idx = idx%K;
    if( r == nullptr )
        return;

    bool valid = true;
    for(int i = 0; i < K; ++i) {
        if( low[i] <= r->val[i] && high[i] >= r->val[i] )
            valid = valid && true;
        else {
            valid = false;
            break;
        }
    }
    if( valid ) {
        std::cout << "(";
        for(int i = 0; i < K; ++i) {
            std::cout << r->val[i] << (( i == K - 1 ) ? "" : " , ");
        }
        std::cout << ") ";
    }

    if( low[idx] < r->val[idx] )
        return printRange(low,high, r->left, idx+1);
    else
        return printRange(low,high, r->right,idx+1);

}

// private : insert an element into KDT
template<typename T>
void KDT<T>::insert(std::vector<T> &v, TreeNode<T> *&r, int idx) {
    idx = idx%K;
    if( r == nullptr )
        r = new TreeNode<T>(v, nullptr, nullptr);
    else if( v[idx] < r->val[idx] )
        insert(v, r->left, idx+1);
    else
        insert(v, r->right,idx+1);
}

#endif