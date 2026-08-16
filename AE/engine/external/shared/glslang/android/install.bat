
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
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\glslang\libGenericCodeGen.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\arm64-v8a\libGenericCodeGen.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\glslang\libglslang.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\arm64-v8a\libglslang.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\glslang\libMachineIndependent.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\arm64-v8a\libMachineIndependent.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\glslang\OSDependent\Unix\libOSDependent.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\arm64-v8a\libOSDependent.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\SPIRV\libSPIRV.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\arm64-v8a\libSPIRV.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\External\spirv-tools\source\libSPIRV-Tools.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\arm64-v8a\libSPIRV-Tools.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\External\spirv-tools\source\link\libSPIRV-Tools-link.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\arm64-v8a\libSPIRV-Tools-link.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\External\spirv-tools\source\lint\libSPIRV-Tools-lint.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\arm64-v8a\libSPIRV-Tools-lint.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\External\spirv-tools\source\opt\libSPIRV-Tools-opt.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\arm64-v8a\libSPIRV-Tools-opt.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\External\spirv-tools\source\reduce\libSPIRV-Tools-reduce.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\arm64-v8a\libSPIRV-Tools-reduce.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang"
copy /Y "%BULDDIR%\arm64-v8a\temp\..\include\glslang\build_info.h" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang\build_info.h"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\glslang\libGenericCodeGen.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\armeabi-v7a\libGenericCodeGen.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\glslang\libglslang.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\armeabi-v7a\libglslang.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\glslang\libMachineIndependent.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\armeabi-v7a\libMachineIndependent.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\glslang\OSDependent\Unix\libOSDependent.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\armeabi-v7a\libOSDependent.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\SPIRV\libSPIRV.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\armeabi-v7a\libSPIRV.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\External\spirv-tools\source\libSPIRV-Tools.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\armeabi-v7a\libSPIRV-Tools.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\External\spirv-tools\source\link\libSPIRV-Tools-link.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\armeabi-v7a\libSPIRV-Tools-link.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\External\spirv-tools\source\lint\libSPIRV-Tools-lint.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\armeabi-v7a\libSPIRV-Tools-lint.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\External\spirv-tools\source\opt\libSPIRV-Tools-opt.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\armeabi-v7a\libSPIRV-Tools-opt.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\External\spirv-tools\source\reduce\libSPIRV-Tools-reduce.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\armeabi-v7a\libSPIRV-Tools-reduce.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang"
copy /Y "%BULDDIR%\armeabi-v7a\temp\..\include\glslang\build_info.h" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang\build_info.h"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86"
copy /Y "%BULDDIR%\x86\temp\glslang\libGenericCodeGen.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86\libGenericCodeGen.a"
copy /Y "%BULDDIR%\x86\temp\glslang\libglslang.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86\libglslang.a"
copy /Y "%BULDDIR%\x86\temp\glslang\libMachineIndependent.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86\libMachineIndependent.a"
copy /Y "%BULDDIR%\x86\temp\glslang\OSDependent\Unix\libOSDependent.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86\libOSDependent.a"
copy /Y "%BULDDIR%\x86\temp\SPIRV\libSPIRV.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86\libSPIRV.a"
copy /Y "%BULDDIR%\x86\temp\External\spirv-tools\source\libSPIRV-Tools.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86\libSPIRV-Tools.a"
copy /Y "%BULDDIR%\x86\temp\External\spirv-tools\source\link\libSPIRV-Tools-link.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86\libSPIRV-Tools-link.a"
copy /Y "%BULDDIR%\x86\temp\External\spirv-tools\source\lint\libSPIRV-Tools-lint.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86\libSPIRV-Tools-lint.a"
copy /Y "%BULDDIR%\x86\temp\External\spirv-tools\source\opt\libSPIRV-Tools-opt.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86\libSPIRV-Tools-opt.a"
copy /Y "%BULDDIR%\x86\temp\External\spirv-tools\source\reduce\libSPIRV-Tools-reduce.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86\libSPIRV-Tools-reduce.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang"
copy /Y "%BULDDIR%\x86\temp\..\include\glslang\build_info.h" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang\build_info.h"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\glslang\libGenericCodeGen.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86_64\libGenericCodeGen.a"
copy /Y "%BULDDIR%\x86_64\temp\glslang\libglslang.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86_64\libglslang.a"
copy /Y "%BULDDIR%\x86_64\temp\glslang\libMachineIndependent.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86_64\libMachineIndependent.a"
copy /Y "%BULDDIR%\x86_64\temp\glslang\OSDependent\Unix\libOSDependent.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86_64\libOSDependent.a"
copy /Y "%BULDDIR%\x86_64\temp\SPIRV\libSPIRV.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86_64\libSPIRV.a"
copy /Y "%BULDDIR%\x86_64\temp\External\spirv-tools\source\libSPIRV-Tools.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86_64\libSPIRV-Tools.a"
copy /Y "%BULDDIR%\x86_64\temp\External\spirv-tools\source\link\libSPIRV-Tools-link.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86_64\libSPIRV-Tools-link.a"
copy /Y "%BULDDIR%\x86_64\temp\External\spirv-tools\source\lint\libSPIRV-Tools-lint.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86_64\libSPIRV-Tools-lint.a"
copy /Y "%BULDDIR%\x86_64\temp\External\spirv-tools\source\opt\libSPIRV-Tools-opt.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86_64\libSPIRV-Tools-opt.a"
copy /Y "%BULDDIR%\x86_64\temp\External\spirv-tools\source\reduce\libSPIRV-Tools-reduce.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\lib\x86_64\libSPIRV-Tools-reduce.a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang"
copy /Y "%BULDDIR%\x86_64\temp\..\include\glslang\build_info.h" "..\..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang\build_info.h"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
mkdir "..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang"
robocopy "temp\glslang\Include" "..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang\Include" /S 
robocopy "temp\glslang\MachineIndependent" "..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang\MachineIndependent" /S 
robocopy "temp\glslang\OSDependent" "..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang\OSDependent" /S 
robocopy "temp\glslang\Public" "..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang\Public" /S 
robocopy "temp\SPIRV" "..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\glslang\SPIRV" *.h /S 
robocopy "temp\External\spirv-tools\include\spirv-tools" "..\..\..\..\..\AE-Bin\external\android-clang\glslang\include\spirv-tools" /S 
copy /Y "temp\SPIRV\spirv.hpp11" "..\..\..\..\..\AE-Bin\external\android-clang\glslang\SPIRV\spirv.hpp11"
copy /Y "temp\LICENSE.txt" "..\..\..\..\..\AE-Bin\external\android-clang\glslang\glslang-LICENSE.txt"
copy /Y "temp\External\spirv-tools\LICENSE" "..\..\..\..\..\AE-Bin\external\android-clang\glslang\SPIRV-Headers-LICENSE.txt"
copy /Y "temp\External\spirv-tools\external\spirv-headers\LICENSE" "..\..\..\..\..\AE-Bin\external\android-clang\glslang\SPIRV-Tools-LICENSE.txt"
rmdir /Q /S "temp"

exit /b 0

:pauseOnError
echo ^[[31m"*** failed ***"^[[0m
pause
exit /b 1
