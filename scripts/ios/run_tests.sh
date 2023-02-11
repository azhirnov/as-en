cd ../..
rm -rf _build_ios

mkdir _build_ios
cd _build_ios
cmake -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 -DCMAKE_OSX_ARCHITECTURES=arm64 ".."

cmake --build . --config Debug
ctest -C Debug --verbose
