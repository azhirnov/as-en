echo "update 'imgui'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/macos-arm64-clang17/imgui"
rm -rf "temp"
mkdir "build"
git clone --branch "v1.92.6" "https://github.com/ocornut/imgui.git" "temp"
cp "imgui_CMakeLists.txt" "temp/CMakeLists.txt"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64-clang17/imgui"  || pauseOnError
cmake --build build --config Debug --target install -j 6 || pauseOnError
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64-clang17/imgui"  || pauseOnError
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
