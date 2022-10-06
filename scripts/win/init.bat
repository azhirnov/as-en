cd ../..

mkdir _build
cd _build
cmake -G "Visual Studio 16 2019" -A x64 ".."

cd ..
mkdir _build_res
cd _build_res
cmake -G "Visual Studio 16 2019" -A x64 "../engine/tools/res_pack"

cd ..
mkdir _build_external
cd _build_external
cmake -G "Visual Studio 16 2019" -A x64 "../engine/external"

pause