rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\Vulkan"
rmdir /Q /S "temp"
git clone --branch "vulkan-sdk-1.4.321.0" "https://github.com/KhronosGroup/Vulkan-Headers.git" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\source\Vulkan"
robocopy "temp\include" "..\..\..\..\..\AE-Bin\external\source\Vulkan" *.h /S 
copy /Y "temp\LICENSE.md" "..\..\..\..\..\AE-Bin\external\source\Vulkan\LICENSE.md"
rmdir /Q /S "temp"
pause
