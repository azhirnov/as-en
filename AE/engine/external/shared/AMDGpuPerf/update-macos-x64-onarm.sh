echo "update 'AMDGpuPerf'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/macos-x64/AMDGpuPerf"
rm -rf "temp"
mkdir "build"
rm -rf "temp"
rm -rf "build"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
