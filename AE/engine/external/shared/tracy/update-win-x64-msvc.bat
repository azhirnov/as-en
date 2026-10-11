
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
echo "update 'tracy'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64-msvc19.44\tracy"
rmdir /Q /S "temp"
mkdir "build"
git clone --branch "v0.14.1" "https://github.com/wolfpld/tracy.git" "temp"
copy /Y "temp\CMakeLists.txt" "temp\origin_CMakeLists.txt"
copy /Y "tracy_CMakeLists.txt" "temp\CMakeLists.txt"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64-msvc19.44\tracy\bin"
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64-msvc19.44/tracy"  || goto :pauseOnError
cmake --build build --config Debug --target install -j 12 || goto :pauseOnError
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64-msvc19.44/tracy"  || goto :pauseOnError
cmake --build build --config Release --target install -j 12 || goto :pauseOnError
copy /Y "build\Debug\TracyClientd.pdb" "..\..\..\..\..\AE-Bin\external\win-x64-msvc19.44\tracy\lib\TracyClientd.pdb"

echo.
choice /C YN /N /M "Build Tracy profiler? [Y/n] "
if errorlevel 2 goto :cleanup
if errorlevel 1 goto :buildProfiler

:buildProfiler
pushd "temp\profiler" || goto :pauseOnError

echo set(CMAKE_OBJECT_PATH_MAX 512)> "CMakeLists.head"
type "CMakeLists.txt" >> "CMakeLists.head"
move /Y "CMakeLists.head" "CMakeLists.txt"

cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release || ( popd & goto :pauseOnError )
cmake --build build --config Release -j 12 || ( popd & goto :pauseOnError )
popd
copy /Y "temp\profiler\build\Release\tracy-profiler.exe" "..\..\..\..\..\AE-Bin\external\win-x64-msvc19.44\tracy\bin\tracy-profiler.exe"
:cleanup
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
