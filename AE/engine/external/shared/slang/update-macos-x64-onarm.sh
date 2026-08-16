echo "update 'slang'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/macos-x64/slang"
rm -rf "temp"
mkdir "build"
git clone --recurse-submodules --branch "vulkan-sdk-1.4.350.1" "https://github.com/shader-slang/slang.git" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "slang_CMakeLists.txt" "temp/CMakeLists.txt"
arch -x86_64 /usr/local/bin/cmake -S temp -B build -G "Xcode" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-x64/slang"  || pauseOnError
arch -x86_64 /usr/local/bin/cmake --build build --config Debug --target "slang" -j 6 || pauseOnError
arch -x86_64 /usr/local/bin/cmake -S temp -B build -G "Xcode" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-x64/slang"  || pauseOnError
arch -x86_64 /usr/local/bin/cmake --build build --config Release --target "slang" -j 6 || pauseOnError
mkdir "../../../../../AE-Bin/external/macos-x64/slang"
mkdir "../../../../../AE-Bin/external/macos-x64/slang/include"
mkdir "../../../../../AE-Bin/external/macos-x64/slang/lib"
cp "temp/LICENSE" "../../../../../AE-Bin/external/macos-x64/slang/slang-LICENSE.txt"
cp "temp/include/slang.h" "../../../../../AE-Bin/external/macos-x64/slang/include/slang.h"
cp "temp/include/slang-deprecated.h" "../../../../../AE-Bin/external/macos-x64/slang/include/slang-deprecated.h"
cp "temp/include/slang-image-format-defs.h" "../../../../../AE-Bin/external/macos-x64/slang/include/slang-image-format-defs.h"
cp "build/Debug/lib/libslang-compiler.dylib" "../../../../../AE-Bin/external/macos-x64/slang/lib/slang-compilerd.dylib"
cp "build/Release/lib/libslang-compiler.dylib" "../../../../../AE-Bin/external/macos-x64/slang/lib/slang-compiler.dylib"
rm -rf "temp"
rm -rf "build"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
