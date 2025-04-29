# SharedPtr Implementation

This is a custom implementation of a `SharedPtr` (a smart pointer) that manages the lifetime of dynamically allocated objects through reference counting. It allows for automatic memory management by ensuring the object is deleted when the last shared pointer to it is destroyed or reset.

## Files

- `shared_ptr.hpp` (contains the implementation)

## Overview

The `SharedPtr` class is a smart pointer that manages an object using reference counting. It provides features like:
- Automatic object deletion when the last reference is released.
- Safe copy and move semantics.
- Custom deleter support.
- Swap and reset functionality.

## Key Components

### `ControlBlock<T>`
This structure holds the reference counts (`shared_count` and `weak_count`), a pointer to the managed object (`ptr`), and a deleter function (`deleter`) that is used when the object is deleted. 

**Constructor:**
- Initializes the reference counts, assigns the pointer to the managed object, and sets the custom deleter (default is `std::default_delete`).

### `SharedPtr<T>`
The main class managing the lifetime of a dynamically allocated object.

#### Private Members:
- `T* ptr`: A raw pointer to the managed object.
- `ControlBlock<T>* cb`: A pointer to the `ControlBlock` that tracks reference counts and handles deletion.

#### Public Methods:

- **Default Constructor (`SharedPtr()`)**:
  - Creates an empty `SharedPtr`, meaning it points to `nullptr`.
  
- **Parameterized Constructor (`explicit SharedPtr(T* p, std::function<void(T*)> deleter = std::default_delete<T>())`)**:
  - Initializes a `SharedPtr` by taking a raw pointer to an object and an optional custom deleter.

- **Copy Constructor (`SharedPtr(const SharedPtr& other)`)**:
  - Copies the `ptr` and `cb` from another `SharedPtr`. The `shared_count` is incremented to reflect an additional reference to the object.

- **Move Constructor (`SharedPtr(SharedPtr&& other) noexcept`)**:
  - Moves the `ptr` and `cb` from another `SharedPtr` without increasing the reference count. After the move, the original `SharedPtr` is left in a valid empty state (`nullptr`).

- **Destructor (`~SharedPtr()`)**:
  - Decrements the `shared_count` and deletes the managed object if it was the last reference.

- **Copy Assignment Operator (`operator=(const SharedPtr& other)`)**:
  - Creates a temporary copy of `other` and swaps the contents of the current object with the temporary one. This avoids resource duplication and ensures exception safety.

- **Move Assignment Operator (`operator=(SharedPtr&& other) noexcept`)**:
  - Releases the current resources and takes ownership of `other`'s resources, leaving `other` in a valid empty state.

- **Reset (`reset(T* p = nullptr)`)**:
  - Resets the managed object to a new pointer (or `nullptr`). The old object, if any, is properly deleted.

- **Swap (`swap(SharedPtr& other)`)**:
  - Swaps the managed object between two `SharedPtr` instances.

- **Getters and Dereference Operators**:
  - `get()`: Returns the raw pointer to the managed object.
  - `operator*()`: Dereferences the managed object.
  - `operator->()`: Allows access to the managed object's members.
  - `operator[](int i)`: Access elements if the object is an array.

- **use_count()**:
  - Returns the number of `SharedPtr` instances managing the same object.

- **unique()**:
  - Checks if the current `SharedPtr` is the only one managing the object.

## Example Usage

```cpp
int main() {
    // Creating a SharedPtr to an integer
    SharedPtr<int> p1(new int(42));
    std::cout << "*p1: " << *p1 << "\n"; // Dereferencing p1
    std::cout << "use_count: " << p1.use_count() << "\n"; // Checking reference count

    // Copy constructor
    SharedPtr<int> p2 = p1;
    std::cout << "After copy, p1.use_count: " << p1.use_count() << "\n"; 
    std::cout << "p2.use_count: " << p2.use_count() << "\n"; 

    // Move constructor
    SharedPtr<int> p3 = std::move(p2);
   

    // Copy assignment
    SharedPtr<int> p4;
    p4 = p3;
    std::cout << "After copy assignment, p4.use_count: " << p4.use_count() << "\n";

    // Move assignment
    SharedPtr<int> p5;
    p5 = std::move(p4);

    // Resetting the pointer
    p5.reset(new int(99));
    std::cout << "After reset, *p5: " << *p5 << ", use_count: " << p5.use_count() << "\n";

    // Swapping two SharedPtrs
    SharedPtr<int> p6(new int(77));
    p5.swap(p6);


    // Using a custom deleter for arrays
    SharedPtr<int> p7(new int[5]{10, 20, 30, 40, 50}, [](int* p) { delete[] p; });
}
```



## Build Instructions

1. Clone the repository:
   ```
   git clone https://github.com/username/shared_ptr_implementation
   ```
2. Navigate to the repository:
   ```
   cd shared_ptr_implementation
   ```
3. Generate build files:
   ```
   cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
   ```
4. Build the project:
   ```
   cmake --build build --config Release
   ```
5. Run the executable from the build directory:
   ```
   ./build/shared_ptr_implementation
   ```
