
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
echo "update 'OpenXR'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64-clang19\OpenXR"
rmdir /Q /S "temp"
mkdir "build"
mkdir "temp"
git clone --branch "release-1.1.50" "https://github.com/KhronosGroup/OpenXR-SDK-Source.git" "temp/OpenXR-SDK-Source"
copy /Y "openxr_CMakeLists.txt" "temp\CMakeLists.txt"
cmake -S temp -B build -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64-clang19/OpenXR"  || goto :pauseOnError
cmake --build build --config Release --target install -j 12 || goto :pauseOnError
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
