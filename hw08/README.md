# hw08: Binary Search Tree

`bst` is a binary search tree of `int` keys. Keys smaller than a node go into
its left subtree; keys greater than or equal to it go into its right subtree, so
duplicate keys are kept. Its insert and search are ported from
`BinarySearchTree.c`. `treestats` uses it to report the size and height of the
tree built from its input.

| Module                   | Purpose                                                        |
|--------------------------|----------------------------------------------------------------|
| `bst.h` / `bst.c`        | `bst_t` and its functions: create, destroy, insert, contains, size, and height |
| `treestats.c`            | `./treestats < file`: builds a tree from the integers in a file and prints its size and height |
| `tests/test_bst.c`       | Unit tests for `bst`                                           |
| `BinarySearchTree.c`     | The original code `bst_insert` and `bst_contains` are ported from; not built |

## Building and Testing

```sh
make           # compile bst.c and treestats.c, link ./treestats and tests/test_bst
make test      # build tests/test_bst and run the unit tests
make memcheck  # make clean, then make test with every file rebuilt under the sanitizers
make clean     # remove all .o files, ./treestats, and tests/test_bst
```

`make`, `make test`, and `make clean` compile with `gcc -std=c11 -Wall -Wextra
-Wshadow`, with no warnings. `make memcheck` adds `-fsanitize=address,undefined
-fno-sanitize-recover=all`, so a memory error, a leak, or undefined behavior
stops the program and makes the target fail. Each `.o` file is rebuilt when its
`.c` file or `bst.h` changes. `make memcheck` leaves sanitized objects behind,
so run `make clean` before a plain `make`.

**`./treestats < file`** reads integers from standard input until the end of the
input, inserting them into a tree in the order read. Integers may be separated
by any whitespace, including newlines. It prints one line to `stdout`:

| Output                          | When                                      | Exit status |
|---------------------------------|-------------------------------------------|-------------|
| `n 7, height 3`                 | The input held integers (here, 7 of them) | 0           |
| `empty tree`                    | The input held no integers                | 0           |
| `not a number after 4 values`   | Something other than an integer was found after 4 integers | 1 |

If the tree cannot be created or a key cannot be inserted, it prints a message
to `stderr` instead and exits with status 1. Integers outside the range of an
`int` are not detected.

**`tests/test_bst`** tests:

- The seven keys from `BinarySearchTree.c` (27 14 35 10 19 31 42): size 7 and
  height 3; 31 is found in 3 visits and 15 is not found in 3 visits.
- A duplicate key is kept: inserting 5 twice gives size 2.
- Keys inserted in increasing order (0 to 99) give a height equal to their
  number.
- `NULL` arguments: a `NULL` tree for every function, `visits == NULL` for
  `bst_contains`, and `bst_destroy(NULL)`.
- Destroying a full tree, duplicates included (leaks are found by
  `make memcheck`).
- `bst_contains` on an empty tree, which crashed in the original port.

Each test prints `[PASS]` or `[FAIL]`, then the expected and actual results on
the next two lines, followed by a blank line. At the end it prints a summary of
how many tests passed and failed, and exits with a non-zero status if any test
failed.

## bst.h

### Types

| Type    | Description                                                          |
|---------|----------------------------------------------------------------------|
| `bst_t` | A binary search tree of `int`s. Its fields are hidden in `bst.c`; use it only through a pointer and the functions below. |

### `bst_create`

```c
bst_t *bst_create(void);
```

Creates a new, empty tree. Must be freed with `bst_destroy`; the caller should
not free it themselves.

**Returns:** `NULL` if there was not enough memory available to create the tree.
Otherwise, a pointer to the new tree.

### `bst_destroy`

```c
void bst_destroy(bst_t *t);
```

Frees the tree and all of its nodes. If `t` is `NULL`, does nothing.

| Parameter | Description                          |
|-----------|--------------------------------------|
| `t`       | The address of the tree to destroy.  |

### `bst_insert`

```c
int bst_insert(bst_t *t, int key);
```

Adds `key` to the tree. Smaller keys go left, all others go right. Duplicate
keys are kept. If `t` is `NULL` or there is not enough memory available, does
nothing. Ported from `insert` in `BinarySearchTree.c`.

| Parameter | Description                          |
|-----------|--------------------------------------|
| `t`       | The address of the tree to add to.   |
| `key`     | The value to add to the tree.        |

**Returns:** -1 if `t` is `NULL` or if there is not enough memory available to
insert `key`. Otherwise, 0.

### `bst_contains`

```c
bool bst_contains(const bst_t *t, int key, size_t *visits);
```

Determines whether `key` is in the tree. If there are duplicate values, the
search stops at the first match. Ported from `search` in `BinarySearchTree.c`.

| Parameter | Description                                                      |
|-----------|------------------------------------------------------------------|
| `t`       | The address of the tree to search.                               |
| `key`     | The value to check for.                                          |
| `visits`  | The address of the variable in which to store the number of nodes compared (whether the value is found or not). May be `NULL`; if it is, the value is discarded. If `t` is `NULL` and `visits` is not, `*visits` is set to 0. |

**Returns:** `false` if `t` is `NULL`. Otherwise, `true` if `key` is in the tree
and `false` if it is not.

### `bst_size`

```c
size_t bst_size(const bst_t *t);
```

Determines the number of keys in the tree. Counts all keys, including
duplicates.

| Parameter | Description                                  |
|-----------|----------------------------------------------|
| `t`       | The address of the tree to count the keys of. |

**Returns:** If `t` is `NULL`, 0. Otherwise, the number of keys in the tree.

### `bst_height`

```c
size_t bst_height(const bst_t *t);
```

Determines the number of nodes on the longest path from the root of the tree
down (i.e. the height of the tree). An empty tree gives 0, and a tree with one
node gives 1.

| Parameter | Description                                  |
|-----------|----------------------------------------------|
| `t`       | The address of the tree to find the height of. |

**Returns:** If `t` is `NULL`, 0. Otherwise, the height of the tree.

# Assignment Prompts
## bst_contains Error
With address sanitization, the original port of `search` gave:
```c
bst.c:96:19: runtime error: member access within null pointer of type 'const struct node_t'
```
The original never encounters this issue (attempting to dereference a NULL
pointer) because it never tests a case where the root is a NULL pointer.
## treestats
Piping `seq 1 1023` into ./treestats gave:
```
n 1023, height 1023
```
Piping `seq 1 1023 | sort -R` into ./treestats gave:
```
n 1023, height 23
```
In the first case, each key inserted is greater than the last, meaning that the
tree grows exclusively down and to the right with each insertion; conversely,
the randomness of values in the shuffled version make the tree much shorter but
much more complex. This leads to the latter case requiring fewer visits per
search on average.
