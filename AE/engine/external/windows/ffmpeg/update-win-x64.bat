echo "update 'ffmpeg'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64\ffmpeg"
rmdir /Q /S "temp"
mkdir "build"
mkdir "temp"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/BtbN/FFmpeg-Builds/releases/download/autobuild-2025-08-31-13-00/ffmpeg-n6.1.3-win64-lgpl-shared-6.1.zip', 'temp.zip')"
powershell Expand-Archive temp.zip -DestinationPath "temp"
del "temp.zip"
robocopy "temp\ffmpeg-n6.1.3-win64-lgpl-shared-6.1" "..\..\..\..\..\AE-Bin\external\win-x64\ffmpeg" *.dll /S 
robocopy "temp\ffmpeg-n6.1.3-win64-lgpl-shared-6.1" "..\..\..\..\..\AE-Bin\external\win-x64\ffmpeg" *.h /S 
copy /Y "temp\ffmpeg-n6.1.3-win64-lgpl-shared-6.1\LICENSE.txt" "..\..\..\..\..\AE-Bin\external\win-x64\ffmpeg"
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause
