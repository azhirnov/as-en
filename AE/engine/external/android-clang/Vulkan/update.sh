rm -rf "../../../../../AE-Bin/external/android-clang/Vulkan"
rm -rf "temp"
wget -O temp.zip "https://github.com/KhronosGroup/Vulkan-ValidationLayers/releases/download/vulkan-sdk-1.4.341.0/android-binaries-1.4.341.0.zip"
unzip temp.zip -d "temp"
rm temp.zip
cp -TR "temp/android-binaries-1.4.341.0" "../../../../../AE-Bin/external/android-clang/Vulkan"
rm -rf "temp"
read -p "press any key..."
