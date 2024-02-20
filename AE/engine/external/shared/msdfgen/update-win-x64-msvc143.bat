rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Data\external\win-x64-msvc143\msdfgen"
rmdir /Q /S "temp"
mkdir "build"
git clone --branch "v1.9.2" "..\..\..\..\..\3party\msdfgen" "temp"
copy /Y "origin_CMakeLists.txt" "temp\origin_CMakeLists.txt"
copy /Y "msdfgen_CMakeLists.txt" "temp\CMakeLists.txt"
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/win-x64-msvc143/msdfgen" 
cmake --build build --config Debug --target install -j 12
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/win-x64-msvc143/msdfgen" 
cmake --build build --config Release --target install -j 12
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause
