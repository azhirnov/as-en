rm -rf "build"
rm -rf "../../../../../AE-Data/external/macos-arm64-clang15/GLFW"
rm -rf "temp"
mkdir "build"
git clone --branch "3.3" "../../../../../3party/glfw" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "glfw_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/macos-arm64-clang15/GLFW" 
cmake --build build --config Release --target install -j 6
rm -rf "temp"
rm -rf "build"
read -p "press any key..."