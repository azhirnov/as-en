cd ../../..
rmdir /Q /S "_build_clang_rem"

mkdir _build_clang_rem
cd _build_clang_rem
cmake -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_SIMD_AVX=2 -DAE_ENABLE_REMOTE_GRAPHICS=ON -DAE_ENABLE_OPENVR=OFF -DAE_USE_PCH=OFF "../AE"

pause

START AE.sln
