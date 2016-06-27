#include <vector>
#include <iostream>
#include<cstddef>
#include<cstdlib>
#include "sort.hpp"
#include<algorithm>
#include<random>

using namespace std;


void gen_array(int n, vector<int> & data, int base_start = 1, int base_end = 99) {
  srand((unsigned int)time(NULL));
  for(int i = 0; i < n; i++)
    data.push_back( rand() % (base_end - base_start) + base_start );
}

void print_array(vector<int> & data, const string s) {
  cout << s << " : ";
  for(int i = 0; i < data.size(); i++)
    cout << data[i] << " ";
  cout << endl;
}

typedef void(*func)(vector<int> &);
void runTest(func f, const string s) {
    cout << endl << s << ":" << endl;
    int n = rand() % 27;
    vector<int> nums;
    gen_array(n,nums);
    print_array(nums, "Data");
    f(nums);
    print_array(nums, "Sort");
}

int main() {

    srand((unsigned int)time(NULL));
    runTest(bubbleSort, "bubble sort");
    runTest(insertionSort, "insertion sort");
    runTest(selectionSort, "selection sort");
    runTest(mergeSort, "merge sort");
    runTest(heapSort, "heap sort");
    runTest(quickSort, "quick sort");

    return 0;
}
