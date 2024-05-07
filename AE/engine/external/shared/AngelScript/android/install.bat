mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\angelscript\libangelscript.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\arm64-v8a\libangelscript.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\libangelscript_stdstring.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\arm64-v8a\libangelscript_stdstring.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\libangelscript_scriptarray.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\arm64-v8a\libangelscript_scriptarray.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\angelscript\libangelscript.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\armeabi-v7a\libangelscript.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libangelscript_stdstring.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\armeabi-v7a\libangelscript_stdstring.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libangelscript_scriptarray.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\armeabi-v7a\libangelscript_scriptarray.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\x86"
copy /Y "%BULDDIR%\x86\temp\angelscript\libangelscript.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\x86\libangelscript.a"
copy /Y "%BULDDIR%\x86\temp\libangelscript_stdstring.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\x86\libangelscript_stdstring.a"
copy /Y "%BULDDIR%\x86\temp\libangelscript_scriptarray.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\x86\libangelscript_scriptarray.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\angelscript\libangelscript.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\x86_64\libangelscript.a"
copy /Y "%BULDDIR%\x86_64\temp\libangelscript_stdstring.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\x86_64\libangelscript_stdstring.a"
copy /Y "%BULDDIR%\x86_64\temp\libangelscript_scriptarray.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\lib\x86_64\libangelscript_scriptarray.a"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
robocopy "temp\angelscript\include" "..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\include" *.h /S 
robocopy "temp\add_on\scriptstdstring" "..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\include" *.h /S 
robocopy "temp\add_on\scriptarray" "..\..\..\..\..\AE-Bin\external\android-clang\AngelScript\include" *.h /S 
rmdir /Q /S "temp"
