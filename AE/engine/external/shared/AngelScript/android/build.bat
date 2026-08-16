
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
echo "update 'AngelScript'..."
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\AngelScript"
rmdir /Q /S "temp"
git clone --branch "v2.38.0" "https://github.com/anjo76/angelscript.git" "temp"
copy /Y "patch\angelscript_CMakeLists.txt" "temp\CMakeLists.txt"
rmdir /Q /S "temp\sdk\add_on\scriptstdstring"
robocopy "patch\scriptstdstring" "temp\sdk\add_on\scriptstdstring" /S 
cd "android"
"gradlew.bat" buildRelease

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
