rmdir /Q /S "..\..\..\..\..\AE-Data\external\android-clang\imgui"
rmdir /Q /S "temp"
git clone --branch "v1.88" "..\..\..\..\..\3party\imgui" "temp"
copy /Y "imgui_CMakeLists.txt" "temp\CMakeLists.txt"
cd "android"
"gradlew.bat" buildRelease
