cd ../..

mkdir _build_22
cd _build_22
cmake -G "Visual Studio 17 2022" -A x64 ".."

cd ..
mkdir _build_res_22
cd _build_res_22
cmake -G "Visual Studio 17 2022" -A x64 "../engine/tools/res_pack"

cd ..
mkdir _build_external_22
cd _build_external_22
cmake -G "Visual Studio 17 2022" -A x64 "../engine/external"

pause