cd ../..

rmdir /Q /S "_tmp_build"

mkdir _tmp_build
cd _tmp_build
cmake -G "Visual Studio 16 2019" -A x64 -DAE_SIMD_AVX=2 ".."

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
