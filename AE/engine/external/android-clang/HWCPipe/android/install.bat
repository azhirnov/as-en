mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\hwcpipe\libhwcpipe.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\arm64-v8a\libhwcpipe.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\backend\device\libdevice.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\arm64-v8a\libdevice.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\hwcpipe\libhwcpipe.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\armeabi-v7a\libhwcpipe.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\backend\device\libdevice.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\armeabi-v7a\libdevice.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\x86"
copy /Y "%BULDDIR%\x86\temp\hwcpipe\libhwcpipe.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\x86\libhwcpipe.a"
copy /Y "%BULDDIR%\x86\temp\backend\device\libdevice.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\x86\libdevice.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\hwcpipe\libhwcpipe.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\x86_64\libhwcpipe.a"
copy /Y "%BULDDIR%\x86_64\temp\backend\device\libdevice.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\x86_64\libdevice.a"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
robocopy "temp\hwcpipe\include" "..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\include" /S 
robocopy "temp\backend\device\include" "..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\include" /S 
copy /Y "temp\LICENSE.md" "..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\LICENSE.md"
rmdir /Q /S "temp"
