
#include <memory>
#include <iostream>

template <typename T>
struct ControlBlock{
    int shared_ptr;
    int weak_ptr;
    T* ptr;
    std::function<void(T*)> deleter;  

    template <typename Deleter = std::default_delete<T>> // std::default_delete<T> is a class/functor object
    ControlBlock(int s,int w, T* p, Deleter d)  // storing it safely requires std::function.
    :   shared_ptr(s),
        weak_ptr(w),
        ptr(p){
            deleter  = [d](T* obj) { d(obj);};
    }

};

template <typename T>
class SharedPtr{
    T* ptr;
    ControlBlock<T>* cb;
public:
    SharedPtr(T* p){
        ptr = p;
        cb = new ControlBlock(1,0,ptr);

    }

    SharedPtr(const SharedPtr& other) {
        ptr = other.ptr;
        cb = other.cb;
        cb->shared_ptr++;
    }

    void swap(SharedPtr& other){
        std::swap(ptr,other.ptr);
        std::swap(cb,other.cb);

    }
    T* get(){
        return ptr;
    }
    SharedPtr& operator=(const SharedPtr& other){
        ptr = other.ptr;
        cb = other.cb;
    }
    template <typename T>
    T& operator->() const{
        return ptr;
    }
    template <typename T>
    T& operator[](int i){
        return *(ptr + i);
    }

    int use_count(){
        return cb->shared_ptr;
    }

    void lreset(){
        this.cb->shared_ptr = 0;
        this.cb->weak_ptr   = 0;
        this.cb->ptr = nullptr;
        this.cb->deleter = delete;
    }
    T& operator*() const {
        return *ptr;  
    }
    bool unique() const{
        return cb->shared_ptr == 1;
    }
    ~SharedPtr() {
        cb->shared_ptr--;
        if (cb->shared_ptr == 0) {
            cb->deleter(ptr);  
            if (cb->weak_ptr == 0)
                delete cb;  
        }
    }

};