/*
 * Sorting Algorithm Performance Analyzer
 * Module 1: Introduction to Algorithm Analysis
 *
 * Compares selection sort and insertion sort on identical data and records
 * comparisons, swaps/shifts and elapsed time.
 *
 * Compile:  gcc -O0 -Wall -o sort_analyzer sort_analyzer.c
 * Run:      ./sort_analyzer
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_N          100000
#define PRINT_LIMIT    50      /* only print arrays up to this size */

enum { SORTED = 1, REVERSE = 2, RANDOM_ORDER = 3 };

typedef struct {
    long long comparisons;
    long long moves;      /* swaps for selection sort, shifts for insertion sort */
    double    ms;         /* elapsed time in milliseconds */
    int       correct;    /* 1 if output is ordered and matches reference */
} Result;

/* ------------------------------------------------------------------ */
/* Instrumented sorting algorithms                                     */
/* ------------------------------------------------------------------ */

void selection_sort(int a[], int n, long long *comps, long long *swaps)
{
    int i, j, min, tmp;
    *comps = 0;
    *swaps = 0;
    for (i = 0; i < n - 1; i++) {
        min = i;
        for (j = i + 1; j < n; j++) {
            (*comps)++;
            if (a[j] < a[min])
                min = j;
        }
        if (min != i) {
            tmp = a[i];
            a[i] = a[min];
            a[min] = tmp;
            (*swaps)++;
        }
    }
}

void insertion_sort(int a[], int n, long long *comps, long long *shifts)
{
    int i, j, key;
    *comps = 0;
    *shifts = 0;
    for (i = 1; i < n; i++) {
        key = a[i];
        j = i - 1;
        while (j >= 0) {
            (*comps)++;
            if (a[j] > key) {
                a[j + 1] = a[j];
                (*shifts)++;
                j--;
            } else {
                break;
            }
        }
        a[j + 1] = key;
    }
}

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

static int cmp_int(const void *x, const void *y)
{
    int a = *(const int *)x, b = *(const int *)y;
    return (a > b) - (a < b);
}

int is_sorted(const int a[], int n)
{
    for (int i = 1; i < n; i++)
        if (a[i - 1] > a[i])
            return 0;
    return 1;
}

int arrays_equal(const int a[], const int b[], int n)
{
    return memcmp(a, b, (size_t)n * sizeof(int)) == 0;
}

void print_array(const int a[], int n)
{
    if (n > PRINT_LIMIT) {
        printf("  (n = %d is too large to print; first 10: ", n);
        for (int i = 0; i < 10 && i < n; i++) printf("%d ", a[i]);
        printf("...)\n");
        return;
    }
    printf("  [");
    for (int i = 0; i < n; i++)
        printf("%d%s", a[i], i < n - 1 ? ", " : "");
    printf("]\n");
}

void generate_data(int a[], int n, int kind)
{
    switch (kind) {
    case SORTED:
        for (int i = 0; i < n; i++) a[i] = i + 1;
        break;
    case REVERSE:
        for (int i = 0; i < n; i++) a[i] = n - i;
        break;
    default:
        for (int i = 0; i < n; i++) a[i] = rand() % (n * 10 + 1);
        break;
    }
}

const char *kind_name(int kind)
{
    switch (kind) {
    case SORTED:  return "Sorted";
    case REVERSE: return "Reverse";
    default:      return "Random";
    }
}

/* Safe integer input; returns 1 on success, 0 on failure */
int read_int(const char *prompt, int *out)
{
    char line[128];
    char *end;
    long v;
    printf("%s", prompt);
    if (!fgets(line, sizeof line, stdin))
        return 0;
    v = strtol(line, &end, 10);
    if (end == line || (*end != '\n' && *end != '\0')) {
        printf("  Invalid number.\n");
        return 0;
    }
    *out = (int)v;
    return 1;
}

/* Read an integer within [lo, hi], re-asking until valid */
int read_range(const char *prompt, int lo, int hi)
{
    int v;
    while (!read_int(prompt, &v) || v < lo || v > hi) {
        printf("  Please enter a value between %d and %d.\n", lo, hi);
    }
    return v;
}

/* ------------------------------------------------------------------ */
/* Running one algorithm on a COPY of the master data                  */
/* ------------------------------------------------------------------ */

/* which: 1 = selection, 2 = insertion.
 * 'out' receives the sorted copy (must hold n ints). */
Result run_sort(const int master[], int n, int which, int out[])
{
    Result r;
    clock_t t0, t1;
    int *ref = malloc((size_t)n * sizeof(int));

    memcpy(out, master, (size_t)n * sizeof(int));   /* work on a copy */

    t0 = clock();
    if (which == 1) selection_sort(out, n, &r.comparisons, &r.moves);
    else            insertion_sort(out, n, &r.comparisons, &r.moves);
    t1 = clock();
    r.ms = (double)(t1 - t0) * 1000.0 / CLOCKS_PER_SEC;

    /* correctness: ordered AND equal to a trusted reference (qsort) */
    memcpy(ref, master, (size_t)n * sizeof(int));
    qsort(ref, (size_t)n, sizeof(int), cmp_int);
    r.correct = is_sorted(out, n) && arrays_equal(out, ref, n);

    free(ref);
    return r;
}

void print_result_header(void)
{
    printf("\n%-10s %-16s %-16s %-12s %-8s\n",
           "Algorithm", "Comparisons", "Swaps/Shifts", "Time (ms)", "Correct");
    printf("-------------------------------------------------------------------\n");
}

void print_result_row(const char *name, Result r)
{
    printf("%-10s %-16lld %-16lld %-12.3f %-8s\n",
           name, r.comparisons, r.moves, r.ms, r.correct ? "PASS" : "FAIL");
}

/* ------------------------------------------------------------------ */
/* Full experiment: several sizes x arrangements x trials              */
/* ------------------------------------------------------------------ */

void run_experiment(int trials)
{
    const int sizes[] = {100, 500, 1000, 2000};
    const int nsizes = (int)(sizeof sizes / sizeof sizes[0]);
    int s, k, t;

    printf("\nFull experiment (averages over %d trial(s))\n", trials);
    printf("====================================================================================\n");
    printf("%-6s %-9s %-10s %-16s %-16s %-10s %-6s\n",
           "n", "Order", "Algorithm", "Avg Comparisons", "Avg Swaps/Shifts", "Avg ms", "Check");
    printf("------------------------------------------------------------------------------------\n");

    for (s = 0; s < nsizes; s++) {
        int n = sizes[s];
        int *master = malloc((size_t)n * sizeof(int));
        int *out    = malloc((size_t)n * sizeof(int));

        for (k = SORTED; k <= RANDOM_ORDER; k++) {
            double c[3] = {0}, m[3] = {0}, ms[3] = {0};
            int ok[3] = {1, 1, 1};

            for (t = 0; t < trials; t++) {
                int w;
                generate_data(master, n, k);        /* master stays untouched */
                for (w = 1; w <= 2; w++) {
                    Result r = run_sort(master, n, w, out);
                    c[w]  += (double)r.comparisons;
                    m[w]  += (double)r.moves;
                    ms[w] += r.ms;
                    if (!r.correct) ok[w] = 0;
                }
            }
            for (int w = 1; w <= 2; w++) {
                printf("%-6d %-9s %-10s %-16.0f %-16.0f %-10.3f %-6s\n",
                       n, kind_name(k), w == 1 ? "Selection" : "Insertion",
                       c[w] / trials, m[w] / trials, ms[w] / trials,
                       ok[w] ? "PASS" : "FAIL");
            }
            printf("\n");
        }
        free(master);
        free(out);
    }

    printf("Observations to relate to theory:\n");
    printf(" - Selection sort: comparisons stay ~ n(n-1)/2 for every arrangement;\n");
    printf("   swaps are at most n-1.\n");
    printf(" - Insertion sort: sorted input needs only n-1 comparisons (best case, O(n));\n");
    printf("   reverse input needs ~ n(n-1)/2 comparisons and shifts (worst case, O(n^2)).\n");
    printf(" - Both are O(n^2) in the worst case, yet the actual work differs a lot.\n");
}

/* ------------------------------------------------------------------ */
/* Menu                                                                */
/* ------------------------------------------------------------------ */

void show_menu(void)
{
    printf("\n========== Sorting Algorithm Performance Analyzer ==========\n");
    printf(" 1. Enter data manually\n");
    printf(" 2. Generate data (sorted / reverse / random)\n");
    printf(" 3. Display master data\n");
    printf(" 4. Run selection sort\n");
    printf(" 5. Run insertion sort\n");
    printf(" 6. Run both and compare\n");
    printf(" 7. Run full experiment (multiple sizes and arrangements)\n");
    printf(" 8. Set number of trials (currently used by option 7)\n");
    printf(" 0. Exit\n");
    printf("=============================================================\n");
}

int main(void)
{
    int *master = NULL;     /* untouched master copy */
    int *out    = NULL;     /* scratch buffer for sorted output */
    int n = 0;
    int trials = 3;
    int choice;

    srand((unsigned)time(NULL));

    master = malloc(MAX_N * sizeof(int));
    out    = malloc(MAX_N * sizeof(int));
    if (!master || !out) {
        printf("Memory allocation failed.\n");
        return 1;
    }

    do {
        show_menu();
        if (!read_int("Choose an option: ", &choice)) {
            choice = -1;
            continue;
        }

        switch (choice) {
        case 1: {
            n = read_range("How many integers? ", 1, MAX_N);
            printf("Enter %d integers (one per line):\n", n);
            for (int i = 0; i < n; i++) {
                char prompt[32];
                int v;
                sprintf(prompt, "  [%d]: ", i);
                while (!read_int(prompt, &v)) { /* retry */ }
                master[i] = v;
            }
            printf("Data stored.\n");
            break;
        }
        case 2: {
            int kind;
            n = read_range("Dataset size (1-100000): ", 1, MAX_N);
            printf("Arrangement: 1) Sorted  2) Reverse-sorted  3) Random\n");
            kind = read_range("Choose arrangement: ", 1, 3);
            generate_data(master, n, kind);
            printf("Generated %d %s values.\n", n, kind_name(kind));
            break;
        }
        case 3:
            if (n == 0) printf("No data yet. Use option 1 or 2.\n");
            else { printf("Master data (n = %d):\n", n); print_array(master, n); }
            break;
        case 4:
        case 5: {
            int which = (choice == 4) ? 1 : 2;
            Result r;
            if (n == 0) { printf("No data yet. Use option 1 or 2.\n"); break; }
            r = run_sort(master, n, which, out);
            printf("\nSorted output (%s sort):\n", which == 1 ? "selection" : "insertion");
            print_array(out, n);
            print_result_header();
            print_result_row(which == 1 ? "Selection" : "Insertion", r);
            break;
        }
        case 6: {
            int *out2;
            Result rs, ri;
            if (n == 0) { printf("No data yet. Use option 1 or 2.\n"); break; }
            out2 = malloc((size_t)n * sizeof(int));
            rs = run_sort(master, n, 1, out);    /* both start from the same master */
            ri = run_sort(master, n, 2, out2);

            printf("\nSelection sort output:\n"); print_array(out, n);
            printf("Insertion sort output:\n");   print_array(out2, n);
            print_result_header();
            print_result_row("Selection", rs);
            print_result_row("Insertion", ri);
            printf("\nBoth outputs identical: %s\n",
                   arrays_equal(out, out2, n) ? "YES" : "NO");
            free(out2);
            break;
        }
        case 7:
            run_experiment(trials);
            break;
        case 8:
            trials = read_range("Number of trials (1-100): ", 1, 100);
            printf("Trials set to %d.\n", trials);
            break;
        case 0:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid option.\n");
        }
    } while (choice != 0);

    free(master);
    free(out);
    return 0;
}
