echo "update 'GLFW'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-x64-gcc14/GLFW"
rm -rf "temp"
mkdir "build"
export CC=/usr/bin/gcc-14
export CXX=/usr/bin/g++-14
git clone --branch "3.4" "https://github.com/glfw/glfw.git" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "glfw_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-gcc14/GLFW"  || pauseOnError
cmake --build build --config Release --target install -j 8 || pauseOnError
rm -rf "temp"
rm -rf "build"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
