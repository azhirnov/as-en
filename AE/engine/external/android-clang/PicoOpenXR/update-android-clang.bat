echo "update 'PicoOpenXR'..."
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\PicoOpenXR"
rmdir /Q /S "temp"
git clone "..\..\..\..\..\3party\PicoOpenXR" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\android-clang\PicoOpenXR"
robocopy "temp\openxr" "..\..\..\..\..\AE-Bin\external\android-clang\PicoOpenXR\openxr" /S 
robocopy "temp\pico" "..\..\..\..\..\AE-Bin\external\android-clang\PicoOpenXR\pico" /S 
rmdir /Q /S "temp"
pause
