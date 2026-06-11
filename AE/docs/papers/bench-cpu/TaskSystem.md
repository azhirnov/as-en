
[source code](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/threading/Perf_TaskSystemCoro.cpp)


# AMD Ryzen 3900X

24 threads, 12 cores

## W=4 H=4

```
Total time: 0.13 s, final jobs: 349524, task time: 5.21 us, overhead: 40.87 %
Total time: 0.12 s, final jobs: 349524, task time: 5.28 us, overhead: 37.53 %
Total time: 0.12 s, final jobs: 349524, task time: 5.19 us, overhead: 37.05 %
Total time: 0.12 s, final jobs: 349524, task time: 5.27 us, overhead: 38.20 %
------------------------
Total time: 0.27 s, final jobs: 349524, task time: 4.96 us, overhead: 72.84 %
Total time: 0.37 s, final jobs: 349524, task time: 5.22 us, overhead: 79.24 %
Total time: 0.43 s, final jobs: 349524, task time: 5.27 us, overhead: 82.28 %
Total time: 0.26 s, final jobs: 349524, task time: 5.01 us, overhead: 71.95 %
------------------------
Total time: 0.52 s, final jobs: 1474559, task time: 5.95 us, overhead: 29.65 %
Total time: 0.55 s, final jobs: 1474560, task time: 6.00 us, overhead: 33.53 %
Total time: 0.54 s, final jobs: 1474560, task time: 6.15 us, overhead: 29.57 %
Total time: 0.59 s, final jobs: 1474560, task time: 6.25 us, overhead: 34.76 %
```

# Intel N150

4 threads, 4 cores

## W=4 H=4

```
Total time: 0.79 s, final jobs: 349524, task time: 5.58 us, overhead: 38.32 %
Total time: 0.67 s, final jobs: 349524, task time: 4.64 us, overhead: 39.31 %
Total time: 0.58 s, final jobs: 349524, task time: 4.34 us, overhead: 34.96 %
Total time: 0.56 s, final jobs: 349524, task time: 4.13 us, overhead: 35.83 %
------------------------
Total time: 1.24 s, final jobs: 349524, task time: 4.52 us, overhead: 68.07 %
Total time: 0.81 s, final jobs: 349524, task time: 4.42 us, overhead: 52.51 %
Total time: 1.16 s, final jobs: 349524, task time: 4.67 us, overhead: 64.91 %
Total time: 0.63 s, final jobs: 349524, task time: 4.29 us, overhead: 40.43 %
------------------------
Total time: 68.57 ms, final jobs: 40960, task time: 4.93 us, overhead: 26.38 %
Total time: 66.78 ms, final jobs: 40960, task time: 4.96 us, overhead: 23.99 %
Total time: 73.09 ms, final jobs: 40960, task time: 5.53 us, overhead: 22.51 %
Total time: 66.21 ms, final jobs: 40960, task time: 4.85 us, overhead: 24.99 %
```

## W=16 H=16

```
Total time: 6.00 s, final jobs: 349524, task time: 65.96 us, overhead: 3.91 %
Total time: 6.02 s, final jobs: 349524, task time: 66.27 us, overhead: 3.74 %
Total time: 5.89 s, final jobs: 349524, task time: 64.89 us, overhead: 3.81 %
Total time: 5.91 s, final jobs: 349524, task time: 65.18 us, overhead: 3.61 %
------------------------
Total time: 6.21 s, final jobs: 349524, task time: 66.17 us, overhead: 6.89 %
Total time: 6.04 s, final jobs: 349524, task time: 65.94 us, overhead: 4.65 %
Total time: 5.96 s, final jobs: 349524, task time: 65.18 us, overhead: 4.41 %
Total time: 6.26 s, final jobs: 349524, task time: 65.66 us, overhead: 8.35 %
------------------------
Total time: 0.81 s, final jobs: 40960, task time: 77.08 us, overhead: 2.68 %
Total time: 0.72 s, final jobs: 40960, task time: 68.23 us, overhead: 2.49 %
Total time: 0.74 s, final jobs: 40960, task time: 70.23 us, overhead: 2.86 %
Total time: 0.70 s, final jobs: 40960, task time: 66.26 us, overhead: 3.29 %
```


# Realme 8i

8 threads, 8 cores (2P + 6E)

## W=4 H=4

```
Total time: 1.41 s, final jobs: 349524, task time: 16.97 us, overhead: 47.48 %
Total time: 1.41 s, final jobs: 349524, task time: 19.38 us, overhead: 39.92 %
Total time: 1.43 s, final jobs: 349524, task time: 19.76 us, overhead: 39.76 %
Total time: 1.98 s, final jobs: 349524, task time: 27.37 us, overhead: 39.53 %
------------------------
Total time: 1.48 s, final jobs: 349524, task time: 20.11 us, overhead: 40.84 %
Total time: 1.42 s, final jobs: 349524, task time: 19.95 us, overhead: 38.42 %
Total time: 1.43 s, final jobs: 349524, task time: 19.18 us, overhead: 41.50 %
Total time: 3.15 s, final jobs: 349524, task time: 44.12 us, overhead: 38.78 %
------------------------
Total time: 0.55 s, final jobs: 163840, task time: 19.98 us, overhead: 26.11 %
Total time: 1.34 s, final jobs: 163840, task time: 49.99 us, overhead: 23.54 %
Total time: 0.57 s, final jobs: 163840, task time: 17.54 us, overhead: 36.52 %
Total time: 0.56 s, final jobs: 163839, task time: 21.26 us, overhead: 22.13 %
```


# Pico 4 Ultra

6 threads, 3 cores available (1P + 2E)

## W=4 H=4

```
Total time: 0.48 s, final jobs: 349524, task time: 5.98 us, overhead: 27.06 %
Total time: 0.47 s, final jobs: 349524, task time: 4.89 us, overhead: 39.61 %
Total time: 0.48 s, final jobs: 349524, task time: 4.26 us, overhead: 48.02 %
Total time: 0.47 s, final jobs: 349524, task time: 4.89 us, overhead: 39.18 %
------------------------
Total time: 0.64 s, final jobs: 349524, task time: 4.70 us, overhead: 56.93 %
Total time: 0.68 s, final jobs: 349524, task time: 4.66 us, overhead: 59.86 %
Total time: 0.54 s, final jobs: 349524, task time: 5.20 us, overhead: 43.76 %
Total time: 0.55 s, final jobs: 349524, task time: 4.98 us, overhead: 47.44 %
------------------------
Total time: 0.10 s, final jobs: 92157, task time: 4.79 us, overhead: 28.27 %
Total time: 0.12 s, final jobs: 92160, task time: 5.05 us, overhead: 36.47 %
Total time: 0.11 s, final jobs: 92160, task time: 5.65 us, overhead: 22.89 %
Total time: 0.12 s, final jobs: 92159, task time: 5.25 us, overhead: 31.04 %
```
