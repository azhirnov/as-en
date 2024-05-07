rm -rf "build"
rm -rf "../../../../../AE-Bin/external/macos-x64-clang15/glslang"
rm -rf "temp"
mkdir "build"
export CC=/usr/local/opt/llvm/bin/clang
export CXX=/usr/local/opt/llvm/bin/clang++
git clone --branch "vulkan-sdk-1.3.280.0" "../../../../../3party/glslang" "temp"
git clone --branch "vulkan-sdk-1.3.280.0" "../../../../../3party/SPIRV-Tools" "temp/External/spirv-tools"
git clone --branch "vulkan-sdk-1.3.280.0" "../../../../../3party/SPIRV-Headers" "temp/External/spirv-tools/external/spirv-headers"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "glslang_CMakeLists.txt" "temp/CMakeLists.txt"
arch -x86_64 /usr/local/bin/cmake -S temp -B build -G "Xcode" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-x64-clang15/glslang" 
arch -x86_64 /usr/local/bin/cmake --build build --config Debug --target install -j 6
arch -x86_64 /usr/local/bin/cmake -S temp -B build -G "Xcode" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-x64-clang15/glslang" 
arch -x86_64 /usr/local/bin/cmake --build build --config Release --target install -j 6
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
