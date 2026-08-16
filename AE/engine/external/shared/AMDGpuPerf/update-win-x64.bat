
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
echo "update 'AMDGpuPerf'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64\AMDGpuPerf"
rmdir /Q /S "temp"
mkdir "build"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/GPUOpen-Tools/gpu_performance_api/releases/download/v4.0-tag/GPUPerfAPI-4.0.0.39.zip', 'temp.zip')"
powershell Expand-Archive temp.zip -DestinationPath "temp"
del "temp.zip"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\AMDGpuPerf"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\AMDGpuPerf\bin"
copy /Y "temp\4_0\bin\GPUPerfAPIVK-x64.dll" "..\..\..\..\..\AE-Bin\external\win-x64\AMDGpuPerf\bin"
copy /Y "temp\4_0\bin\GPUPerfAPICounters-x64.dll" "..\..\..\..\..\AE-Bin\external\win-x64\AMDGpuPerf\bin"
copy /Y "temp\4_0\LICENSE.txt" "..\..\..\..\..\AE-Bin\external\win-x64\AMDGpuPerf"
copy /Y "temp\4_0\NOTICES.txt" "..\..\..\..\..\AE-Bin\external\win-x64\AMDGpuPerf"
robocopy "temp\4_0\include" "..\..\..\..\..\AE-Bin\external\win-x64\AMDGpuPerf\include" /S 
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
