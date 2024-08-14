rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\imgui"
rmdir /Q /S "temp"
git clone --branch "v1.91.0" "..\..\..\..\..\3party\imgui" "temp"
copy /Y "imgui_CMakeLists.txt" "temp\CMakeLists.txt"
cd "android"
"gradlew.bat" buildRelease
