mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\libutf8proc.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc\lib\arm64-v8a\libutf8proc.a"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libutf8proc.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc\lib\armeabi-v7a\libutf8proc.a"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc\lib\x86"
copy /Y "%BULDDIR%\x86\temp\libutf8proc.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc\lib\x86\libutf8proc.a"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\libutf8proc.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc\lib\x86_64\libutf8proc.a"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
mkdir "..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc\include"
copy /Y "temp\utf8proc.h" "..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc\include\utf8proc.h"
copy /Y "temp\LICENSE.md" "..\..\..\..\..\AE-Data\external\android-clang\Utf8Proc\LICENSE.md"
rmdir /Q /S "temp"
