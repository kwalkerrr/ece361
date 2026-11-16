# hw07: Integer Stack and Bracket Balancing

`istack` is a stack of `int`s stored in a dynamic array. It starts with room for
`ISTACK_START` (8) values and doubles its array with `realloc` whenever a push
finds it full. `balance` uses it to check that the brackets `()`, `[]`, and `{}`
in its input are balanced.

| Module                   | Purpose                                                        |
|--------------------------|----------------------------------------------------------------|
| `istack.h` / `istack.c`  | `istack_t` and its functions: create, destroy, push, pop, peek, count, is_empty, capacity, and to_array |
| `balance.c`              | `./balance < file`: checks the brackets in a file and prints one result line |
| `tests/test_istack.c`    | Unit tests for `istack`                                        |
| `regress.sh`             | Regression test: compares `./balance` on each input with its expected output |

## Building and Testing

```sh
make           # compile istack.c and balance.c, link ./balance
make test      # build ./balance and tests/test_istack, run the unit tests, then sh regress.sh
make memcheck  # make clean, then make test with every file rebuilt under the sanitizers
make clean     # remove all .o files, ./balance, and tests/test_istack
```

`make`, `make test`, and `make clean` compile with `gcc -std=c11 -Wall -Wextra
-Wshadow`, with no warnings. `make memcheck` adds `-fsanitize=address,undefined
-fno-sanitize-recover=all`, so a memory error, a leak, or undefined behavior
stops the program and makes the target fail. Each `.o` file is rebuilt when its
`.c` file or `istack.h` changes. `make memcheck` leaves sanitized objects
behind, so run `make clean` before a plain `make`.

`make memcheck` finds leaks on Linux only. On macOS, find them with:

```sh
leaks --atExit -- ./tests/test_istack
```

**`./balance < file`** reads standard input one character at a time. It pushes
every `(`, `[`, and `{`. On every `)`, `]`, and `}` it pops the innermost open
bracket and checks that the two match. It stops at the first error and prints
one line to `stdout`: the result, padded to 48 characters, followed by the exit
status. For example:

```
balanced: 6 pairs, deepest 3                     exit status 0
line 1, column 8: found ')', expected ']'        exit status 1
line 1, column 12: found ')', nothing open       exit status 1
end of input: 2 still open, innermost '('        exit status 1
```

- `deepest` is the largest number of brackets that were open at once.
- Lines and columns start at 1. Every byte, including a tab, is one column.
- `expected` names the bracket that would close the innermost open one.
- `end of input` gives the number of brackets still open and the innermost of
  them.

`balance` exits with status 0 if the input is balanced and 1 if it is not. If
the stack cannot be created or grown, it prints a message to `stderr` and exits
with status 1.

**`tests/test_istack`** tests:

- `istack_pop` and `istack_peek` on an empty stack return -1 and leave `*v`
  unchanged.
- Last in, first out: after pushing 1, 2, 3, `istack_peek` gives 3 and three
  pops give 3, 2, 1.
- `istack_count` and `istack_capacity` at 8 values (8 and 8), then at 9 values
  (9 and 16).
- 1000 pushes all succeed, then 1000 pops give 999 down to 0 and leave the stack
  empty.
- `istack_to_array` returns a bottom-first copy; changing the copy does not
  change the stack; on an empty stack it returns `NULL` and sets `*n` to 0.
- `NULL` arguments: a `NULL` stack for every function, `n == NULL` for
  `istack_to_array`, `v == NULL` for `istack_pop` and `istack_peek`, and
  `istack_destroy(NULL)`.
- Two stacks are independent: growing and popping one leaves the other's count,
  top, and capacity unchanged.
- Destroying a stack that still holds values (leaks are found by
  `make memcheck`).

Each test prints `[PASS]` or `[FAIL]`, then the expected and actual results on
the next two lines, followed by a blank line. At the end it prints a summary of
how many tests passed and failed, and exits with a non-zero status if any test
failed. The tests that pass bad arguments also make `istack` print its error
messages to `stderr`.

**`regress.sh`** runs `./balance` on every file in `inputs/` and compares its
output with `diff` against the file of the same name in `expected/`. It prints
`PASS` or `FAIL` per file and exits with 1 if any output differs.

| Input              | What it covers                                                 |
|--------------------|----------------------------------------------------------------|
| `balanced.txt`     | A short C function: 6 pairs, 3 deep, over 4 lines              |
| `deep.txt`         | 10 nested brackets on one line, deeper than `ISTACK_START`     |
| `deep_text.txt`    | 10 nested brackets with text between them, over 3 lines        |
| `mismatch.txt`     | `found ')', expected ']'` on line 3                            |
| `nothing_open.txt` | `found ')', nothing open` on line 2                            |
| `unclosed.txt`     | `end of input` with 3 still open, innermost `[`                |
| `empty.txt`        | An empty file: 0 pairs, deepest 0                              |

## memcheck Bugs
If you attempt to destroy a stack twice, you get the error:
```
==649138==ERROR: AddressSanitizer: heap-use-after-free on address 0x7be5261e0040
at pc 0x5569ab01c4e3 bp 0x7ffd7afb7d70 sp 0x7ffd7afb7d60
```
This occurs because C has already freed the memory used by the stack. Attempting
to free it again by calling destroy is essentially asking the program to operate
on memory that it doesn't own.

Another error occurs if you attempt to push a value onto a stack after
destroying it.
```
==649643==ERROR: AddressSanitizer: heap-use-after-free on address 0x7b5857be0048
at pc 0x564d98940863 bp 0x7fff884ad2c0 sp 0x7fff884ad2b0
```
This error occurs for essentially the same reason; the program no longer owns
the freed memory, so it shouldn't be able to write any values there.

Lastly, a sanitization error occurs when the program exits without destroying
the stack.
```
==650021==ERROR: LeakSanitizer: detected memory leaks
```
This error indicates a memory leak. A memory leak occurs when the program has
allocated memory with `malloc` or `calloc` that it never frees before exiting.
This may not be a problem in the short-term as long as the amount of memory that
leaks is not significant, but memory leaks can add up over time and become
problematic.

## istack.h

### Constants

| Constant       | Value | Meaning                                              |
|----------------|-------|------------------------------------------------------|
| `ISTACK_START` | 8     | The capacity of a new stack; doubles when full       |

### Types

| Type       | Description                                                       |
|------------|-------------------------------------------------------------------|
| `istack_t` | A stack of `int`s. Its fields are hidden in `istack.c`; use it only through a pointer and the functions below. |

### `istack_create`

```c
istack_t *istack_create(void);
```

Creates a new stack. The stack's data structure is a dynamic array with an
initial capacity of `ISTACK_START`. Free the stack after use with
`istack_destroy`.

**Returns:** `NULL` if there was not enough free memory. Otherwise, a pointer to
the new stack.

### `istack_destroy`

```c
void istack_destroy(istack_t *s);
```

Frees all memory associated with the given stack.

| Parameter | Description            |
|-----------|------------------------|
| `s`       | The stack to destroy.  |

### `istack_push`

```c
int istack_push(istack_t *s, int v);
```

Pushes `v` onto the stack. If the stack is full, its array size is doubled with
`realloc`.

| Parameter | Description            |
|-----------|------------------------|
| `s`       | The stack to push to.  |
| `v`       | The value to push.     |

**Returns:** -1 if `s` is `NULL` or there was not enough free memory. Otherwise,
0.

**Errors:** If `s` is `NULL` or there was not enough free memory, a message is
printed to `stderr` and the stack is not changed.

### `istack_pop`

```c
int istack_pop(istack_t *s, int *v);
```

Puts the top value from the stack into `*v` and removes it from the stack.

| Parameter | Description                                                      |
|-----------|------------------------------------------------------------------|
| `s`       | The stack to pop from.                                           |
| `v`       | The address of the variable in which to store the top value. May be `NULL`; the value is still removed. |

**Returns:** -1 if `s` is `NULL` or the stack is empty. Otherwise, 0.

**Errors:** If `s` is `NULL` or the stack is empty, a message is printed to
`stderr` and nothing is changed, including `*v`.

### `istack_peek`

```c
int istack_peek(const istack_t *s, int *v);
```

Puts the top value from the stack into `*v` without removing it.

| Parameter | Description                                                      |
|-----------|------------------------------------------------------------------|
| `s`       | The stack to peek into.                                          |
| `v`       | The address of the variable in which to store the top value. May be `NULL`. |

**Returns:** -1 if `s` is `NULL` or the stack is empty. Otherwise, 0.

**Errors:** If `s` is `NULL` or the stack is empty, a message is printed to
`stderr` and `*v` is not changed.

### `istack_count`

```c
size_t istack_count(const istack_t *s);
```

Gives the number of values on the stack.

| Parameter | Description                          |
|-----------|--------------------------------------|
| `s`       | The stack to count the values of.    |

**Returns:** 0 if `s` is `NULL`. Otherwise, the number of values on the stack.

**Errors:** If `s` is `NULL`, a message is printed to `stderr`.

### `istack_is_empty`

```c
bool istack_is_empty(const istack_t *s);
```

Determines whether the stack is empty.

| Parameter | Description            |
|-----------|------------------------|
| `s`       | The stack to check.    |

**Returns:** `true` if `s` is `NULL` or the stack is empty. `false` if it holds
one or more values.

**Errors:** If `s` is `NULL`, a message is printed to `stderr`.

### `istack_capacity`

```c
size_t istack_capacity(const istack_t *s);
```

Gives the total number of values the stack can hold at its current array
allocation size.

| Parameter | Description                                  |
|-----------|----------------------------------------------|
| `s`       | The stack to determine the capacity of.      |

**Returns:** 0 if `s` is `NULL`. Otherwise, the current capacity of the stack.

**Errors:** If `s` is `NULL`, a message is printed to `stderr`.

### `istack_to_array`

```c
int *istack_to_array(const istack_t *s, size_t *n);
```

Copies the stack bottom-first to a new array. The caller owns the new array and
must `free` it after use.

| Parameter | Description                                                      |
|-----------|------------------------------------------------------------------|
| `s`       | The stack to copy to a new array.                                |
| `n`       | The address of the variable in which to store the number of elements in the new array. Must not be `NULL`. |

**Returns:** `NULL` if `s` is `NULL`, the stack is empty, there was not enough
free memory to create the new array, or `n` is `NULL`. Otherwise, a pointer to
the new array.

**Errors:** In every case that returns `NULL`, a message is printed to
`stderr`. If `n` is not `NULL`, `*n` is set to 0.
