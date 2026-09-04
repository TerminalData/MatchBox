# MatchBox

MatchBox is a C++20 limit-order matching engine inspired by _Building Low Latency Applications with C++_ by Sourav Ghosh.

The engine maintains separate buy and sell order books, matches compatible orders by price, and preserves FIFO priority between orders at the same price level.

I am building this to learn more about low-level latency-sensitive programming.
I plan on starting on a very basic version of a matching algorithm, and gradually
add to it in order to make it more complete, faster and more efficient. The progress is calculated
via a benchmark of a file I fetched from databento.com for every trade from MSFT
on the NASDAQ for 2026, August 25 which represents 2,075,526 orders. I will always
use a ThinkPad T14s Gen6 AMD from Lenovo to benchmark this program.

## Version History

### 1.1.0
Switched my home made parser to simdjson on-demand API. It's awesome.

    * Parse and match:  604.12 ms
    * Parse:            377.63 ms
    * Match:            139.89 ms

### 1.0.1

This is a naive version I built just to have a platform unto which I can try
latency reducing techniques and learn how to better operate in a low-level
programming setting. 

    * Parse and match:  5719.17 ms
    * Parse:            5571.07 ms
    * Match:             155.41 ms

- Added the initial C++20 limit-order matching engine.
- Added buy and sell order books with price-time priority.
- Added partial fills, multi-level matching, resting orders, and cancellations.
- Added newline-delimited JSON input processing.
- Added GoogleTest coverage for the core matching behavior.
- Added Make targets for building, running, testing, cleaning, and Valgrind checks.

## Features

- Buy and sell limit orders
- Price-time priority matching
- Partial fills across one or more price levels
- Resting orders for any unfilled quantity
- Full, partial, and over-cancellation of resting orders
- GoogleTest unit tests for core matching behavior
- Build flags suitable for both optimization and debugging

## Project Structure

```text
include/       Public order, inventory, and matching-engine declarations
src/           Application entry point and matching-engine implementation
test/          GoogleTest test suite
test/data/     Newline-delimited JSON input files
Makefile       Build, test, run, and memory-check targets
```

## Requirements

- A C++20-compatible compiler, such as `g++`
- GNU Make
- GoogleTest development libraries
- Valgrind (optional, for `make memcheck`)
- `nlohmann/json` headers

On Debian or Ubuntu, the compiler and test dependencies can be installed with:

```bash
sudo apt install build-essential libgtest-dev nlohmann-json3-dev valgrind
```

## Building

Build the application with:

```bash
make
```

The application is written to `bin/MatchBox`. Intermediate object files and dependency files are written to `obj/`.

## Running

Run the application with:

```bash
bear -- make run
```

When prompted, enter the name of a JSON input file located in `test/data/`, for example:

```text
test1.json
```

The program processes the file line by line and reports the number of input records processed and the elapsed time.

## Input Format

Each non-empty line must contain one JSON order record. The application reads these fields:

```json
{
  "action": "A",
  "price": "100",
  "size": "10",
  "order_id": "1",
  "side": "B",
  "hd": { "ts_event": "123456789" }
}
```

| Field         | Description                                                      |
| ------------- | ---------------------------------------------------------------- |
| `action`      | `A` to add an order or `C` to cancel one                         |
| `price`       | Limit price; numeric values and numeric strings are accepted     |
| `size`        | Quantity; numeric values and numeric strings are accepted        |
| `order_id`    | Identifier of the order                                          |
| `side`        | `B` for buy; any other value is treated as sell                  |
| `hd.ts_event` | Event timestamp; numeric values and numeric strings are accepted |

Records with actions `T`, `F`, or `R` are currently ignored. Malformed records are reported and processing continues with the next line.

## Matching Rules

- A buy order matches the lowest-priced sell orders at or below its limit price.
- A sell order matches the highest-priced buy orders at or above its limit price.
- Orders at the same price are matched in insertion order.
- If an incoming order is only partially filled, its remaining quantity is added to the appropriate book.
- Cancellation identifies an order by both its side and `order_id`, at the requested price.

## Tests

Build and run the test suite with:

```bash
make test
```

The tests cover non-crossing orders, partial fills, exhausted price levels, FIFO priority, multi-level sweeps, and cancellation behavior.

Run the suite under Valgrind with:

```bash
make memcheck
```

Remove generated build artifacts with:

```bash
make clean
```

## Current Limitations

- The executable accepts an input filename interactively rather than through a command-line argument.
- Order books are held in memory and are not persisted after the process exits.
- There is no output trade stream or book snapshot yet.
- The application expects input files under `test/data/`.
- The current input path treats any side other than `B` as a sell side.

## Roadmap

Possible next steps include:

- Add command-line arguments for input files
- Emit execution reports and final book state
- Add validation for required fields and order values
- Add broader integration and malformed-input tests
- Benchmark matching throughput and latency

## License

No license has been specified yet.
