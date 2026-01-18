echo "update 'Brotli'..."
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\Brotli"
rmdir /Q /S "temp"
git clone --branch "v1.2.0" "https://github.com/google/brotli.git" "temp"
copy /Y "temp\CMakeLists.txt" "temp\origin_CMakeLists.txt"
copy /Y "brotli_CMakeLists.txt" "temp\CMakeLists.txt"
cd "android"
"gradlew.bat" buildRelease
