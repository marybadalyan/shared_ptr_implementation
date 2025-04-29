#include <functional>
#include <iostream>
#include <memory>
template <typename T>
struct ControlBlock {
    int shared_count;
    int weak_count;
    T* ptr;
    std::function<void(T*)> deleter;

    template <typename Deleter = std::default_delete<T>>
    ControlBlock(T* p, Deleter d = Deleter())
        : shared_count(1), weak_count(0), ptr(p), deleter([d](T* obj) { d(obj); }) {}
};

template <typename T>
class SharedPtr {
private:
    T* ptr = nullptr;
    ControlBlock<T>* cb = nullptr;

    void release() noexcept{
        if (cb) {
            if (--cb->shared_count == 0) {
                cb->deleter(ptr);
                if (cb->weak_count == 0)
                    delete cb;
            }
        }
        ptr = nullptr;
        cb = nullptr;
    }

public:
    SharedPtr() = default;

    explicit SharedPtr(T* p, std::function<void(T*)> deleter = std::default_delete<T>()) //explisit in case it gets assigned to a T*
        : ptr(p), cb(new ControlBlock<T>(p, deleter)) {}

    SharedPtr(const SharedPtr& other)
        : ptr(other.ptr), cb(other.cb) {
        if (cb) cb->shared_count++;
    }
    //move constructor
    SharedPtr(SharedPtr&& other) noexcept //mark noexcept; otherwise, it falls back to copy constructor 
    : ptr(other.ptr), cb(other.cb) {
    other.ptr = nullptr;
    other.cb = nullptr;
    }

    ~SharedPtr() {
        release();
    }
    //copy assignment
    SharedPtr& operator=(const SharedPtr& other) {
        SharedPtr temp(other);   
        swap(temp);              
        return *this;          
    }

    //move assignment
    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            release();           //won't throw
            ptr = other.ptr;
            cb = other.cb;
            other.ptr = nullptr;
            other.cb = nullptr;
        }
        return *this;
    }

    void reset(T* p = nullptr) {
        release();
        if (p) {
            ptr = p;
            cb = new ControlBlock<T>(p);
        }
    }

    void swap(SharedPtr& other) {
        std::swap(ptr, other.ptr);
        std::swap(cb, other.cb);
    }

    T* get() const {
        return ptr;
    }

    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }

    T& operator[](int i) const {
        return ptr[i];
    }

    int use_count() const {
        return cb ? cb->shared_count : 0;
    }

    bool unique() const {
        return use_count() == 1;
    }
};


int main() {
    SharedPtr<int> p1(new int(42));
    std::cout << "*p1: " << *p1 << "\n";
    std::cout << "use_count: " << p1.use_count() << "\n\n";

    // Copy constructor
    SharedPtr<int> p2 = p1;
    std::cout << "After copy, p1.use_count: " << p1.use_count() << "\n";
    std::cout << "p2.use_count: " << p2.use_count() << "\n\n";

    // Move constructor
    SharedPtr<int> p3 = std::move(p2);
    std::cout << "After move, p3.use_count: " << p3.use_count() << "\n";
    std::cout << "p2.use_count (should be 0): " << p2.use_count() << "\n\n";

    // Copy assignment
    SharedPtr<int> p4;
    p4 = p3;
    std::cout << "After copy assignment, p4.use_count: " << p4.use_count() << "\n\n";

    // Move assignment
    SharedPtr<int> p5;
    p5 = std::move(p4);
    std::cout << "After move assignment, p5.use_count: " << p5.use_count() << "\n";
    std::cout << "p4.use_count (should be 0): " << p4.use_count() << "\n\n";

    // Reset
    p5.reset(new int(99));
    std::cout << "After reset, *p5: " << *p5 << ", use_count: " << p5.use_count() << "\n\n";

    // Swap
    SharedPtr<int> p6(new int(77));
    std::cout << "Before swap: *p5 = " << *p5 << ", *p6 = " << *p6 << "\n";
    p5.swap(p6);
    std::cout << "After swap: *p5 = " << *p5 << ", *p6 = " << *p6 << "\n";

    SharedPtr<int> p7(new int[5]{10, 20, 30, 40, 50}, [](int* p) { delete[] p; }); //use cutum deleter
    std::cout << "p7[2]: " << p7[2] << "\n";
    std::cout << "use_count: " << p7.use_count() << "\n";

    SharedPtr<int> p8 = p7;
    std::cout << "After copy, use_count: " << p7.use_count() << "\n";

    p2.reset();
}
