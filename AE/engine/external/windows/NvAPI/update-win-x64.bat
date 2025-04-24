echo "update 'NvAPI'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64\NvAPI"
rmdir /Q /S "temp"
mkdir "build"
git clone "..\..\..\..\..\3party\nvapi" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\win-x64\NvAPI\lib"
robocopy "temp" "..\..\..\..\..\AE-Bin\external\win-x64\NvAPI\include" *.h /S 
copy /Y "temp\License.txt" "..\..\..\..\..\AE-Bin\external\win-x64\NvAPI\License.txt"
copy /Y "temp\amd64\nvapi64.lib" "..\..\..\..\..\AE-Bin\external\win-x64\NvAPI\lib\nvapi64.lib"
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause
