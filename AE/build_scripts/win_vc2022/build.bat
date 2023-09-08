cd ../../..
rmdir /Q /S "_build"

mkdir _build
cd _build
cmake -G "Visual Studio 17 2022" -A x64 -DAE_SIMD_AVX=2 -DAE_ENABLE_VULKAN=ON "../AE"

cmake --build . --config Debug
pause

cmake --build . --config Release
pause
