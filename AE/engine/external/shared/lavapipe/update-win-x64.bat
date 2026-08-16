
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
echo "update 'lavapipe'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64\lavapipe"
rmdir /Q /S "temp"
mkdir "build"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/pal1000/mesa-dist-win/releases/download/25.3.0/mesa3d-25.3.0-release-msvc.7z', 'temp.7z')"
"C:\Program Files\7-Zip\7z.exe" x -o"temp\lavapipe" temp.7z
del "temp.7z"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://sdk.lunarg.com/sdk/download/1.4.350.1/windows/VulkanRT-1.4.350.1-Components.zip', 'temp.7z')"
"C:\Program Files\7-Zip\7z.exe" x -o"temp\vk-rt" temp.7z
del "temp.7z"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\lavapipe"
copy /Y "temp\lavapipe\x64\lvp_icd.x86_64.json" "..\..\..\..\..\AE-Bin\external\win-x64\lavapipe\lvp_icd.x86_64.json"
copy /Y "temp\lavapipe\x64\vulkan_lvp.dll" "..\..\..\..\..\AE-Bin\external\win-x64\lavapipe\vulkan_lvp.dll"
copy /Y "temp\vk-rt\VulkanRT-1.4.350.1-Components\x64\vulkan-1.dll" "..\..\..\..\..\AE-Bin\external\win-x64\lavapipe\vulkan-1.dll"
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
