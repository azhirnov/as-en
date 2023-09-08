rmdir /Q /S "..\..\..\..\..\AE-Data\external\android-clang\HWCPipe"
rmdir /Q /S "temp"
git clone "..\..\..\..\..\3party\ARM-HWCPipe" "temp"
cd "android"
"gradlew.bat" buildRelease
