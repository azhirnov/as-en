rm -rf "../../../../../AE-Bin/external/source/VMA"
rm -rf "temp"
git clone --branch "v3.3.0" "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git" "temp"
mkdir "../../../../../AE-Bin/external/source/VMA"
cp "temp/include/vk_mem_alloc.h" "../../../../../AE-Bin/external/source/VMA/vk_mem_alloc.h"
cp "temp/LICENSE.txt" "../../../../../AE-Bin/external/source/VMA/LICENSE.txt"
rm -rf "temp"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
