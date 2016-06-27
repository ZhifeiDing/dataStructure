#include <iostream>
#include "BloomFilter.hpp"

using namespace std;

int main() {
    vector<string> s = {"hello", "world", "good", "morning"};
    int size = 25, num = 3;
    BloomFilter<char> bf(size,num);
    cout << "Insert : ";
    for(auto itr : s) {
        cout << itr << "\t";
        bf.insert(itr.c_str(),itr.size());
    }
    cout << endl;
    vector<string> test = {"world", "morning", "China", "Red"};
    for(auto itr : test) {
        cout << itr << " : ";
        if( bf.exists(itr.c_str(), itr.size()) )
            cout << "maybe exist in BloomFilter";
        else
            cout << "don't in BloomFilter";
        cout << endl;
    }
    return 0;
}
