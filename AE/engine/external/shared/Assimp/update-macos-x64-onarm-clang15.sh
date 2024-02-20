rm -rf "build"
rm -rf "../../../../../AE-Data/external/macos-x64-clang15/Assimp"
rm -rf "temp"
mkdir "build"
export CC=/usr/local/opt/llvm/bin/clang
export CXX=/usr/local/opt/llvm/bin/clang++
git clone --branch "v5.2.5" "../../../../../3party/Assimp" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "assimp_CMakeLists.txt" "temp/CMakeLists.txt"
arch -x86_64 /usr/local/bin/cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/macos-x64-clang15/Assimp" 
arch -x86_64 /usr/local/bin/cmake --build build --config Debug --target install -j 6
arch -x86_64 /usr/local/bin/cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/macos-x64-clang15/Assimp" 
arch -x86_64 /usr/local/bin/cmake --build build --config Release --target install -j 6
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
