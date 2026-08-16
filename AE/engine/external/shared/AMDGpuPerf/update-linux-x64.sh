echo "update 'AMDGpuPerf'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-x64/AMDGpuPerf"
rm -rf "temp"
mkdir "build"
wget -O temp.zip "https://github.com/GPUOpen-Tools/gpu_performance_api/releases/download/v4.0-tag/GPUPerfAPI-Linux-4.0.0.39.tgz"
unzip temp.zip -d "temp/linux-x64"
rm temp.zip
mkdir "../../../../../AE-Bin/external/linux-x64/AMDGpuPerf"
mkdir "../../../../../AE-Bin/external/linux-x64/AMDGpuPerf/bin"
cp "temp/4_0/bin/libGPUPerfAPICounters.so" "../../../../../AE-Bin/external/linux-x64/AMDGpuPerf/bin"
cp "temp/4_0/bin/libGPUPerfAPIVK.so" "../../../../../AE-Bin/external/linux-x64/AMDGpuPerf/bin"
cp "temp/4_0/LICENSE.txt" "../../../../../AE-Bin/external/linux-x64/AMDGpuPerf"
cp "temp/4_0/NOTICES.txt" "../../../../../AE-Bin/external/linux-x64/AMDGpuPerf"
cp -TR "temp/4_0/include" "../../../../../AE-Bin/external/linux-x64/AMDGpuPerf/include"
rm -rf "temp"
rm -rf "build"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
