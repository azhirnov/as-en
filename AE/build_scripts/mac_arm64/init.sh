cd ../../..
rm -rf _build_arm64

mkdir _build_arm64
cd _build_arm64
cmake -G "Xcode" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 "../AE"
