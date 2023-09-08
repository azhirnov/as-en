rmdir /Q /S "..\..\..\..\..\AE-Data\external\source\Vulkan"
rmdir /Q /S "temp"
git clone --branch "sdk-1.3.243.0" "..\..\..\..\..\3party\Vulkan-Headers" "temp"
robocopy "temp\include" "..\..\..\..\..\AE-Data\external\source\Vulkan" *.h /S 
copy /Y "temp\LICENSE.txt" "..\..\..\..\..\AE-Data\external\source\Vulkan\LICENSE.txt"
rmdir /Q /S "temp"
pause
