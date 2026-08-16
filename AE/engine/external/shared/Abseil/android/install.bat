
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

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
