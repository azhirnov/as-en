rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64-clang17\Compressonator"
rmdir /Q /S "temp"
mkdir "build"
git clone "..\..\..\..\..\3party\AMD-Compressonator" "temp"
copy /Y "temp\CMakeLists.txt" "temp\origin_CMakeLists.txt"
copy /Y "Compressonator_CMakeLists.txt" "temp\CMakeLists.txt"
cmake -S temp -B build -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64-clang17/Compressonator" 
cmake --build build --config Debug --target "CMP_Compressonator" -j 12
robocopy "build\lib\Debug" "..\..\..\..\..\AE-Bin\external\win-x64-clang17\Compressonator\lib" *.lib /S 
cmake -S temp -B build -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64-clang17/Compressonator" 
cmake --build build --config Release --target "CMP_Compressonator" -j 12
robocopy "build\lib\Release" "..\..\..\..\..\AE-Bin\external\win-x64-clang17\Compressonator\lib" *.lib /S 
mkdir "..\..\..\..\..\AE-Bin\external\win-x64-clang17\Compressonator\include"
copy /Y "temp\cmp_compressonatorlib\compressonator.h" "..\..\..\..\..\AE-Bin\external\win-x64-clang17\Compressonator\include\compressonator.h"
robocopy "temp\license" "..\..\..\..\..\AE-Bin\external\win-x64-clang17\Compressonator\license" /S 
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause
