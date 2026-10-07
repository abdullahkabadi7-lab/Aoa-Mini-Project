#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_N 100000

typedef struct {
    long long comps;
    long long moves;
    double ms;
    int ok;
} Stats;

void selection_sort(int a[], int n, long long *comps, long long *swaps)
{
    *comps = 0;
    *swaps = 0;
    for (int i = 0; i < n - 1; i++) {
        int min = i;
        for (int j = i + 1; j < n; j++) {
            (*comps)++;
            if (a[j] < a[min])
                min = j;
        }
        if (min != i) {
            int t = a[i];
            a[i] = a[min];
            a[min] = t;
            (*swaps)++;
        }
    }
}

void insertion_sort(int a[], int n, long long *comps, long long *shifts)
{
    *comps = 0;
    *shifts = 0;
    for (int i = 1; i < n; i++) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0) {
            (*comps)++;
            if (a[j] <= key)
                break;
            a[j + 1] = a[j];
            (*shifts)++;
            j--;
        }
        a[j + 1] = key;
    }
}

int cmp(const void *x, const void *y)
{
    int a = *(const int *)x;
    int b = *(const int *)y;
    return (a > b) - (a < b);
}

int is_sorted(int a[], int n)
{
    for (int i = 1; i < n; i++)
        if (a[i - 1] > a[i])
            return 0;
    return 1;
}

void print_array(int a[], int n)
{
    if (n > 50) {
        printf("  too many values to print, first ten: ");
        for (int i = 0; i < 10; i++)
            printf("%d ", a[i]);
        printf("...\n");
        return;
    }
    printf("  ");
    for (int i = 0; i < n; i++)
        printf("%d ", a[i]);
    printf("\n");
}

void fill(int a[], int n, int type)
{
    for (int i = 0; i < n; i++) {
        if (type == 1)
            a[i] = i + 1;
        else if (type == 2)
            a[i] = n - i;
        else
            a[i] = rand() % (n * 10);
    }
}

const char *type_name(int type)
{
    if (type == 1) return "Sorted";
    if (type == 2) return "Reverse";
    return "Random";
}

int get_int(const char *msg, int *value)
{
    char buf[100];
    char *end;
    printf("%s", msg);
    if (fgets(buf, sizeof(buf), stdin) == NULL)
        return 0;
    long v = strtol(buf, &end, 10);
    if (end == buf || (*end != '\n' && *end != '\0')) {
        printf("That is not a number.\n");
        return 0;
    }
    *value = (int)v;
    return 1;
}

int get_range(const char *msg, int lo, int hi)
{
    int v;
    while (!get_int(msg, &v) || v < lo || v > hi)
        printf("Enter a number from %d to %d.\n", lo, hi);
    return v;
}

Stats run(int master[], int n, int which, int result[])
{
    Stats s;
    int *check = malloc(n * sizeof(int));

    memcpy(result, master, n * sizeof(int));

    clock_t start = clock();
    if (which == 1)
        selection_sort(result, n, &s.comps, &s.moves);
    else
        insertion_sort(result, n, &s.comps, &s.moves);
    clock_t end = clock();
    s.ms = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;

    memcpy(check, master, n * sizeof(int));
    qsort(check, n, sizeof(int), cmp);
    s.ok = is_sorted(result, n) && memcmp(result, check, n * sizeof(int)) == 0;

    free(check);
    return s;
}

void print_header(void)
{
    printf("\n%-10s %-14s %-14s %-12s %s\n", "Algorithm", "Comparisons", "Swaps/Shifts", "Time (ms)", "Check");
    printf("----------------------------------------------------------------\n");
}

void print_row(const char *name, Stats s)
{
    printf("%-10s %-14lld %-14lld %-12.3f %s\n", name, s.comps, s.moves, s.ms, s.ok ? "PASS" : "FAIL");
}

void experiment(int trials)
{
    int sizes[] = {100, 500, 1000, 2000};
    int count = sizeof(sizes) / sizeof(sizes[0]);

    printf("\nResults averaged over %d trial(s)\n\n", trials);
    printf("%-6s %-8s %-10s %-14s %-14s %-10s %s\n", "n", "Order", "Algorithm", "Comparisons", "Swaps/Shifts", "Time (ms)", "Check");
    printf("----------------------------------------------------------------------------\n");

    for (int s = 0; s < count; s++) {
        int n = sizes[s];
        int *master = malloc(n * sizeof(int));
        int *result = malloc(n * sizeof(int));

        for (int type = 1; type <= 3; type++) {
            double comps[3] = {0, 0, 0};
            double moves[3] = {0, 0, 0};
            double ms[3] = {0, 0, 0};
            int ok[3] = {1, 1, 1};

            for (int t = 0; t < trials; t++) {
                fill(master, n, type);
                for (int w = 1; w <= 2; w++) {
                    Stats r = run(master, n, w, result);
                    comps[w] += r.comps;
                    moves[w] += r.moves;
                    ms[w] += r.ms;
                    if (!r.ok)
                        ok[w] = 0;
                }
            }

            for (int w = 1; w <= 2; w++) {
                printf("%-6d %-8s %-10s %-14.0f %-14.0f %-10.3f %s\n",
                       n, type_name(type), w == 1 ? "Selection" : "Insertion",
                       comps[w] / trials, moves[w] / trials, ms[w] / trials,
                       ok[w] ? "PASS" : "FAIL");
            }
            printf("\n");
        }

        free(master);
        free(result);
    }

    printf("Selection sort does about n(n-1)/2 comparisons whatever the order,\n");
    printf("but never more than n-1 swaps.\n");
    printf("Insertion sort needs only n-1 comparisons on sorted data and about\n");
    printf("n(n-1)/2 comparisons and shifts on reversed data.\n");
    printf("Both are O(n^2) in the worst case, yet the work done differs a lot.\n");
}

void menu(void)
{
    printf("\n--- Sorting Algorithm Performance Analyzer ---\n");
    printf("1. Enter data manually\n");
    printf("2. Generate data\n");
    printf("3. Display data\n");
    printf("4. Run selection sort\n");
    printf("5. Run insertion sort\n");
    printf("6. Run both and compare\n");
    printf("7. Run full experiment\n");
    printf("8. Set number of trials\n");
    printf("0. Exit\n");
}

int main(void)
{
    int *master = malloc(MAX_N * sizeof(int));
    int *result = malloc(MAX_N * sizeof(int));
    int *result2 = malloc(MAX_N * sizeof(int));
    int n = 0;
    int trials = 3;
    int choice = -1;

    if (master == NULL || result == NULL || result2 == NULL) {
        printf("Not enough memory.\n");
        return 1;
    }

    srand((unsigned)time(NULL));

    while (choice != 0) {
        menu();
        if (!get_int("Choice: ", &choice)) {
            choice = -1;
            continue;
        }

        if (choice == 1) {
            n = get_range("How many numbers? ", 1, MAX_N);
            for (int i = 0; i < n; i++) {
                char msg[30];
                sprintf(msg, "Number %d: ", i + 1);
                while (!get_int(msg, &master[i]))
                    ;
            }
            printf("Data saved.\n");
        } else if (choice == 2) {
            n = get_range("Size (1 to 100000): ", 1, MAX_N);
            printf("1. Sorted\n2. Reverse sorted\n3. Random\n");
            int type = get_range("Arrangement: ", 1, 3);
            fill(master, n, type);
            printf("Generated %d %s values.\n", n, type_name(type));
        } else if (choice >= 3 && choice <= 6) {
            if (n == 0) {
                printf("There is no data yet.\n");
                continue;
            }
            if (choice == 3) {
                print_array(master, n);
            } else if (choice == 4 || choice == 5) {
                int which = choice - 3;
                Stats s = run(master, n, which, result);
                printf("Sorted list:\n");
                print_array(result, n);
                print_header();
                print_row(which == 1 ? "Selection" : "Insertion", s);
            } else {
                Stats a = run(master, n, 1, result);
                Stats b = run(master, n, 2, result2);
                printf("Selection sort result:\n");
                print_array(result, n);
                printf("Insertion sort result:\n");
                print_array(result2, n);
                print_header();
                print_row("Selection", a);
                print_row("Insertion", b);
                printf("\nBoth lists are the same: %s\n",
                       memcmp(result, result2, n * sizeof(int)) == 0 ? "yes" : "no");
            }
        } else if (choice == 7) {
            experiment(trials);
        } else if (choice == 8) {
            trials = get_range("Number of trials (1 to 100): ", 1, 100);
        } else if (choice != 0) {
            printf("Invalid choice.\n");
        }
    }

    free(master);
    free(result);
    free(result2);
    return 0;
}