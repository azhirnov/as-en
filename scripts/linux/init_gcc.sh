cd ../..

export CC=/usr/bin/gcc
export CXX=/usr/bin/gcc++

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
