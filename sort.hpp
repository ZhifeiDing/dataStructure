#ifndef _SORT_HPP_
#define _SORT_HPP_

#include <vector>
#include <iostream>
#include <functional>
#include <algorithm>

// bubble sort
template<typename T>
void bubbleSort(std::vector<T> &nums) {
    for(int i = nums.size()-1; i > 0; --i)
        for(int j = 0; j < i; ++j)
            if( nums[j+1] < nums[j] )
                std::swap(nums[j+1], nums[j]);
}

// insertion sort
template<typename T>
void insertionSort(std::vector<T> &nums) {
    for(int i = 1; i < nums.size(); ++i) {
        int j = i - 1, val = nums[i];
        while( j >= 0 && val < nums[j] ) {
            nums[j+1] = nums[j];
            --j;
        }
        nums[j+1] = val;
    }
}

// selection sort
template<typename T>
void selectionSort(std::vector<T> &nums) {
    for(int i = 0; i < nums.size() - 1; ++i) {
        int j = i + 1;
        int minVal = i;
        while( j < nums.size() ) {
            if( nums[j] < nums[minVal] )
                minVal = j;
            ++j;
        }
        std::swap(nums[i], nums[minVal]);
    }
}

// merge sort
template<typename T>
void merge(std::vector<T> &nums, int left, int mid, int right) {
    std::vector<T> r;
    int i = left, j = mid+1;
    while( i <= mid && j <= right ) {
        if( nums[i] <= nums[j] ) {
            r.push_back(nums[i]);
            ++i;
        } else {
            r.push_back(nums[j]);
            ++j;
        }
    }
    if( j <= right ) {
        i = j;
        mid = right;
    }
    while( i <= mid ) {
        r.push_back(nums[i]);
        ++i;
    }
    copy(r.begin(), r.end(), nums.begin()+left);
}

template<typename T>
void mergeSort(std::vector<T> &nums, int left, int right) {
    if( right <= left )
        return;
    int mid = left + ( right - left ) / 2;
    mergeSort(nums, left, mid);
    mergeSort(nums, mid+1, right);
    merge(nums, left, mid, right);
}

template<typename T>
void mergeSort(std::vector<T> &nums) {
    mergeSort(nums, 0, nums.size()-1);
}

// heap sort
template<typename T>
void siftDown(std::vector<T> &nums, int start, int end) {
    while( 2 * start + 1 <= end ) {
        int child = 2 * start + 1;
        int swap = start;
        if( nums[start] < nums[child] )
            swap = child;
        if( child + 1 <= end && nums[swap] < nums[child+1] )
            swap = child + 1;
        if( swap == start )
            return;
        else {
            std::swap(nums[swap], nums[start]);
            start = swap;
        }
    }
}
template<typename T>
void heapify(std::vector<T> &nums) {
    for(int i = (nums.size()-2)/2; i >= 0; --i)
        siftDown(nums, i, nums.size()-1);
}
template<typename T>
void heapSort(std::vector<T> &nums) {
    // build the heap
    heapify(nums);
    // exchange the largest value to the end of the array
    // then call siftDown() to rebalance the heap[0..end]
    int end = nums.size()-1;
    while( end ) {
        std::swap(nums[0], nums[end]);
        --end;
        siftDown(nums, 0, end);
    }
}

// quick sort
template<typename T>
int pivot(std::vector<T> &nums, int start, int end) {
    int mid = start + ( end - start )/2;
    int p = mid;
    if( nums[start] < nums[end] ) {
        if( nums[mid] <= nums[start] )
            p = start;
        else if( nums[end] < nums[mid] )
            p = end;
    } else {
        if( nums[mid] <= nums[end] )
            p = end;
        else if( nums[start] < nums[mid] )
            p = start;
    }
    return nums[p];
}
template<typename T>
int partition(std::vector<T> &nums, int start, int end) {
    int p = pivot(nums, start, end);
    int l = start - 1, r = end + 1;
    while( l < r ) {
        while( nums[++l] < p );
        while( p < nums[--r] );
        if( l >= r )
            return r;
        std::swap(nums[l], nums[r]);
    }
    return r;
}
template<typename T>
void quickSort(std::vector<T> &nums, int start, int end) {
    if( end <= start )
        return;
    int p = partition(nums, start, end);
    quickSort(nums, start, p);
    quickSort(nums, p + 1, end);
}
template<typename T>
void quickSort(std::vector<T> &nums) {
    quickSort(nums, 0, nums.size()-1);
}
#endif
