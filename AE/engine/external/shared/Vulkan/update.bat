
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
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\Vulkan"
rmdir /Q /S "temp"
git clone --branch "vulkan-sdk-1.4.350.1" "https://github.com/KhronosGroup/Vulkan-Headers.git" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\source\Vulkan"
robocopy "temp\include" "..\..\..\..\..\AE-Bin\external\source\Vulkan" *.h /S 
copy /Y "temp\LICENSE.md" "..\..\..\..\..\AE-Bin\external\source\Vulkan\LICENSE.md"
rmdir /Q /S "temp"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
