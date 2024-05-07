mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib"

cd "app\.cxx\RelWithDebInfo"
for /f "delims=" %%a in ('dir /s /b') do (
 set BULDDIR=%%a
 goto copylibs
)
:copylibs
cd "../../.."
echo %BULDDIR%
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\arm64-v8a"
mkdir "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\armeabi-v7a"

copy /Y "%BULDDIR%\arm64-v8a\temp\lib\gpu\mali\libhpc-gpu-mali-bifrost.a"      "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\arm64-v8a\libhpc-gpu-mali-bifrost.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\lib\gpu\mali\libhpc-gpu-mali-valhall.a"      "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\arm64-v8a\libhpc-gpu-mali-valhall.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\lib\gpu\mali\libhpc-gpu-mali-context.a"      "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\arm64-v8a\libhpc-gpu-mali-context.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\lib\gpu\mali\libhpc-gpu-mali-driver-ioctl.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\arm64-v8a\libhpc-gpu-mali-driver-ioctl.a"

copy /Y "%BULDDIR%\arm64-v8a\temp\lib\gpu\adreno\libhpc-gpu-adreno-a5xx.a"         "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\arm64-v8a\libhpc-gpu-adreno-a5xx.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\lib\gpu\adreno\libhpc-gpu-adreno-a6xx.a"         "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\arm64-v8a\libhpc-gpu-adreno-a6xx.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\lib\gpu\adreno\libhpc-gpu-adreno-context.a"      "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\arm64-v8a\libhpc-gpu-adreno-context.a"
copy /Y "%BULDDIR%\arm64-v8a\temp\lib\gpu\adreno\libhpc-gpu-adreno-driver-ioctl.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\arm64-v8a\libhpc-gpu-adreno-driver-ioctl.a"

copy /Y "%BULDDIR%\armeabi-v7a\temp\lib\gpu\mali\libhpc-gpu-mali-bifrost.a"      "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\armeabi-v7a\libhpc-gpu-mali-bifrost.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\lib\gpu\mali\libhpc-gpu-mali-valhall.a"      "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\armeabi-v7a\libhpc-gpu-mali-valhall.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\lib\gpu\mali\libhpc-gpu-mali-context.a"      "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\armeabi-v7a\libhpc-gpu-mali-context.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\lib\gpu\mali\libhpc-gpu-mali-driver-ioctl.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\armeabi-v7a\libhpc-gpu-mali-driver-ioctl.a"

copy /Y "%BULDDIR%\armeabi-v7a\temp\lib\gpu\adreno\libhpc-gpu-adreno-a5xx.a"         "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\armeabi-v7a\libhpc-gpu-adreno-a5xx.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\lib\gpu\adreno\libhpc-gpu-adreno-a6xx.a"         "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\armeabi-v7a\libhpc-gpu-adreno-a6xx.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\lib\gpu\adreno\libhpc-gpu-adreno-context.a"      "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\armeabi-v7a\libhpc-gpu-adreno-context.a"
copy /Y "%BULDDIR%\armeabi-v7a\temp\lib\gpu\adreno\libhpc-gpu-adreno-driver-ioctl.a" "..\..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\lib\armeabi-v7a\libhpc-gpu-adreno-driver-ioctl.a"

rmdir /Q /S ".gradle"
rmdir /Q /S "app\.cxx"
rmdir /Q /S "app\build"
cd ".."
robocopy "temp\include" "..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\include" /S 
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Bin\external\android-clang\HWPerfCounter\LICENSE"
rmdir /Q /S "temp"
