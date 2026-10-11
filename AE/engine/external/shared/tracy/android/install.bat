
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
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\tracy"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\tracy\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\tracy\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\libTracyClient.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\tracy\lib\arm64-v8a\libTracyClient.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\tracy\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libTracyClient.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\tracy\lib\armeabi-v7a\libTracyClient.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\tracy\lib\x86"
copy /Y "%BULDDIR%\x86\temp\libTracyClient.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\tracy\lib\x86\libTracyClient.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\tracy\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\libTracyClient.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\tracy\lib\x86_64\libTracyClient.a"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
mkdir "..\..\..\..\..\AE-Bin\external\android-clang\tracy\include"
robocopy "temp\public\client" "..\..\..\..\..\AE-Bin\external\android-clang\tracy\include\client" *.h *.hpp /S 
robocopy "temp\public\common" "..\..\..\..\..\AE-Bin\external\android-clang\tracy\include\common" *.h *.hpp /S 
robocopy "temp\public\tracy" "..\..\..\..\..\AE-Bin\external\android-clang\tracy\include\tracy" *.h *.hpp /S 
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Bin\external\android-clang\tracy\LICENSE"
rmdir /Q /S "temp"

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
