
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
echo "update 'PicoOpenXR'..."
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\PicoOpenXR"
rmdir /Q /S "temp"
mkdir "..\..\..\..\..\AE-Bin\external\android-clang\PicoOpenXR"
robocopy "temp\openxr" "..\..\..\..\..\AE-Bin\external\android-clang\PicoOpenXR\openxr" /S 
robocopy "temp\pico" "..\..\..\..\..\AE-Bin\external\android-clang\PicoOpenXR\pico" /S 
rmdir /Q /S "temp"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
