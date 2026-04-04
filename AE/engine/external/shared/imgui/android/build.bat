echo "update 'imgui'..."
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\imgui"
rmdir /Q /S "temp"
git clone --branch "v1.92.6" "https://github.com/ocornut/imgui.git" "temp"
copy /Y "imgui_CMakeLists.txt" "temp\CMakeLists.txt"
cd "android"
"gradlew.bat" buildRelease
