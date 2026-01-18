echo "update 'OpenXR'..."
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\OpenXR"
rmdir /Q /S "temp"
mkdir "temp"
git clone --branch "release-1.1.50" "https://github.com/KhronosGroup/OpenXR-SDK-Source.git" "temp/OpenXR-SDK-Source"
copy /Y "openxr_CMakeLists.txt" "temp\CMakeLists.txt"
cd "android"
"gradlew.bat" buildRelease
