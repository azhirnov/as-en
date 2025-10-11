mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\build\cmake\libgit2.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\arm64-v8a\libgit2.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\build\cmake\libgit2.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\armeabi-v7a\libgit2.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\x86"
copy /Y "%BULDDIR%\x86\temp\build\cmake\libgit2.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\x86\libgit2.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\build\cmake\libgit2.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\x86_64\libgit2.a"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
rmdir /Q /S "temp"
