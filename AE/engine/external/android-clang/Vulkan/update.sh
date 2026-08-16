rm -rf "../../../../../AE-Bin/external/android-clang/Vulkan"
rm -rf "temp"
wget -O temp.zip "https://github.com/KhronosGroup/Vulkan-ValidationLayers/releases/download/vulkan-sdk-1.4.350.1/android-binaries-1.4.350.1.zip"
unzip temp.zip -d "temp"
rm temp.zip
cp -TR "temp/android-binaries-1.4.350.1" "../../../../../AE-Bin/external/android-clang/Vulkan"
rm -rf "temp"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
