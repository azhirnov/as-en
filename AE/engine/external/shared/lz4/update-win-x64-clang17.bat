rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Data\external\win-x64-clang17\lz4"
rmdir /Q /S "temp"
mkdir "build"
git clone --branch "v1.9.4" "..\..\..\..\..\3party\lz4" "temp"
copy /Y "lz4_CMakeLists.txt" "temp\CMakeLists.txt"
cmake -S temp -B build -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/win-x64-clang17/lz4" 
cmake --build build --config Release --target install -j 12
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause
