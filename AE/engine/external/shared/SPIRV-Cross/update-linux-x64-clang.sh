echo "update 'SPIRV-Cross'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-x64-clang20/SPIRV-Cross"
rm -rf "temp"
mkdir "build"
export CC=/usr/bin/clang-20
export CXX=/usr/bin/clang++-20
export CPP=/usr/bin/clang-cpp-20
export LD=/usr/bin/ld.lld-20
git clone --branch "vulkan-sdk-1.4.341.0" "https://github.com/KhronosGroup/SPIRV-Cross.git" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "spirvcross_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-clang20/SPIRV-Cross" 
cmake --build build --config Debug --target install -j 8
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-clang20/SPIRV-Cross" 
cmake --build build --config Release --target install -j 8
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
