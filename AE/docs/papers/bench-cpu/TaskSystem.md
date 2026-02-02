
[source code](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/threading/Perf_TaskSystemCoro.cpp)


# AMD Ryzen 3900X

24 threads?

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

4 threads

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
