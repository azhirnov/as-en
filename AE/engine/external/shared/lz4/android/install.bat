
@echo off
rem Enable virtual terminal processing so that ANSI escapes work
for /f "tokens=*" %%i in ('reg query HKCU\Console /v VirtualTerminalLevel ^| findstr /r "[0-9]"') do (
    set vt=%%i
)
if "%vt%"=="VirtualTerminalLevel REG_DWORD 0x1" (
    rem already enabled – nothing to do
) else (
    reg add "HKCU\Console" /v VirtualTerminalLevel /t REG_DWORD /d 1 /f >nul
)

@echo on
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\lz4"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\lz4\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\lz4\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\build\cmake\liblz4.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\lz4\lib\arm64-v8a\liblz4.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\lz4\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\build\cmake\liblz4.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\lz4\lib\armeabi-v7a\liblz4.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\lz4\lib\x86"
copy /Y "%BULDDIR%\x86\temp\build\cmake\liblz4.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\lz4\lib\x86\liblz4.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\lz4\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\build\cmake\liblz4.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\lz4\lib\x86_64\liblz4.a"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
mkdir "..\..\..\..\..\AE-Bin\external\android-clang\lz4\include"
copy /Y "temp\lib\lz4.h" "..\..\..\..\..\AE-Bin\external\android-clang\lz4\include\lz4.h"
copy /Y "temp\lib\lz4hc.h" "..\..\..\..\..\AE-Bin\external\android-clang\lz4\include\lz4hc.h"
copy /Y "temp\lib\lz4frame.h" "..\..\..\..\..\AE-Bin\external\android-clang\lz4\include\lz4frame.h"
copy /Y "temp\lib\LICENSE" "..\..\..\..\..\AE-Bin\external\android-clang\lz4\LICENSE"
rmdir /Q /S "temp"

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
