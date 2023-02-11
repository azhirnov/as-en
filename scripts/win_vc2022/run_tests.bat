cd ../..

rmdir /Q /S "_tmp_build"

mkdir _build_22
cd _build_22
cmake -G "Visual Studio 17 2022" -A x64 -DAE_SIMD_AVX=2 ".."

cmake --build . --config Debug
ctest -C Debug --verbose
pause

cmake --build . --config Develop
ctest -C Develop --verbose
pause

cmake --build . --config Profile
ctest -C Profile --verbose
pause

cmake --build . --config Release
ctest -C Release --verbose
pause

rmdir /Q /S "_tmp_build"
