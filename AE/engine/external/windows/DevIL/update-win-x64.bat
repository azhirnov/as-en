
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
echo "update 'DevIL'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64\DevIL"
rmdir /Q /S "temp"
mkdir "build"
mkdir "temp"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://sourceforge.net/projects/openil/files/DevIL Win32 and Win64/DevIL-EndUser-x64-1.8.0.zip/download', 'temp.zip')"
powershell Expand-Archive temp.zip -DestinationPath "temp\win-x64"
del "temp.zip"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://sourceforge.net/projects/openil/files/DevIL/1.8.0/DevIL-1.8.0.zip/download', 'temp.zip')"
powershell Expand-Archive temp.zip -DestinationPath "temp\src"
del "temp.zip"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\DevIL"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\DevIL\bin"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\DevIL\include\IL"
copy /Y "temp\src\DevIL\DevIL\include\IL\il.h" "..\..\..\..\..\AE-Bin\external\win-x64\DevIL\include\IL\il.h"
copy /Y "temp\src\DevIL\DevIL\include\IL\ilu.h" "..\..\..\..\..\AE-Bin\external\win-x64\DevIL\include\IL\ilu.h"
copy /Y "temp\win-x64\DevIL.dll" "..\..\..\..\..\AE-Bin\external\win-x64\DevIL\bin\DevIL.dll"
copy /Y "temp\win-x64\ILU.dll" "..\..\..\..\..\AE-Bin\external\win-x64\DevIL\bin\ILU.dll"
copy /Y "temp\src\LICENSE" "..\..\..\..\..\AE-Bin\external\win-x64\DevIL"
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
