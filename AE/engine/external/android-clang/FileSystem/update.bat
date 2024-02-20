rmdir /Q /S "..\..\..\..\..\AE-Data\external\source\FileSystem"
rmdir /Q /S "temp"
git clone --branch "v1.5.8" "..\..\..\..\..\3party\filesystem" "temp"
mkdir "..\..\..\..\..\AE-Data\external\source\FileSystem"
robocopy "temp\include" "..\..\..\..\..\AE-Data\external\source\FileSystem\include" /S 
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Data\external\source\FileSystem\LICENSE.txt"
rmdir /Q /S "temp"
pause
