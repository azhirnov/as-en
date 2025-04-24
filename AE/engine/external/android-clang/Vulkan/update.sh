rm -rf "../../../../../AE-Bin/external/android-clang/Vulkan"
rm -rf "temp"
wget -O temp.zip "https://github.com/KhronosGroup/Vulkan-ValidationLayers/releases/download/vulkan-sdk-1.4.309.0/android-binaries-1.4.309.0.zip"
unzip temp.zip -d "temp"
rm temp.zip
cp -R "temp/android-binaries-1.4.309.0" "../../../../../AE-Bin/external/android-clang/Vulkan"
rm -rf "temp"
read -p "press any key..."
