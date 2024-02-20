rm -rf "build"
rm -rf "../../../../../AE-Data/external/linux-x64-clang16/CDT"
rm -rf "temp"
mkdir "build"
export CC=/usr/bin/clang-16
export CXX=/usr/bin/clang++-16
export CPP=/usr/bin/clang-cpp-16
export LD=/usr/bin/ld.lld-16
git clone --branch "1.3.0" "../../../../../3party/CDT" "temp"
cp "temp/CDT/CMakeLists.txt" "temp/CDT/origin_CMakeLists.txt"
cp "CDT_CMakeLists.txt" "temp/CDT/CMakeLists.txt"
cmake -S temp/CDT -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/linux-x64-clang16/CDT" 
cmake --build build --config Debug --target install -j 4
cmake -S temp/CDT -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/linux-x64-clang16/CDT" 
cmake --build build --config Release --target install -j 4
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
