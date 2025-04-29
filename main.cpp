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

    void release() {
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

    explicit SharedPtr(T* p)
        : ptr(p), cb(new ControlBlock<T>(p)) {}

    SharedPtr(const SharedPtr& other)
        : ptr(other.ptr), cb(other.cb) {
        if (cb) cb->shared_count++;
    }

    ~SharedPtr() {
        release();
    }

    SharedPtr& operator=(const SharedPtr& other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            cb = other.cb;
            if (cb) cb->shared_count++;
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
    SharedPtr<int> p1(new int[5]{10, 20, 30, 40, 50});
    std::cout << "p1[2]: " << p1[2] << "\n";
    std::cout << "use_count: " << p1.use_count() << "\n";

    SharedPtr<int> p2 = p1;
    std::cout << "After copy, use_count: " << p1.use_count() << "\n";

    p2.reset();
    std::cout << "After reset p2, p1.use_count(): " << p1.use_count() << "\n";
}