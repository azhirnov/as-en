echo "update 'CDT'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-x64-clang20/CDT"
rm -rf "temp"
mkdir "build"
export CC=/usr/bin/clang-20
export CXX=/usr/bin/clang++-20
export CPP=/usr/bin/clang-cpp-20
export LD=/usr/bin/ld.lld-20
git clone --branch "1.4.4" "https://github.com/artem-ogre/CDT.git" "temp"
cp "temp/CDT/CMakeLists.txt" "temp/CDT/origin_CMakeLists.txt"
cp "CDT_CMakeLists.txt" "temp/CDT/CMakeLists.txt"
cmake -S temp/CDT -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-clang20/CDT" 
cmake --build build --config Debug --target install -j 8
cmake -S temp/CDT -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-clang20/CDT" 
cmake --build build --config Release --target install -j 8
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
