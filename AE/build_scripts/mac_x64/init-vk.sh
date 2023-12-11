# warning: on ARM CPU use 'init_on_arm.sh'

cd ../../..
rm -rf _build_x64_vk

mkdir _build_x64_vk
cd _build_x64_vk
cmake -G "Xcode" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_SSE=20 -DAE_ENABLE_VULKAN=ON "../AE"

read -p "open project?"
open ./AE.xcodeproj
