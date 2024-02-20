rm -rf "build"
rm -rf "../../../../../AE-Data/external/linux-x64-clang16/AngelScript"
rm -rf "temp"
mkdir "build"
export CC=/usr/bin/clang-16
export CXX=/usr/bin/clang++-16
export CPP=/usr/bin/clang-cpp-16
export LD=/usr/bin/ld.lld-16
git clone "../../../../../3party/AngelScript" "temp"
cp "patch/angelscript_CMakeLists.txt" "temp/CMakeLists.txt"
cp "patch/scriptarray.h" "temp/add_on/scriptarray/scriptarray.h"
rm -rf "temp/add_on/scriptstdstring"
cp -R "patch/scriptstdstring" "temp/add_on/scriptstdstring"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/linux-x64-clang16/AngelScript" 
cmake --build build --config Debug --target install -j 4
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/linux-x64-clang16/AngelScript" 
cmake --build build --config Release --target install -j 4
rm -rf "temp"
rm -rf "build"
read -p "press any key..."