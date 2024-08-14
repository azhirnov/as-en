rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\nvml"
rmdir /Q /S "temp"
git clone "..\..\..\..\..\3party\nvml" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\source\nvml"
copy /Y "temp\nvml.h" "..\..\..\..\..\AE-Bin\external\source\nvml\nvml.h"
rmdir /Q /S "temp"
pause
