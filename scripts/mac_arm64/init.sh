cd ../..

mkdir _build
cd _build
cmake -G "Xcode" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 ".."

#cd ..
#mkdir _build_res
#cd _build_res
#cmake -G "Xcode" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 "../engine/tools/res_pack"

cd ..
mkdir _build_external
cd _build_external
cmake -G "Xcode" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 "../engine/external"
