#include <set>
#include <iostream>

using namespace std;

int main() {
    set<int> s;
    s.insert(5);
    s.insert(6);
    set<int>::iterator itr = s.begin();
    cout << *itr << endl;
    for(; itr != s.end(); ++itr)
        cout << *itr << endl;
    return 0;
}