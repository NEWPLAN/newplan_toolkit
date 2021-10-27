## INTRODUCTION:
bfc_allocator is a simple memory pool implemented for C++ programmer to manage memory in an easy way.



## COMPILE:
```bash
make
```

## USAGE:
### example
```cpp
#include "bfc_allocator.h"
#include <iostream>
using namespace std;

int main()
{
    memory::BfcAllocator &allocator = memory::Allocator();

    void *r1 = allocator.allocate(100);
    cout << "after allocating 100 bytes, allocated bytes: " << allocator.allocatedBytes()
         << ", cached bytes: " << allocator.cachedBytes() << endl;
    void *r2 = allocator.allocate(100 * 1000 * 1000);
    cout << "After allocating r2(100000000) bytes, allocated bytes: " << allocator.allocatedBytes()
         << ", cached bytes: " << allocator.cachedBytes() << endl;

    allocator.release(r2);
    cout << "After releasing r2(100000000), allocated bytes: " << allocator.allocatedBytes()
         << " " << allocator.cachedBytes() << endl;
    allocator.release(r1);
    cout << "After releasing r1(100), allocated bytes: " << allocator.allocatedBytes()
         << ", cached bytes: " << allocator.cachedBytes() << endl;

    void *p3 = allocator.allocate(100 * 1000 * 1000);
    cout << "After releasing r3(100000000), allocated bytes: " << allocator.allocatedBytes()
         << " " << allocator.cachedBytes() << endl;

    allocator.release(p3);
    cout << "After releasing r3(100000000), allocated bytes: " << allocator.allocatedBytes()
         << ", cached bytes: " << allocator.cachedBytes() << endl;
}
```

### result
```bash
(main)> ./bfcallocator_benchmark                                                                        
after allocating 100 bytes, allocated bytes: 100, cached bytes: 1048576
After allocating r2(100000000) bytes, allocated bytes: 100000100, cached bytes: 135266304
After releasing r2(100000000), allocated bytes: 100 135266304
After releasing r1(100), allocated bytes: 0, cached bytes: 135266304
After releasing r3(100000000), allocated bytes: 100000000 135266304
After releasing r3(100000000), allocated bytes: 0, cached bytes: 135266304
(main)>      
```