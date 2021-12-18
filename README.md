# CHeap
A simple, cross platform heap allocator. The manager uses a naive
linked list to manage used addresses.

## Build
To install the build system, just install scons using python3 pip

There are two targets
```
scons static
scons test
```

There is also a debug flag
```
debug=1
```

### Static Target
The static target will create a static library named CHeap

### Test Target
The test target will create a program named CHeapTest

## Integrate into project
You can integrate CHeap into any C/C++ project. Just copy all the
files from src and include