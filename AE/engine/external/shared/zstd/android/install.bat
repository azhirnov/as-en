mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\zstd"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\zstd\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\zstd\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\build\cmake\lib\libzstd.a" "..\..\..\..\..\..\AE-Data\external\android-clang\zstd\lib\arm64-v8a\libzstd.a"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\zstd\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\build\cmake\lib\libzstd.a" "..\..\..\..\..\..\AE-Data\external\android-clang\zstd\lib\armeabi-v7a\libzstd.a"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\zstd\lib\x86"
copy /Y "%BULDDIR%\x86\temp\build\cmake\lib\libzstd.a" "..\..\..\..\..\..\AE-Data\external\android-clang\zstd\lib\x86\libzstd.a"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\zstd\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\build\cmake\lib\libzstd.a" "..\..\..\..\..\..\AE-Data\external\android-clang\zstd\lib\x86_64\libzstd.a"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
mkdir "..\..\..\..\..\AE-Data\external\android-clang\zstd\include"
copy /Y "temp\lib\zstd.h" "..\..\..\..\..\AE-Data\external\android-clang\zstd\include\zstd.h"
copy /Y "temp\lib\zdict.h" "..\..\..\..\..\AE-Data\external\android-clang\zstd\include\zdict.h"
copy /Y "temp\lib\zstd_errors.h" "..\..\..\..\..\AE-Data\external\android-clang\zstd\include\zstd_errors.h"
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Data\external\android-clang\zstd\LICENSE"
rmdir /Q /S "temp"
