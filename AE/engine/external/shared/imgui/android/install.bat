
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
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\imgui"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\imgui\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\imgui\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\libimgui.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\imgui\lib\arm64-v8a\libimgui.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\imgui\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libimgui.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\imgui\lib\armeabi-v7a\libimgui.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\imgui\lib\x86"
copy /Y "%BULDDIR%\x86\temp\libimgui.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\imgui\lib\x86\libimgui.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\imgui\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\libimgui.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\imgui\lib\x86_64\libimgui.a"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
mkdir "..\..\..\..\..\AE-Bin\external\android-clang\imgui\include"
copy /Y "temp\imconfig.h" "..\..\..\..\..\AE-Bin\external\android-clang\imgui\include\imconfig.h"
copy /Y "temp\imgui.h" "..\..\..\..\..\AE-Bin\external\android-clang\imgui\include\imgui.h"
copy /Y "temp\imgui_internal.h" "..\..\..\..\..\AE-Bin\external\android-clang\imgui\include\imgui_internal.h"
copy /Y "temp\imstb_textedit.h" "..\..\..\..\..\AE-Bin\external\android-clang\imgui\include\imstb_textedit.h"
copy /Y "temp\LICENSE.txt" "..\..\..\..\..\AE-Bin\external\android-clang\imgui\LICENSE.txt"
rmdir /Q /S "temp"

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
