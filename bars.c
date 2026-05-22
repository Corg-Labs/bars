/*
 * bars.c - Animated sorting algorithm visualizer in C
 *
 * Renders an array as a row of colored vertical bars and animates
 * five classic sorting algorithms in sequence: bubble, insertion,
 * selection, quick, and merge. Bars are colored by their current
 * state (idle, being compared, being swapped, already sorted).
 *
 * Build:  gcc bars.c -o bars
 * Run:    ./bars
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define N        48
#define HEIGHT   24
#define BAR_W    2

#define COL_BG      16
#define COL_IDLE    27     /* blue   */
#define COL_CMP     226    /* yellow */
#define COL_ACT     196    /* red    */
#define COL_DONE    46     /* green  */

static int  arr[N];
static int  state[N];   /* 0 idle, 1 compare, 2 active, 3 sorted */

static int  step_sleep_us = 4000;

/* ---------- rendering ---------- */
static void render(void) {
    printf("\x1b[H");
    char line[N * BAR_W * 16 + 32];
    for (int r = HEIGHT; r >= 1; r--) {
        int prev = -1;
        char *q = line;
        for (int c = 0; c < N; c++) {
            int filled = arr[c] >= r;
            int col;
            if (!filled) {
                col = COL_BG;
            } else {
                switch (state[c]) {
                    case 1: col = COL_CMP;  break;
                    case 2: col = COL_ACT;  break;
                    case 3: col = COL_DONE; break;
                    default: col = COL_IDLE; break;
                }
            }
            if (col != prev) {
                q += sprintf(q, "\x1b[48;5;%dm", col);
                prev = col;
            }
            for (int b = 0; b < BAR_W; b++) *q++ = ' ';
        }
        q += sprintf(q, "\x1b[0m\n");
        fwrite(line, 1, q - line, stdout);
    }
    fflush(stdout);
}

static void frame(int us) {
    render();
    usleep(us);
}

static void shuffle(void) {
    for (int i = 0; i < N; i++) arr[i] = (i % HEIGHT) + 1;
    for (int i = N - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = arr[i]; arr[i] = arr[j]; arr[j] = t;
    }
    for (int i = 0; i < N; i++) state[i] = 0;
}

static void clear_idle(void) {
    for (int i = 0; i < N; i++) if (state[i] != 3) state[i] = 0;
}

static void mark_all_sorted(void) {
    for (int i = 0; i < N; i++) state[i] = 3;
}

/* ---------- algorithms ---------- */
static void bubble(void) {
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N - 1 - i; j++) {
            clear_idle();
            state[j] = 1; state[j+1] = 1;
            frame(step_sleep_us);
            if (arr[j] > arr[j+1]) {
                int t = arr[j]; arr[j] = arr[j+1]; arr[j+1] = t;
                state[j] = 2; state[j+1] = 2;
                frame(step_sleep_us);
            }
        }
        state[N - 1 - i] = 3;
    }
    state[0] = 3;
}

static void insertion(void) {
    state[0] = 3;
    for (int i = 1; i < N; i++) {
        int v = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > v) {
            clear_idle();
            for (int k = 0; k < i; k++) if (state[k] != 3) state[k] = 3;
            state[j] = 1; state[j+1] = 2;
            arr[j+1] = arr[j];
            frame(step_sleep_us * 2);
            j--;
        }
        arr[j+1] = v;
        for (int k = 0; k <= i; k++) state[k] = 3;
    }
}

static void selection(void) {
    for (int i = 0; i < N - 1; i++) {
        int m = i;
        for (int j = i + 1; j < N; j++) {
            clear_idle();
            state[m] = 2; state[j] = 1;
            frame(step_sleep_us * 2);
            if (arr[j] < arr[m]) m = j;
        }
        int t = arr[i]; arr[i] = arr[m]; arr[m] = t;
        state[i] = 3;
    }
    state[N - 1] = 3;
}

static void quick_h(int lo, int hi) {
    if (lo > hi) return;
    if (lo == hi) { if (state[lo] != 3) state[lo] = 3; return; }
    int pivot = arr[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        clear_idle();
        state[hi] = 2;
        state[j]  = 1;
        frame(step_sleep_us * 6);
        if (arr[j] < pivot) {
            i++;
            int t = arr[i]; arr[i] = arr[j]; arr[j] = t;
            state[i] = 2; state[j] = 2;
            frame(step_sleep_us * 3);
        }
    }
    i++;
    int t = arr[i]; arr[i] = arr[hi]; arr[hi] = t;
    state[i] = 3;
    frame(step_sleep_us * 6);
    quick_h(lo, i - 1);
    quick_h(i + 1, hi);
}

static void quicksort(void) {
    quick_h(0, N - 1);
}

static int  buf[N];
static void merge_h(int lo, int hi) {
    if (lo >= hi) return;
    int mid = (lo + hi) / 2;
    merge_h(lo, mid);
    merge_h(mid + 1, hi);
    int i = lo, j = mid + 1, k = 0;
    while (i <= mid && j <= hi) {
        clear_idle();
        state[i] = 1; state[j] = 1;
        frame(step_sleep_us * 4);
        if (arr[i] <= arr[j]) buf[k++] = arr[i++];
        else                  buf[k++] = arr[j++];
    }
    while (i <= mid) buf[k++] = arr[i++];
    while (j <= hi)  buf[k++] = arr[j++];
    for (int x = 0; x < k; x++) {
        arr[lo + x] = buf[x];
        state[lo + x] = 2;
        frame(step_sleep_us * 2);
    }
    for (int x = 0; x < k; x++) state[lo + x] = (hi - lo + 1 == N) ? 3 : 0;
}

static void mergesort(void) {
    merge_h(0, N - 1);
    mark_all_sorted();
}

/* ---------- main ---------- */
int main(void) {
    srand((unsigned)time(NULL));
    printf("\x1b[?25l\x1b[2J");

    void  (*algos[])(void) = { bubble, insertion, selection, quicksort, mergesort };
    int   sleeps[]          = { 3500,   7000,      6500,     35000,     22000 };
    int   n = sizeof algos / sizeof algos[0];

    int idx = 0;
    for (;;) {
        shuffle();
        render();
        usleep(600000);

        step_sleep_us = sleeps[idx];
        algos[idx]();

        mark_all_sorted();
        render();
        usleep(1500000);

        idx = (idx + 1) % n;
    }
    return 0;
}
