rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\stb"
rmdir /Q /S "temp"
git clone "..\..\..\..\..\3party\stb" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\source\stb"
copy /Y "temp\stb_image.h" "..\..\..\..\..\AE-Bin\external\source\stb\stb_image.h"
copy /Y "temp\stb_image_write.h" "..\..\..\..\..\AE-Bin\external\source\stb\stb_image_write.h"
copy /Y "temp\stb_rect_pack.h" "..\..\..\..\..\AE-Bin\external\source\stb\stb_rect_pack.h"
copy /Y "temp\stb_truetype.h" "..\..\..\..\..\AE-Bin\external\source\stb\stb_truetype.h"
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Bin\external\source\stb\LICENSE"
rmdir /Q /S "temp"
pause
