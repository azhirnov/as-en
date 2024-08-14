rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\VMA"
rmdir /Q /S "temp"
git clone --branch "v3.1.0" "..\..\..\..\..\3party\VulkanMemoryAllocator" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\source\VMA"
copy /Y "temp\include\vk_mem_alloc.h" "..\..\..\..\..\AE-Bin\external\source\VMA\vk_mem_alloc.h"
copy /Y "temp\LICENSE.txt" "..\..\..\..\..\AE-Bin\external\source\VMA\LICENSE.txt"
rmdir /Q /S "temp"
pause
