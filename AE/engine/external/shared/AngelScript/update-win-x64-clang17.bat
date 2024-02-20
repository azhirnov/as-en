rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Data\external\win-x64-clang17\AngelScript"
rmdir /Q /S "temp"
mkdir "build"
git clone "..\..\..\..\..\3party\AngelScript" "temp"
copy /Y "patch\angelscript_CMakeLists.txt" "temp\CMakeLists.txt"
copy /Y "patch\scriptarray.h" "temp\add_on\scriptarray\scriptarray.h"
rmdir /Q /S "temp\add_on\scriptstdstring"
robocopy "patch\scriptstdstring" "temp\add_on\scriptstdstring" /S 
cmake -S temp -B build -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/win-x64-clang17/AngelScript" 
cmake --build build --config Debug --target install -j 12
cmake -S temp -B build -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/win-x64-clang17/AngelScript" 
cmake --build build --config Release --target install -j 12
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause
