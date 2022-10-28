cd ../..
mkdir _build_external_22
cd _build_external_22

cmake -G "Visual Studio 17 2022" -A x64 "../engine/external"
cmake --build . --config Debug
ctest -C Debug --verbose
pause