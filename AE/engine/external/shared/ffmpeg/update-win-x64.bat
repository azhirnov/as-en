echo "update 'ffmpeg'..."
rmdir /Q /S "build"
rmdir /Q /S "..\..\..\..\..\AE-Bin\external\win-x64\ffmpeg"
rmdir /Q /S "temp"
mkdir "build"
mkdir "temp"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/BtbN/FFmpeg-Builds/releases/download/autobuild-2026-03-20-13-06/ffmpeg-n8.0.1-76-gfa4ee7ab3c-win64-lgpl-shared-8.0.zip', 'temp.zip')"
powershell Expand-Archive temp.zip -DestinationPath "temp"
del "temp.zip"
robocopy "temp\ffmpeg-n8.0.1-76-gfa4ee7ab3c-win64-lgpl-shared-8.0" "..\..\..\..\..\AE-Bin\external\win-x64\ffmpeg" *.dll /S 
robocopy "temp\ffmpeg-n8.0.1-76-gfa4ee7ab3c-win64-lgpl-shared-8.0" "..\..\..\..\..\AE-Bin\external\win-x64\ffmpeg" *.h /S 
copy /Y "temp\ffmpeg-n8.0.1-76-gfa4ee7ab3c-win64-lgpl-shared-8.0\LICENSE.txt" "..\..\..\..\..\AE-Bin\external\win-x64\ffmpeg"
rmdir /Q /S "temp"
rmdir /Q /S "build"
pause
