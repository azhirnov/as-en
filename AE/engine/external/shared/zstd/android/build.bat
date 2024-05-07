rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\zstd"
rmdir /Q /S "temp"
git clone --branch "v1.5.5-kernel" "..\..\..\..\..\3party\zstd" "temp"
copy /Y "zstd_CMakeLists.txt" "temp\CMakeLists.txt"
cd "android"
"gradlew.bat" buildRelease
