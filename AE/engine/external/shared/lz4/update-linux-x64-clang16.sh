rm -rf "build"
rm -rf "../../../../../AE-Data/external/linux-x64-clang16/lz4"
rm -rf "temp"
mkdir "build"
export CC=/usr/bin/clang-16
export CXX=/usr/bin/clang++-16
export CPP=/usr/bin/clang-cpp-16
export LD=/usr/bin/ld.lld-16
git clone --branch "v1.9.4" "../../../../../3party/lz4" "temp"
cp "lz4_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/linux-x64-clang16/lz4" 
cmake --build build --config Release --target install -j 4
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
