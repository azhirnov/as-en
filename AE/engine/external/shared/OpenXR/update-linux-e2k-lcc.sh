echo "update 'OpenXR'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-e2k-lcc9/OpenXR"
rm -rf "temp"
mkdir "build"
export PATH=$PATH:/opt/mcst/lcc-1.27.21.e2k-v6.5.10-e2c3/bin/
export CC=lcc
export CXX=l++
mkdir "temp"
git clone --branch "release-1.1.50" "../../../../../3party/OpenXR-SDK-Source" "temp/OpenXR-SDK-Source"
cp "openxr_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-e2k-lcc9/OpenXR" 
cmake --build build --config Release --target install -j 6
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
