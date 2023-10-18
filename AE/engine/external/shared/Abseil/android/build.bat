rmdir /Q /S "..\..\..\..\..\AE-Data\external\android-clang\Abseil"
rmdir /Q /S "temp"
git clone --branch "20230802.0" "..\..\..\..\..\3party\Abseil" "temp"
copy /Y "abseil_CMakeLists.txt" "temp\CMakeLists.txt"
cd "android"
"gradlew.bat" buildRelease
