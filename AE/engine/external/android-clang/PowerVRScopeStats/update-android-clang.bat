
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
echo "update 'PowerVRScopeStats'..."
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\PowerVRScopeStats"
rmdir /Q /S "temp"
git clone --branch "R25.2-v5.15" "https://github.com/powervr-graphics/Native_SDK.git" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\android-clang\PowerVRScopeStats"
mkdir "..\..\..\..\..\AE-Bin\external\android-clang\PowerVRScopeStats\include"
mkdir "..\..\..\..\..\AE-Bin\external\android-clang\PowerVRScopeStats\lib"
mkdir "..\..\..\..\..\AE-Bin\external\android-clang\PowerVRScopeStats\lib\arm64-v8a"
mkdir "..\..\..\..\..\AE-Bin\external\android-clang\PowerVRScopeStats\lib\armeabi-v7a"
copy /Y "temp\lib\Android_arm64-v8a\libPVRScopeDeveloper.a" "..\..\..\..\..\AE-Bin\external\android-clang\PowerVRScopeStats\lib\arm64-v8a\libPVRScopeDeveloper.a"
copy /Y "temp\lib\Android_armeabi-v7a\libPVRScopeDeveloper.a" "..\..\..\..\..\AE-Bin\external\android-clang\PowerVRScopeStats\lib\armeabi-v7a\libPVRScopeDeveloper.a"
copy /Y "temp\include\PVRScopeStats.h" "..\..\..\..\..\AE-Bin\external\android-clang\PowerVRScopeStats\include\PVRScopeStats.h"
copy /Y "temp\include\PVRScopeComms.h" "..\..\..\..\..\AE-Bin\external\android-clang\PowerVRScopeStats\include\PVRScopeComms.h"
rmdir /Q /S "temp"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
