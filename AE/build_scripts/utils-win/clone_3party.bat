:: clone 3party dependencies to local server
:: updated in 2025.08

cd "../public"

rmdir /Q /S "_backup_3party"
mkdir "_backup_3party"

robocopy "ae-glm" "_backup_3party/ae-glm" /S
rmdir /Q /S "ae-glm"
::git clone --bare "https://github.com/g-truc/glm.git" "glm"
git clone --bare "https://github.com/azhirnov/glm.git" "ae-glm"

robocopy "Assimp" "_backup_3party/Assimp" /S
rmdir /Q /S "Assimp"
git clone --bare "https://github.com/assimp/assimp.git" "Assimp"

robocopy "tinygltf" "_backup_3party/tinygltf" /S
rmdir /Q /S "tinygltf"
git clone --bare "https://github.com/syoyo/tinygltf.git" "tinygltf"

robocopy "glslang" "_backup_3party/glslang" /S
rmdir /Q /S "glslang"
git clone --bare "https://github.com/KhronosGroup/glslang.git" "glslang"

robocopy "SPIRV-Tools" "_backup_3party/SPIRV-Tools" /S
rmdir /Q /S "SPIRV-Tools"
git clone --bare "https://github.com/KhronosGroup/SPIRV-Tools.git" "SPIRV-Tools"

robocopy "SPIRV-Headers" "_backup_3party/SPIRV-Headers" /S
rmdir /Q /S "SPIRV-Headers"
git clone --bare "https://github.com/KhronosGroup/SPIRV-Headers.git" "SPIRV-Headers"

robocopy "Vulkan-Headers" "_backup_3party/Vulkan-Headers" /S
rmdir /Q /S "Vulkan-Headers"
git clone --bare "https://github.com/KhronosGroup/Vulkan-Headers.git" "Vulkan-Headers"

robocopy "imgui" "_backup_3party/imgui" /S
rmdir /Q /S "imgui"
git clone --bare "https://github.com/ocornut/imgui.git" "imgui"

robocopy "brotli" "_backup_3party/brotli" /S
rmdir /Q /S "brotli"
git clone --bare "https://github.com/google/brotli.git" "brotli"

robocopy "glfw" "_backup_3party/glfw" /S
rmdir /Q /S "glfw"
git clone --bare "https://github.com/glfw/glfw.git" "glfw"

robocopy "SPIRV-Cross" "_backup_3party/SPIRV-Cross" /S
rmdir /Q /S "SPIRV-Cross"
git clone --bare "https://github.com/KhronosGroup/SPIRV-Cross.git" "SPIRV-Cross"

robocopy "utf8proc" "_backup_3party/utf8proc" /S
rmdir /Q /S "utf8proc"
git clone --bare "https://github.com/JuliaStrings/utf8proc.git" "utf8proc"

robocopy "filesystem" "_backup_3party/filesystem" /S
rmdir /Q /S "filesystem"
git clone --bare "https://github.com/gulrak/filesystem.git" "filesystem"

robocopy "VulkanMemoryAllocator" "_backup_3party/VulkanMemoryAllocator" /S
rmdir /Q /S "VulkanMemoryAllocator"
git clone --bare "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git" "VulkanMemoryAllocator"

robocopy "openvr" "_backup_3party/openvr" /S
rmdir /Q /S "openvr"
git clone --bare "https://github.com/ValveSoftware/openvr.git" "openvr"

robocopy "msdfgen" "_backup_3party/msdfgen" /S
rmdir /Q /S "msdfgen"
git clone --bare "https://github.com/Chlumsky/msdfgen.git" "msdfgen"

robocopy "stb" "_backup_3party/stb" /S
rmdir /Q /S "stb"
git clone --bare "https://github.com/nothings/stb.git" "stb"

robocopy "ARM-HWCPipe" "_backup_3party/ARM-HWCPipe" /S
rmdir /Q /S "ARM-HWCPipe"
::git clone --bare "https://github.com/ARM-software/HWCPipe.git" "ARM-HWCPipe"
::git clone --bare "https://github.com/ARM-software/libGPUCounters.git" "ARM-HWCPipe"
git clone --bare "https://github.com/azhirnov/libGPUCounters.git" "ARM-HWCPipe"

robocopy "xxHash" "_backup_3party/xxHash" /S
rmdir /Q /S "xxHash"
git clone --bare "https://github.com/Cyan4973/xxHash.git" "xxHash"

robocopy "Abseil" "_backup_3party/Abseil" /S
rmdir /Q /S "Abseil"
git clone --bare "https://github.com/abseil/abseil-cpp.git" "Abseil"

robocopy "CDT" "_backup_3party/CDT" /S
rmdir /Q /S "CDT"
git clone --bare "https://github.com/artem-ogre/CDT.git" "CDT"

robocopy "meshoptimizer" "_backup_3party/meshoptimizer" /S
rmdir /Q /S "meshoptimizer"
git clone --bare "https://github.com/zeux/meshoptimizer.git" "meshoptimizer"

robocopy "zstd" "_backup_3party/zstd" /S
rmdir /Q /S "zstd"
git clone --bare "https://github.com/facebook/zstd.git" "zstd"

robocopy "lz4" "_backup_3party/lz4" /S
rmdir /Q /S "lz4"
git clone --bare "https://github.com/lz4/lz4.git" "lz4"

robocopy "astc-encoder" "_backup_3party/astc-encoder" /S
rmdir /Q /S "astc-encoder"
git clone --bare "https://github.com/ARM-software/astc-encoder.git" "astc-encoder"

robocopy "nvapi" "_backup_3party/nvapi" /S
rmdir /Q /S "nvapi"
git clone --bare "https://github.com/NVIDIA/nvapi.git" "nvapi"

robocopy "slang" "_backup_3party/slang" /S
rmdir /Q /S "slang"
git clone --bare "https://github.com/shader-slang/slang.git" "slang"


robocopy "OpenXR-SDK" "_backup_3party/OpenXR-SDK" /S
rmdir /Q /S "OpenXR-SDK"
git clone --bare "https://github.com/KhronosGroup/OpenXR-SDK.git" "OpenXR-SDK"

robocopy "OpenXR-SDK-Source" "_backup_3party/OpenXR-SDK-Source" /S
rmdir /Q /S "OpenXR-SDK-Source"
git clone --bare "https://github.com/KhronosGroup/OpenXR-SDK-Source.git" "OpenXR-SDK-Source"

robocopy "llama.cpp" "_backup_3party/llama.cpp" /S
rmdir /Q /S "llama.cpp"
git clone --bare "https://github.com/ggml-org/llama.cpp.git" "llama.cpp"

::robocopy "breakpad" "_backup_3party/breakpad" /S
::rmdir /Q /S "breakpad"
::git clone --bare "https://github.com/google/breakpad.git" "breakpad"

::robocopy "curl" "_backup_3party/curl" /S
::rmdir /Q /S "curl"
::git clone --bare "https://github.com/curl/curl.git" "curl"

::git clone --bare "https://github.com/gcesarmza/curl-android-ios.git"

::robocopy "prebuilt-libcurl" "_backup_3party/prebuilt-libcurl" /S
::rmdir /Q /S "prebuilt-libcurl"
::git clone --bare "https://github.com/djp952/prebuilt-libcurl.git" "prebuilt-libcurl"


:: delete 'temp'
cd ..
rmdir /Q /S "temp"
mkdir "temp"
cd "temp"


:: download angel script
mkdir AngelScript
cd AngelScript
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://www.angelcode.com/angelscript/sdk/files/angelscript_2.37.0.zip', 'as.zip')"
powershell Expand-Archive as.zip -DestinationPath "."
del "as.zip"
cd sdk
rmdir /Q /S "docs"
rmdir /Q /S "angelscript/lib"
rmdir /Q /S "angelscript/projects/android"
rmdir /Q /S "angelscript/projects/codeblocks"
rmdir /Q /S "angelscript/projects/gnuc"
rmdir /Q /S "angelscript/projects/meson"
rmdir /Q /S "angelscript/projects/mingw"
rmdir /Q /S "angelscript/projects/msvc2019"
rmdir /Q /S "angelscript/projects/xcode"
rmdir /Q /S "samples"
git init
git add -A
git commit -m "init"
cd ../..
robocopy "../public/AngelScript" "../public/_backup_3party/AngelScript" /S
rmdir /Q /S "../public/AngelScript"
robocopy "AngelScript/sdk/.git" "../public/AngelScript" /S


:: download FFmpeg
:: if failed then go to https://github.com/BtbN/FFmpeg-Builds/releases and get link to new versions
git init ffmpeg
cd ffmpeg
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-n6.1-latest-win64-lgpl-shared-6.1.zip', 'win64_lgpl.zip')"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-n6.1-latest-win64-gpl-shared-6.1.zip', 'win64_gpl.zip')"
powershell Expand-Archive "win64_lgpl.zip" -DestinationPath "temp_win64_lgpl"
powershell Expand-Archive "win64_gpl.zip" -DestinationPath "temp_win64_gpl"
move "temp_win64_lgpl/ffmpeg-n6.1-latest-win64-lgpl-shared-6.1" "win64-lgpl"
move "temp_win64_gpl/ffmpeg-n6.1-latest-win64-gpl-shared-6.1" "win64-gpl"
del "win64_lgpl.zip"
del "win64_gpl.zip"
git add "*.dll"
git add "*.h"
git add "win64_lgpl/LICENSE.txt"
git add "win64_gpl/LICENSE.txt"
git commit -m "init"
cd ..
robocopy "../public/ffmpeg-win64" "../public/_backup_3party/ffmpeg-win64" /S
rmdir /Q /S "../public/ffmpeg-win64"
robocopy "ffmpeg/.git" "../public/ffmpeg-win64" /S


:: download FreeType
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://sourceforge.net/projects/freetype/files/freetype2/2.13.3/ft2133.zip/download', 'ft.zip')"
powershell Expand-Archive ft.zip -DestinationPath "."
del "ft.zip"
cd "freetype-2.13.3"
git init
git add -A
git commit -m "init"
cd ..
robocopy "../public/FreeType" "../public/_backup_3party/FreeType" /S
rmdir /Q /S "../public/FreeType"
robocopy "freetype-2.13.3/.git" "../public/FreeType" /S


:: download DevIL
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://sourceforge.net/projects/openil/files/DevIL Win32 and Win64/DevIL-EndUser-x64-1.8.0.zip/download', 'devil-win-x64.zip')"
powershell Expand-Archive "devil-win-x64.zip" -DestinationPath "devil/win-x64"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://sourceforge.net/projects/openil/files/DevIL/1.8.0/DevIL-1.8.0.zip/download', 'devil.zip')"
powershell Expand-Archive "devil.zip" -DestinationPath "temp"
del "devil-win-x64.zip"
del "devil.zip"
robocopy "temp/DevIL/DevIL/include" "devil/include" "*.h" /S
copy /Y "temp\DevIL\LICENSE" "devil"
cd devil
git init
git add -A
git commit -m "init"
cd ..
robocopy "../public/DevIL" "../public/_backup_3party/DevIL" /S
rmdir /Q /S "../public/DevIL"
robocopy "DevIL/.git" "../public/DevIL" /S


:: download KTX
git clone --recurse-submodules --branch "v4.4.0" "https://github.com/KhronosGroup/KTX-Software.git" "KTX-Software" --single-branch
cd "KTX-Software"
rmdir /Q /S ".git"
rmdir /Q /S "tests"
del /s /q ".gitattributes"
del /s /q ".gitignore"
del /s /q ".gitmodules"
del /s /q ".gitrepo"
mkdir "tests"
copy NUL "tests/CMakeLists.txt"
git init
git add -A
git commit -m "v4.4.0"
cd ..
robocopy "../public/KTX-Software" "../public/_backup_3party/KTX-Software" /S
rmdir /Q /S "../public/KTX-Software"
robocopy "KTX-Software/.git" "../public/KTX-Software" /S


:: download PowerVR-SDK
git clone --branch "R24.2-v5.13" "https://github.com/powervr-graphics/Native_SDK.git" "PowerVR-SDK" --single-branch
cd "PowerVR-SDK"
rmdir /Q /S ".git"
git init
git add "include/PVRScopeComms.h"
git add "include/PVRScopeStats.h"
git add "lib/Android_arm64-v8a"
git add "lib/Android_armeabi-v7a"
git add "lib/Android_x86"
git add "lib/Android_x86_64"
git commit -m "R24.2-v5.13"
cd ..
robocopy "../public/PowerVR-SDK" "../public/_backup_3party/PowerVR-SDK" /S
rmdir /Q /S "../public/PowerVR-SDK"
robocopy "PowerVR-SDK/.git" "../public/PowerVR-SDK" /S


:: download AMD Compressonator
git clone --branch "V4.5.52" "https://github.com/GPUOpen-Tools/compressonator.git" "AMD-Compressonator" --single-branch
cd "AMD-Compressonator"
rmdir /Q /S ".git"
del /s /q ".gitattributes"
git init
git add -A
git commit -m "V4.5.52"
cd ..
robocopy "../public/AMD-Compressonator" "../public/_backup_3party/AMD-Compressonator" /S
rmdir /Q /S "../public/AMD-Compressonator"
robocopy "AMD-Compressonator/.git" "../public/AMD-Compressonator" /S


:: download AMD GPUPerfAPI
:: https://github.com/GPUOpen-Tools/gpu_performance_api/releases
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/GPUOpen-Tools/gpu_performance_api/releases/download/v4.0-tag/GPUPerfAPI-4.0.0.39.zip', 'GPUPerfAPI-win64.zip')"
powershell Expand-Archive "GPUPerfAPI-win64.zip" -DestinationPath "GPUPerfAPI/win-x64"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/GPUOpen-Tools/gpu_performance_api/releases/download/v4.0-tag/GPUPerfAPI-Linux-4.0.0.39.tgz', 'GPUPerfAPI-linux64.tgz')"
mkdir "GPUPerfAPI/linux-x64"
tar -xzvf "GPUPerfAPI-linux64.tgz" -C "GPUPerfAPI/linux-x64"
cd GPUPerfAPI/win-x64
mkdir "bin"
copy /Y "4_0\bin\GPUPerfAPIVK-x64.dll" "bin"
copy /Y "4_0\bin\GPUPerfAPICounters-x64.dll" "bin"
copy /Y "4_0\LICENSE.txt" "."
copy /Y "4_0\NOTICES.txt" "."
robocopy "4_0\include" "include" /S
rmdir /Q /S "4_0"
cd ../linux-x64
mkdir "bin"
copy /Y "4_0\bin\libGPUPerfAPICounters.so" "bin"
copy /Y "4_0\bin\libGPUPerfAPIVK.so" "bin"
copy /Y "4_0\LICENSE.txt" "."
copy /Y "4_0\NOTICES.txt" "."
robocopy "4_0\include" "include" /S
rmdir /Q /S "4_0"
cd ..
git init
git add -A
git commit -m "v4.0"
cd ..
robocopy "../public/AMD-GPUPerfAPI" "../public/_backup_3party/AMD-GPUPerfAPI" /S
rmdir /Q /S "../public/AMD-GPUPerfAPI"
robocopy "GPUPerfAPI/.git" "../public/AMD-GPUPerfAPI" /S


:: download nvml
mkdir nvml
cd nvml
wget "https://raw.githubusercontent.com/nvpro-samples/nvpro_core/9cabfaab3ba7ae8857504bcfc7b326dec953bb2c/third_party/binaries/nvml/nvml.h"
git init
git add -A
git commit -m "init"
cd ..
robocopy "../public/nvml" "../public/_backup_3party/nvml" /S
rmdir /Q /S "../public/nvml"
robocopy "nvml/.git" "../public/nvml" /S


:: download PICO OpenXR SDK
:: from https://developer.picoxr.com/resources/


:: delete 'temp'
cd ..
rmdir /Q /S "temp"


:: Make backup
::cd "backup"
::powershell Compress-Archive -Path "../public" -DestinationPath ($(Get-Date).ToString('yyyy-MM-dd-HH-mm') + '_3party.zip')
