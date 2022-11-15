cd ../..

mkdir _build_22
cd _build_22
cmake -G "Visual Studio 17 2022" -A x64 ".." -DAE_SIMD_AVX=2

pause