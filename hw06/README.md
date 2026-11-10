# hw06: Scenario Replay on iom361

`replay` plays a scenario file through the iom361 I/O module, one tick per
second. On each tick, the line in force sets the "world" (the temperature, the
humidity, and the switches). The program then reads the sensor and the switches
back **through the registers**, toggles LED k for every switch k that was just
pressed, and prints one trace line. Timestamps are printed in UTC, so a trace
is the same on every computer.

| Module                       | Purpose                                                       |
|------------------------------|---------------------------------------------------------------|
| `sensor.h` / `sensor.c`      | `reading_t` and `sensor_read`: reads and converts the temperature and humidity registers |
| `panel.h` / `panel.c`        | `panel_pressed`, `panel_released`, `panel_led`, and `panel_toggle`: switch edges and single-LED writes |
| `scenario.h` / `scenario.c`  | `scenario_t`, `scenario_load`, and `scenario_at`: loads a scenario file with no `malloc` and finds the line in force at a tick |
| `iom361_r4.h` / `iom361_r4.c`| The iom361 hardware model, version 4.0 (unchanged)            |
| `main.c`                     | `./replay scenario.txt`: replays a scenario and prints the trace |
| `tests/test_hw06.c`          | Unit tests for `sensor`, `panel`, and `scenario`              |
| `regress.sh`                 | Regression test: compares `./replay` on each scenario with its expected trace |

## Building and Testing

```sh
make         # compile every module and iom361_r4.c, link ./replay
make test    # build ./replay and tests/test_hw06, run the unit tests, then sh regress.sh
make clean   # remove all .o files, ./replay, and tests/test_hw06
```

Everything is compiled with `gcc -std=c11 -Wall -Wextra -Wshadow
-fsanitize=address` and linked with `-lm`. Each `.o` file is rebuilt when its
`.c` file or any header it includes changes. Run `make test` from `hw06/`: the
unit tests open their scenario files by paths relative to it.

**`./replay scenario.txt`** loads the scenario, initializes iom361 with 4
switches and 4 LEDs, and turns the LED display off. For every tick from 0 to
the last tick in the file, it prints:

```
<YYYY-MM-DD HH:MM:SS> temp=<°C> rh=<%RH> switches=0x<hex> pressed=0x<hex> leds=0x<hex>
```

The time is `start + tick`. `switches` and `leds` are read from `SWITCHES_REG`
and `LEDS_REG`, and `pressed` holds the switches that went from off to on since
the tick before. iom361 starts with every switch off, so a switch that is on at
tick 0 counts as pressed. For example, the start of
`./replay scenarios/presses.txt`:

```
2026-10-06 08:00:00 temp=21.5 rh=40.0 switches=0x0 pressed=0x0 leds=0x0
2026-10-06 08:00:01 temp=21.5 rh=40.0 switches=0x0 pressed=0x0 leds=0x0
2026-10-06 08:00:02 temp=21.5 rh=40.0 switches=0x1 pressed=0x1 leds=0x1
2026-10-06 08:00:03 temp=21.5 rh=40.0 switches=0x0 pressed=0x0 leds=0x1
2026-10-06 08:00:04 temp=21.5 rh=40.0 switches=0x1 pressed=0x1 leds=0x0
```

If the arguments are wrong, the scenario does not load, or a register access
fails, `replay` prints a message to `stderr` and exits with status 1.

**`tests/test_hw06`** tests:

- `sensor_read` at 91.0 °C and 45.0 %RH reads back exactly 91.0 and 45.0, with
  the timestamp it was given.
- A failed `sensor_read` (a `NULL` base) returns non-zero and leaves every
  field of the reading unchanged.
- `panel_pressed` and `panel_released` for a press, a release, a held switch,
  and a mix of all three.
- `panel_led` and `panel_toggle` keep the other LEDs, starting from `0xA`,
  including an LED that is already on or off, and an out-of-range `n` that
  changes nothing.
- `scenario_load` on a file with comments and blank lines, checking the start
  time, the line count, and every value.
- `scenario_at` between lines, on a line, after the last line, and at tick -1.
- `scenario_load` on files with no `start` line, a missing field, a repeated
  tick, a tick that goes back, and a file that does not exist.

The scenario files for these tests are in `tests/data/`. Each test prints
`[PASS]` or `[FAIL]`, then the expected and actual results on the next two
lines, followed by a blank line. At the end it prints a summary of how many
tests passed and failed, and exits with a non-zero status if any test failed.

**`regress.sh`** runs `./replay` on every `scenarios/*.txt` and compares its
output with `diff` against the file of the same name in `expected/`. It prints
`PASS` or `FAIL` per scenario and exits with 1 if any trace differs.

| Scenario         | What it covers                                                 |
|------------------|----------------------------------------------------------------|
| `presses.txt`    | Presses, releases, holds, and simultaneous presses on all four switches; bit 4, which iom361 clears |
| `midnight.txt`   | Starts at 2027-02-28 23:59:55 UTC and runs into 1 March, with a switch held across midnight |

## Scenario Files

```
start 1791273600                # 2026-10-06 08:00:00 UTC
# tick  temp   rh    switches
0       21.0   45    0x0
5       21.0   45    0x1        # ARM pressed
6       21.0   45    0x0        # and released
60      22.0   45    0x0
```

- Text from `#` to the end of a line is a comment. Blank lines and lines that
  hold only a comment are skipped.
- The first line that is not skipped is `start <timestamp>`, where
  `<timestamp>` is a non-negative Unix timestamp in decimal.
- Every line after it is `<tick> <temp> <rh> <switches>`: a tick in seconds
  since the start (decimal), a temperature in °C, a relative humidity in %RH,
  and the switch register in hexadecimal (the `0x` is optional). The
  temperature and humidity must be finite; iom361 clamps out-of-range values.
- The first data line has tick 0, and every later tick is greater than the one
  before it.
- At most `SCENARIO_MAX_LINES` (1000) data lines.
- Values hold between lines.

## sensor.h

### Types

| Type        | Fields                                                              |
|-------------|---------------------------------------------------------------------|
| `reading_t` | `time_t timestamp`, `float temperature_celsius`, `float relative_humidity_percent` |

### `sensor_read`

```c
int sensor_read(uint32_t *base, time_t now, reading_t *reading);
```

Reads the temperature and humidity registers of iom361, converts them with the
AHT20 formulas, rounds them to the nearest 0.1, and stores them in `*reading`
along with the timestamp `now`.

| Parameter | Description                                                      |
|-----------|------------------------------------------------------------------|
| `base`    | The base address of the I/O register block.                      |
| `now`     | The current timestamp.                                           |
| `reading` | Where to store the temperature, relative humidity, and time.     |

**Returns:** 0 on success. Otherwise, the return code of the failed
`iom361_readReg` call.

**Errors:** On a failed register read, a message is printed to `stderr` and
`*reading` is not changed.

## panel.h

### `panel_pressed`

```c
uint32_t panel_pressed(uint32_t now, uint32_t before);
```

Finds the switches that went from off to on between ticks. One bit is one
switch.

| Parameter | Description                                         |
|-----------|-----------------------------------------------------|
| `now`     | The switch register right now.                      |
| `before`  | The switch register from the previous tick.         |

**Returns:** A 32-bit register with 1s in the positions of the switches that
went from off to on.

### `panel_released`

```c
uint32_t panel_released(uint32_t now, uint32_t before);
```

Finds the switches that went from on to off between ticks. One bit is one
switch.

| Parameter | Description                                         |
|-----------|-----------------------------------------------------|
| `now`     | The switch register right now.                      |
| `before`  | The switch register from the previous tick.         |

**Returns:** A 32-bit register with 1s in the positions of the switches that
went from on to off.

### `panel_led`

```c
int panel_led(uint32_t *base, int n, int on);
```

Turns LED `n` on or off by a read-modify-write of `LEDS_REG`, so every other
LED keeps its value.

| Parameter | Description                                                      |
|-----------|------------------------------------------------------------------|
| `base`    | The base address of the iom361. `LEDS_REG` is used as the offset of the LEDs register. |
| `n`       | The LED to change. `0 <= n <= 31`. This may change LEDs beyond the number iom361 was initialized with; use with caution. |
| `on`      | 0 turns the LED off. Any non-zero value turns it on.             |

**Returns:** 0 on success. -1 if `n` is out of range or `base` is `NULL`.
Otherwise, the non-zero return code of the failed iom361 call.

**Errors:** If `n` is out of range or `base` is `NULL`, a message is printed to
`stderr` and nothing is changed.

### `panel_toggle`

```c
int panel_toggle(uint32_t *base, int n);
```

Toggles LED `n` by a read-modify-write of `LEDS_REG`, so every other LED keeps
its value.

| Parameter | Description                                                      |
|-----------|------------------------------------------------------------------|
| `base`    | The base address of the iom361. `LEDS_REG` is used as the offset of the LEDs register. |
| `n`       | The LED to toggle. `0 <= n <= 31`. This may change LEDs beyond the number iom361 was initialized with; use with caution. |

**Returns:** 0 on success. -1 if `n` is out of range or `base` is `NULL`.
Otherwise, the non-zero return code of the failed iom361 call.

**Errors:** If `n` is out of range or `base` is `NULL`, a message is printed to
`stderr` and nothing is changed.

## scenario.h

### Constants

| Constant               | Value | Meaning                                         |
|------------------------|-------|-------------------------------------------------|
| `SCENARIO_MAX_LINES`   | 1000  | The most data lines a scenario can hold         |
| `SCENARIO_ERR_OPEN`    | -1    | `scenario_load` could not open the file         |
| `SCENARIO_ERR_MISSING` | -2    | The file had no `start` line or no data lines   |

### Types

| Type              | Fields                                                        |
|-------------------|---------------------------------------------------------------|
| `scenario_line_t` | One data line, the world from `tick` until the next line: `long tick`, `float temperature_celsius`, `float relative_humidity_percent`, `uint32_t switches` |
| `scenario_t`      | A loaded scenario: `time_t start`, `size_t count`, and `scenario_line_t lines[SCENARIO_MAX_LINES]`, a fixed array with no `malloc` |

A `scenario_t` is about 16 KB, so `main.c` and the tests keep it `static`
rather than on the stack.

### `scenario_load`

```c
int scenario_load(const char *path, scenario_t *scenario);
```

Loads a scenario file (see [Scenario Files](#scenario-files)) into
`*scenario`.

| Parameter  | Description                                                |
|------------|------------------------------------------------------------|
| `path`     | The path of the scenario file.                             |
| `scenario` | Where the scenario is stored. Must not be `NULL`.          |

**Returns:** 0 on success. The number of the first wrong line if there is one.
`SCENARIO_ERR_OPEN` (-1) if the file cannot be opened. `SCENARIO_ERR_MISSING`
(-2) if the file has no `start` line or no data lines.

Line numbers count every line in the file, starting at 1, including comment
and blank lines. A line is wrong if it has:

- a bad or missing field, an extra field, or a value out of range;
- a data line before `start`, or a second `start`;
- a first tick that is not 0, or a tick that does not increase;
- more characters than the internal line buffer holds;
- the data line after the first `SCENARIO_MAX_LINES`.

**Errors:** On any error, `scenario->count` is set to 0 and the rest of
`*scenario` is unspecified.

### `scenario_at`

```c
const scenario_line_t *scenario_at(const scenario_t *scenario, long tick);
```

Finds the data line in force at `tick`: the last line whose tick is at or
before `tick`. Values hold between lines, so a tick between two lines gets the
earlier one, and a tick after the last line gets the last line.

| Parameter  | Description                                                   |
|------------|---------------------------------------------------------------|
| `scenario` | A scenario loaded by `scenario_load`. Must not be `NULL`.     |
| `tick`     | Seconds since the start of the scenario.                      |

**Returns:** The line in force, or `NULL` if `tick` is negative or the scenario
holds no lines.
