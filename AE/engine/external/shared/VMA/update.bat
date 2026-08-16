
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
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\VMA"
rmdir /Q /S "temp"
git clone --branch "v3.3.0" "https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\source\VMA"
copy /Y "temp\include\vk_mem_alloc.h" "..\..\..\..\..\AE-Bin\external\source\VMA\vk_mem_alloc.h"
copy /Y "temp\LICENSE.txt" "..\..\..\..\..\AE-Bin\external\source\VMA\LICENSE.txt"
rmdir /Q /S "temp"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
