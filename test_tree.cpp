#include "bst.hpp"
#include "rbt.hpp"
#include<cstddef>
#include <iostream>
#include<cstdlib>

using namespace std;

template<typename T>
void testTree() {
    cout << endl;
    int n = rand()%5;
    int i = 0;
    while( i++ < n ) {
        T bst;
        cout << "###########" << endl;
        cout << "Test step : " << i << endl;
        int m = rand()%10;
        cout << "insert : ";
        int v;
        while( m-- ) {
            v = rand()%100;
            cout << v << "\t";
            bst.insert(v);
        }
        cout << endl << "bst = ";
        typename T::iterator itr = bst.begin();
        while( itr != bst.end() ) {
            cout << *itr << "\t";
            itr++;
        }
        cout << "\nbst.size() = " << bst.size();
        itr = bst.begin();
        if( itr != bst.end() ) {
            cout << endl << "after erase " << *itr << endl << "bst =";
            bst.erase(itr);
        }
        itr = bst.begin();
        while( itr != bst.end() ) {
            cout << *itr << "\t";
            itr++;
        }
        cout << "\nbst.size() = " << bst.size();
        int x = v;
        itr = bst.find(x);
        if( itr != bst.end() ) {
            cout << "\nFound " << x << ", now delete it" << endl;
            bst.erase(itr);
        } else {
            cout << "\nCouldn't find " << x << endl;
        }
        cout << "bst = ";
        itr = bst.begin();
        while( itr != bst.end() ) {
            cout << *itr << "\t";
            itr++;
        }
        cout << "\nbst.size() = " << bst.size();
        cout << endl << "###########" << endl;
        cout << endl;
    }
}

int main() {
    srand((unsigned int)time(NULL));
    cout << "Test BST<int>\n";
    testTree<BST<int> >();
    cout << "Test RBT<int>\n";
    testTree<RBT<int> >();
    return 0;
}
