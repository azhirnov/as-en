rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\xxHash"
rmdir /Q /S "temp"
git clone --branch "v0.8.3" "https://github.com/Cyan4973/xxHash.git" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\source\xxHash"
copy /Y "temp\xxhash.h" "..\..\..\..\..\AE-Bin\external\source\xxHash\xxhash.h"
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Bin\external\source\xxHash\LICENSE.txt"
rmdir /Q /S "temp"
pause
