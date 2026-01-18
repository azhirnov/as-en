rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\nvml"
rmdir /Q /S "temp"
mkdir "temp"
powershell -Command "(New-Object Net.WebClient).DownloadFile('https://raw.githubusercontent.com/jp7677/dxvk-nvapi/refs/heads/master/inc/nvml/nvml.h', 'temp\nvml.h')"
mkdir "..\..\..\..\..\AE-Bin\external\source\nvml"
copy /Y "temp\nvml.h" "..\..\..\..\..\AE-Bin\external\source\nvml\nvml.h"
rmdir /Q /S "temp"
pause
