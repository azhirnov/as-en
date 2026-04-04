echo "update 'ffmpeg'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-x64/ffmpeg"
rm -rf "temp"
mkdir "build"
mkdir "temp"
wget -O temp.zip "https://github.com/BtbN/FFmpeg-Builds/releases/download/autobuild-2026-03-20-13-06/ffmpeg-n8.0.1-76-gfa4ee7ab3c-linux64-lgpl-shared-8.0.tar.xz"
unzip temp.zip -d "temp"
rm temp.zip
cd "temp/ffmpeg-n8.0.1-76-gfa4ee7ab3c-linux64-lgpl-shared-8.0/lib"
find . -type d -exec mkdir -p "../../../../../../../../AE-Bin/external/linux-x64/ffmpeg/lib/"{} \;
find . -type f -name '*.so' -exec cp {} "../../../../../../../../AE-Bin/external/linux-x64/ffmpeg/lib/"{} \;
cd "../../.."
cd "temp/ffmpeg-n8.0.1-76-gfa4ee7ab3c-linux64-lgpl-shared-8.0/include"
find . -type d -exec mkdir -p "../../../../../../../../AE-Bin/external/linux-x64/ffmpeg/include/"{} \;
find . -type f -name '*.h' -exec cp {} "../../../../../../../../AE-Bin/external/linux-x64/ffmpeg/include/"{} \;
cd "../../.."
cp "temp/ffmpeg-n8.0.1-76-gfa4ee7ab3c-linux64-lgpl-shared-8.0/LICENSE.txt" "../../../../../AE-Bin/external/linux-x64/ffmpeg"
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
