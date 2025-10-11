echo "update 'glslang'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-x64-gcc14/glslang"
rm -rf "temp"
mkdir "build"
export CC=/usr/bin/gcc-14
export CXX=/usr/bin/g++-14
git clone --branch "vulkan-sdk-1.4.321.0" "../../../../../3party/glslang" "temp"
git clone --branch "vulkan-sdk-1.4.321.0" "../../../../../3party/SPIRV-Tools" "temp/External/spirv-tools"
git clone --branch "vulkan-sdk-1.4.321.0" "../../../../../3party/SPIRV-Headers" "temp/External/spirv-tools/external/spirv-headers"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "glslang_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-gcc14/glslang" 
cmake --build build --config Debug --target install -j 8
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-gcc14/glslang" 
cmake --build build --config Release --target install -j 8
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
