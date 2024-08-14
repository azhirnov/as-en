:: clone 3party dependencies to local server

cd ../..
mkdir 3party
cd 3party

rmdir /Q /S "_backup_3party"
mkdir "_backup_3party"

move "ae-glm" "_backup_3party/ae-glm"
git clone --bare "https://github.com/azhirnov/glm.git" "ae-glm"

move "Assimp" "_backup_3party/Assimp"
git clone --bare "https://github.com/assimp/assimp.git" "Assimp"

move "tinygltf" "_backup_3party/tinygltf"
git clone --bare "https://github.com/syoyo/tinygltf.git" "tinygltf"

move "glslang" "_backup_3party/glslang"
git clone --bare "https://github.com/KhronosGroup/glslang.git" "glslang"

move "SPIRV-Tools" "_backup_3party/SPIRV-Tools"
git clone --bare "https://github.com/KhronosGroup/SPIRV-Tools.git" "SPIRV-Tools"

move "SPIRV-Headers" "_backup_3party/SPIRV-Headers"
git clone --bare "https://github.com/KhronosGroup/SPIRV-Headers.git" "SPIRV-Headers"

move "Vulkan-Headers" "_backup_3party/Vulkan-Headers"
git clone --bare "https://github.com/KhronosGroup/Vulkan-Headers.git" "Vulkan-Headers"

move "imgui" "_backup_3party/imgui"
git clone --bare "https://github.com/ocornut/imgui.git" "imgui"

move "brotli" "_backup_3party/brotli"
git clone --bare "https://github.com/google/brotli.git" "brotli"

move "glfw" "_backup_3party/glfw"
git clone --bare "https://github.com/glfw/glfw.git" "glfw"

move "SPIRV-Cross" "_backup_3party/SPIRV-Cross"
git clone --bare "https://github.com/KhronosGroup/SPIRV-Cross.git" "SPIRV-Cross"

move "utf8proc" "_backup_3party/utf8proc"
git clone --bare "https://github.com/JuliaStrings/utf8proc.git" "utf8proc"

move "filesystem" "_backup_3party/filesystem"
git clone --bare "https://github.com/gulrak/filesystem.git" "filesystem"

move "VulkanMemoryAllocator" "_backup_3party/VulkanMemoryAllocator"
git clone --bare "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git" "VulkanMemoryAllocator"

move "openvr" "_backup_3party/openvr"
git clone --bare "https://github.com/ValveSoftware/openvr.git" "openvr"

move "msdfgen" "_backup_3party/msdfgen"
git clone --bare "https://github.com/Chlumsky/msdfgen.git" "msdfgen"

move "stb" "_backup_3party/stb"
git clone --bare "https://github.com/nothings/stb.git" "stb"

move "ARM-HWCPipe" "_backup_3party/ARM-HWCPipe"
::git clone --bare "https://github.com/ARM-software/HWCPipe.git" "ARM-HWCPipe"
git clone --bare "https://github.com/ARM-software/libGPUCounters.git" "ARM-HWCPipe"

move "xxHash" "_backup_3party/xxHash"
git clone --bare "https://github.com/Cyan4973/xxHash.git" "xxHash"

move "Abseil" "_backup_3party/Abseil"
git clone --bare "https://github.com/abseil/abseil-cpp.git" "Abseil"

move "CDT" "_backup_3party/CDT"
git clone --bare "https://github.com/artem-ogre/CDT.git" "CDT"

move "meshoptimizer" "_backup_3party/meshoptimizer"
git clone --bare "https://github.com/zeux/meshoptimizer.git" "meshoptimizer"

move "zstd" "_backup_3party/zstd"
git clone --bare "https://github.com/facebook/zstd.git" "zstd"

move "lz4" "_backup_3party/lz4"
git clone --bare "https://github.com/lz4/lz4.git" "lz4"

move "astc-encoder" "_backup_3party/astc-encoder"
git clone --bare "https://github.com/ARM-software/astc-encoder.git" "astc-encoder"

::move "glm" "backup_3party/glm"
::git clone --bare "https://github.com/g-truc/glm.git" "glm"

::move "breakpad" "_backup_3party/breakpad"
::git clone --bare "https://github.com/google/breakpad.git" "breakpad"

::move "curl" "_backup_3party/curl"
::git clone --bare "https://github.com/curl/curl.git" "curl"

::git clone --bare "https://github.com/gcesarmza/curl-android-ios.git"

::move "prebuilt-libcurl" "_backup_3party/prebuilt-libcurl"
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
move "../public/AngelScript" "../public/_backup_3party/AngelScript"
robocopy "AngelScript/sdk/.git" "../public/AngelScript" /S


:: download FFmpeg
:: if failed then go to https://github.com/BtbN/FFmpeg-Builds/releases and get link to new versions
git init ffmpeg
cd ffmpeg
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/BtbN/FFmpeg-Builds/releases/download/autobuild-2023-12-31-12-55/ffmpeg-n6.1.1-win64-lgpl-shared-6.1.zip', 'win64_lgpl.zip')"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/BtbN/FFmpeg-Builds/releases/download/autobuild-2023-12-31-12-55/ffmpeg-n6.1.1-win64-gpl-shared-6.1.zip', 'win64_gpl.zip')"
powershell Expand-Archive "win64_lgpl.zip" -DestinationPath "temp_win64_lgpl"
powershell Expand-Archive "win64_gpl.zip" -DestinationPath "temp_win64_gpl"
move "temp_win64_lgpl/ffmpeg-n6.1.1-win64-lgpl-shared-6.1" "win64-lgpl"
move "temp_win64_gpl/ffmpeg-n6.1.1-win64-gpl-shared-6.1" "win64-gpl"
del "win64_lgpl.zip"
del "win64_gpl.zip"
git add "*.dll"
git add "*.h"
git add "win64_lgpl/LICENSE.txt"
git add "win64_gpl/LICENSE.txt"
git commit -m "init"
cd ..
move "../public/ffmpeg-win64" "../public/_backup_3party/ffmpeg-win64"
robocopy "ffmpeg/.git" "../public/ffmpeg-win64" /S


:: download FreeType
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://sourceforge.net/projects/freetype/files/freetype2/2.11.1/ft2111.zip', 'ft.zip')"
powershell Expand-Archive ft.zip -DestinationPath "."
del "ft.zip"
cd "freetype-2.11.1"
git init
git add -A
git commit -m "init"
cd ..
move "../public/FreeType" "../public/_backup_3party/FreeType"
robocopy "freetype-2.11.1/.git" "../public/FreeType" /S


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
move "../public/DevIL" "../public/_backup_3party/DevIL"
robocopy "DevIL/.git" "../public/DevIL" /S


:: download KTX
git clone --recurse-submodules --branch "v4.3.2" "https://github.com/KhronosGroup/KTX-Software.git" "KTX-Software" --single-branch
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
git commit -m "v4.2.1"
cd ..
move "../public/KTX-Software" "../public/_backup_3party/KTX-Software"
robocopy "KTX-Software/.git" "../public/KTX-Software" /S


:: download PowerVR-SDK
git clone --branch "R24.1-v5.12" "https://github.com/powervr-graphics/Native_SDK.git" "PowerVR-SDK" --single-branch
cd "PowerVR-SDK"
rmdir /Q /S ".git"
git init
git add "include/PVRScopeComms.h"
git add "include/PVRScopeStats.h"
git add "lib/Android_arm64-v8a"
git add "lib/Android_armeabi-v7a"
git add "lib/Android_x86"
git add "lib/Android_x86_64"
git commit -m "R24.1-v5.12"
cd ..
move "../public/PowerVR-SDK" "../public/_backup_3party/PowerVR-SDK"
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
move "../public/AMD-Compressonator" "../public/_backup_3party/AMD-Compressonator"
robocopy "AMD-Compressonator/.git" "../public/AMD-Compressonator" /S


:: download AMD GPUPerfAPI
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/GPUOpen-Tools/gpu_performance_api/releases/download/v3.16-tag/GPUPerfAPI-3.16.0.30.zip', 'GPUPerfAPI-win64.zip')"
powershell Expand-Archive "GPUPerfAPI-win64.zip" -DestinationPath "GPUPerfAPI/win-x64"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/GPUOpen-Tools/gpu_performance_api/releases/download/v3.16-tag/GPUPerfAPI-Linux-3.16.0.30.tgz', 'GPUPerfAPI-linux64.tgz')"
mkdir "GPUPerfAPI/linux-x64"
tar -xzvf "GPUPerfAPI-linux64.tgz" -C "GPUPerfAPI/linux-x64"
cd GPUPerfAPI/win-x64
mkdir "bin"
copy /Y "GPUPerfAPI-3.16.0.30\3_16\Bin\x64\GPUPerfAPIVK-x64.dll" "bin"
copy /Y "GPUPerfAPI-3.16.0.30\3_16\Bin\x64\GPUPerfAPICounters-x64.dll" "bin"
copy /Y "GPUPerfAPI-3.16.0.30\3_16\LICENSE.txt" "."
copy /Y "GPUPerfAPI-3.16.0.30\3_16\NOTICES.txt" "."
robocopy "GPUPerfAPI-3.16.0.30\3_16\include" "include" /S
rmdir /Q /S "GPUPerfAPI-3.16.0.30"
cd ../linux-x64
mkdir "bin"
copy /Y "3_16\Bin\Linx64\libGPUPerfAPICounters.so" "bin"
copy /Y "3_16\Bin\Linx64\libGPUPerfAPIVK.so" "bin"
copy /Y "3_16\LICENSE.txt" "."
copy /Y "3_16\NOTICES.txt" "."
robocopy "3_16\include" "include" /S
rmdir /Q /S "3_16"
cd ..
git init
git add -A
git commit -m "v3.16"
cd ..
move "../public/AMD-GPUPerfAPI" "../public/_backup_3party/AMD-GPUPerfAPI"
robocopy "GPUPerfAPI/.git" "../public/AMD-GPUPerfAPI" /S


:: download nvml
mkdir nvml
cd nvml
wget "https://raw.githubusercontent.com/nvpro-samples/nvpro_core/master/third_party/binaries/nvml/nvml.h"
git init
git add -A
git commit -m "init"
cd ..
move "../public/nvml" "../public/_backup_3party/nvml"
robocopy "nvml/.git" "../public/nvml" /S


:: delete 'temp'
cd ..
rmdir /Q /S "temp"


:: Make backup
::cd "backup"
::powershell Compress-Archive -Path "../public" -DestinationPath ($(Get-Date).ToString('yyyy-MM-dd-HH-mm') + '_3party.zip')
