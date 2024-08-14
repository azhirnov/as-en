rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\Vulkan"
rmdir /Q /S "temp"
git clone --branch "vulkan-sdk-1.3.290.0" "..\..\..\..\..\3party\Vulkan-Headers" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\source\Vulkan"
robocopy "temp\include" "..\..\..\..\..\AE-Bin\external\source\Vulkan" *.h /S 
copy /Y "temp\LICENSE.md" "..\..\..\..\..\AE-Bin\external\source\Vulkan\LICENSE.md"
rmdir /Q /S "temp"
pause
