echo "update 'AngelScript'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-x64-gcc14/AngelScript"
rm -rf "temp"
mkdir "build"
export CC=/usr/bin/gcc-14
export CXX=/usr/bin/g++-14
git clone "../../../../../3party/AngelScript" "temp"
cp "patch/angelscript_CMakeLists.txt" "temp/CMakeLists.txt"
rm -rf "temp/add_on/scriptstdstring"
cp -TR "patch/scriptstdstring" "temp/add_on/scriptstdstring"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-gcc14/AngelScript" 
cmake --build build --config Debug --target install -j 8
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-gcc14/AngelScript" 
cmake --build build --config Release --target install -j 8
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
