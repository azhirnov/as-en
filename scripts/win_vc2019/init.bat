cd ../..

rmdir /Q /S "_build"

mkdir _build
cd _build
cmake -G "Visual Studio 16 2019" -A x64 ".." -DAE_SIMD_AVX=2

pause