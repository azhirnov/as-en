rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\lz4"
rmdir /Q /S "temp"
git clone --branch "v1.10.0" "..\..\..\..\..\3party\lz4" "temp"
copy /Y "lz4_CMakeLists.txt" "temp\CMakeLists.txt"
cd "android"
"gradlew.bat" buildRelease
