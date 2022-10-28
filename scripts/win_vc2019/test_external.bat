cd ../..
mkdir _build_external
cd _build_external
cmake -G "Visual Studio 16 2019" -A x64 "../engine/external"
cmake --build . --config Debug
ctest -C Debug --verbose
pause