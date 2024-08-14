rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64\AMDGpuPerf"
rmdir /Q /S "temp"
mkdir "build"
git clone "..\..\..\..\..\3party\AMD-GPUPerfAPI" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\AMDGpuPerf"
robocopy "temp\win-x64" "..\..\..\..\..\AE-Bin\external\win-x64\AMDGpuPerf" /S 
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause
