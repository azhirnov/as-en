rmdir /Q /S "..\..\..\..\..\AE-Data\external\source\Vulkan"
rmdir /Q /S "temp"
git clone --branch "vulkan-sdk-1.3.275.0" "..\..\..\..\..\3party\Vulkan-Headers" "temp"
mkdir "..\..\..\..\..\AE-Data\external\source\Vulkan"
robocopy "temp\include" "..\..\..\..\..\AE-Data\external\source\Vulkan" *.h /S 
copy /Y "temp\LICENSE.txt" "..\..\..\..\..\AE-Data\external\source\Vulkan\LICENSE.txt"
rmdir /Q /S "temp"
pause
