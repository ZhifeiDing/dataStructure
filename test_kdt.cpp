#include "kdt.hpp"
#include<cstddef>
#include <iostream>
#include<cstdlib>

using namespace std;

template<typename T>
void testTree() {
    T kdt(3);
    int n = rand()%10;
    for(int i = 0; i < n; ++i) {
        cout << "Step " << i << endl;
        vector<int> points;
        for(int j = 0; j < 3; ++j)
            points.push_back(rand()%100);
        kdt.insert(points);
        kdt.printRange(points,points);
        cout << endl << "#########" << endl;
    }
    vector<int> low = {0,0,0};
    vector<int> high = {100,100,100};
    kdt.printRange(low,high);
    cout << endl << "#########" << endl;
    cout << endl;
}

int main() {
    srand((unsigned int)time(NULL));
    cout << "Test KDT<int>\n";
    testTree<KDT<int> >();
    return 0;
}
