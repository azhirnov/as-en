cd ../..

mkdir _build_x64
cd _build_x64
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_AVX=2 ".."

cmake --build . --config Debug
ctest -C Debug --verbose
