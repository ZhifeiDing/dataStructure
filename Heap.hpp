#ifndef _HEAP_HPP_
#define _HEAP_HPP_

#include <vector>
#include <functional>
#include <exception>

template<typename T, typename comparator = std::less<T> >
class heap {
public:

    // empty arguments constructor
    heap(comparator cmp = comparator());
    // one argument constructor ( default comparator )
    heap(const std::vector<T> &nums, comparator cmp = comparator());
    // iterator constructor

    // copy constructor
    heap(const heap *other);

    // assign constructor
    const heap &operator=(const heap *other);

    // find min/max
    const T top() const;

    // extract top
    void pop();

    // insert value
    void push(T val);

    // size -- returns the number of elements
    const size_t size() const;

    // empty -- checks whether the heap is empty
    const bool empty() const;

    // destructor
    ~heap();

private:
    std::vector<T> data;
    comparator comp;
    void percolateDown(int idx);
    void percolateUp(int idx);
};

// one default argument constrctor
template<typename T, typename comparator>
heap<T,comparator>::heap(comparator cmp) : comp(cmp) {
    // just initialize the data to size 1
    data.resize(1);
    //this->comp = cmp;
}

// two argument constructor
template<typename T, typename comparator>
heap<T, comparator>::heap(const std::vector<T> &nums, comparator cmp) {
    this->data.resize(nums.size()+1);
    this->comp = cmp;

    // copy nums to data
    copy(nums.begin(), nums.end(), data.begin()+1);
    // balance the heap
    for(int idx = this->size()/2;idx > 0;--idx)
        percolateDown(idx);
}

// copy constructor
template<typename T, typename comparator>
heap<T,comparator>::heap(const heap *other) {
    copy(other->data.begin(), other->data.end(), this->data.begin());
    this->comp = other->comp;
}

// assign constructor
template<typename T, typename comparator>
const heap<T,comparator>& heap<T,comparator>::operator =(const heap *other) {
    copy(other->data.begin(), other->data.end(), this->data.begin());
    this->comp = other->comp;
    return this;
}

// destructor
template<typename T, typename comparator>
heap<T,comparator>::~heap() {
}

// find-min/max
template<typename T, typename comparator>
const T heap<T,comparator>::top() const {
    if( !empty() )
        return data[1];
}

// remove the top element
template<typename T, typename comparator>
void heap<T,comparator>::pop() {
    if( !empty() ) {
        data[1] = data[size()];
        data.resize(size());
        percolateDown(1);
    }
}

// insert an element
template<typename T, typename comparator>
void heap<T,comparator>::push(T val) {
    this->data.push_back(val);
    percolateUp(size());
}

// check whether the heap is empty
template<typename T, typename comparator>
const bool heap<T,comparator>::empty() const {
    return this->size() == 0;
}

// get the size of the heap
template<typename T, typename comparator>
const size_t heap<T,comparator>::size() const {
    return this->data.size() - 1;
}

// private functions
// affter extracting root value, rebalance the heap
template<typename T, typename comparator>
void heap<T,comparator>::percolateDown(int idx) {
    if( idx > this->size()/2 )
        return;
    int midx = idx;
    if( comp(data[2*idx],data[midx])  )
        midx = 2 * idx;
    if( 2 * idx + 1 <= this->size() && comp(data[2*idx+1],data[midx]) )
        midx = 2 * idx + 1;
    if( midx != idx ) {
        std::swap(data[midx], data[idx]);
        percolateDown(midx);
    }
}

// after insert new element into heap, we need percolateup the element to
// rebalance the heap
template<typename T, typename comparator>
void heap<T,comparator>::percolateUp(int idx) {
    if( idx <= 1 )
        return;
    if( comp(data[idx] , data[idx/2]) ) {
        std::swap(data[idx], data[idx/2]);
        percolateUp(idx/2);
    }
}

#endif
