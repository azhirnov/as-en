
[source code](https://github.com/azhirnov/as-en/blob/dev/AE/engine/performance/threading/Perf_AsyncMutex.cpp)


Results of 10'000 AM calls, used 4 threads

| time (ms)  | CPU          |
|------|--------------------|
| 2    | Ryzen 3900 4.2GHz  |
| 2    | Max M1             |
| 4.5  | avr for Android    |
| 5.6  | Intel N150 3.6GHz  |
| 2.55 | ARM Cortex A76 2GHz (Realme 8i) |
| 4.32 | ARM Cortex A78C 2GHz 2T (Pico 4 Ultra) |
| 6.01 | ARM Cortex A78C 2GHz (Pico 4 Ultra) |
