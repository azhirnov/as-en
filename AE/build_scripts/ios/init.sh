cd ../../..

mkdir _build_ios
cd _build_ios
cmake -G "Xcode" -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 -DCMAKE_OSX_ARCHITECTURES=arm64 "../AE"
