rmdir /Q /S "..\..\..\..\..\AE-Data\external\source\VMA"
rmdir /Q /S "temp"
git clone --branch "v2.3.0" "..\..\..\..\..\3party\VulkanMemoryAllocator" "temp"
mkdir "..\..\..\..\..\AE-Data\external\source\VMA"
copy /Y "temp\src\vk_mem_alloc.h" "..\..\..\..\..\AE-Data\external\source\VMA\vk_mem_alloc.h"
copy /Y "temp\LICENSE.txt" "..\..\..\..\..\AE-Data\external\source\VMA\LICENSE.txt"
rmdir /Q /S "temp"
pause
