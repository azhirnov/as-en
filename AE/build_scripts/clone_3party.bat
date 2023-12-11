:: clone 3party dependencies to local server

cd ../..
mkdir 3party
cd 3party

rmdir /Q /S "ae-glm"
git clone --bare "https://github.com/azhirnov/glm.git" "ae-glm"

rmdir /Q /S "Assimp"
git clone --bare "https://github.com/assimp/assimp.git" "Assimp"

rmdir /Q /S "tinygltf"
git clone --bare "https://github.com/syoyo/tinygltf.git" "tinygltf"

rmdir /Q /S "glslang"
git clone --bare "https://github.com/KhronosGroup/glslang.git" "glslang"

rmdir /Q /S "SPIRV-Tools"
git clone --bare "https://github.com/KhronosGroup/SPIRV-Tools.git" "SPIRV-Tools"

rmdir /Q /S "SPIRV-Headers"
git clone --bare "https://github.com/KhronosGroup/SPIRV-Headers.git" "SPIRV-Headers"

rmdir /Q /S "Vulkan-Headers"
git clone --bare "https://github.com/KhronosGroup/Vulkan-Headers.git" "Vulkan-Headers"

rmdir /Q /S "imgui"
git clone --bare "https://github.com/ocornut/imgui.git" "imgui"

rmdir /Q /S "brotli"
git clone --bare "https://github.com/google/brotli.git" "brotli"

rmdir /Q /S "glfw"
git clone --bare "https://github.com/glfw/glfw.git" "glfw"

rmdir /Q /S "SPIRV-Cross"
git clone --bare "https://github.com/KhronosGroup/SPIRV-Cross.git" "SPIRV-Cross"

rmdir /Q /S "utf8proc"
git clone --bare "https://github.com/JuliaStrings/utf8proc.git" "utf8proc"

rmdir /Q /S "filesystem"
git clone --bare "https://github.com/gulrak/filesystem.git" "filesystem"

rmdir /Q /S "VulkanMemoryAllocator"
git clone --bare "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git" "VulkanMemoryAllocator"

rmdir /Q /S "openvr"
git clone --bare "https://github.com/ValveSoftware/openvr.git" "openvr"

rmdir /Q /S "msdfgen"
git clone --bare "https://github.com/Chlumsky/msdfgen.git" "msdfgen"

rmdir /Q /S "stb"
git clone --bare "https://github.com/nothings/stb.git" "stb"

rmdir /Q /S "ARM-HWCPipe"
git clone --bare "https://github.com/ARM-software/HWCPipe.git" "ARM-HWCPipe"

rmdir /Q /S "hardware-perfcounter"
git clone --bare "https://github.com/google/hardware-perfcounter.git" "hardware-perfcounter"
rmdir /Q /S "hardware-perfcounter-envytools"
git clone --bare "https://github.com/freedreno/envytools.git" "hardware-perfcounter-envytools"

rmdir /Q /S "xxHash"
git clone --bare "https://github.com/Cyan4973/xxHash.git" "xxHash"

rmdir /Q /S "Abseil"
git clone --bare "https://github.com/abseil/abseil-cpp.git" "Abseil"

rmdir /Q /S "CDT"
git clone --bare "https://github.com/artem-ogre/CDT.git" "CDT"

:: delete 'temp'
cd ..
rmdir /Q /S "temp"
mkdir "temp"
cd "temp"


:: download angel script
mkdir AngelScript
cd AngelScript
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://www.angelcode.com/angelscript/sdk/files/angelscript_2.36.0.zip', 'as.zip')"
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
rmdir /Q /S "../3party/AngelScript"
robocopy "AngelScript/sdk/.git" "../3party/AngelScript" /S


:: download FFmpeg
:: if failed then go to https://github.com/BtbN/FFmpeg-Builds/releases and get link to new versions
git init ffmpeg
cd ffmpeg
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-n5.1-latest-win64-lgpl-shared-5.1.zip', 'win64_lgpl.zip')"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-n5.1-latest-win64-gpl-shared-5.1.zip', 'win64_gpl.zip')"
powershell Expand-Archive "win64_lgpl.zip" -DestinationPath "temp_win64_lgpl"
powershell Expand-Archive "win64_gpl.zip" -DestinationPath "temp_win64_gpl"
move "temp_win64_lgpl/ffmpeg-n5.1-latest-win64-lgpl-shared-5.1" "win64-lgpl"
move "temp_win64_gpl/ffmpeg-n5.1-latest-win64-gpl-shared-5.1" "win64-gpl"
del "win64_lgpl.zip"
del "win64_gpl.zip"
git add "*.dll"
git add "*.h"
git add "win64_lgpl/LICENSE.txt"
git add "win64_gpl/LICENSE.txt"
git commit -m "init"
cd ..
rmdir /Q /S "../3party/ffmpeg-win64"
robocopy "ffmpeg/.git" "../3party/ffmpeg-win64" /S


:: download FreeType
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://sourceforge.net/projects/freetype/files/freetype2/2.11.1/ft2111.zip', 'ft.zip')"
powershell Expand-Archive ft.zip -DestinationPath "."
del "ft.zip"
cd "freetype-2.11.1"
git init
git add -A
git commit -m "init"
cd ..
rmdir /Q /S "../3party/FreeType"
robocopy "freetype-2.11.1/.git" "../3party/FreeType" /S


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
rmdir /Q /S "../3party/DevIL"
robocopy "DevIL/.git" "../3party/DevIL" /S


:: download KTX
git clone --recurse-submodules --branch "v4.2.1" "https://github.com/KhronosGroup/KTX-Software.git" "KTX-Software"
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
rmdir /Q /S "../3party/KTX-Software"
robocopy "KTX-Software/.git" "../3party/KTX-Software" /S


:: delete 'temp'
cd ..
rmdir /Q /S "temp"

pause
