#ifndef _LINKLIST_HPP
#define _LINKLIST_HPP

#include<cassert>
#include<cstddef>
#include<mutex>
#include<cstdlib>

// generic linklist without multi-thread support
template<typename T>
class linklist {
public:
    // empty-argument constructor
    explicit linklist() : head_(nullptr), tail_(nullptr) { }

    // return whether the list is empty
    bool empty() const {
        return size() == 0;
    }
    // return the size of list
    std::size_t size() const {
        std::size_t size = 0;
        node* p = head_;
        while( p ) {
            ++size;
            p = p->next_;
        }
        return size;
    }

    // return the front element
    T& front() {
        assert( head_ != nullptr );
        return head_->val_;
    }

    T& back() {
        assert( tail_ != nullptr );
        assert( tail_->next_ == nullptr );
        return tail_->val_;
    }

    // pop the front element
    void pop_front() {
        assert( head_ );
        head_ = head_->next_;
        if( head_ == nullptr )
            tail_ = head_;
    }

    // push back element
    void push_back(const T& val) {
        node* n = new node(val, nullptr);
        if( !tail_ ) {
            assert( !head_ );
            head_ = tail_ = n;
        } else {
            tail_->next_ = n;
            tail_ = tail_->next_;
        }
    }

    void remove(const T& val) {
        node* prev = nullptr;
        node* p = head_;
        node** pp = &head_;
        while( p ) {
            if( p->val_ == val ) {
                *pp = p->next_;
                p = *pp;
                if( !*pp )
                    tail_ = prev;
            } else {
                prev = p;
                pp = &p->next_;
                p = p->next_;
            }
        }
    }
private:
    // node class definition
    struct node;
    struct node {
        T val_;
        node *next_;
        // constructor
        node() : val_(), next_(nullptr) {}
        node(const T& value, node* next)
            : val_(value), next_(next) {}
        // non-copyable
        node(const node&) = delete;
        node(node &&) = delete;
        node &operator=(const node&) = delete;
    };
    node* head_;
    node* tail_;
};

// global lock linklist implementation
// for this linklist, before every operation
// should obtain a unique_lock
// with c++11 unique_lock, we needn't manage the lock
// by ourselfves
template<typename T>
class linklist_glock {
public:
    // empty-argument constructor
    explicit linklist_glock() : head_(nullptr), tail_(nullptr), mutex_() { }

    // return whether the list is empty
    bool empty() const {
        return size() == 0;
    }
    // return the size of list
    std::size_t size() const {
        // first obtain the unique_lock
        unique_lock lock(mutex_);
        std::size_t size = 0;
        node* p = head_;
        while( p ) {
            ++size;
            p = p->next_;
        }
        return size;
    }

    // return the front element
    T& front() {
        // first obtain the unique_lock
        unique_lock lock(mutex_);
        assert( head_ != nullptr );
        return head_->val_;
    }

    T& back() {
        // first obtain the unique_lock
        unique_lock lock(mutex_);
        assert( tail_ != nullptr );
        assert( tail_->next_ == nullptr );
        return tail_->val_;
    }

    // pop the front element
    void pop_front() {
        // first obtain the unique_lock
        unique_lock lock(mutex_);
        assert( head_ );
        head_ = head_->next_;
        if( head_ == nullptr )
            tail_ = head_;
    }

    // pop the front element and return the pair
    std::pair<bool,T> try_pop_front() {
        // first obtain the unique_lock
        unique_lock lock(mutex_);
        //assert( head_ );
        if( head_ == nullptr )
            return std::make_pair(false,T());
        T v = head_->val_;
        head_ = head_->next_;
        if( head_ == nullptr )
            tail_ = head_;
        return std::make_pair(true, v);
    }

    // push back element
    void push_back(const T& val) {
        // first obtain the unique_lock
        unique_lock lock(mutex_);
        node* n = new node(val, nullptr);
        if( !tail_ ) {
            assert( !head_ );
            head_ = tail_ = n;
        } else {
            tail_->next_ = n;
            tail_ = tail_->next_;
        }
    }

    void remove(const T& val) {
        // first obtain the unique_lock
        unique_lock lock(mutex_);
        node* prev = nullptr;
        node* p = head_;
        node** pp = &head_;
        while( p ) {
            if( p->val_ == val ) {
                *pp = p->next_;
                p = *pp;
                if( !*pp )
                    tail_ = prev;
            } else {
                prev = p;
                pp = &p->next_;
                p = p->next_;
            }
        }
    }
private:
    // typedef unique_lock
    typedef std::unique_lock<std::mutex> unique_lock;
    // mutex definition
    mutable std::mutex mutex_;
    // node class definition
    struct node;
    struct node {
        T val_;
        node *next_;
        // constructor
        node() : val_(), next_(nullptr) {}
        node(const T& value, node* next)
            : val_(value), next_(next) {}
        // non-copyable
        node(const node&) = delete;
        node(node &&) = delete;
        node &operator=(const node&) = delete;
    };
    node* head_;
    node* tail_;
};


// per-node lock linklist implementation
// this implementation add lock for the node which is different
// from the glock lock
template<typename T>
class linklist_nodelock {
public:
    // empty-argument constructor
    explicit linklist_nodelock() : head_(new node()), tail_(nullptr) { }

    // return whether the list is empty
    bool empty() const {
        return size() == 0;
    }
    // return the size of list
    std::size_t size() const {
        std::size_t size = 0;
        // first obtain the unique_lock of head_
        head_->mutex_.lock();
        node* prev = head_;
        node* p = head_->next_;
        while( p ) {

            // if we don't lock current node
            // it may be deleted after we unlock prev
            // node
            p->mutex_.lock();
            prev->mutex_.unlock();
            ++size;

            prev = p;
            p = p->next_;
        }
        prev->mutex_.unlock();
        return size;
    }

    // return the front element
    T& front() {
        // first obtain the unique_lock of head_
        unique_lock lock(head_->mutex_);
        node* p = head_->next_;
        assert( p != nullptr );
        return p->val_;
    }

    T& back() {
        // first obtain the unique_lock of tail_
        unique_lock lock(tail_->mutex_);
        assert( tail_ != nullptr );
        assert( tail_->next_ == nullptr );
        return tail_->val_;
    }

    // pop the front element
    void pop_front() {
        // first obtain the unique_lock of head_
        unique_lock lock(head_->mutex_);
        node* p = head_->next_;
        assert( p );

        // lock current node
        unique_lock lock0(p->mutex_);
        bool isTail = !p->next_;

        if( isTail ) {

            assert( head_->next_ == p );
            if( p->next_ ) {
                // no longer tail, retry
                return pop_front();
            }
            assert( tail_ == p );
        }
        head_->next_ = p->next_;
        if( isTail ) {
            tail_ = head_->next_;
        }
    }

    // pop the front element and return the pair
    std::pair<bool,T> try_pop_front() {
        // first obtain the unique_lock
        unique_lock lock(head_->mutex_);
        node* p = head_->next_;
        //assert( head_ );
        if( p == nullptr )
            return std::make_pair(false,T());
        unique_lock lock0(p->mutex_);

        T v = p->val_;
        bool isTail = !p->next_;
        if( isTail ) {
            assert(head_->next_ == p);
            if( p->next_ ) {
                // no longer tail, retry
                return try_pop_front();
            }
            assert( tail_ == p );
        }

        head_->next_ = p->next_;
        if( isTail )
            tail_ = head_;
        return std::make_pair(true, v);
    }

    // push back element
    void push_back(const T& val) {
        // first obtain the unique_lock
        unique_lock lock(head_->mutex_);
        node* n = new node(val, nullptr);
        if( !tail_ ) {
            assert( !head_->next_ );
            head_->next_ = tail_ = n;
        } else {
            unique_lock lock(tail_->mutex_);
            tail_->next_ = n;
            tail_ = tail_->next_;
        }
    }

    void remove(const T& val) {
        // first obtain the unique_lock
        head_->mutex_.lock();

        node* prev = head_;
        node* p = head_->next_;

        while( p ) {
            p->mutex_.lock();
            if( p->val_ == val ) {
                bool isTail = !p->next_;
                if( isTail ) {
                    assert(tail_ == p);
                }
                prev->next_ = p->next_;
                if( isTail ) {
                    tail_ = prev;
                }
                p->mutex_.unlock();
                p = prev->next_;
            } else {
                prev->mutex_.unlock();
                prev = p;
                p = p->next_;
            }
        }
        prev->mutex_.unlock();
    }
private:
    // typedef unique_lock
    typedef std::unique_lock<std::mutex> unique_lock;
    // node class definition
    struct node;
    struct node {
        // mutex definition
        mutable std::mutex mutex_;
        T val_;
        node *next_;
        // constructor
        node() : val_(), next_(nullptr) {}
        node(const T& value, node* next)
            : val_(value), next_(next) {}
        // non-copyable
        node(const node&) = delete;
        node(node &&) = delete;
        node &operator=(const node&) = delete;
    };
    node* head_; // head is a sentinel node
    node* tail_;
};


#define CAS(addr, oldVal, newVal) __sync_bool_compare_and_swap(addr, oldVal, newVal)

template<typename T>
class linklist_lockfree {
public:
    // empty-argument constructor
    explicit linklist_lockfree() : head_(new node()), tail_(nullptr) { }

    // return whether the list is empty
    bool empty() const {
        return size() == 0;
    }
    // return the size of list
    std::size_t size() const {
        std::size_t size = 0;
        node* p = head_->next();
        while( p ) {
            if( !p->isMarked() )
                ++size;
            else{
            }
            p = p->next();
        }
        return size;
    }

    // return the front element
    T& front() {
        assert( !head_->isMarked() );
        node* p = head_->next();
        assert( p != nullptr );
        if( p->isMarked() )
            return front();
        T& val = p->val_;
        if( p->isMarked() )
            return front();

        if( !p->next() && tail_ != p )
            tail_ = p;
        return val;
    }

    T& back() {
        assert( !head_->isMarked() );
        assert( tail_ != nullptr );
        if( tail_->next() )
            return back();
        if( tail_->isMarked() )
            return back();
        return tail_->val_;
    }

    // pop the front element
    void pop_front() {

        assert( !head_->isMarked() );
        node* p = head_->next();
        //assert( p );

        if( p == nullptr )
            return;
        if( p->isMarked() )
            return pop_front();
        // mark current node
        p->mark();
        if( !p->isMarked() )
            return pop_front();

        //node* pp = head_->next();
        if( !CAS(&head_->next_ , p, p->next()) )
            return pop_front();

        CAS(&tail_, p, head_->next());
    }

    // pop the front element and return the pair
    std::pair<bool,T> try_pop_front() {

        assert( !head_->isMarked() );
        node* p = head_->next();
        //assert( p );
        if( p == nullptr )
            return std::make_pair(false,T());
        if( p->isMarked() )
            return try_pop_front();

        p->mark();
        if( !p->isMarked() )
            return try_pop_front();
        //node* pp = head_->next();
        if( !CAS(&head_->next_, p, p->next()) )
            return try_pop_front();
        CAS(&tail_, p, head_->next());

        return std::make_pair(true, p->val_);
    }

    // push back element
    void push_back(const T& val) {

        //assert( !head_->isMarked() );

        node* n = new node(val, nullptr);
        if( !tail_ ) {
            if( !head_->next() ) {
                if( !CAS(&head_->next_, tail_, n) )
                    return push_back(val);
                tail_ = head_->next_;
            } else
                return push_back(val);
        } else {
            if( tail_->next() || tail_->isMarked() )
                return push_back(val);
            tail_->setNext(n);
            tail_ = tail_->next();
        }
    }

    void remove(const T& val) {

        node* prev = head_;
        node* p = head_->next();

        while( p ) {
            if( p->val_ == val ) {
                p->mark();
                if( !CAS(&prev->next_, p, p->next()) )
                    return remove(val);
                CAS(&tail_, p, head_->next());
                p = prev->next();
            } else {
                prev = p;
                p = p->next();
            }
        }
    }
private:
    // node class definition
    struct node;
    struct node {
        T val_;
        node* next_;
        // constructor
        node() : val_() , next_(nullptr) {}
        node(const T& value,node* p) : val_(value), next_(p) {}
        // return if current node is marked
        inline bool isMarked() {
            return intptr_t(next_) & 0x1;
        }
        inline void mark() {
           next_ = (node*)(intptr_t(next_) | 0x1);
        }
        inline node* next() {
            return (node*)(intptr_t(next_) & ~0x1);
        }
        inline void setNext(node* ptr) {
            next_ = ptr;
        }
        // non-copyable
        node(const node&) = delete;
        node(node &&) = delete;
        node &operator=(const node&) = delete;
    };
    node* head_; // head is a sentinel node
    node* tail_;
};
#endif
