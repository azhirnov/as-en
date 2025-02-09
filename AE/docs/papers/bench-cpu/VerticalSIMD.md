
# AMD Ryzen 3900X

```
Step=2, Step2=4
	Vertical SIMD, Dot test:
	  vert DotAVX v1: 17.95 ms             - 622.9MOp/s,  12.5GFLOPS,  17.4GB/s  <<<
	  vert DotAVX v5: 18.20 ms  +1.4%      - 614.5MOp/s,  12.3GFLOPS,  17.2GB/s
	  vert DotAVX v2: 21.51 ms  +19.8%     - 520.0MOp/s,  10.4GFLOPS,  14.6GB/s
	  vert Dot v2   : 21.79 ms  +21.4%     - 513.2MOp/s,  10.3GFLOPS,  14.4GB/s
	  vert DotAVX v3: 22.15 ms  +23.4%     - 504.9MOp/s,  10.1GFLOPS,  14.1GB/s
	  vert DotAVX v4: 22.64 ms  +26.1%     - 494.1MOp/s,  9.88GFLOPS,  13.8GB/s
	  vert Dot v5   : 28.08 ms  +56.4%     - 398.3MOp/s,  7.97GFLOPS,  11.2GB/s
	  vert Dot v1   : 29.17 ms  +62.5%     - 383.4MOp/s,  7.67GFLOPS,  10.7GB/s
	  vert Dot v4   : 37.53 ms  +109.0%    - 298.0MOp/s,  5.96GFLOPS,  8.34GB/s
	  Dot3 SSE2     : 53.56 ms  +198.3%    - 208.8MOp/s,  4.18GFLOPS,  10.0GB/s
	  vert Dot v3   : 63.69 ms  +254.7%    - 175.6MOp/s,  3.51GFLOPS,  4.92GB/s
	  Dot3 SSE3     : 74.37 ms  +314.2%    - 150.4MOp/s,  3.01GFLOPS,  7.22GB/s
	  Dot3 SSE4.1   : 77.59 ms  +332.1%    - 144.2MOp/s,  2.88GFLOPS,  6.92GB/s

	Vertical SIMD, Cross test:
	  vert CrossAVX v3: 31.97 ms           - 349.9MOp/s,  12.6GFLOPS,  12.6GB/s  <<
	  vert Cross v3   : 37.72 ms  +18.0%   - 296.6MOp/s,  10.7GFLOPS,  10.7GB/s
	  vert CrossAVX v2: 37.87 ms  +18.5%   - 295.3MOp/s,  10.6GFLOPS,  10.6GB/s
	  vert Cross v2   : 38.19 ms  +19.4%   - 292.9MOp/s,  10.5GFLOPS,  10.5GB/s
	  vert CrossAVX v1: 41.28 ms  +29.1%   - 271.0MOp/s,  9.75GFLOPS,  9.75GB/s
	  vert Cross v1   : 48.12 ms  +50.5%   - 232.4MOp/s,  8.37GFLOPS,  8.37GB/s
	  Cross SSE2 v2   : 53.47 ms  +67.3%   - 209.2MOp/s,  7.53GFLOPS,  10.0GB/s
	  Cross SSE2 v1   : 53.52 ms  +67.4%   - 209.0MOp/s,  7.52GFLOPS,  10.0GB/s


Step=4, Step2=8
	Vertical SIMD, Dot test:
	  vert DotAVX v1: 20.48 ms             - 546.1MOp/s,  10.9GFLOPS,  15.3GB/s
	  vert DotAVX v5: 21.40 ms  +4.5%      - 522.6MOp/s,  10.5GFLOPS,  14.6GB/s
	  vert Dot v2   : 23.11 ms  +12.8%     - 484.0MOp/s,  9.68GFLOPS,  13.6GB/s
	  vert DotAVX v2: 24.32 ms  +18.7%     - 459.9MOp/s,  9.20GFLOPS,  12.9GB/s
	  vert DotAVX v4: 28.47 ms  +39.0%     - 392.9MOp/s,  7.86GFLOPS,  11.0GB/s
	  vert DotAVX v3: 28.57 ms  +39.5%     - 391.6MOp/s,  7.83GFLOPS,  11.0GB/s
	  vert Dot v1   : 31.31 ms  +52.9%     - 357.3MOp/s,  7.15GFLOPS,  10.0GB/s
	  vert Dot v5   : 33.27 ms  +62.5%     - 336.1MOp/s,  6.72GFLOPS,  9.41GB/s
	  vert Dot v4   : 35.45 ms  +73.1%     - 315.5MOp/s,  6.31GFLOPS,  8.83GB/s
	  vert Dot v3   : 36.36 ms  +77.5%     - 307.6MOp/s,  6.15GFLOPS,  8.61GB/s
	  Dot3 SSE2     : 51.15 ms  +149.7%    - 218.7MOp/s,  4.37GFLOPS,  10.5GB/s
	  Dot3 SSE3     : 59.15 ms  +188.8%    - 189.1MOp/s,  3.78GFLOPS,  9.08GB/s
	  Dot3 SSE4.1   : 67.29 ms  +228.6%    - 166.2MOp/s,  3.32GFLOPS,  7.98GB/s

	Vertical SIMD, Cross test:
	  vert Cross v2   : 36.74 ms           - 304.4MOp/s,  11.0GFLOPS,  11.0GB/s
	  vert CrossAVX v1: 37.01 ms  +0.7%    - 302.2MOp/s,  10.9GFLOPS,  10.9GB/s
	  vert CrossAVX v3: 37.87 ms  +3.1%    - 295.3MOp/s,  10.6GFLOPS,  10.6GB/s
	  vert Cross v1   : 39.11 ms  +6.4%    - 286.0MOp/s,  10.3GFLOPS,  10.3GB/s
	  vert Cross v3   : 39.21 ms  +6.7%    - 285.3MOp/s,  10.3GFLOPS,  10.3GB/s
	  vert CrossAVX v2: 41.87 ms  +13.9%   - 267.2MOp/s,  9.62GFLOPS,  9.62GB/s
	  Cross SSE2 v2   : 54.53 ms  +48.4%   - 205.1MOp/s,  7.38GFLOPS,  9.85GB/s
	  Cross SSE2 v1   : 54.63 ms  +48.7%   - 204.7MOp/s,  7.37GFLOPS,  9.83GB/s


Step=1, Step2=2
	Vertical SIMD, Dot test:
	  vert DotAVX v1: 20.20 ms             - 553.7MOp/s,  16.6GFLOPS,  15.5GB/s
	  vert DotAVX v2: 21.86 ms  +8.2%      - 511.6MOp/s,  15.3GFLOPS,  14.3GB/s
	  vert DotAVX v4: 22.84 ms  +13.1%     - 489.7MOp/s,  14.7GFLOPS,  13.7GB/s
	  vert DotAVX v5: 23.39 ms  +15.8%     - 478.2MOp/s,  14.3GFLOPS,  13.4GB/s
	  vert Dot v2   : 23.94 ms  +18.5%     - 467.1MOp/s,  14.0GFLOPS,  13.1GB/s
	  vert DotAVX v3: 24.65 ms  +22.0%     - 453.8MOp/s,  13.6GFLOPS,  12.7GB/s
	  vert Dot v1   : 25.10 ms  +24.3%     - 445.5MOp/s,  13.4GFLOPS,  12.5GB/s
	  vert Dot v3   : 25.16 ms  +24.5%     - 444.6MOp/s,  13.3GFLOPS,  12.4GB/s
	  vert Dot v5   : 25.50 ms  +26.2%     - 438.6MOp/s,  13.2GFLOPS,  12.3GB/s
	  vert Dot v4   : 25.53 ms  +26.4%     - 438.1MOp/s,  13.1GFLOPS,  12.3GB/s
	  Dot3 SSE2     : 62.87 ms  +211.3%    - 177.9MOp/s,  5.34GFLOPS,  8.54GB/s
	  Dot3 SSE4.1   : 83.33 ms  +312.5%    - 134.2MOp/s,  4.03GFLOPS,  6.44GB/s
	  Dot3 SSE3     : 83.43 ms  +313.1%    - 134.1MOp/s,  4.02GFLOPS,  6.43GB/s

	Vertical SIMD, Cross test:
	  vert CrossAVX v3: 31.74 ms           - 352.4MOp/s,  19.0GFLOPS,  12.7GB/s  <<<
	  vert CrossAVX v1: 31.83 ms  +0.3%    - 351.4MOp/s,  19.0GFLOPS,  12.7GB/s
	  vert CrossAVX v2: 31.93 ms  +0.6%    - 350.3MOp/s,  18.9GFLOPS,  12.6GB/s
	  vert Cross v3   : 35.48 ms  +11.8%   - 315.3MOp/s,  17.0GFLOPS,  11.3GB/s
	  vert Cross v2   : 35.99 ms  +13.4%   - 310.8MOp/s,  16.8GFLOPS,  11.2GB/s
	  vert Cross v1   : 36.04 ms  +13.5%   - 310.4MOp/s,  16.8GFLOPS,  11.2GB/s
	  Cross SSE2 v1   : 71.55 ms  +125.4%  - 156.3MOp/s,  8.44GFLOPS,  7.50GB/s
	  Cross SSE2 v2   : 73.01 ms  +130.0%  - 153.2MOp/s,  8.27GFLOPS,  7.35GB/s


Step=1, Step2=1
	Vertical SIMD, Dot test:
	  vert DotAVX v1: 17.97 ms             - 622.3MOp/s,  12.4GFLOPS,  17.4GB/s  <<<
	  vert DotAVX v5: 18.46 ms  +2.7%      - 605.9MOp/s,  12.1GFLOPS,  17.0GB/s
	  vert DotAVX v2: 22.23 ms  +23.7%     - 503.1MOp/s,  10.1GFLOPS,  14.1GB/s
	  vert DotAVX v3: 22.67 ms  +26.1%     - 493.4MOp/s,  9.87GFLOPS,  13.8GB/s
	  vert DotAVX v4: 22.95 ms  +27.7%     - 487.4MOp/s,  9.75GFLOPS,  13.6GB/s
	  vert Dot v4   : 23.22 ms  +29.2%     - 481.6MOp/s,  9.63GFLOPS,  13.5GB/s
	  vert Dot v2   : 23.94 ms  +33.2%     - 467.3MOp/s,  9.35GFLOPS,  13.1GB/s
	  vert Dot v3   : 24.12 ms  +34.2%     - 463.6MOp/s,  9.27GFLOPS,  13.0GB/s
	  vert Dot v1   : 24.56 ms  +36.7%     - 455.4MOp/s,  9.11GFLOPS,  12.7GB/s
	  vert Dot v5   : 25.52 ms  +42.0%     - 438.2MOp/s,  8.76GFLOPS,  12.3GB/s
	  Dot3 SSE2     : 52.38 ms  +191.4%    - 213.5MOp/s,  4.27GFLOPS,  10.2GB/s
	  Dot3 SSE4.1   : 69.52 ms  +286.8%    - 160.9MOp/s,  3.22GFLOPS,  7.72GB/s
	  Dot3 SSE3     : 73.20 ms  +307.2%    - 152.8MOp/s,  3.06GFLOPS,  7.33GB/s

	Vertical SIMD, Cross test:
	  vert CrossAVX v3: 32.06 ms           - 348.9MOp/s,  12.6GFLOPS,  12.6GB/s  <<
	  vert Cross v1   : 35.66 ms  +11.2%   - 313.6MOp/s,  11.3GFLOPS,  11.3GB/s
	  vert Cross v3   : 35.75 ms  +11.5%   - 312.9MOp/s,  11.3GFLOPS,  11.3GB/s
	  vert Cross v2   : 35.84 ms  +11.8%   - 312.1MOp/s,  11.2GFLOPS,  11.2GB/s
	  vert CrossAVX v2: 37.00 ms  +15.4%   - 302.3MOp/s,  10.9GFLOPS,  10.9GB/s
	  vert CrossAVX v1: 40.96 ms  +27.7%   - 273.1MOp/s,  9.83GFLOPS,  9.83GB/s
	  Cross SSE2 v1   : 66.18 ms  +106.4%  - 169.0MOp/s,  6.08GFLOPS,  8.11GB/s
	  Cross SSE2 v2   : 70.50 ms  +119.9%  - 158.6MOp/s,  5.71GFLOPS,  7.61GB/s
```


# Apple M1, P-core, 3.2 GHz, 5nm (Mac Mini M1 16GB, Firestorm)

```
Step=4
	Vertical SIMD, Dot test:
	  vert Dot v2 : 6.46 ms             - 1.73GOp/s,  8.65GFLOPS,  48.4GB/s  <<
	  vert Dot v4 : 6.49 ms  +0.4%      - 1.72GOp/s,  8.61GFLOPS,  48.2GB/s
	  vert Dot v5 : 6.50 ms  +0.5%      - 1.72GOp/s,  8.61GFLOPS,  48.2GB/s
	  vert Dot v3 : 6.53 ms  +1.0%      - 1.71GOp/s,  8.57GFLOPS,  48.0GB/s
	  vert Dot v1 : 6.62 ms  +2.4%      - 1.69GOp/s,  8.44GFLOPS,  47.3GB/s
	  Dot3 Neon v2: 14.22 ms  +120.0%   - 786.3MOp/s, 3.93GFLOPS,  37.7GB/s
	  Dot3 Neon v1: 15.25 ms  +135.9%   - 733.5MOp/s, 3.67GFLOPS,  35.2GB/s

	Vertical SIMD, Cross test:
	  vert Cross v2: 9.49 ms            - 1.18GOp/s,  10.6GFLOPS,  42.4GB/s  <<
	  vert Cross v3: 9.50 ms  +0.2%     - 1.18GOp/s,  10.6GFLOPS,  42.4GB/s
	  vert Cross v1: 9.54 ms  +0.5%     - 1.17GOp/s,  10.6GFLOPS,  42.2GB/s
	  Cross Neon v1: 13.64 ms  +43.8%   - 820.1MOp/s,  7.38GFLOPS, 39.4GB/s


Step=2
	Vertical SIMD, Dot test:
	  vert Dot v5 : 7.29 ms             - 1.53GOp/s,  7.67GFLOPS,  42.9GB/s
	  vert Dot v4 : 7.36 ms  +0.9%      - 1.52GOp/s,  7.60GFLOPS,  42.6GB/s
	  vert Dot v3 : 7.46 ms  +2.3%      - 1.50GOp/s,  7.50GFLOPS,  42.0GB/s
	  vert Dot v2 : 8.25 ms  +13.1%     - 1.36GOp/s,  6.78GFLOPS,  38.0GB/s
	  vert Dot v1 : 8.30 ms  +13.8%     - 1.35GOp/s,  6.74GFLOPS,  37.7GB/s
	  Dot3 Neon v2: 15.70 ms  +115.3%   - 712.2MOp/s, 3.56GFLOPS,  34.2GB/s
	  Dot3 Neon v1: 17.45 ms  +139.3%   - 640.8MOp/s, 3.20GFLOPS,  30.8GB/s

	Vertical SIMD, Cross test:
	  vert Cross v1: 10.19 ms           - 1.10GOp/s,  9.88GFLOPS,  39.5GB/s
	  vert Cross v3: 10.25 ms  +0.7%    - 1.09GOp/s,  9.82GFLOPS,  39.3GB/s
	  vert Cross v2: 10.28 ms  +0.9%    - 1.09GOp/s,  9.80GFLOPS,  39.2GB/s
	  Cross Neon v1: 14.70 ms  +44.3%   - 760.8MOp/s, 6.85GFLOPS,  36.5GB/s


Step=1
	Vertical SIMD, Dot test:
	  vert Dot v4 : 8.87 ms             - 1.26GOp/s,  6.30GFLOPS,  35.3GB/s
	  vert Dot v5 : 8.88 ms  +0.2%      - 1.26GOp/s,  6.29GFLOPS,  35.2GB/s
	  vert Dot v3 : 9.36 ms  +5.5%      - 1.20GOp/s,  5.98GFLOPS,  33.5GB/s
	  vert Dot v2 : 9.85 ms  +11.0%     - 1.14GOp/s,  5.68GFLOPS,  31.8GB/s
	  vert Dot v1 : 9.90 ms  +11.6%     - 1.13GOp/s,  5.65GFLOPS,  31.6GB/s
	  Dot3 Neon v2: 17.95 ms  +102.4%   - 622.9MOp/s, 3.11GFLOPS,  29.9GB/s
	  Dot3 Neon v1: 18.79 ms  +111.8%   - 595.4MOp/s, 2.98GFLOPS,  28.6GB/s

	Vertical SIMD, Cross test:
	  vert Cross v2: 11.74 ms           - 952.8MOp/s,  8.57GFLOPS,  34.3GB/s
	  vert Cross v3: 11.79 ms  +0.4%    - 948.8MOp/s,  8.54GFLOPS,  34.2GB/s
	  vert Cross v1: 11.90 ms  +1.4%    - 939.8MOp/s,  8.46GFLOPS,  33.8GB/s
	  Cross Neon v1: 16.65 ms  +41.9%   - 671.6MOp/s,  6.04GFLOPS,  32.2GB/s
```

# Apple M1, E-core, 2.06 GHz, 5nm (Mac Mini M1 16GB, Icestorm)


```
Step=4
	Vertical SIMD, Dot test:
	  vert Dot v4 : 19.65 ms             - 569.1MOp/s,  2.85GFLOPS,  15.9GB/s
	  vert Dot v3 : 21.58 ms  +9.8%      - 518.3MOp/s,  2.59GFLOPS,  14.5GB/s
	  vert Dot v5 : 21.77 ms  +10.8%     - 513.7MOp/s,  2.57GFLOPS,  14.4GB/s
	  vert Dot v1 : 22.07 ms  +12.3%     - 506.7MOp/s,  2.53GFLOPS,  14.2GB/s
	  vert Dot v2 : 25.41 ms  +29.3%     - 440.3MOp/s,  2.20GFLOPS,  12.3GB/s
	  Dot3 Neon v2: 48.75 ms  +148.1%    - 229.4MOp/s,  1.15GFLOPS,  11.0GB/s
	  Dot3 Neon v1: 63.05 ms  +220.8%    - 177.4MOp/s,  887.0MFLOPS, 8.51GB/s

	Vertical SIMD, Cross test:
	  vert Cross v2: 30.95 ms            - 361.4MOp/s,  3.25GFLOPS,  13.0GB/s
	  vert Cross v3: 35.15 ms  +13.6%    - 318.2MOp/s,  2.86GFLOPS,  11.5GB/s
	  vert Cross v1: 36.65 ms  +18.4%    - 305.1MOp/s,  2.75GFLOPS,  11.0GB/s
	  Cross Neon v1: 88.55 ms  +186.1%   - 126.3MOp/s,  1.14GFLOPS,  6.06GB/s


Step=2
	Vertical SIMD, Dot test:
	  vert Dot v5 : 21.05 ms             - 531.4MOp/s,  2.66GFLOPS,  14.9GB/s
	  vert Dot v4 : 21.83 ms  +3.7%      - 512.4MOp/s,  2.56GFLOPS,  14.3GB/s
	  vert Dot v1 : 23.04 ms  +9.5%      - 485.5MOp/s,  2.43GFLOPS,  13.6GB/s
	  vert Dot v2 : 24.40 ms  +15.9%     - 458.4MOp/s,  2.29GFLOPS,  12.8GB/s
	  vert Dot v3 : 25.03 ms  +18.9%     - 446.8MOp/s,  2.23GFLOPS,  12.5GB/s
	  Dot3 Neon v2: 44.07 ms  +109.4%    - 253.8MOp/s,  1.27GFLOPS,  12.2GB/s
	  Dot3 Neon v1: 51.28 ms  +143.6%    - 218.1MOp/s,  1.09GFLOPS,  10.5GB/s

	Vertical SIMD, Cross test:
	  vert Cross v2: 27.94 ms            - 400.3MOp/s,  3.60GFLOPS,  14.4GB/s
	  vert Cross v3: 28.44 ms  +1.8%     - 393.3MOp/s,  3.54GFLOPS,  14.2GB/s
	  vert Cross v1: 29.86 ms  +6.9%     - 374.5MOp/s,  3.37GFLOPS,  13.5GB/s
	  Cross Neon v1: 82.19 ms  +194.2%   - 136.1MOp/s,  1.22GFLOPS,  6.53GB/s


Step=1
	Vertical SIMD, Dot test:
	  vert Dot v3 : 18.47 ms             - 605.7MOp/s,  3.03GFLOPS,  17.0GB/s  <<
	  vert Dot v4 : 19.13 ms  +3.6%      - 584.5MOp/s,  2.92GFLOPS,  16.4GB/s
	  vert Dot v5 : 19.75 ms  +7.0%      - 566.3MOp/s,  2.83GFLOPS,  15.9GB/s
	  vert Dot v1 : 20.11 ms  +8.9%      - 556.1MOp/s,  2.78GFLOPS,  15.6GB/s
	  vert Dot v2 : 20.58 ms  +11.4%     - 543.6MOp/s,  2.72GFLOPS,  15.2GB/s
	  Dot3 Neon v2: 35.22 ms  +90.7%     - 317.5MOp/s,  1.59GFLOPS,  15.2GB/s
	  Dot3 Neon v1: 45.65 ms  +147.2%    - 245.0MOp/s,  1.22GFLOPS,  11.8GB/s

	Vertical SIMD, Cross test:
	  vert Cross v2: 24.64 ms            - 453.9MOp/s,  4.09GFLOPS,  16.3GB/s <<
	  vert Cross v1: 25.01 ms  +1.5%     - 447.2MOp/s,  4.02GFLOPS,  16.1GB/s
	  vert Cross v3: 25.05 ms  +1.7%     - 446.5MOp/s,  4.02GFLOPS,  16.1GB/s
	  Cross Neon v1: 72.37 ms  +193.7%   - 154.6MOp/s,  1.39GFLOPS,  7.42GB/s
```

