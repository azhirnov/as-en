rm -rf "build"
rm -rf "../../../../../AE-Bin/external/macos-x64-clang15/CDT"
rm -rf "temp"
mkdir "build"
export CC=/usr/local/opt/llvm/bin/clang
export CXX=/usr/local/opt/llvm/bin/clang++
git clone --branch "1.3.0" "../../../../../3party/CDT" "temp"
cp "temp/CDT/CMakeLists.txt" "temp/CDT/origin_CMakeLists.txt"
cp "CDT_CMakeLists.txt" "temp/CDT/CMakeLists.txt"
arch -x86_64 /usr/local/bin/cmake -S temp/CDT -B build -G "Xcode" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-x64-clang15/CDT" 
arch -x86_64 /usr/local/bin/cmake --build build --config Debug --target install -j 6
arch -x86_64 /usr/local/bin/cmake -S temp/CDT -B build -G "Xcode" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-x64-clang15/CDT" 
arch -x86_64 /usr/local/bin/cmake --build build --config Release --target install -j 6
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
