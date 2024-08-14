rm -rf "build"
rm -rf "../../../../../AE-Bin/external/macos-arm64-clang15/lz4"
rm -rf "temp"
mkdir "build"
git clone --branch "v1.10.0" "../../../../../3party/lz4" "temp"
cp "lz4_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64-clang15/lz4" 
cmake --build build --config Release --target install -j 6
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
