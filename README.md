# DSA practice

Data structures built from scratch: a byte list written in C (three interchangeable implementations),
a C++ `SimpleArray<T>` wrapper on top of it, and stacks, queues and deques built from that wrapper.

## Building and testing

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Executables end up in `build/S1/` and `build/S2/` (with `.exe` on Windows).
Warnings (`-Wall -Wextra`, or `/W4` on MSVC) are on by default; turn them off with `-DDSA_WARNINGS=OFF`.

Every program is built once per list implementation, so the same code can be compared on linked and contiguous storage.

## Project layout

```
include/utils.h              STATUS codes, Exception and the check_st_ / RETURN_IF_FAILED helpers
include/S1/simplearray.hpp   SimpleArray<T>: typed C++ wrapper over the C byte list
include/S2/simplestack.hpp   SimpleStack<T>
include/S2/simplequeue.hpp   SimpleQueue<T>
include/S2/simpledeque.hpp   SimpleDeque<T>
include/S2/manualinput.hpp   Input parsing shared by the S2 "manual input" programs
S1/                          Byte list implementations, C tester and the student sample
S2/                          Bracket tester, manual stack / queue / list programs and container tests
S1/tests, S2/tests           Input files (*.in) and expected outputs (*.out) run by ctest
cmake/RunIOTest.cmake        Runs a program with an input file and compares its output
```

## Session 1

**Arrays and lists**
```c
typedef struct _SimpleByteArray SimpleByteArray;

STATUS SimpleByteArray_init( SimpleByteArray **out_list );
STATUS SimpleByteArray_free( SimpleByteArray *in_list );
STATUS SimpleByteArray_insert( SimpleByteArray *list, size_t index, BYTE value );
STATUS SimpleByteArray_insert_bytes( SimpleByteArray *list, size_t index, const BYTE *values, size_t count );
STATUS SimpleByteArray_remove( SimpleByteArray *list, size_t index, BYTE *opt_removed );
STATUS SimpleByteArray_get( const SimpleByteArray *list, size_t index, BYTE *value );
STATUS SimpleByteArray_size( const SimpleByteArray *list, size_t *value );
```

We're testing a simple byte array. Bytes can be conformed to other, longer types as well.

Every function returns a `STATUS`: `SUCCESS`, `POINTER` for a null argument, `BOUNDS` for a bad index,
`OUTOFMEMORY` when allocation fails. Diagnostics are written to `stderr`, so they never mix with program output.
`SimpleByteArray_insert_bytes` with `count == 0` is a no-op, and a failed insertion leaves the list unchanged.

### Contiguous lists

`S1/contiguouslists.c`

Involves a contiguous **dynamic** array that grows and shrinks automatically.

| Time Complexity  | Beginning | Arbitrary | Ending |
|------------------|----------:|----------:|-------:|
| Access           | O(1)      | O(1)      | O(1)   |
| Insertion        | O(n)      | O(n)      | O(1)*  |
| Removal          | O(n)      | O(n)      | O(1)** |

*: Could become O(n) if capacity is grown.  
**: Could become O(n) if capacity is shrunk.

`insert_bytes` grows the array once and shifts the existing bytes once, so inserting `k` bytes costs O(n + k), not O(n·k).

### Linked lists

`S1/linkedlists.c`

Involves a singly linked list with head and tail pointers.

| Time Complexity  | Beginning | Arbitrary | Ending |
|------------------|----------:|----------:|-------:|
| Access           | O(1)      | O(n)      | O(1)   |
| Insertion        | O(1)      | O(n)      | O(1)   |
| Removal          | O(1)      | O(n)      | O(n)   |

`insert_bytes` builds the new nodes first and splices them in with a single walk.

### Doubly linked lists

`S1/doublylinkedlists.c`

Same as the linked list, but each node also points to the previous one. Lookups walk from whichever end is closer.

| Time Complexity  | Beginning | Arbitrary | Ending |
|------------------|----------:|----------:|-------:|
| Access           | O(1)      | O(n)      | O(1)   |
| Insertion        | O(1)      | O(n)      | O(1)   |
| Removal          | O(1)      | O(n)      | O(1)   |

***All three list implementations are interchangeable: they implement the same functions.***

### SimpleArray&lt;T&gt;

`include/S1/simplearray.hpp`

Stores any trivially copyable `T` as raw bytes in the list. `std::string` is also supported and is stored as `[length][characters]`.
It provides `Insert`, `Remove`, `Get`, `Set`, `Find`, `Size`, `Empty`, `operator[]` and `operator<<` (push front).
Copying is disabled because the array owns its C list; use `std::move` to transfer it.

### Tester

`S1/tester.c`

Tests various different aspects of the byte lists, including invalid arguments, splicing in the middle,
and growing and shrinking. Built as `LinkedListsTests`, `DoublyLinkedListsTests` and `ContiguousListsTests`.

### Sample

`S1/sample.cpp`

Tests a real world sample: a student menu built on `SimpleArray<Student>`.
It runs a self-test of `SimpleArray` first, and re-asks when the input is not a number.

## Session 2

Stacks, queues and deques built on `SimpleArray<T>`. Each program is built twice, for example
`ManualStack_LinkedStacks` and `ManualStack_ContiguousStacks`.

| Container        | Operation               | Linked lists | Contiguous lists |
|------------------|-------------------------|-------------:|-----------------:|
| `SimpleStack`    | Push / Pop / Top        | O(1) (head)  | O(1)* (end)      |
| `SimpleQueue`    | Push                    | O(1)         | O(1)*            |
| `SimpleQueue`    | Pop / Front             | O(1)         | O(n)             |
| `SimpleDeque`    | PushFront / PopFront    | O(1)         | O(n)             |
| `SimpleDeque`    | PushBack                | O(1)         | O(1)*            |
| `SimpleDeque`    | PopBack                 | O(n)         | O(1)*            |

*: Amortized. Elements are stored byte by byte, so every cost is also multiplied by `sizeof(T)`
(or the string's length for `std::string`).

The stack stores its top at whichever end is cheap for the backing list, chosen with `LINKED_LISTS` or `CONTIGUOUS_LISTS`.
Building it without either is a compile error.

### Question 1: Bracket tester

`S2/brackettester.cpp`

Reads one line and prints `YES` if every `()`, `[]` and `{}` is properly nested, otherwise `NO`.
Other characters, including spaces, are ignored.

```
in:  { [ ( ) ( ) ] }      out: YES
in:  ([)]                 out: NO
```

### Question 2: Manual stack

`S2/manualstack.cpp`

The first line is the number of operations, followed by one operation per line:
`PUSH <value>`, `POP`, `TOP`, `SIZE`, `EMPTY`. `POP` and `TOP` print `EMPTY` on an empty stack,
and `EMPTY` prints `YES` or `NO`. Values may contain spaces.

```
in:              out:
4                b
PUSH a           2
PUSH b
TOP
SIZE
```

### Question 3: Manual queue

`S2/manualqueue.cpp`

Same input format as the stack, with `ENQUEUE <value>`, `DEQUEUE`, `FRONT`, `SIZE`, `EMPTY`.

### Question 4: Manual list

`S2/manuallist.cpp`

The first line is the number of operations, followed by:

| Command               | Output                                   |
|-----------------------|------------------------------------------|
| `APPEND <value>`      | nothing                                  |
| `INSERT <index> <v>`  | `ERROR` if the index is out of range     |
| `DELETE <index>`      | the removed value, or `ERROR`            |
| `GET <index>`         | the value, or `ERROR`                    |
| `SET <index> <value>` | `ERROR` if the index is out of range     |
| `FIND <value>`        | the first index, or `-1`                 |
| `SIZE`                | the number of elements                   |
| `PRINT`               | the elements separated by spaces, or `EMPTY` |

Indices must be non-negative whole numbers. A command with a missing or invalid argument prints `ERROR` and never changes the list.

All three manual programs print their results after reading every operation, tolerate Windows line endings,
and exit with an error message (instead of crashing) when the operation count is missing or invalid.

### Container tests

`S2/tester.cpp`

Unit tests for `SimpleStack`, `SimpleQueue` and `SimpleDeque`, built as `ContainerTests_Linked` and `ContainerTests_Contiguous`.
