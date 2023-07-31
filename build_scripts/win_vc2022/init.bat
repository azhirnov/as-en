cd ../..
rmdir /Q /S "_build"

mkdir _build
cd _build
cmake -G "Visual Studio 17 2022" -A x64 -DAE_SIMD_AVX=2 -DAE_ENABLE_VULKAN=ON -DENGINE_LIBS_PATH="" ..

pause

START AsEn.sln
