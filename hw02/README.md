# hw02: Sensor Readings Summary

`readings` reads temperature and humidity readings from standard input and
prints a summary: the number of readings, the minimum, maximum, and mean
temperature and humidity, the longest run of readings above a temperature
threshold, and a histogram of the temperatures.

The program was refactored from a single file into modules:

| Module                        | Purpose                                         |
|-------------------------------|-------------------------------------------------|
| `main.c`                      | Argument handling and printing the summary      |
| `reading.h` / `reading.c`     | Reading and parsing the input                   |
| `stats.h` / `stats.c`         | Minimum, maximum, mean, and longest run         |
| `histogram.h` / `histogram.c` | Printing the temperature histogram              |

## Building and Testing

```sh
make         # compile each .c file to a .o file and link ./readings
make test    # build, run tests/test_stats, then run the regression test
make clean   # remove ./readings, all .o files, and tests/test_stats
```

Everything is compiled with `gcc -std=c11 -Wall -Wextra -Wshadow
-fsanitize=address`. Each `.o` file is rebuilt when its `.c` file or any header
it includes changes.

`make test` runs two test programs and fails if either one does:

1. **`tests/test_stats`** unit-tests the functions in `stats.c`: arrays with one
   value, the maximum at the start and at the end, values exactly equal to the
   threshold, and `stats_max` on an array of 10,000 values. It prints `[PASS]`
   or `[FAIL]` for each test, then a summary, and exits with a non-zero status
   if any test fails.
2. **`regress.sh`** runs `./readings` on every file in `data/` and uses `diff`
   to compare its output with the file of the same name in `expected/`. It also
   runs `data/edge.txt` with a threshold of 25 and compares the output with
   `expected/edge_25.txt`. Any difference is a failure, and the script exits
   with 1.

To run a single input by hand:

```sh
./readings < data/normal.txt
./readings 25 < data/edge.txt
```

## Usage

```sh
./readings [threshold] < input.txt
```

`threshold` is a temperature in degrees C and defaults to `30.0`.

### Input format

One reading per line, as three numbers: `tick temperature humidity`. For
example:

```
12  23.5  41.0
```

- Blank lines, and lines whose first non-blank character is `#`, are ignored.
- A line that does not start with three numbers is counted as skipped.
- At most 1,000 readings are stored. After that, a warning is printed to
  `stderr` and the rest of the input is ignored.
- Lines are read up to 127 characters at a time, so a longer line is read in
  pieces, and each piece is handled as its own line.

### Output

```
readings: 48
skipped:  0
temperature: min   24.4  max   33.6  mean  29.75 C
humidity:    min   56.1  max   70.5  mean  62.27 %RH
above 30.0 C: longest run 25 readings, from tick 780 to tick 2220
histogram:
   20.0 to  25.0 C | *** 3
   25.0 to  30.0 C | ***************** 17
   30.0 to  35.0 C | **************************** 28
```

If no temperature is above the threshold, the run line is
`above <threshold> C: never`. The histogram has 20 bins, each 5 C wide,
covering 0 to 100 C. Only bins that hold at least one reading are printed.

### Errors

| Situation                      | Behavior                                                         |
|--------------------------------|------------------------------------------------------------------|
| More than one argument         | Prints `usage: ./readings [threshold] < readings.txt` to `stderr`, exits with 1 |
| `threshold` is not a number    | Prints `error: threshold '<arg>' is not a number` to `stderr`, exits with 1 |
| No valid readings              | Prints the counts, then `no readings, no summary`, and exits with 0 |
| More than 1,000 readings       | Prints a warning to `stderr` and summarizes the first 1,000      |

## reading.h

### `reading_data_t`

```c
typedef struct {
    int count;
    int skipped;
} reading_data_t;
```

The result of `reading_read`: how many readings were stored (`count`) and how
many lines were invalid (`skipped`).

### `reading_read`

```c
reading_data_t reading_read(int ticks[], float temps[], float hums[],
    uint16_t array_len, uint16_t line_len);
```

Reads lines of the form `<tick> <temperature> <humidity>` from `stdin` and
stores the values in the three arrays. Elements `0` to `count - 1` of each
array are filled. Blank lines and lines whose first non-blank character is `#`
are ignored, and lines that don't hold three numbers are counted as skipped.

| Parameter   | Description                                              |
|-------------|----------------------------------------------------------|
| `ticks`     | Array for tick values to be stored in.                   |
| `temps`     | Array for temperature values to be stored in.            |
| `hums`      | Array for humidity values to be stored in.               |
| `array_len` | Length of each array.                                    |
| `line_len`  | Size of the line buffer, in characters. Must be at least 2. |

**Returns:** A `reading_data_t` whose `count` is the number of readings stored
and whose `skipped` is the number of invalid lines.

**Errors:** Once `array_len` readings have been stored, a warning is printed to
`stderr` and the rest of the input is ignored. A `line_len` below 2 is not
checked and causes undefined behavior or an infinite loop.

## stats.h

All four functions take an array of floats and its length. None of them check
their arguments any more than in the original readings.c.

### `stats_max`

```c
float stats_max(const float data[], int len);
```

Returns the largest value in `data`, found recursively. `len` must be at
least 1.

### `stats_min`

```c
float stats_min(const float data[], int len);
```

Returns the smallest value in `data`. `len` must be at least 1.

### `stats_mean`

```c
float stats_mean(const float data[], int len);
```

Returns the mean of the values in `data`. `len` must be at least 1.

### `stats_run_t`

```c
typedef struct {
    int best;
    int best_start;
} stats_run_t;
```

The result of `stats_threshold_run`: the length of the longest run (`best`)
and the index in the array where it starts (`best_start`).

### `stats_threshold_run`

```c
stats_run_t stats_threshold_run(const float data[], int len, float threshold);
```

Finds the longest run of consecutive values strictly greater than `threshold`.
A value equal to `threshold` ends a run. If two runs are equally long, the
first one is returned.

| Parameter   | Description                                                   |
|-------------|---------------------------------------------------------------|
| `data`      | The array of floats to search.                                |
| `len`       | The number of elements in the array. May be 0.                |
| `threshold` | The value that an element must be greater than to start or continue a run. |

**Returns:** A `stats_run_t` with the length of the longest run (`best`) and
the **index** of its first element (`best_start`). If no value is above the
threshold, `best` is 0 and `best_start` is -1. To get tick numbers, look the
index up in the matching `ticks` array, as `main.c` does.

## histogram.h

### `histogram_print`

```c
void histogram_print(const float data[], int len, int num_bins,
    float bin_width);
```

Prints `histogram:` followed by a histogram of `data` to `stdout`. Bin `b`
holds the values from `b * bin_width` up to (but not including)
`(b + 1) * bin_width`, so the bins cover 0 to `num_bins * bin_width`. Values
below 0 are counted in the first bin, and values at or above
`num_bins * bin_width` are counted in the last bin. Only bins holding at least
one value are printed, one per line, as
`<low> to <high> C | <one * per value> <count>`.

| Parameter   | Description                                    |
|-------------|------------------------------------------------|
| `data`      | The array of values to sort into bins.         |
| `len`       | The number of elements in `data`. May be 0.    |
| `num_bins`  | The number of bins. Must be at least 1.        |
| `bin_width` | The width of each bin. Must be greater than 0. |

**Errors:** The arguments are not checked. Breaking the requirements above is
undefined behavior.
