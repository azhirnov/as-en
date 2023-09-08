mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\imgui"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\imgui\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\imgui\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\libimgui.a" "..\..\..\..\..\..\AE-Data\external\android-clang\imgui\lib\arm64-v8a\libimgui.a"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\imgui\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\libimgui.a" "..\..\..\..\..\..\AE-Data\external\android-clang\imgui\lib\armeabi-v7a\libimgui.a"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\imgui\lib\x86"
copy /Y "%BULDDIR%\x86\temp\libimgui.a" "..\..\..\..\..\..\AE-Data\external\android-clang\imgui\lib\x86\libimgui.a"
mkdir "..\..\..\..\..\..\AE-Data\external\android-clang\imgui\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\libimgui.a" "..\..\..\..\..\..\AE-Data\external\android-clang\imgui\lib\x86_64\libimgui.a"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
mkdir "..\..\..\..\..\AE-Data\external\android-clang\imgui\include"
copy /Y "temp\imconfig.h" "..\..\..\..\..\AE-Data\external\android-clang\imgui\include\imconfig.h"
copy /Y "temp\imgui.h" "..\..\..\..\..\AE-Data\external\android-clang\imgui\include\imgui.h"
copy /Y "temp\imgui_internal.h" "..\..\..\..\..\AE-Data\external\android-clang\imgui\include\imgui_internal.h"
copy /Y "temp\imstb_textedit.h" "..\..\..\..\..\AE-Data\external\android-clang\imgui\include\imstb_textedit.h"
copy /Y "temp\LICENSE.txt" "..\..\..\..\..\AE-Data\external\android-clang\imgui\LICENSE.txt"
rmdir /Q /S "temp"
