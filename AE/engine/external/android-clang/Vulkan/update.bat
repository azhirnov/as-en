
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
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\Vulkan"
rmdir /Q /S "temp"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/KhronosGroup/Vulkan-ValidationLayers/releases/download/vulkan-sdk-1.4.350.1/android-binaries-1.4.350.1.zip', 'temp.zip')"
powershell Expand-Archive temp.zip -DestinationPath "temp"
del "temp.zip"
robocopy "temp\android-binaries-1.4.350.1" "..\..\..\..\..\AE-Bin\external\android-clang\Vulkan" /S 
rmdir /Q /S "temp"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
