rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\AngelScript"
rmdir /Q /S "temp"
git clone "..\..\..\..\..\3party\AngelScript" "temp"
copy /Y "patch\angelscript_CMakeLists.txt" "temp\CMakeLists.txt"
copy /Y "patch\scriptarray.h" "temp\add_on\scriptarray\scriptarray.h"
rmdir /Q /S "temp\add_on\scriptstdstring"
robocopy "patch\scriptstdstring" "temp\add_on\scriptstdstring" /S 
cd "android"
"gradlew.bat" buildRelease
