#include<iostream>
#include "Heap.hpp"
#include<cstddef>
#include<cstdlib>
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

void heapSort(vector<int> &nums) {
    heap<int> pq(nums);
    nums.resize(0);
    while(!pq.empty() ) {
        nums.push_back(pq.top());
        pq.pop();
    }
}

void heapSort_reverse(vector<int> &nums) {
    heap<int, greater<int> > pq(nums);
    nums.resize(0);
    while(!pq.empty() ) {
        nums.push_back(pq.top());
        pq.pop();
    }
    reverse(nums.begin(), nums.end());
}

int main() {

      cout << "test one argument constructor and top() && pop() member function\n";
      vector<int> nums;
      srand((unsigned int)time(NULL));
      int n = rand() % 27;
      gen_array(n, nums);
      print_array(nums,"Data");
      heapSort(nums);
      print_array(nums,"Sort");

      // C++ random number generator
      // common use case: binding a RNG with a distribution
      default_random_engine e;
      uniform_int_distribution<> d(0, 10);
      function<int()> rnd = std::bind(d, e); // a copy of e is stored in rnd

      cout << "test push() member function\n";
      nums.push_back(rnd());
      heap<int> pq(nums);
      pq.push(rnd());
      nums.resize(0);
      while(!pq.empty() ) {
          nums.push_back(pq.top());
          pq.pop();
      }
      print_array(nums,"Sort");

      cout << "test max-heap\n";
      nums.push_back(rnd());
      heapSort_reverse(nums);
      print_array(nums,"Sort");

      cout << "test default no-argument constructor\n";
      heap<int> emptyPQ;
      int k = rnd();
      cout << "Data = ";
      for(int i = 0; i < k; ++i) {
          int val = rnd();
          cout << val << "\t";
          emptyPQ.push(val);
      }

      cout << endl << "Sort = ";
      while(!emptyPQ.empty() ) {
          cout << emptyPQ.top() << "\t";
          emptyPQ.pop();
      }
      cout << endl;

      return 0;
}
