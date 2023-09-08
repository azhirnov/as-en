rmdir /Q /S "..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc"
rmdir /Q /S "temp"
git clone --branch "v2.6.1" "..\..\..\..\..\3party\utf8proc" "temp"
copy /Y "temp\CMakeLists.txt" "temp\origin_CMakeLists.txt"
copy /Y "utf8proc_CMakeLists.txt" "temp\CMakeLists.txt"
cd "android"
"gradlew.bat" buildRelease
