# Sorting Algorithm Performance Analyzer

A small C program I wrote to see, with real numbers, how **selection sort** and **insertion sort** behave on different kinds of input. Both are O(n²) in the worst case, but textbooks also say they behave very differently in practice. I wanted to check that myself instead of just taking it on faith.

You can type in your own numbers, generate data (sorted, reverse sorted or random), and run either algorithm or both side by side. The program counts comparisons and swaps/shifts, times each run, and double-checks that the output is actually sorted.

## What it does

- Lets you **enter data manually** or **generate** up to 100,000 values
- Runs **selection sort**, **insertion sort**, or **both and compares them**
- Reports **comparisons**, **swaps (selection) / shifts (insertion)** and **time in milliseconds**
- **Verifies every result** by checking it is in order and matching it against `qsort` from the standard library (shown as PASS / FAIL)
- Has a **full experiment mode** that tests sizes 100, 500, 1000 and 2000 on sorted, reverse and random data, averaged over a number of trials you choose
- Handles bad input without crashing (letters instead of numbers, out of range values, and so on)

## How to build and run

You need a C compiler such as GCC.

```bash
gcc sorting_analyzer.c -o sorting_analyzer
./sorting_analyzer
```

On Windows (MinGW):

```bash
gcc sorting_analyzer.c -o sorting_analyzer.exe
sorting_analyzer.exe
```

Replace `sorting_analyzer.c` with whatever you named the source file.

## Menu

```
1. Enter data manually
2. Generate data
3. Display data
4. Run selection sort
5. Run insertion sort
6. Run both and compare
7. Run full experiment
8. Set number of trials
0. Exit
```

## What I found

| Input order | Selection sort | Insertion sort |
|-------------|----------------|----------------|
| Sorted      | ~n(n-1)/2 comparisons, 0 swaps | n-1 comparisons, 0 shifts (very fast) |
| Reverse     | ~n(n-1)/2 comparisons, about n/2 swaps | ~n(n-1)/2 comparisons and shifts (slowest case) |
| Random      | ~n(n-1)/2 comparisons, fewer than n swaps | roughly half the worst case |

The main takeaways:

- Selection sort **always** does about the same number of comparisons, no matter how the data is arranged. It never does more than n-1 swaps, though, which makes it useful when writing to memory is expensive.
- Insertion sort adapts to the data. On nearly sorted input it is close to linear, but on reversed input it does the most work.
- Same big-O in the worst case, noticeably different behaviour in practice.

## How it works (briefly)

- `selection_sort` finds the smallest remaining element on each pass and swaps it into place.
- `insertion_sort` takes each element and shifts larger ones to the right until it finds the right spot.
- `run()` copies the original data first, so both algorithms always start from identical input, then times the sort with `clock()` and verifies the result.
- `experiment()` repeats the runs for each size and order and prints the averages in one table.

## Notes

- Timings depend on your machine and compiler settings, so compare the algorithms with each other rather than treating the exact milliseconds as fixed values.
- Sorting 100,000 values with these O(n²) algorithms can take a while. That is expected and is sort of the point of the project.
- If more than 50 values are loaded, the program only prints the first ten to keep the screen readable.

## Possible improvements

- Add more algorithms (bubble, merge, quick) for comparison
- Export results to a CSV file for graphing
- Add a test with partially sorted data

## Author

Your Name, your course / college, year
