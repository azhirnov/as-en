cd ../..

export CC=/usr/bin/clang
export CXX=/usr/bin/clang++

mkdir _build
cd _build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ".."

cd ..
mkdir _build_res
cd _build_res
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug "../engine/tools/res_pack"

cd ..
mkdir _build_external
cd _build_external
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug "../engine/external"
