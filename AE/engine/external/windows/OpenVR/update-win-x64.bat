
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
echo "update 'OpenVR'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64\OpenVR"
rmdir /Q /S "temp"
mkdir "build"
git clone --branch "v2.12.14" "https://github.com/ValveSoftware/openvr.git" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\OpenVR"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\OpenVR\bin"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\OpenVR\include"
copy /Y "temp\bin\win64\openvr_api.dll" "..\..\..\..\..\AE-Bin\external\win-x64\OpenVR\bin\openvr_api.dll"
copy /Y "temp\headers\openvr_capi.h" "..\..\..\..\..\AE-Bin\external\win-x64\OpenVR\include\openvr_capi.h"
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
