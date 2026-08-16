
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
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\nvml"
rmdir /Q /S "temp"
mkdir "temp"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://raw.githubusercontent.com/jp7677/dxvk-nvapi/refs/heads/master/inc/nvml/nvml.h', 'temp\nvml.h')"
mkdir "..\..\..\..\..\AE-Bin\external\source\nvml"
copy /Y "temp\nvml.h" "..\..\..\..\..\AE-Bin\external\source\nvml\nvml.h"
rmdir /Q /S "temp"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
