rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\Vulkan"
rmdir /Q /S "temp"
git clone --branch "vulkan-sdk-1.3.280.0" "..\..\..\..\..\3party\Vulkan-Headers" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\source\Vulkan"
robocopy "temp\include" "..\..\..\..\..\AE-Bin\external\source\Vulkan" *.h /S 
copy /Y "temp\LICENSE.txt" "..\..\..\..\..\AE-Bin\external\source\Vulkan\LICENSE.txt"
rmdir /Q /S "temp"
pause
