echo "update 'glslang'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/macos-arm64-clang17/glslang"
rm -rf "temp"
mkdir "build"
git clone --branch "vulkan-sdk-1.4.350.1" "https://github.com/KhronosGroup/glslang.git" "temp"
git clone --branch "vulkan-sdk-1.4.350.1" "https://github.com/KhronosGroup/SPIRV-Tools.git" "temp/External/spirv-tools"
git clone --branch "vulkan-sdk-1.4.350.1" "https://github.com/KhronosGroup/SPIRV-Headers.git" "temp/External/spirv-tools/external/spirv-headers"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "glslang_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64-clang17/glslang"  || pauseOnError
cmake --build build --config Debug --target install -j 6 || pauseOnError
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64-clang17/glslang"  || pauseOnError
cmake --build build --config Release --target install -j 6 || pauseOnError
rm -rf "temp"
rm -rf "build"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
