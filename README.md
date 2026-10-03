# Sorting Algorithm Performance Analyzer

## 1. Project Overview

This is a mini project for the Analysis of Algorithms (AOA) subject. It experimentally compares **Selection Sort** and **Insertion Sort** by running both algorithms on identical datasets and recording the exact number of comparisons, the number of swaps or shifts performed, and the wall-clock elapsed time. The program is written in pure C (standard C90/C99) and uses an interactive menu-driven interface so that the behaviour can be explored manually as well as via an automated full-experiment mode.

---

## 2. Objective

The objective of the project is to:

- Understand the practical meaning of asymptotic time complexity by measuring real operation counts.
- Observe how the *arrangement* of the input (sorted, reverse-sorted, or random) changes the real cost of an algorithm even when the input size is the same.
- Verify the theoretical best / average / worst case bounds of Selection Sort and Insertion Sort with actual measured data.
- Learn how to benchmark two algorithms fairly using identical input copies and a trusted reference sort.

---

## 3. Features

The program contains the following features (as implemented in the source):

- **Manual dataset input** – the user can enter any number of integers one by one.
- **Automatic dataset generation** – datasets of any size (up to 100,000) can be generated.
- **Sorted data generation** – values 1, 2, 3, …, n.
- **Reverse-sorted data generation** – values n, n-1, n-2, …, 1.
- **Random data generation** – random integers in the range `[0, 10·n]`.
- **Selection Sort** with instrumented comparison and swap counters.
- **Insertion Sort** with instrumented comparison and shift counters.
- **Comparison counting** – exact count of every key-to-key comparison inside both sort loops.
- **Swap counting** – number of three-way swaps performed by Selection Sort.
- **Shift counting** – number of individual element moves performed by Insertion Sort.
- **Execution-time measurement** – CPU time measured with `clock()` and reported in milliseconds.
- **Correctness verification** – every sorted output is checked to be in non-decreasing order AND is compared against the result produced by the standard library `qsort`.
- **Multiple dataset sizes** – the automated experiment runs on n = 100, 500, 1000, 2000.
- **Performance comparison table** – single-run and averaged multi-trial side-by-side tables for both algorithms.
- **Configurable number of trials** – number of repetitions used by the full experiment (1 – 100).

---

## 4. Algorithms Used

### Selection Sort

Selection Sort divides the array into a sorted and an unsorted portion. On every pass it scans the unsorted portion to find the minimum element, and then swaps that minimum into the first position of the unsorted portion. Because the inner scan always runs to the end, Selection Sort performs the same number of comparisons for any ordering of the same size n.

### Insertion Sort

Insertion Sort also builds a sorted portion one element at a time. It takes the next unsorted element (the "key") and shifts larger sorted elements one slot to the right until the correct sorted position for the key is found, then places the key there. On already-sorted input it terminates the inner while-loop immediately after one comparison, giving its best-case linear behaviour.

---

## 5. Complexity Analysis

### Selection Sort

| Case               | Time Complexity |
|--------------------|-----------------|
| Best Case          | O(n²)           |
| Average Case       | O(n²)           |
| Worst Case         | O(n²)           |
| Space Complexity   | O(1)            |

Because the `j` loop always compares every element in the unsorted portion, the comparison count is always n(n−1)/2 regardless of the data. Swap count is at most n−1. It is an in-place sort.

### Insertion Sort

| Case               | Time Complexity |
|--------------------|-----------------|
| Best Case          | O(n)            |
| Average Case       | O(n²)           |
| Worst Case         | O(n²)           |
| Space Complexity   | O(1)            |

Insertion Sort has a data-dependent inner loop:

- **Best case (sorted input):** the inner loop body executes exactly once per key and immediately breaks because `a[j] > key` is false. Only n−1 comparisons in total and zero shifts.
- **Worst case (reverse-sorted input):** every key is shifted all the way to the front, giving approximately n(n−1)/2 comparisons and n(n−1)/2 shifts.
- **Average case (random input):** roughly half the sorted portion is scanned on average, still O(n²).

This input-dependent performance is the key contrast between Insertion Sort and Selection Sort.

---

## 6. Performance Metrics

The program records four metrics, counted exactly as follows inside the code:

### Comparisons
- **Selection Sort** – incremented exactly once per execution of the condition `a[j] < a[min]` inside the inner `j` loop. This always totals n(n−1)/2 for any input arrangement of size n.
- **Insertion Sort** – incremented exactly once per execution of the condition `a[j] > key` inside the inner `while` loop. The count is data-dependent (minimum n−1 for sorted input, maximum ~n²/2 for reverse input).

### Swaps
Used only for **Selection Sort**. Incremented once per three-way exchange (`tmp = a[i]; a[i] = a[min]; a[min] = tmp;`) when `min != i`. The maximum possible value is n−1. Stored in the `moves` field of the `Result` struct.

### Shifts
Used only for **Insertion Sort**. Incremented once each time an element is moved right via `a[j + 1] = a[j];`. This counts *each individual element move*, not each pass. On sorted input the shift count is zero. Stored in the same `moves` field of the `Result` struct (renamed to "Swaps/Shifts" in the printed tables depending on the algorithm).

### Elapsed time
- Measured using `clock()` from `<time.h>`, taken immediately before and after the call to the sort function.
- Converted to milliseconds with `(t1 − t0) · 1000 / CLOCKS_PER_SEC`.
- The printed value is CPU time (not wall-clock time), and includes only the time spent inside the sorting function itself (copying of the master dataset and correctness verification are excluded).

---

## 7. Input Cases

The program supports four ways of preparing the master dataset:

1. **Manual input (Menu option 1)** – User is prompted for n, then asked to enter n integers one per line. Input is validated and re-asked if an invalid number is entered.
2. **Sorted (Menu option 2 → Arrangement 1)** – `a[i] = i + 1`, i.e. the integers `1, 2, 3, …, n` in order. Exercises Insertion Sort's best case.
3. **Reverse-sorted (Menu option 2 → Arrangement 2)** – `a[i] = n − i`, i.e. the integers `n, n−1, …, 1` in reverse order. Exercises Insertion Sort's worst case.
4. **Random (Menu option 2 → Arrangement 3)** – `a[i] = rand() % (n · 10 + 1)`, giving random integers in the range `[0, 10·n]`. The RNG is seeded once with `srand(time(NULL))` on program start.

---

## 8. Fair Comparison

Yes, the comparison is deliberately fair. A single untouched **master** dataset is kept, and before each algorithm runs the program performs:

```c
memcpy(out, master, (size_t)n * sizeof(int));   /* work on a copy */
```

inside `run_sort()`. This means:

- Selection Sort operates on a fresh, byte-identical copy of the master.
- Insertion Sort operates on another fresh, byte-identical copy of the same master.
- The master array itself is never modified by any sort.

Even in the full-experiment mode (`run_experiment()`), the same freshly-generated master is fed into both algorithms before regenerating the dataset for the next trial. This is the standard correct methodology for empirical algorithm comparison.

---

## 9. Correctness Verification

After each algorithm finishes sorting a copy, correctness is verified with **two independent checks** inside `run_sort()`:

1. **Output is sorted** – `is_sorted(out, n)` scans the output array and checks `a[i−1] ≤ a[i]` for every i from 1 to n−1. Returns 0 on the first violation.
2. **Output matches a trusted reference** – a third copy of the master array is sorted with the standard library `qsort()` (using `cmp_int`) and then compared byte-for-byte with the program's output using `memcmp` via the `arrays_equal()` helper.

Both checks must pass for the `correct` flag to be 1, which is printed as **PASS** in the tables. If either check fails the cell shows **FAIL**.

Additionally, in Menu option 6 ("Run both and compare"), the two sorted outputs are also compared directly with each other and the program prints `Both outputs identical: YES / NO`.

---

## 10. Sample Output

> **Note:** The values shown below are for illustrative format only. Actual numbers depend on the random seed, dataset size, CPU speed, and compiler flags. For real results compile and run the program locally.

### Menu
```
========== Sorting Algorithm Performance Analyzer ==========
 1. Enter data manually
 2. Generate data (sorted / reverse / random)
 3. Display master data
 4. Run selection sort
 5. Run insertion sort
 6. Run both and compare
 7. Run full experiment (multiple sizes and arrangements)
 8. Set number of trials (currently used by option 7)
 0. Exit
=============================================================
Choose an option:
```

### Single dataset comparison (Option 6) – representative format
```
Selection sort output:
  [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
Insertion sort output:
  [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

Algorithm  Comparisons      Swaps/Shifts     Time (ms)    Correct
-------------------------------------------------------------------
Selection  45               4                0.002        PASS
Insertion  45               27               0.001        PASS

Both outputs identical: YES
```

### Full experiment (Option 7) – representative table format
```
Full experiment (averages over 3 trial(s))
====================================================================================
n      Order     Algorithm  Avg Comparisons  Avg Swaps/Shifts Avg ms     Check
------------------------------------------------------------------------------------
100    Sorted    Selection  4950             0                0.003      PASS
100    Sorted    Insertion  99               0                0.001      PASS

100    Reverse   Selection  4950             50               0.003      PASS
100    Reverse   Insertion  4950             4950             0.005      PASS

100    Random    Selection  4950             ...              ...        PASS
100    Random    Insertion  ...              ...              ...        PASS
...

Observations to relate to theory:
 - Selection sort: comparisons stay ~ n(n-1)/2 for every arrangement;
   swaps are at most n-1.
 - Insertion sort: sorted input needs only n-1 comparisons (best case, O(n));
   reverse input needs ~ n(n-1)/2 comparisons and shifts (worst case, O(n^2)).
 - Both are O(n^2) in the worst case, yet the actual work differs a lot.
```

---

## 11. How to Compile and Run

The program is standard C and requires only a C compiler. No external libraries or build systems are needed.

### Linux / macOS

```bash
gcc -O0 -Wall -o sort_analyzer main.c
./sort_analyzer
```

(`-O0` is recommended so that the compiler does not reorder / remove the instrumented operations and the timing is meaningful for the comparison exercise.)

### Windows (GCC / MinGW)

```cmd
gcc -O0 -Wall -o sort_analyzer.exe main.c
sort_analyzer.exe
```

### Windows (MSVC, Visual Studio Command Prompt)

```cmd
cl main.c
main.exe
```

---

## 12. Project Structure

```
Sorting-Algorithm-Performance-Analyzer/
├── main.c
├── README.md
└── .gitignore
```

| File          | Purpose                                                            |
|---------------|--------------------------------------------------------------------|
| `main.c`      | Complete C source: menu, data generation, sorts, counters, tables. |
| `README.md`   | This file – project description, build, and documentation.         |
| `.gitignore`  | Ignores compiled binaries, object files, and IDE/OS temp files.    |

---

## 13. Learning Outcome

After working through this project, the following ideas are concretely demonstrated:

1. **Asymptotic complexity is an upper bound, not the whole story.** Both Selection and Insertion sort are nominally O(n²), but Insertion Sort can be dramatically faster on nearly-sorted data and slower on reverse data.
2. **Input arrangement matters in practice.** The same n can yield n−1 comparisons or ~n²/2 comparisons purely because of the order in which values appear.
3. **Practical performance vs. theory.** Counting exact comparisons/swaps (not just seconds) lets us connect code directly to the formulas taught in class – e.g. 4950 comparisons for n = 100 matches n(n−1)/2 exactly.
4. **Comparisons and swaps/shifts have different costs.** Selection Sort has a fixed comparison count but very few swaps; Insertion Sort can have fewer comparisons on sorted data but many moves on unsorted data.
5. **Fair benchmarking methodology.** By keeping an untouched master and sorting independent copies, the comparison between the two algorithms is not distorted by one algorithm seeing already-modified data. Using a trusted reference (`qsort`) to verify correctness also eliminates the risk of a "fast" but buggy sort.

---

## 14. Conclusion

This mini project successfully implements Selection Sort and Insertion Sort with detailed instrumentation, and provides both manual and automated modes to measure their behaviour across multiple input sizes and arrangements. The measured counts match the theoretical analysis well: Selection Sort is stable at n(n−1)/2 comparisons for every arrangement, while Insertion Sort shows a striking gap between its best (sorted, O(n)) and worst (reverse, O(n²)) cases. The fairness of the comparison is guaranteed by sorting separate copies of the same untouched master dataset, and the correctness of each sort is independently cross-checked against the C standard library's `qsort`. This exercise reinforces the lesson that asymptotic notation must always be combined with empirical data and input-awareness to reason about real-world algorithm performance.
