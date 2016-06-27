#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <algorithm>

using namespace std;


int nativeStringSearch(string &text, string &p) {
    for(int i = 0; i <= text.size() - p.size(); ++i) {
        int j;
        for(j = 0; j < p.size() && text[i+j] == p[j]; ++j);
        if( j == p.size() )
            return i;
    }
    return -1;  // not found
}

// KMP search
void KMPTable(string &p, vector<int> &kmp_table) {
    kmp_table[0] = -1;
    if( p.size() <= 2 )
        return;
    int pos = 2; // the current position
    int cnd = 0; // the next character of current candidate
    while( pos < p.size() ) {
        if(p[pos-1] == p[cnd]) {
            kmp_table[pos] = cnd;
            ++pos;
            ++cnd;
        } else if( cnd > 0 ) {
            cnd = kmp_table[cnd];
        } else {
            kmp_table[pos] = 0;
            ++pos;
        }
    }
}
int KMPSearch(string &text, string &p) {
    vector<int> kmp_table(p.size(),0);
    KMPTable(p, kmp_table);

    int m = 0; // the index of text
    int i = 0; // the index of p
    while( m + p.size() <= text.size() ) {
        if( text[m+i] == p[i] ) {
            if( i == p.size() - 1 )
                return m;
            ++i;
        } else if( kmp_table[i] >= 0 ) {
            m = m + i - kmp_table[i];
            i = kmp_table[i];
        } else {
            m = m + 1;
            i = 0;
        }
    }
    return -1;
}
// build the bad character shift rule table
void BMBadChar(string &p, vector<int> &badChar) {
    // we aasume badChar has been initialized and have default value of pattern's length
    for(int i = 0; i < p.size(); ++i)
        // if the mismatched char is found in pattern we can shift the pattern to match this char
        badChar[p[i]] = p.size() -1 - i;
}
// check whether the substring p[p..p.size()-1] is a prefix of itself
bool isPrefix(string &p, int idx) {
    for(int i = 0, j = idx; j < p.size(); ++i, ++j)
        if( p[i] != p[j] )
            return false;
    return true;
}

// record the matched suffix's length
int suffix(string &p, int idx) {
    int i, j;
    for(i = idx, j = p.size()-1; i >= 0 && p[i] == p[j]; --i,--j);
    return idx - i;
}

// build the good suffix shift rule table
void BMGoodSuffix(string &p, vector<int> &goodSuffix) {
    // first check that the matched suffix has a matched prefix
    int lastMatchedPrefix = p.size();  // record the last matched prefix's index
    for(int i = p.size() - 1; i >= 0; --i) {
        if( isPrefix(p,i+1) )
            lastMatchedPrefix = i+1;
        // when there's matched prefix we should shift the pattern to the matched the suffix
        goodSuffix[p.size() - 1 - i] = lastMatchedPrefix - i + p.size() - 1;
    }

    // second if there exists substring matched with the matched suffix
    // we should shift pattern to match the substring instead of previous prefix
    for(int i = 0; i < p.size() - 1; ++i) {
        int slen = suffix(p,i);
        goodSuffix[slen] = slen + p.size() - 1 - i;
    }
}
int BMSearch(string &text, string &p) {
    vector<int> badChar(256, p.size()); // initialize bad character table to pattern's length
    BMBadChar(p,badChar);

    vector<int> goodSuffix(p.size()); // declare of the good suffix shift table
    BMGoodSuffix(p, goodSuffix);

    int i = p.size() - 1; // index of text
    while( i < text.size() ) {
        int j = p.size() - 1;
        for(; j >= 0 && p[j] == text[i]; --j,--i);
        if( j < 0 )
            return i+1;
        // the difference with other string searching algorithm
        // choose the larger between good suffix table and bad character table
        i += max(goodSuffix[p.size() - 1 - j], badChar[text[i]]);
    }
    return -1;  // not found
}

// Boyer-Moore-Horspool
//
// build Boyer-Moore-Horspool bad character table
void BMHBadChar(string &p, vector<int> &badChar) {
    for(int i = 0; i < p.size() - 1; ++i)
        badChar[p[i]] = p.size() -1 - i;
}

// Boyer-Moore-Horspool
int BMHSearch(string &text, string &p) {
  // initialize bad character table to pattern's length
    vector<int> badChar(256, p.size());
    BMHBadChar(p,badChar);

    int i = p.size() - 1; // index of text
    while( i < text.size() ) {
        int j = p.size()-1; // index of pattern
        for(; j >= 0 && text[i-p.size()+1+j] == p[j]; --j);
        if( j < 0 )
            return i-p.size() + 1;
        // the difference with Boyer-Moore slgorithm
        // we always choose the first matched text char
        i += badChar[text[i]];
    }
    return -1; // not found
}

typedef int(*fptr)(string&,string&);

void test_strSearch(fptr func, string &text, string &p, string funcName) {
    int idx = func(text,p);
    cout << funcName << "(" << text << "," << p << ") return " << idx << endl;
}
void test_strSearch() {
    unordered_map<string, fptr> funcMap = {
        {"nativeStringSearch",nativeStringSearch},
        {"KMPSearch", KMPSearch},
        {"BMSearch", BMSearch},
        {"BMHSearch", BMHSearch}
    };
    unordered_map<string, string> TextPattern = {
        {"this is a simple example", "example"},
        {"this should have no match", "gave"},
        {"match at the begin", "match"},
        {"match in the middle", "th"}
    };
    srand((unsigned int)time(NULL));
    int i = 0;
    auto itr = TextPattern.begin();
    while( itr != TextPattern.end() ) {
        ++i;
        cout << endl << "#################" << endl;
        cout << "Test Step : " << i << endl;
        string text = (*itr).first;
        string p = (*itr).second;
        ++itr;
        for(auto itr : funcMap)
            test_strSearch(static_cast<fptr>(itr.second), text, p, static_cast<string>(itr.first));
        cout << "#################" << endl;
    }
}
int main() {
    test_strSearch();
    return 0;
}