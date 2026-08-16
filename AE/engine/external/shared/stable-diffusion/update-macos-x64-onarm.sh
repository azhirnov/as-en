echo "update 'stable-diffusion'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/macos-x64/stable-diffusion"
rm -rf "temp"
mkdir "build"
git clone --recurse-submodules --branch "master-487-43e829f" "https://github.com/leejet/stable-diffusion.cpp.git" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "stable-diffusion_CMakeLists.txt" "temp/CMakeLists.txt"
mkdir "../../../../../AE-Bin/external/macos-x64/stable-diffusion"
mkdir "../../../../../AE-Bin/external/macos-x64/stable-diffusion/include"
mkdir "../../../../../AE-Bin/external/macos-x64/stable-diffusion/lib"
cp "temp/LICENSE" "../../../../../AE-Bin/external/macos-x64/stable-diffusion/stable-diffusion-LICENSE.txt"
cp "temp/stable-diffusion.h" "../../../../../AE-Bin/external/macos-x64/stable-diffusion/include/stable-diffusion.h"
rm -rf "temp"
rm -rf "build"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
