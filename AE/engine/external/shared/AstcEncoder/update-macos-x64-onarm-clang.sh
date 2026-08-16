echo "update 'AstcEncoder'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/macos-x64-clang17/AstcEncoder"
rm -rf "temp"
mkdir "build"
export CC=/usr/local/opt/llvm/bin/clang
export CXX=/usr/local/opt/llvm/bin/clang++
git clone --branch "5.3.0" "https://github.com/ARM-software/astc-encoder.git" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "AstcEncoder_CMakeLists.txt" "temp/CMakeLists.txt"
arch -x86_64 /usr/local/bin/cmake -S temp -B build -G "Xcode" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-x64-clang17/AstcEncoder"  || pauseOnError
arch -x86_64 /usr/local/bin/cmake --build build --config Debug --target install -j 6 || pauseOnError
arch -x86_64 /usr/local/bin/cmake -S temp -B build -G "Xcode" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-x64-clang17/AstcEncoder"  || pauseOnError
arch -x86_64 /usr/local/bin/cmake --build build --config Release --target install -j 6 || pauseOnError
rm -rf "temp"
rm -rf "build"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
