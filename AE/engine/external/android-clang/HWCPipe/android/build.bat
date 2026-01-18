echo "update 'HWCPipe'..."
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe"
rmdir /Q /S "temp"
git clone --branch "ae-24.12" "https://github.com/azhirnov/libGPUCounters.git" "temp"
cd "android"
"gradlew.bat" buildRelease
