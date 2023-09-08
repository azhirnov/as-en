mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\libbrotlicommon-static.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\arm64-v8a\libbrotlicommon-static.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\libbrotlidec-static.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\arm64-v8a\libbrotlidec-static.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\libbrotlienc-static.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\arm64-v8a\libbrotlienc-static.a"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libbrotlicommon-static.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\armeabi-v7a\libbrotlicommon-static.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libbrotlidec-static.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\armeabi-v7a\libbrotlidec-static.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libbrotlienc-static.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\armeabi-v7a\libbrotlienc-static.a"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\x86"
copy /Y "%BULDDIR%\x86\temp\libbrotlicommon-static.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\x86\libbrotlicommon-static.a"
copy /Y "%BULDDIR%\x86\temp\libbrotlidec-static.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\x86\libbrotlidec-static.a"
copy /Y "%BULDDIR%\x86\temp\libbrotlienc-static.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\x86\libbrotlienc-static.a"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\libbrotlicommon-static.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\x86_64\libbrotlicommon-static.a"
copy /Y "%BULDDIR%\x86_64\temp\libbrotlidec-static.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\x86_64\libbrotlidec-static.a"
copy /Y "%BULDDIR%\x86_64\temp\libbrotlienc-static.a" "..\..\..\..\..\..\AE-Data\external\android-clang\Brotli\lib\x86_64\libbrotlienc-static.a"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
robocopy "temp\c\include" "..\..\..\..\..\AE-Data\external\android-clang\Brotli\include" /S 
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Data\external\android-clang\Brotli\LICENSE"
rmdir /Q /S "temp"
