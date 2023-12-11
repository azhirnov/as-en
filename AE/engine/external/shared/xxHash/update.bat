rmdir /Q /S "..\..\..\..\..\AE-Data\external\source\xxHash"
rmdir /Q /S "temp"
git clone --branch "v0.8.2" "..\..\..\..\..\3party\xxHash" "temp"
mkdir "..\..\..\..\..\AE-Data\external\source\xxHash"
copy /Y "temp\xxhash.h" "..\..\..\..\..\AE-Data\external\source\xxHash\xxhash.h"
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Data\external\source\xxHash\LICENSE.txt"
rmdir /Q /S "temp"
pause
