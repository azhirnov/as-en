rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Data\external\win-x64-msvc143\glslang"
rmdir /Q /S "temp"
mkdir "build"
git clone --branch "sdk-1.3.243.0" "..\..\..\..\..\3party\glslang" "temp"
git clone --branch "sdk-1.3.243.0" "..\..\..\..\..\3party\SPIRV-Tools" "temp/External/spirv-tools"
git clone --branch "sdk-1.3.243.0" "..\..\..\..\..\3party\SPIRV-Headers" "temp/External/spirv-tools/external/spirv-headers"
copy /Y "temp\CMakeLists.txt" "temp\origin_CMakeLists.txt"
copy /Y "glslang_CMakeLists.txt" "temp\CMakeLists.txt"
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/win-x64-msvc143/glslang" 
cmake --build build --config Debug --target install -j 12
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/win-x64-msvc143/glslang" 
cmake --build build --config Release --target install -j 12
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause
