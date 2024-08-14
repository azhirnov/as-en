rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe"
rmdir /Q /S "temp"
git clone --branch "2.3.0" "..\..\..\..\..\3party\ARM-HWCPipe" "temp"
cd "android"
"gradlew.bat" buildRelease
