cd ../..

rm -rf _build

mkdir _build
cd _build
cmake -G "Xcode" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 ".."
