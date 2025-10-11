echo "update 'OpenXR'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64-clang19\OpenXR"
rmdir /Q /S "temp"
mkdir "build"
mkdir "temp"
git clone --branch "release-1.1.50" "..\..\..\..\..\3party\OpenXR-SDK-Source" "temp/OpenXR-SDK-Source"
copy /Y "openxr_CMakeLists.txt" "temp\CMakeLists.txt"
cmake -S temp -B build -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/win-x64-clang19/OpenXR" 
cmake --build build --config Release --target install -j 12
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause
