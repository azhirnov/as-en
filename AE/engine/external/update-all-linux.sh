find . -name "*.sh" -exec chmod +x {}\;
cd "shared/VMA" && "./update.sh"  && cd "../.." && cd "shared/Vulkan" && "./update.sh"  && cd "../.." && cd "shared/GLM" && "./update.sh"  && cd "../.." && cd "shared/tinygltf" && "./update.sh"  && cd "../.." && cd "shared/stb" && "./update.sh"  && cd "../.." && cd "shared/xxHash" && "./update.sh"  && cd "../.." && cd "android-clang/FileSystem" && "./update.sh"  && cd "../.." && cd "android-clang/Vulkan" && "./update.sh"  && cd "../.."