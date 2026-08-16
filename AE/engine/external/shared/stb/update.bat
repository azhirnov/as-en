
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
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\stb"
rmdir /Q /S "temp"
git clone "https://github.com/nothings/stb.git" "temp"
cd "temp"
git checkout "f1c79c02822848a9bed4315b12c8c8f3761e1296"
cd ".."
mkdir "..\..\..\..\..\AE-Bin\external\source\stb"
copy /Y "temp\stb_image.h" "..\..\..\..\..\AE-Bin\external\source\stb\stb_image.h"
copy /Y "temp\stb_image_write.h" "..\..\..\..\..\AE-Bin\external\source\stb\stb_image_write.h"
copy /Y "temp\stb_rect_pack.h" "..\..\..\..\..\AE-Bin\external\source\stb\stb_rect_pack.h"
copy /Y "temp\stb_truetype.h" "..\..\..\..\..\AE-Bin\external\source\stb\stb_truetype.h"
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Bin\external\source\stb\LICENSE"
rmdir /Q /S "temp"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
