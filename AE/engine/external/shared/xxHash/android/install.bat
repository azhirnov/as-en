rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\xxHash"
rmdir /Q /S "temp"
git clone --branch "v0.8.2" "http://readonly:369@192.168.0.104/Bonobo.Git.Server/xxHash.git" "temp"
copy /Y "temp\cmake_unofficial\CMakeLists.txt" "temp\cmake_unofficial\origin_CMakeLists.txt"
copy /Y "xxHash_CMakeLists.txt" "temp\cmake_unofficial\CMakeLists.txt"
cmake -S temp/cmake_unofficial -B build  -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/android-clang/xxHash" 
cmake --build build --config Release --target install -j 2
rmdir /Q /S "temp"
