rm -rf "build"
rm -rf "../../../../../AE-Data/external/macos-arm64-clang15/FreeType"
rm -rf "temp"
mkdir "build"
git clone "../../../../../3party/FreeType" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "freetype_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/macos-arm64-clang15/FreeType" 
cmake --build build --config Release --target install -j 6
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
