rm -rf "../../../../../AE-Bin/external/source/spirv"
rm -rf "temp"
git clone --branch "vulkan-sdk-1.4.350.1" "https://github.com/KhronosGroup/SPIRV-Headers.git" "temp"
mkdir "../../../../../AE-Bin/external/source/spirv"
cd "temp/include/spirv/unified1"
find . -type d -exec mkdir -p "../../../../../../../../../AE-Bin/external/source/spirv/"{} \;
find . -type f -name '*.h' -exec cp {} "../../../../../../../../../AE-Bin/external/source/spirv/"{} \;
cd "../../../.."
rm -rf "temp"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
