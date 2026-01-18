mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\arm64-v8a"
copy /Y "%BULDDIR%\arm64-v8a\temp\OpenXR-SDK-Source\src\api_layers\libXrApiLayer_core_validation.so" "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\arm64-v8a\libXrApiLayer_core_validation.so"
copy /Y "%BULDDIR%\arm64-v8a\temp\OpenXR-SDK-Source\src\api_layers\libopenxr_loader.so" "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\arm64-v8a\libopenxr_loader.so"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\armeabi-v7a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\OpenXR-SDK-Source\src\api_layers\libXrApiLayer_core_validation.so" "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\armeabi-v7a\libXrApiLayer_core_validation.so"
copy /Y "%BULDDIR%\armeabi-v7a\temp\OpenXR-SDK-Source\src\api_layers\libopenxr_loader.so" "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\armeabi-v7a\libopenxr_loader.so"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\x86"
copy /Y "%BULDDIR%\x86\temp\OpenXR-SDK-Source\src\api_layers\libXrApiLayer_core_validation.so" "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\x86\libXrApiLayer_core_validation.so"
copy /Y "%BULDDIR%\x86\temp\OpenXR-SDK-Source\src\api_layers\libopenxr_loader.so" "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\x86\libopenxr_loader.so"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\x86_64"
copy /Y "%BULDDIR%\x86_64\temp\OpenXR-SDK-Source\src\api_layers\libXrApiLayer_core_validation.so" "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\x86_64\libXrApiLayer_core_validation.so"
copy /Y "%BULDDIR%\x86_64\temp\OpenXR-SDK-Source\src\api_layers\libopenxr_loader.so" "..\..\..\..\..\..\AE-Bin\external\android-clang\OpenXR\lib\x86_64\libopenxr_loader.so"
rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
rmdir /Q /S "temp"
