rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-x64-clang16/zstd"
rm -rf "temp"
mkdir "build"
export CC=/usr/bin/clang-16
export CXX=/usr/bin/clang++-16
export CPP=/usr/bin/clang-cpp-16
export LD=/usr/bin/ld.lld-16
git clone --branch "v1.5.6" "../../../../../3party/zstd" "temp"
cp "zstd_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-clang16/zstd" 
cmake --build build --config Release --target install -j 4
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
