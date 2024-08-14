rm -rf "build"
rm -rf "../../../../../AE-Bin/external/macos-arm64-clang15/CDT"
rm -rf "temp"
mkdir "build"
git clone --branch "1.4.1" "../../../../../3party/CDT" "temp"
cp "temp/CDT/CMakeLists.txt" "temp/CDT/origin_CMakeLists.txt"
cp "CDT_CMakeLists.txt" "temp/CDT/CMakeLists.txt"
cmake -S temp/CDT -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64-clang15/CDT" 
cmake --build build --config Debug --target install -j 6
cmake -S temp/CDT -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64-clang15/CDT" 
cmake --build build --config Release --target install -j 6
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
