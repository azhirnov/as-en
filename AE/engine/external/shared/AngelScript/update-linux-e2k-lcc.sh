echo "update 'AngelScript'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-e2k-lcc9/AngelScript"
rm -rf "temp"
mkdir "build"
export PATH=$PATH:/opt/mcst/lcc-1.27.21.e2k-v6.5.10-e2c3/bin/
export CC=lcc
export CXX=l++
git clone "../../../../../3party/AngelScript" "temp"
cp "patch/angelscript_CMakeLists.txt" "temp/CMakeLists.txt"
cp "patch/scriptarray.h" "temp/add_on/scriptarray/scriptarray.h"
rm -rf "temp/add_on/scriptstdstring"
cp -R "patch/scriptstdstring" "temp/add_on/scriptstdstring"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-e2k-lcc9/AngelScript" 
cmake --build build --config Debug --target install -j 6
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-e2k-lcc9/AngelScript" 
cmake --build build --config Release --target install -j 6
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
