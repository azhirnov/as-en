
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
echo "update 'OpenXR'..."
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\OpenXR"
rmdir /Q /S "temp"
mkdir "temp"
git clone --branch "release-1.1.50" "https://github.com/KhronosGroup/OpenXR-SDK-Source.git" "temp/OpenXR-SDK-Source"
copy /Y "openxr_CMakeLists.txt" "temp\CMakeLists.txt"
cd "android"
"gradlew.bat" buildRelease

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
