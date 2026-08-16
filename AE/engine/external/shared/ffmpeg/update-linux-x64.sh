echo "update 'ffmpeg'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-x64/ffmpeg"
rm -rf "temp"
mkdir "build"
mkdir "temp"
wget -O temp.zip "https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-n8.1-latest-linux64-lgpl-shared-8.1.tar.xz"
unzip temp.zip -d "temp"
rm temp.zip
cd "temp/ffmpeg-n8.1-latest-linux64-lgpl-shared-8.1/lib"
find . -type d -exec mkdir -p "../../../../../../../../AE-Bin/external/linux-x64/ffmpeg/lib/"{} \;
find . -type f -name '*.so' -exec cp {} "../../../../../../../../AE-Bin/external/linux-x64/ffmpeg/lib/"{} \;
cd "../../.."
cd "temp/ffmpeg-n8.1-latest-linux64-lgpl-shared-8.1/include"
find . -type d -exec mkdir -p "../../../../../../../../AE-Bin/external/linux-x64/ffmpeg/include/"{} \;
find . -type f -name '*.h' -exec cp {} "../../../../../../../../AE-Bin/external/linux-x64/ffmpeg/include/"{} \;
cd "../../.."
cp "temp/ffmpeg-n8.1-latest-linux64-lgpl-shared-8.1/LICENSE.txt" "../../../../../AE-Bin/external/linux-x64/ffmpeg"
rm -rf "temp"
rm -rf "build"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
