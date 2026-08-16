rm -rf "../../../../../AE-Bin/external/source/nvml"
rm -rf "temp"
mkdir "temp"
wget -O "temp/nvml.h" "https://raw.githubusercontent.com/jp7677/dxvk-nvapi/refs/heads/master/inc/nvml/nvml.h"
mkdir "../../../../../AE-Bin/external/source/nvml"
cp "temp/nvml.h" "../../../../../AE-Bin/external/source/nvml/nvml.h"
rm -rf "temp"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
