
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
echo "update 'Compressonator'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64-clang19\Compressonator"
rmdir /Q /S "temp"
mkdir "build"
git clone --branch "V4.5.52" "https://github.com/GPUOpen-Tools/compressonator.git" "temp"
copy /Y "temp\CMakeLists.txt" "temp\origin_CMakeLists.txt"
copy /Y "Compressonator_CMakeLists.txt" "temp\CMakeLists.txt"
cmake -S temp -B build -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64-clang19/Compressonator"  || goto :pauseOnError
cmake --build build --config Debug --target "CMP_Compressonator" -j 12 || goto :pauseOnError
robocopy "build\lib\Debug" "..\..\..\..\..\AE-Bin\external\win-x64-clang19\Compressonator\lib" *.lib /S 
cmake -S temp -B build -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64-clang19/Compressonator"  || goto :pauseOnError
cmake --build build --config Release --target "CMP_Compressonator" -j 12 || goto :pauseOnError
robocopy "build\lib\Release" "..\..\..\..\..\AE-Bin\external\win-x64-clang19\Compressonator\lib" *.lib /S 
mkdir "..\..\..\..\..\AE-Bin\external\win-x64-clang19\Compressonator\include"
copy /Y "temp\cmp_compressonatorlib\compressonator.h" "..\..\..\..\..\AE-Bin\external\win-x64-clang19\Compressonator\include\compressonator.h"
robocopy "temp\license" "..\..\..\..\..\AE-Bin\external\win-x64-clang19\Compressonator\license" /S 
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
