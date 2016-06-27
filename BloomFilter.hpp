#ifndef _BLOOM_FILTER_HPP_
#define _BLOOM_FILTER_HPP_

#include<vector>
#include "MurmurHash3.h"
#include<array>

template<typename T>
class BloomFilter {
public:
   // constructor
   BloomFilter(int size, int num);
   // insert element
   void insert(const T *v, size_t len);
   // check whether the bloom filter contains the element
   bool exists(const T *v, size_t len) const;
private:
   std::vector<bool> bloomFilter;
   int numHashes;
   // generate the nth hash values
   uint64_t nthHash(int n, std::array<uint64_t,2> &hashVals) const;
   // generate 128bit hash value using MurmurHash3
   std::array<uint64_t,2> hash(const T *data, size_t len) const;
};

// constructor
template<typename T>
BloomFilter<T>::BloomFilter(int size, int num) {
   numHashes = num;
   bloomFilter.resize(size);
}
// insert element
template<typename T>
void BloomFilter<T>::insert(const T *v,size_t len) {
   std::array<uint64_t,2> hashValues = hash(v,len);
   for(int i = 0; i < numHashes; ++i)
      bloomFilter[nthHash(i,hashValues)] = true;
}

// check if element exists in BloomFilter
template<typename T>
bool BloomFilter<T>::exists(const T *v,size_t len) const {
   std::array<uint64_t,2> hashValues = hash(v,len);
   for(int i = 0; i < numHashes; ++i) {
      if( bloomFilter[static_cast<int>(nthHash(i,hashValues))] == false )
         return false;
   }
   return true;
}
// private function : generate nth hash value
template<typename T>
uint64_t BloomFilter<T>::nthHash(int n, std::array<uint64_t,2> &hashValues) const {
   return ( hashValues[0] + n * hashValues[1]) % bloomFilter.size();
}

template<typename T>
std::array<uint64_t,2> BloomFilter<T>::hash(const T *data, size_t len) const {
    std::array<uint64_t,2> hashValues;
    MurmurHash3_x64_128(data, len, 0, hashValues.data());
    return hashValues;
}
#endif