rmdir /Q /S "..\..\..\..\..\AE-Bin\external\android-clang\Vulkan"
rmdir /Q /S "temp"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://github.com/KhronosGroup/Vulkan-ValidationLayers/releases/download/vulkan-sdk-1.3.290.0/android-binaries-1.3.290.0.zip', 'temp.zip')"
powershell Expand-Archive temp.zip -DestinationPath "temp"
del "temp.zip"
robocopy "temp\android-binaries-1.3.290.0" "..\..\..\..\..\AE-Bin\external\android-clang\Vulkan" /S 
rmdir /Q /S "temp"
pause
