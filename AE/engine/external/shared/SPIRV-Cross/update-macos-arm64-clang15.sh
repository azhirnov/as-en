rm -rf "build"
rm -rf "../../../../../AE-Bin/external/macos-arm64-clang15/SPIRV-Cross"
rm -rf "temp"
mkdir "build"
git clone --branch "vulkan-sdk-1.3.290.0" "../../../../../3party/SPIRV-Cross" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "spirvcross_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64-clang15/SPIRV-Cross" 
cmake --build build --config Debug --target install -j 6
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64-clang15/SPIRV-Cross" 
cmake --build build --config Release --target install -j 6
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
