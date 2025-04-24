echo "update 'CDT'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-e2k-lcc9/CDT"
rm -rf "temp"
mkdir "build"
export PATH=$PATH:/opt/mcst/lcc-1.27.21.e2k-v6.5.10-e2c3/bin/
export CC=lcc
export CXX=l++
git clone --branch "1.4.1" "../../../../../3party/CDT" "temp"
cp "temp/CDT/CMakeLists.txt" "temp/CDT/origin_CMakeLists.txt"
cp "CDT_CMakeLists.txt" "temp/CDT/CMakeLists.txt"
cmake -S temp/CDT -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-e2k-lcc9/CDT" 
cmake --build build --config Debug --target install -j 6
cmake -S temp/CDT -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-e2k-lcc9/CDT" 
cmake --build build --config Release --target install -j 6
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
