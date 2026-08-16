
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
echo "update 'llama'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64\llama"
rmdir /Q /S "temp"
mkdir "build"
git clone --recurse-submodules --branch "b8665" "https://github.com/ggml-org/llama.cpp.git" "temp"
copy /Y "temp\CMakeLists.txt" "temp\origin_CMakeLists.txt"
copy /Y "llama_CMakeLists.txt" "temp\CMakeLists.txt"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\llama"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\llama\include"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\llama\lib"
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Bin\external\win-x64\llama\llama-LICENSE.txt"
copy /Y "temp\include\llama.h" "..\..\..\..\..\AE-Bin\external\win-x64\llama\include\llama.h"
robocopy "temp\ggml\include" "..\..\..\..\..\AE-Bin\external\win-x64\llama\include" *.h /S 
rmdir /Q /S "build"
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64/llama"  -DCPU_TYPE=1 || goto :pauseOnError
cmake --build build --config Release --target "ggml-cpu" -j 12 || goto :pauseOnError
copy /Y "build\bin\Release\ggml-cpu.dll" "..\..\..\..\..\AE-Bin\external\win-x64\llama\lib\ggml-cpu-sse4.dll"
rmdir /Q /S "build"
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64/llama"  -DCPU_TYPE=2 || goto :pauseOnError
cmake --build build --config Release --target "ggml-cpu" -j 12 || goto :pauseOnError
copy /Y "build\bin\Release\ggml-cpu.dll" "..\..\..\..\..\AE-Bin\external\win-x64\llama\lib\ggml-cpu-avx.dll"
rmdir /Q /S "build"
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64/llama"  -DCPU_TYPE=3 || goto :pauseOnError
cmake --build build --config Release --target "ggml-cpu" -j 12 || goto :pauseOnError
copy /Y "build\bin\Release\ggml-cpu.dll" "..\..\..\..\..\AE-Bin\external\win-x64\llama\lib\ggml-cpu-avx2.dll"
rmdir /Q /S "build"
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64/llama"  -DCPU_TYPE=4 || goto :pauseOnError
cmake --build build --config Release --target "ggml-cpu" -j 12 || goto :pauseOnError
copy /Y "build\bin\Release\ggml-cpu.dll" "..\..\..\..\..\AE-Bin\external\win-x64\llama\lib\ggml-cpu-avx512_bf16.dll"
rmdir /Q /S "build"
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64/llama"  -DCPU_TYPE=5 || goto :pauseOnError
cmake --build build --config Release --target "ggml-cpu" -j 12 || goto :pauseOnError
copy /Y "build\bin\Release\ggml-cpu.dll" "..\..\..\..\..\AE-Bin\external\win-x64\llama\lib\ggml-cpu-avx512_bf16_vbmi_vnni.dll"
rmdir /Q /S "build"
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64/llama"  -DCPU_TYPE=6 || goto :pauseOnError
cmake --build build --config Release --target "ggml-cpu" -j 12 || goto :pauseOnError
copy /Y "build\bin\Release\ggml-cpu.dll" "..\..\..\..\..\AE-Bin\external\win-x64\llama\lib\ggml-cpu-avx2_vnni.dll"
rmdir /Q /S "build"
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64/llama"  -DCPU_TYPE=1 -DENABLE_VULKAN=ON || goto :pauseOnError
cmake --build build --config Release --target "ggml-vulkan" -j 12 || goto :pauseOnError
copy /Y "build\bin\Release\ggml-vulkan.dll" "..\..\..\..\..\AE-Bin\external\win-x64\llama\lib\ggml-vulkan.dll"
rmdir /Q /S "build"
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64/llama"  -DCPU_TYPE=1 || goto :pauseOnError
cmake --build build --config Release --target "llama" -j 12 || goto :pauseOnError
copy /Y "build\bin\Release\llama.dll" "..\..\..\..\..\AE-Bin\external\win-x64\llama\lib\llama.dll"
copy /Y "build\bin\Release\ggml.dll" "..\..\..\..\..\AE-Bin\external\win-x64\llama\lib\ggml.dll"
copy /Y "build\bin\Release\ggml-base.dll" "..\..\..\..\..\AE-Bin\external\win-x64\llama\lib\ggml-base.dll"
rmdir /Q /S "temp2"
rmdir /Q /S "temp3"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/ggml-org/llama.cpp/releases/download/b8665/cudart-llama-bin-win-cuda-13.1-x64.zip', 'temp.zip')"
powershell Expand-Archive temp.zip -DestinationPath "temp2"
del "temp.zip"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/ggml-org/llama.cpp/releases/download/b8665/llama-b8665-bin-win-cuda-13.1-x64.zip', 'temp.zip')"
powershell Expand-Archive temp.zip -DestinationPath "temp3"
del "temp.zip"
robocopy "temp2" "..\..\..\..\..\AE-Bin\external\win-x64\llama\lib" *.dll /S 
copy /Y "temp3\ggml-cuda.dll" "..\..\..\..\..\AE-Bin\external\win-x64\llama\lib\ggml-cuda.dll"
rmdir /Q /S "temp2"
rmdir /Q /S "temp3"
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
