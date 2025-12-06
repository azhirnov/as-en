:: PCH is disabled, it is better for developing Base module

cd ../../..
rmdir /Q /S "_build_cxx20"

mkdir _build_cxx20
cd _build_cxx20
cmake -G "Visual Studio 17 2022" -A x64 -DAE_SIMD_AVX=2 -DAE_ENABLE_VULKAN=ON -DAE_EXCLUDE_PACK_RES=ON -DAE_FORCE_CXX20=ON "../AE"

pause

START AE.sln
