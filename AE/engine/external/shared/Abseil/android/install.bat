mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\Abseil"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\Abseil\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\Abseil\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\libAbseil.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Abseil\lib\arm64-v8a\libAbseil.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\Abseil\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libAbseil.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Abseil\lib\armeabi-v7a\libAbseil.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\Abseil\lib\x86"
copy /Y "%BULDDIR%\x86\temp\libAbseil.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Abseil\lib\x86\libAbseil.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\Abseil\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\libAbseil.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Abseil\lib\x86_64\libAbseil.a"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
robocopy "temp\absl" "..\..\..\..\..\AE-Bin\external\android-clang\Abseil\absl" *.h *.inc /S 
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Bin\external\android-clang\Abseil\LICENSE"
rmdir /Q /S "temp"
