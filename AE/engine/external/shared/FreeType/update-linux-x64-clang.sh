echo "update 'FreeType'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-x64-clang20/FreeType"
rm -rf "temp"
mkdir "build"
export CC=/usr/bin/clang-20
export CXX=/usr/bin/clang++-20
export CPP=/usr/bin/clang-cpp-20
export LD=/usr/bin/ld.lld-20
git clone --branch "VER-2-14-1" "https://github.com/freetype/freetype.git" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "freetype_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-clang20/FreeType" 
cmake --build build --config Release --target install -j 8
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
