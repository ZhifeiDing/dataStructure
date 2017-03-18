#ifndef _SKIPLIST_HPP_
#define _SKIPLIST_HPP_

#include <cassert>
#include <cstddef>
#include <ctime>
#include <cstdlib>

template<typename Key, typename Comparator>
class skiplist {
public:
    // skiplist node class
    class Node;
    // constructor
    explicit skiplist(Comparator cmp);
    // skiplist search method
    bool contains(const Key& key);
    // erase method
    void erase(const Key& key);
    // insert an item into skiplist
    // don't allowed duplicate items
    void insert(const Key& key);
private:
    Comparator cmp_;
    Node* head_;
    unsigned maxHeight_;
    // the max height limitation
    const unsigned int MAX_HEIGHT = 12;
    // the probability of each item in level i appeared in level i+1
    // usually 2 or 4
    const unsigned int BRANCH = 4;
    // construct new node
    Node* newNode(const Key& key, const unsigned int height);
    // check if the key is after the current node
    bool isAfterNode(const Key& key, Node *node);
    // find the first node which is greater or equal to the key
    Node* findGreater(const Key& key, Node** prev);
    // get the random height between 0 and MAX_HEIGHT
    unsigned int getHeight();
};

// class Node implementation
template<typename Key, typename Comparator>
class skiplist<Key, Comparator>::Node {
public:
    Key key;
    // constructor
    explicit Node(const Key& key) : key(key) {}
    // get next node
    Node* next(const unsigned int level) {
        assert( level >= 0 );
        return next_[level];
    }
    // set next node pointer
    void setNext(const unsigned int level, Node *node) {
        assert( level >= 0 );
        next_[level] = node;
    }
private:
    Node* next_[1];
};

// skiplist constructor
template<typename Key, typename Comparator>
skiplist<Key, Comparator>::skiplist(Comparator cmp) : cmp_(cmp), maxHeight_(1) {
    head_ = newNode(-1, MAX_HEIGHT);
    for(int i = 0; i < MAX_HEIGHT; ++i)
        head_->setNext(i, nullptr);
    // set the seed of rand
    srand((unsigned int)time(NULL));
}

// construct new node
template<typename Key, typename Comparator>
typename skiplist<Key, Comparator>::Node* skiplist<Key, Comparator>::newNode(const Key& key, const unsigned int height) {
    char* mem = (char*)malloc(sizeof(Node) + sizeof(Node*) * (height - 1));
    return new (mem) Node(key);
}

// check if the key is after the current node
template<typename Key, typename Comparator>
bool skiplist<Key, Comparator>::isAfterNode(const Key& key, Node* node) {
    return ( node != nullptr ) && cmp_(node->key, key) < 0;
}

// find the first node which is greater or equal to the key
template<typename Key, typename Comparator>
typename skiplist<Key, Comparator>::Node* skiplist<Key, Comparator>::findGreater(const Key& key, Node** prev) {
    int level = maxHeight_ - 1;
    Node* p = head_;
    while( true ) {
        Node* n = p->next(level);
        if( isAfterNode(key, n) )
            p = n;
        else {
            if( prev != nullptr )
                prev[level] = p;
            if( level == 0 )
                return n;
            else
                --level;
        }
    }
}

// insert an item into skiplist
// don't allowed duplicate items
template<typename Key, typename Comparator>
void skiplist<Key, Comparator>::insert(const Key& key) {

    // first get the node which is greater or equal to the key
    Node* prev[MAX_HEIGHT];
    Node* t = findGreater(key, prev);

    // if the key has already exist, just return
    if( t != nullptr && cmp_(t->key, key) == 0 )
        return;

    // randomly get the new node's height
    int h = getHeight();
    // if the new Node's height is higher than current skiplist's maxHeight_
    // we should set the express lane fron the head_
    // and update the maxHeight_
    if( h > maxHeight_ ) {
        for(int i = maxHeight_; i < h; ++i)
            prev[i] = head_;
        maxHeight_ = h;
    }

    // construct the node with the key and height
    t = newNode(key, h);
    // then we can simply wire the prev[] to the new node
    for(int i = 0; i < h; ++i) {
        t->setNext(i, prev[i]->next(i));
        prev[i]->setNext(i, t);
    }
}

// skiplist search method
template<typename Key, typename Comparator>
bool skiplist<Key, Comparator>::contains(const Key& key) {
    Node *p = findGreater(key, nullptr);
    return ( p != nullptr ) && cmp_(key, p->key) == 0;
}

// get the random height between 0 and MAX_HEIGHT
template<typename Key, typename Comparator>
unsigned int skiplist<Key, Comparator>::getHeight() {
    unsigned int h = 1;
    while( h < MAX_HEIGHT && ( rand()%BRANCH == 0 ) ) {
        ++h;
    }
    assert( h > 0 );
    assert( h <= MAX_HEIGHT );
    return h;
}
#endif