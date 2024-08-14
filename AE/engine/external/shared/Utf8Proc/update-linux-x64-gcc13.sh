rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-x64-gcc13/Utf8Proc"
rm -rf "temp"
mkdir "build"
export CC=/usr/bin/gcc-13
export CXX=/usr/bin/g++-13
git clone --branch "v2.9.0" "../../../../../3party/utf8proc" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "utf8proc_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-gcc13/Utf8Proc" 
cmake --build build --config Release --target install -j 4
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
