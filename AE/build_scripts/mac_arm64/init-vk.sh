cd ../../..
rm -rf _build_arm64_vk

mkdir _build_arm64_vk
cd _build_arm64_vk
cmake -G "Xcode" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DAE_ENABLE_VULKAN=ON "../AE"

read -p "open project?"
open ./AE.xcodeproj
