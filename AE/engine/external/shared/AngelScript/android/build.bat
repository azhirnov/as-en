echo "update 'AngelScript'..."
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\AngelScript"
rmdir /Q /S "temp"
git clone --branch "v2.38.0" "https://github.com/anjo76/angelscript.git" "temp"
copy /Y "patch\angelscript_CMakeLists.txt" "temp\CMakeLists.txt"
rmdir /Q /S "temp\sdk\add_on\scriptstdstring"
robocopy "patch\scriptstdstring" "temp\sdk\add_on\scriptstdstring" /S 
cd "android"
"gradlew.bat" buildRelease
