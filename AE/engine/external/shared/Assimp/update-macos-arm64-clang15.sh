rm -rf "build"
rm -rf "../../../../../AE-Data/external/macos-arm64-clang15/Assimp"
rm -rf "temp"
mkdir "build"
git clone --branch "v5.2.5" "../../../../../3party/Assimp" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "assimp_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/macos-arm64-clang15/Assimp" 
cmake --build build --config Debug --target install -j 6
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/macos-arm64-clang15/Assimp" 
cmake --build build --config Release --target install -j 6
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
