
@echo off
rem Enable virtual terminal processing so that ANSI escapes work
for /f "tokens=*" %%i in ('reg query HKCU\Console /v VirtualTerminalLevel ^| findstr /r "[0-9]"') do (
    set vt=%%i
)
if "%vt%"=="VirtualTerminalLevel REG_DWORD 0x1" (
    rem already enabled – nothing to do
) else (
    reg add "HKCU\Console" /v VirtualTerminalLevel /t REG_DWORD /d 1 /f >nul
)

@echo on
echo "update 'glslang'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64-msvc19.44\glslang"
rmdir /Q /S "temp"
mkdir "build"
git clone --branch "vulkan-sdk-1.4.350.1" "https://github.com/KhronosGroup/glslang.git" "temp"
git clone --branch "vulkan-sdk-1.4.350.1" "https://github.com/KhronosGroup/SPIRV-Tools.git" "temp/External/spirv-tools"
git clone --branch "vulkan-sdk-1.4.350.1" "https://github.com/KhronosGroup/SPIRV-Headers.git" "temp/External/spirv-tools/external/spirv-headers"
copy /Y "temp\CMakeLists.txt" "temp\origin_CMakeLists.txt"
copy /Y "glslang_CMakeLists.txt" "temp\CMakeLists.txt"
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64-msvc19.44/glslang"  || goto :pauseOnError
cmake --build build --config Debug --target install -j 12 || goto :pauseOnError
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64-msvc19.44/glslang"  || goto :pauseOnError
cmake --build build --config Release --target install -j 12 || goto :pauseOnError
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
