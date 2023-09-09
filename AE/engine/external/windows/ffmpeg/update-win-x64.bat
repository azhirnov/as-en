rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Data\external\win-x64\ffmpeg"
rmdir /Q /S "temp"
mkdir "build"
git clone "..\..\..\..\..\3party\ffmpeg-win64" "temp"
robocopy "temp\win64-lgpl" "..\..\..\..\..\AE-Data\external\win-x64\ffmpeg" /S 
::robocopy "temp\win64-gpl" "..\..\..\..\..\AE-Data\external\win-x64\ffmpeg" /S 
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause
