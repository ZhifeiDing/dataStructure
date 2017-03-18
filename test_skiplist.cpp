#include "skiplist.hpp"
#include <iostream>
#include <vector>

using namespace std;

template<typename Key>
struct Comparator {
  int operator()(const Key& a, const Key& b) const {
    if (a < b) {
      return -1;
    } else if (a > b) {
      return +1;
    } else {
      return 0;
    }
  }
};

void testSkiplist() {
    Comparator<int> cmp;
    vector<int> nums = {10, 5, 7, 2, 9, 0, 1};
    skiplist<int, Comparator<int> > sl(cmp);
    assert( sl.contains(nums[0]) == false );
    for(int i = 0; i < nums.size(); ++i) {
        sl.insert(nums[i]);
        assert( sl.contains(nums[i]) == true );
        if( i < nums.size() - 1 )
            assert( sl.contains(nums[i+1]) == false );
    }
}

int main() {
    testSkiplist();
    return 0;
}
