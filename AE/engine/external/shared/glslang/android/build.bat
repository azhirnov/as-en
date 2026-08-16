
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
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\glslang"
rmdir /Q /S "temp"
git clone --branch "vulkan-sdk-1.4.350.1" "https://github.com/KhronosGroup/glslang.git" "temp"
git clone --branch "vulkan-sdk-1.4.350.1" "https://github.com/KhronosGroup/SPIRV-Tools.git" "temp/External/spirv-tools"
git clone --branch "vulkan-sdk-1.4.350.1" "https://github.com/KhronosGroup/SPIRV-Headers.git" "temp/External/spirv-tools/external/spirv-headers"
copy /Y "temp\CMakeLists.txt" "temp\origin_CMakeLists.txt"
copy /Y "glslang_CMakeLists.txt" "temp\CMakeLists.txt"
cd "android"
"gradlew.bat" buildRelease

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
