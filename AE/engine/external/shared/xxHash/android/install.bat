rmdir /Q /S "..\..\..\..\..\AE-Data\external\android-clang\xxHash"
rmdir /Q /S "temp"
git clone --branch "v0.8.2" "..\..\..\..\..\3party\xxHash" "temp"
copy /Y "temp\cmake_unofficial\CMakeLists.txt" "temp\cmake_unofficial\origin_CMakeLists.txt"
copy /Y "xxHash_CMakeLists.txt" "temp\cmake_unofficial\CMakeLists.txt"
cmake -S temp/cmake_unofficial -B build  -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="../../../../../AE-Data/external/android-clang/xxHash" 
cmake --build build --config Release --target install -j 2
rmdir /Q /S "temp"
