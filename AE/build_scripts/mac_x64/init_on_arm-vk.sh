# how to compile on ARM64
# https://stackoverflow.com/questions/67386941/using-x86-libraries-and-openmp-on-macos-arm64-architecture/67418208#67418208

export CC=/usr/local/opt/llvm/bin/clang
export CXX=/usr/local/opt/llvm/bin/clang++

cd ../../..
rm -rf _build_x64_vk

mkdir _build_x64_vk
cd _build_x64_vk
arch -x86_64 /usr/local/bin/cmake -G "Xcode" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_SSE=20 -DAE_ENABLE_VULKAN=ON "../AE"

read -p "open project?"
open ./AE.xcodeproj
