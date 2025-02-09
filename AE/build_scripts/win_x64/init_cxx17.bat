cd ../../..
rmdir /Q /S "_build_cxx17"

mkdir _build_cxx17
cd _build_cxx17
cmake -G "Visual Studio 17 2022" -A x64 -DAE_SIMD_AVX=2 -DAE_ENABLE_VULKAN=ON -DAE_FORCE_CXX17=ON "../AE"

pause

START AE.sln
