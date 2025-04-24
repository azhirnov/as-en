# clone 3party dependencies to local server

cd "../public"

rm -rf "_backup_3party"
mkdir "_backup_3party"

mv "ae-glm" "_backup_3party/ae-glm"
#git clone --bare "https://github.com/g-truc/glm.git" "glm"
git clone --bare "https://github.com/azhirnov/glm.git" "ae-glm"

mv "Assimp" "_backup_3party/Assimp"
git clone --bare "https://github.com/assimp/assimp.git" "Assimp"

mv "tinygltf" "_backup_3party/tinygltf"
git clone --bare "https://github.com/syoyo/tinygltf.git" "tinygltf"

mv "glslang" "_backup_3party/glslang"
git clone --bare "https://github.com/KhronosGroup/glslang.git" "glslang"

mv "SPIRV-Tools" "_backup_3party/SPIRV-Tools"
git clone --bare "https://github.com/KhronosGroup/SPIRV-Tools.git" "SPIRV-Tools"

mv "SPIRV-Headers" "_backup_3party/SPIRV-Headers"
git clone --bare "https://github.com/KhronosGroup/SPIRV-Headers.git" "SPIRV-Headers"

mv "Vulkan-Headers" "_backup_3party/Vulkan-Headers"
git clone --bare "https://github.com/KhronosGroup/Vulkan-Headers.git" "Vulkan-Headers"

mv "imgui" "_backup_3party/imgui"
git clone --bare "https://github.com/ocornut/imgui.git" "imgui"

mv "brotli" "_backup_3party/brotli"
git clone --bare "https://github.com/google/brotli.git" "brotli"

mv "glfw" "_backup_3party/glfw"
git clone --bare "https://github.com/glfw/glfw.git" "glfw"

mv "SPIRV-Cross" "_backup_3party/SPIRV-Cross"
git clone --bare "https://github.com/KhronosGroup/SPIRV-Cross.git" "SPIRV-Cross"

mv "utf8proc" "_backup_3party/utf8proc"
git clone --bare "https://github.com/JuliaStrings/utf8proc.git" "utf8proc"

mv "filesystem" "_backup_3party/filesystem"
git clone --bare "https://github.com/gulrak/filesystem.git" "filesystem"

mv "VulkanMemoryAllocator" "_backup_3party/VulkanMemoryAllocator"
git clone --bare "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git" "VulkanMemoryAllocator"

mv "openvr" "_backup_3party/openvr"
git clone --bare "https://github.com/ValveSoftware/openvr.git" "openvr"

mv "msdfgen" "_backup_3party/msdfgen"
git clone --bare "https://github.com/Chlumsky/msdfgen.git" "msdfgen"

mv "stb" "_backup_3party/stb"
git clone --bare "https://github.com/nothings/stb.git" "stb"

mv "ARM-HWCPipe" "_backup_3party/ARM-HWCPipe"
#git clone --bare "https://github.com/ARM-software/HWCPipe.git" "ARM-HWCPipe"
#git clone --bare "https://github.com/ARM-software/libGPUCounters.git" "ARM-HWCPipe"
git clone --bare "https://github.com/azhirnov/libGPUCounters.git" "ARM-HWCPipe"

mv "xxHash" "_backup_3party/xxHash"
git clone --bare "https://github.com/Cyan4973/xxHash.git" "xxHash"

mv "Abseil" "_backup_3party/Abseil"
git clone --bare "https://github.com/abseil/abseil-cpp.git" "Abseil"

mv "CDT" "_backup_3party/CDT"
git clone --bare "https://github.com/artem-ogre/CDT.git" "CDT"

mv "meshoptimizer" "_backup_3party/meshoptimizer"
git clone --bare "https://github.com/zeux/meshoptimizer.git" "meshoptimizer"

mv "zstd" "_backup_3party/zstd"
git clone --bare "https://github.com/facebook/zstd.git" "zstd"

mv "lz4" "_backup_3party/lz4"
git clone --bare "https://github.com/lz4/lz4.git" "lz4"

mv "astc-encoder" "_backup_3party/astc-encoder"
git clone --bare "https://github.com/ARM-software/astc-encoder.git" "astc-encoder"

mv "nvapi" "_backup_3party/nvapi"
git clone --bare "https://github.com/NVIDIA/nvapi.git" "nvapi"

mv "slang" "_backup_3party/slang"
git clone --bare "https://github.com/shader-slang/slang.git" "slang"

#mv "breakpad" "_backup_3party/breakpad"
#git clone --bare "https://github.com/google/breakpad.git" "breakpad"

#mv "curl" "_backup_3party/curl"
#git clone --bare "https://github.com/curl/curl.git" "curl"

#git clone --bare "https://github.com/gcesarmza/curl-android-ios.git"

#mv "prebuilt-libcurl" "_backup_3party/prebuilt-libcurl"
#git clone --bare "https://github.com/djp952/prebuilt-libcurl.git" "prebuilt-libcurl"


# delete 'temp'
cd ..
rm -rf "temp"
mkdir "temp"
cd "temp"

































# delete 'temp'
cd ..
rm -rf "temp"

