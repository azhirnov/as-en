mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\libbrotlicommon.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\arm64-v8a\libbrotlicommon.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\libbrotlidec.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\arm64-v8a\libbrotlidec.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\libbrotlienc.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\arm64-v8a\libbrotlienc.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libbrotlicommon.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\armeabi-v7a\libbrotlicommon.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libbrotlidec.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\armeabi-v7a\libbrotlidec.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libbrotlienc.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\armeabi-v7a\libbrotlienc.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\x86"
copy /Y "%BULDDIR%\x86\temp\libbrotlicommon.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\x86\libbrotlicommon.a"
copy /Y "%BULDDIR%\x86\temp\libbrotlidec.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\x86\libbrotlidec.a"
copy /Y "%BULDDIR%\x86\temp\libbrotlienc.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\x86\libbrotlienc.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\libbrotlicommon.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\x86_64\libbrotlicommon.a"
copy /Y "%BULDDIR%\x86_64\temp\libbrotlidec.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\x86_64\libbrotlidec.a"
copy /Y "%BULDDIR%\x86_64\temp\libbrotlienc.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\Brotli\lib\x86_64\libbrotlienc.a"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
robocopy "temp\c\include" "..\..\..\..\..\AE-Bin\external\android-clang\Brotli\include" /S 
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Bin\external\android-clang\Brotli\LICENSE"
rmdir /Q /S "temp"
