echo "update 'Abseil'..."
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\Abseil"
rmdir /Q /S "temp"
git clone --branch "20250127.1" "https://github.com/abseil/abseil-cpp.git" "temp"
copy /Y "abseil_CMakeLists.txt" "temp\CMakeLists.txt"
cd "android"
"gradlew.bat" buildRelease
