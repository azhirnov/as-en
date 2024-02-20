rmdir /Q /S "..\..\..\..\..\AE-Data\external\android-clang\lz4"
rmdir /Q /S "temp"
git clone --branch "v1.9.4" "..\..\..\..\..\3party\lz4" "temp"
copy /Y "lz4_CMakeLists.txt" "temp\CMakeLists.txt"
cd "android"
"gradlew.bat" buildRelease
