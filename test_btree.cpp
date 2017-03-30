#include "btree.hpp"
#include <iostream>
#include<cstddef>
#include <cstdlib>
#include <vector>

using namespace std;

int main() {
    BPlusTree<int, int, 2, 2> b;
    assert( b.empty() );
    b.insert(10,20);
    assert( b.size() == 1 );

    auto r = b.find(20);
    assert( r.first == false );

    r = b.find(10);
    assert( r.first == true );
    assert( r.second == make_pair(10,20) );

    b.insert(20,20);
    r = b.find(20);
    assert( r.first == true );
    assert( r.second == make_pair(20,20) );

    b.insert(20,20);
    r = b.find(20);
    assert( r.first == true );
    assert( r.second == make_pair(20,20) );

    b.insert(30,20);
    r = b.find(30);
    assert( r.first == true );
    assert( r.second == make_pair(30,20) );

    r = b.find(40);
    assert( r.first == false );

    b.insert(40,20);
    r = b.find(40);
    assert( r.first == true );
    assert( r.second == make_pair(40,20) );

    b.insert(50,20);
    r = b.find(50);
    assert( r.first == true );
    assert( r.second == make_pair(50,20) );

    vector<pair<int, int> > res = b.dumpTree();
    assert( res.size() == 5 );
    for(int i = 0; i < res.size(); ++i)
        assert( res[i] == make_pair(10 * ( i + 1 ), 20) );

    b.erase(50);
    assert(b.size() == 4);
    r = b.find(50);
    assert( r.first == false );

    b.erase(40);
    assert(b.size() == 3);
    r = b.find(40);
    assert( r.first == false );

    res = b.dumpTree();
    assert( res.size() == 3 );
    for(int i = 0; i < res.size(); ++i)
        assert( res[i] == make_pair(10 * ( i + 1 ), 20) );

    cout << "-- Test Pass --" << endl;
    return 0;
}
