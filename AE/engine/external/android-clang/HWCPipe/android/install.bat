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
copy /Y "%BULDDIR%\arm64-v8a\temp\libhwcpipe.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\arm64-v8a\libhwcpipe.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libhwcpipe.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\armeabi-v7a\libhwcpipe.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\x86"
copy /Y "%BULDDIR%\x86\temp\libhwcpipe.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\x86\libhwcpipe.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\libhwcpipe.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\lib\x86_64\libhwcpipe.a"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
mkdir "..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\include"
copy /Y "temp\hwcpipe.h" "..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\include\hwcpipe.h"
copy /Y "temp\cpu_profiler.h" "..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\include\cpu_profiler.h"
copy /Y "temp\gpu_profiler.h" "..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\include\gpu_profiler.h"
copy /Y "temp\value.h" "..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\include\value.h"
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe\LICENSE"
rmdir /Q /S "temp"
