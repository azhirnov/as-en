rmdir /Q /S "..\..\..\..\..\AE-Data\external\android-clang\AngelScript"
rmdir /Q /S "temp"
git clone "..\..\..\..\..\3party\AngelScript" "temp"
copy /Y "patch\angelscript_CMakeLists.txt" "temp\CMakeLists.txt"
rmdir /Q /S "temp\add_on\scriptstdstring"
robocopy "patch\scriptstdstring" "temp\add_on\scriptstdstring" /S 
cd "android"
"gradlew.bat" buildRelease
