# hw04: Tally

`tally.c` implements a tally: a list of names, each with a count of how many
times it has been added. Tallies are handed out from a fixed pool, so no memory
is allocated at run time. `tally_t` is an opaque type, so callers can only use
a tally through the functions in `tally.h`.

| Module                | Purpose                                                      |
|-----------------------|--------------------------------------------------------------|
| `tally.h` / `tally.c` | `tally_create`, `tally_destroy`, `tally_add`, `tally_count`, `tally_size`, and `tally_print` |
| `events.c`            | Tallies the lines of `stdin` and prints the result           |
| `tests/test_tally.c`  | Unit tests for every function in `tally.c`                   |

## Building and Testing

```sh
make         # compile tally.c and events.c, link ./events
make test    # compile tally.c and tests/test_tally.c, link and run tests/test_tally
make clean   # remove all .o files, ./events, and tests/test_tally
```

Everything is compiled with `gcc -std=c11 -Wall -Wextra -Wshadow
-fsanitize=address`. Each `.o` file is rebuilt when its `.c` file or `tally.h`
changes.

**`./events`** reads `stdin` line by line, for example `./events <
data/data.txt`. It removes each line's newline characters, skips blank lines
and lines starting with `#`, and adds every other line to one tally. It then
prints the tally with `tally_print`, followed by a last line:

```
names: <number of names in the tally>, lines skipped: <number of lines refused by tally_add>
```

**`tests/test_tally`** unit-tests the functions in `tally.c`:

- `tally_add` and `tally_count` counting a name up, and `tally_count` on a name
  that was never added.
- `tally_add` with an empty name, a name of exactly `TALLY_LEN` characters, and
  a name of `TALLY_LEN + 1` characters.
- Every function with NULL arguments.
- `tally_add` with new and existing names on a full tally.
- `tally_create` called `TALLY_MAX` times, then once more.
- Every function on a destroyed tally, and `tally_create` after destroying one.
- `tally_print` on an empty tally and in alphabetical order.

It prints `[PASS]` or `[FAIL]` for each test, then a summary, and exits with a
non-zero status if any test fails. The error cases also print their error
messages to `stderr`, so those lines are expected in the output.

## tally.h

| Constant      | Value | Description                                  |
|---------------|-------|----------------------------------------------|
| `TALLY_MAX`   | 4     | Tallies that can exist at the same time.     |
| `TALLY_NAMES` | 16    | Different names one tally can hold.          |
| `TALLY_LEN`   | 15    | Longest name, not counting the `'\0'`.       |

`tally_add`, `tally_count`, `tally_size`, and `tally_print` check their
arguments. On a bad argument they print a message to `stderr` starting with the
function's name, such as `tally_add: tally is full` or `tally_size: tally is
not in use`, and return without changing the tally. A tally is not in use once
it has been destroyed.

### `tally_create`

```c
tally_t *tally_create(void);
```

Creates a new, empty tally.

**Returns:** A pointer to the new tally, or NULL if `TALLY_MAX` tallies already
exist.

### `tally_destroy`

```c
void tally_destroy(tally_t *t);
```

Frees up a tally so that `tally_create` can hand it out again. This does not
deallocate any memory, and callers should set the pointer to NULL immediately
after calling it.

| Parameter | Description                                               |
|-----------|-----------------------------------------------------------|
| `t`       | The tally to free up. If NULL, nothing happens.           |

### `tally_add`

```c
int tally_add(tally_t *t, const char *name);
```

Adds one more occurrence of `name` to a tally and returns its new count. If the
tally is not full and does not already contain `name`, the name is inserted
alphabetically into the tally's array of names.

| Parameter | Description                          |
|-----------|--------------------------------------|
| `t`       | The tally to modify.                 |
| `name`    | The name to add an occurrence of.    |

**Returns:** The name's new count in the tally, or -1 on an error.

**Errors:** If `t` or `name` is NULL, `name` is empty or longer than
`TALLY_LEN`, `name` is new and the tally is full, or `t` is not in use, an error
is printed to `stderr` and -1 is returned.

### `tally_count`

```c
int tally_count(const tally_t *t, const char *name);
```

Counts the occurrences of a name.

| Parameter | Description                          |
|-----------|--------------------------------------|
| `t`       | The tally to read from.              |
| `name`    | The name to count occurrences of.    |

**Returns:** The number of occurrences of `name` in the tally, or 0 if `name`
is not in the tally.

**Errors:** If `t` or `name` is NULL or `t` is not in use, an error is printed
to `stderr` and 0 is returned.

### `tally_size`

```c
int tally_size(const tally_t *t);
```

Counts how many different names a tally holds.

| Parameter | Description              |
|-----------|--------------------------|
| `t`       | The tally to read from.  |

**Returns:** The number of different names in the tally.

**Errors:** If `t` is NULL or not in use, an error is printed to `stderr` and 0
is returned.

### `tally_print`

```c
void tally_print(const tally_t *t, FILE *out);
```

Prints a line for each name in a tally, in the format `<name> <count>`,
alphabetically by name.

| Parameter | Description                  |
|-----------|------------------------------|
| `t`       | The tally to print out.      |
| `out`     | The file stream to print to. |

**Errors:** If `t` is NULL or not in use, an error is printed to `stderr` and
the function returns without printing anything to `out`.

## The Importance of Opaque Structs
Attempting to compile a file that only includes tally.h (and not tally.c)
but also attempts an operation such as `t->size = 1` results in the following
compilation error:

```
error: invalid use of incomplete typedef ‘tally_t’ {aka ‘struct tally’}
```

This error occurs because the compiler knows that there is a data type called
tally_t and that it is a struct, but since tally_t is opaque, it has no
information about the struct's fields. Therefore, it has no way to modify its
fields; it doesn't even know how much memory the field takes up, so it couldn't
possibly make the assignment. The reason opaque data types are useful is that
callers can't accidentally (or intentionally) assign an invalid value to a
struct's field. All interactions with the struct's fields happen through
functions, allowing the author of the data type to perform any data validation
they wish.