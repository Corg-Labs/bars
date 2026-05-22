# Sorting Algorithm Visualizer in C

A terminal-based, full-color visualizer of five classic sorting algorithms — written entirely in C.

The program renders an array as a row of colored vertical bars, then animates each sort step by step so you can watch the algorithm work. Bars are colored by their current state (idle, being compared, being moved, already sorted), giving every algorithm a distinctive visual *personality*.

No graphics library. No GPU. Just an array, some swaps, and stdout.

This project focuses on learning:

- five classic sorting algorithms
- in-place vs. auxiliary-buffer sorting
- O(n^2) vs. O(n log n) algorithms
- comparison-based vs. partition-based vs. divide-and-conquer
- step-by-step visualization
- ANSI 256-color terminal rendering
- real-time animation in C

---

# Features

- Five algorithms in rotation:
  - **Bubble sort**
  - **Insertion sort**
  - **Selection sort**
  - **Quick sort**
  - **Merge sort**
- Color-coded bar states:
  - **Blue** - idle
  - **Yellow** - being compared
  - **Red** - being moved / swapped
  - **Green** - confirmed sorted
- Adaptive step pacing per algorithm
- Pure terminal rendering
- ANSI 256-color output
- Written entirely in C
- Standard libraries only

---

# How It Works

The array `arr[N]` holds the values and `state[N]` holds the per-bar status (idle / compare / active / sorted). Every algorithm updates these as it runs, and a render function paints both arrays to the terminal between every step.

For every step:

- The algorithm updates `arr` and `state`
- `render()` draws the array as bars from bottom to top using `state` for color
- `usleep()` adds a small delay so the human eye can follow

The same render function works for every algorithm — only the state-marking conventions differ.

---

# Tutorial / Visualization Pipeline

## 1. The Array and Its State

Two parallel arrays of length `N`:

```
int arr[N];     // bar heights
int state[N];   // 0 idle, 1 compare, 2 active, 3 sorted
```

`arr` holds a shuffled range `1..N`. `state` lets us paint bars differently depending on what the algorithm is currently doing.

---

## 2. Bar Rendering

For each row from the top down, every column is either *empty* (the bar isn't tall enough yet) or *filled* with the color associated with the bar's current state:

```
for row in HEIGHT..1:
    for col in 0..N-1:
        if arr[col] >= row:  paint state-colored cell
        else:                paint background
```

Bar widths can be 1 or 2 characters — wider bars are easier to see in screen recordings.

---

## 3. The Five Algorithms

### Bubble sort
Compares adjacent pairs and swaps if out of order, "bubbling" the largest unsorted element to the end. After pass `i`, the last `i + 1` items are confirmed sorted.

```
for i in 0..N-1:
    for j in 0..N-1-i:
        if arr[j] > arr[j+1]: swap
    mark arr[N-1-i] as sorted
```

### Insertion sort
Walks through the array, taking each new element and inserting it into the correct position among the already-sorted prefix.

```
for i in 1..N-1:
    v = arr[i]
    shift arr[j] right while arr[j] > v
    arr[j+1] = v
```

### Selection sort
On each pass, find the index of the smallest unsorted element and swap it into position.

```
for i in 0..N-1:
    m = index of min in arr[i..N-1]
    swap arr[i] <-> arr[m]
```

### Quick sort
Pick a pivot, partition the array around it, then recurse on each side. Classic Lomuto partitioning.

```
pivot = arr[hi]
i = lo - 1
for j in lo..hi-1:
    if arr[j] < pivot:
        i++; swap arr[i] <-> arr[j]
swap arr[++i] <-> arr[hi]
quick(lo, i-1); quick(i+1, hi)
```

### Merge sort
Recursively split the array in halves, then merge sorted halves together using an auxiliary buffer.

```
merge(lo, hi):
    mid = (lo + hi) / 2
    merge(lo, mid)
    merge(mid+1, hi)
    merge the two halves into a buffer
    copy the buffer back into arr
```

Each algorithm gets its own pacing (e.g., bubble runs faster per step because it has many more steps), so each segment of the visualization takes roughly the same amount of wall-clock time.

---

## 4. ANSI 256-Color Output

Each cell is a colored space character:

```
printf("\x1b[48;5;%dm ", index);
```

Adjacent cells with the same color share one escape code to minimize bytes per frame.

---

# Build

Compile using:

```
gcc bars.c -o bars
```

Or just:

```
make
```

No math library needed.

---

# Run

```
./bars
```

Press `Ctrl+C` to exit.

For best results, run in a 256-color terminal at roughly 96 columns × 24 rows.

---

# Customizing

Edit the constants near the top of `bars.c`:

- `N` - number of bars
- `HEIGHT` - vertical resolution (also the maximum bar value)
- `BAR_W` - characters per bar (1 or 2)
- `sleeps[]` in `main()` - per-algorithm step pacing

Adding a new algorithm? Implement it as a `void f(void)`, then add it to the `algos[]` array.

---

# Concepts Practiced

- Classic sorting algorithms
- Algorithm visualization
- Step-by-step animation
- State machines per array slot
- Adaptive pacing
- ANSI 256-color rendering
- Real-time animation
- Terminal graphics

---

# Dependencies

Standard C libraries only:

- `stdio.h`
- `stdlib.h`
- `string.h`
- `unistd.h`
- `time.h`

No graphics engine required.

---

# Inspiration

Inspired by the long tradition of "sound of sorting" videos on YouTube. Watching algorithms actually *behave* — bubble sort's relentless pair-comparisons, quick sort's vertiginous recursion, merge sort's quiet halving — beats any number of textbook diagrams.

This is the same idea, distilled into a single C file and a terminal.
