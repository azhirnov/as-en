rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Data\external\win-x64-msvc143\imgui"
rmdir /Q /S "temp"
mkdir "build"
git clone --branch "v1.88" "..\..\..\..\..\3party\imgui" "temp"
copy /Y "imgui_CMakeLists.txt" "temp\CMakeLists.txt"
cmake -S temp -B build -G "Visual Studio 17 2022" -A x64 -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/win-x64-msvc143/imgui" 
cmake --build build --config Release --target install -j 12
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause
