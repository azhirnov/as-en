cd ../..

mkdir _build
cd _build
cmake -G "Visual Studio 16 2019" -A x64 ".."

cmake --build . --config Debug
ctest -C Debug --verbose
