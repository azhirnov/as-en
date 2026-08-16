
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
echo "update 'ffmpeg'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64\ffmpeg"
rmdir /Q /S "temp"
mkdir "build"
mkdir "temp"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-n8.1-latest-win64-lgpl-shared-8.1.zip', 'temp.zip')"
powershell Expand-Archive temp.zip -DestinationPath "temp"
del "temp.zip"
robocopy "temp\ffmpeg-n8.1-latest-win64-lgpl-shared-8.1" "..\..\..\..\..\AE-Bin\external\win-x64\ffmpeg" *.dll /S 
robocopy "temp\ffmpeg-n8.1-latest-win64-lgpl-shared-8.1" "..\..\..\..\..\AE-Bin\external\win-x64\ffmpeg" *.h /S 
copy /Y "temp\ffmpeg-n8.1-latest-win64-lgpl-shared-8.1\LICENSE.txt" "..\..\..\..\..\AE-Bin\external\win-x64\ffmpeg"
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
