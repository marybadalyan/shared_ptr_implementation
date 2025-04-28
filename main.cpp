
template <typename T>
struct ControlBlock{
    int shared_ptr;
    int weak_ptr;
    T* ptr;
    void (*deleter)(*T);

    template <typename Deleter>
    ControlBlock(int s = 0,int w = 0, T* p  = nullptr, Deleter d) 
    :   shared_ptr(s), 
        weak_ptr(w),
        ptr(p){
        d = [d](T* obj) { d(obj);}
    }

};

template <typename T>
class SharedPtr{
    T* ptr;
    ControlBlock cb;

    SharedPtr(T* p): p(ptr){
        ptr = p;
        cb = new ControlBlock(1,0,ptr,);

    }

    SharedPtr(const SharedPtr& other) {
        ptr = other.ptr;
        cb = other.cb;
        cb->shared_ptr++;
    }

    ~SharedPtr() {
        control->shared_ptr--;
        if (control->shared_ptr == 0) {
            delete ptr;  
            if (control->weak_ptr == 0)
                delete cb;  
        }
    }

};