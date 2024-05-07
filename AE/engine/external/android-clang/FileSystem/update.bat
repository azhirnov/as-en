rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\FileSystem"
rmdir /Q /S "temp"
git clone --branch "v1.5.8" "..\..\..\..\..\3party\filesystem" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\source\FileSystem"
robocopy "temp\include" "..\..\..\..\..\AE-Bin\external\source\FileSystem\include" /S 
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Bin\external\source\FileSystem\LICENSE.txt"
rmdir /Q /S "temp"
pause
