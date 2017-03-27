#include "linklist.hpp"
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <functional>
#include <algorithm>

using namespace std;

template<typename list>
void single_threaded_test() {
    list l;
    assert(l.empty());

    l.push_back(1);
    assert( l.front() == 1 );
    assert( l.back() == 1 );
    assert( l.size() == 1 );

    l.push_back(2);
    assert( l.front() == 1 );
    assert( l.back() == 2 );
    assert( l.size() == 2 );

    l.pop_front();
    assert( l.front() == 2 );
    assert( l.back() == 2 );
    assert( l.size() == 1 );

    l.pop_front();
    assert( l.empty() );

    l.push_back(10);
    l.push_back(10);
    l.push_back(20);
    l.push_back(30);
    l.push_back(10);
    assert( l.front() == 10 );
    assert( l.back() == 10 );
    assert( l.size() == 5 );

    l.remove(10);
    while( !l.empty() ) {
        assert( l.front() != 10 );
        l.pop_front();
    }
}

template<typename list, typename T>
void getElemsOfList(list &l, vector<T> &res) {
    while( !l.empty() ) {
        res.push_back(l.front());
        l.pop_front();
    }
}

template<typename T>
void rangeCompare(vector<T>& res, int start, int end) {
    assert( res.size() == end - start );
    for(int i = start; i < end; ++i)
        assert(res[i-start] == i);
}

inline void nop_pause() {
    __asm__ volatile ("pause" ::);
}

template<typename list, typename T>
void list_insert(list &l, atomic<bool> &f, T start, T end) {
    while( !f.load() )
        nop_pause();
    for(T i = start; i < end; ++i)
        l.push_back(i);
}

template<typename list, typename T>
void list_pop_front(list &l, atomic<bool> &f, atomic<bool> &stop, vector<T> &res) {
    while( !f.load() )
        nop_pause();
    while( true ) {
        auto val = l.try_pop_front();
        if( val.first == false && stop.load() )
            break;
        if( val.first )
            res.push_back(val.second);
    }
}

template<typename list, typename T>
void list_remove(list &l, atomic<bool> &f, T start, T end) {
    while( !f.load() )
        nop_pause();
    for(T i = start; i < end; ++i)
        l.remove(i);
}


template<typename list>
void multiple_threaded_test() {
    // try a bunch of concurrent inserts
    // make sure no value lost
    {
        list l;
        const int NUM_ELEMENTS_PER_THREAD = 2000;
        const int NUM_THREADS = 1;
        vector<thread> threads;
        atomic<bool> start_flag(false);
        for(int i = 0; i < NUM_THREADS; ++i) {
            thread t(list_insert<list, int>, ref(l), ref(start_flag),
                i * NUM_ELEMENTS_PER_THREAD, (i+1) * NUM_ELEMENTS_PER_THREAD);
            threads.push_back(move(t));
        }
        start_flag.store(true);

        for(auto &t : threads)
            t.join();
        vector<int> list_elems;
        getElemsOfList(l, list_elems);
        sort(list_elems.begin(), list_elems.end());
        rangeCompare(list_elems, 0, NUM_ELEMENTS_PER_THREAD * NUM_THREADS);
    }

    // try a bunch of concurrent pop_front
    {
        list l;
        const int NUM_ELEMENTS_PER_THREAD = 2000;
        const int NUM_THREADS = 2;
        for(int i = 0; i < NUM_ELEMENTS_PER_THREAD; ++i)
            l.push_back(i);
        vector<thread> threads;
        vector<vector<int> > res;
        res.resize(NUM_THREADS);
        atomic<bool> start_flag(false);
        atomic<bool> stop(true);
        for(int i = 0; i < NUM_THREADS; ++i) {
            thread t(list_pop_front<list, int>, ref(l), ref(stop), ref(start_flag),
                ref(res[i]));
            threads.push_back(move(t));
        }
        start_flag.store(true);

        for(auto &t : threads)
            t.join();
        assert( l.empty() );
        vector<int> list_elems;
        for(auto &r : res)
            list_elems.insert(list_elems.end(), r.begin(), r.end());
        sort(list_elems.begin(), list_elems.end());
        rangeCompare(list_elems, 0, NUM_ELEMENTS_PER_THREAD);
    }

    // try a bunch of concurrent remove
    {
        list l;
        const int NUM_ELEMENTS_PER_THREAD = 2000;
        const int NUM_THREADS = 8;
        for(int i = 0; i < NUM_THREADS * NUM_ELEMENTS_PER_THREAD; ++i)
            l.push_back(i);
        assert( l.size() == NUM_ELEMENTS_PER_THREAD * NUM_THREADS );

        vector<thread> threads;
        atomic<bool> start_flag(false);

        for(int i = 0; i < NUM_THREADS; ++i) {
            thread t(list_remove<list, int>, ref(l), ref(start_flag),
                i * NUM_ELEMENTS_PER_THREAD, (i + 1) * NUM_ELEMENTS_PER_THREAD);
            threads.push_back(move(t));
        }
        start_flag.store(true);

        for(auto &t : threads)
            t.join();
        assert( l.empty() );
    }

    // try remove with push_back
    {
        list l;
        const int NUM_ELEMENTS_PER_THREAD = 2000;
        const int NUM_THREADS = 8;

        for(int i = 0; i < NUM_THREADS * NUM_ELEMENTS_PER_THREAD; ++i)
            l.push_back(i);
        assert( l.size() == NUM_ELEMENTS_PER_THREAD * NUM_THREADS );

        vector<thread> threads;
        atomic<bool> start_flag(false);
        // remove first
        for(int i = 0; i < NUM_THREADS; ++i) {
            thread t(list_remove<list, int>, ref(l), ref(start_flag),
                i * NUM_ELEMENTS_PER_THREAD, (i+1) * NUM_ELEMENTS_PER_THREAD);
            threads.push_back(move(t));
        }

        // then push_back
        for(int i = 0; i < NUM_THREADS; ++i) {
            thread t(list_insert<list, int>, ref(l), ref(start_flag),
                (i+NUM_THREADS) * NUM_ELEMENTS_PER_THREAD, (i+1+NUM_THREADS) * NUM_ELEMENTS_PER_THREAD);
            threads.push_back(move(t));
        }
        start_flag.store(true);

        for(auto &t : threads)
            t.join();
        vector<int> list_elems;
        getElemsOfList(l, list_elems);
        sort(list_elems.begin(), list_elems.end());
        rangeCompare(list_elems, NUM_ELEMENTS_PER_THREAD * NUM_THREADS,NUM_ELEMENTS_PER_THREAD * NUM_THREADS * 2);
    }

    // try a producer/consumer queue
    {
        list l;

        atomic<bool> start_flag(false);
        atomic<bool> stop(false);

        thread producer(list_insert<list, int>, ref(l), ref(start_flag),0,10000);

        vector<int> res;
        thread consumer(list_pop_front<list, int>, ref(l), ref(start_flag),ref(stop), ref(res));
        start_flag.store(true);

        producer.join();
        stop.store(true);

        consumer.join();

        stop.load();
        rangeCompare(res, 0, 10000);
        assert( l.empty() );
    }

}

template<typename Function>
void Test(Function &&f, const string &name) {
    f();
    cout << "Test -- " << name << " passed" << endl;
}

int main(int argc, char **argv) {
    // generic linklist which doesn't support multithread
    Test(single_threaded_test<linklist<int> >, "single-thread-generic-linklist");
    //Test(multiple_threaded_test<linklist<int> >, "multiple-thread-generic-linklist");

    // glock lock linklist
    Test(single_threaded_test<linklist_glock<int> >, "single-thread-linklist-glock");
    Test(multiple_threaded_test<linklist_glock<int> >, "multiple-thread-linklist-glock");
    //
    // per-node lock
    Test(single_threaded_test<linklist_nodelock<int> >, "single-thread-linklist-nodelock");
    Test(multiple_threaded_test<linklist_nodelock<int> >, "multiple-thread-linklist-nodelock");

    // lock free linklist
    Test(single_threaded_test<linklist_lockfree<int> >, "single-thread-linklist-lockfree");
    Test(multiple_threaded_test<linklist_lockfree<int> >, "multiple-thread-linklist-lockfree");
    return 0;
}
