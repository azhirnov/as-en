cd ../../..
rmdir /Q /S "_build_clang"

mkdir _build_clang
cd _build_clang
cmake -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_SIMD_AVX=2 -DAE_ENABLE_VULKAN=ON -DAE_USE_PCH=OFF "../AE"

pause

START AE.sln
