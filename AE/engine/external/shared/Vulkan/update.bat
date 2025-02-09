rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\Vulkan"
rmdir /Q /S "temp"
git clone --branch "v1.4.307" "..\..\..\..\..\3party\Vulkan-Headers" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\source\Vulkan"
robocopy "temp\include" "..\..\..\..\..\AE-Bin\external\source\Vulkan" *.h /S 
copy /Y "temp\LICENSE.md" "..\..\..\..\..\AE-Bin\external\source\Vulkan\LICENSE.md"
rmdir /Q /S "temp"
pause
