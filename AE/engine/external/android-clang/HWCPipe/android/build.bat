
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
echo "update 'HWCPipe'..."
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\HWCPipe"
rmdir /Q /S "temp"
git clone --branch "ae-24.12" "https://github.com/azhirnov/libGPUCounters.git" "temp"
cd "android"
"gradlew.bat" buildRelease

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
