rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64-clang17\CDT"
rmdir /Q /S "temp"
mkdir "build"
git clone --branch "1.3.0" "..\..\..\..\..\3party\CDT" "temp"
copy /Y "temp\CDT\CMakeLists.txt" "temp\CDT\origin_CMakeLists.txt"
copy /Y "CDT_CMakeLists.txt" "temp\CDT\CMakeLists.txt"
cmake -S temp/CDT -B build -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64-clang17/CDT" 
cmake --build build --config Debug --target install -j 12
cmake -S temp/CDT -B build -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64-clang17/CDT" 
cmake --build build --config Release --target install -j 12
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause
