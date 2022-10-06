cd ../..

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

mkdir _build_external
cd _build_external
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug "../engine/external"
cmake --build . --config Debug
ctest -C Debug --verbose

read -p "press any key..."
