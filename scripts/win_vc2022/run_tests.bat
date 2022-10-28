cd ../..

mkdir _build_22
cd _build_22
cmake -G "Visual Studio 17 2022" -A x64 ".."

cmake --build . --config Debug
ctest -C Debug --verbose
