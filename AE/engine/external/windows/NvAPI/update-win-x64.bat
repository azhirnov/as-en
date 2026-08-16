
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
echo "update 'NvAPI'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64\NvAPI"
rmdir /Q /S "temp"
mkdir "build"
git clone "https://github.com/NVIDIA/nvapi.git" "temp"
cd "temp"
git checkout "3d34a4faf095996663321646ebe003539a908f89"
cd ".."
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\NvAPI\lib"
robocopy "temp" "..\..\..\..\..\AE-Bin\external\win-x64\NvAPI\include" *.h /S 
copy /Y "temp\License.txt" "..\..\..\..\..\AE-Bin\external\win-x64\NvAPI\License.txt"
copy /Y "temp\amd64\nvapi64.lib" "..\..\..\..\..\AE-Bin\external\win-x64\NvAPI\lib\nvapi64.lib"
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
