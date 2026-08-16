rm -rf "../../../../../AE-Bin/external/source/xxHash"
rm -rf "temp"
git clone --branch "v0.8.3" "https://github.com/Cyan4973/xxHash.git" "temp"
mkdir "../../../../../AE-Bin/external/source/xxHash"
cp "temp/xxhash.h" "../../../../../AE-Bin/external/source/xxHash/xxhash.h"
cp "temp/LICENSE" "../../../../../AE-Bin/external/source/xxHash/LICENSE.txt"
rm -rf "temp"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
