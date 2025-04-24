rm -rf "../../../../../AE-Bin/external/source/Vulkan"
rm -rf "temp"
git clone --branch "vulkan-sdk-1.4.309.0" "../../../../../3party/Vulkan-Headers" "temp"
mkdir "../../../../../AE-Bin/external/source/Vulkan"
cd "temp/include"
find . -type d -exec mkdir -p "../../../../../../../AE-Bin/external/source/Vulkan/"{} \;
find . -type f -name '*.h' -exec cp {} "../../../../../../../AE-Bin/external/source/Vulkan/"{} \;
cd "../.."
cp "temp/LICENSE.md" "../../../../../AE-Bin/external/source/Vulkan/LICENSE.md"
rm -rf "temp"
read -p "press any key..."
