cd ../../..
rm -rf _build_rem_arm64

mkdir _build_rem_arm64
cd _build_rem_arm64
cmake -G "Xcode" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DAE_ENABLE_REMOTE_GRAPHICS=ON -DAE_EXCLUDE_PACK_RES=ON "../AE"

read -p "open project?"
open ./AE.xcodeproj
