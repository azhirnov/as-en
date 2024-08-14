rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\tinygltf"
rmdir /Q /S "temp"
git clone --branch "v2.9.2" "..\..\..\..\..\3party\tinygltf" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\source\tinygltf"
copy /Y "temp\tiny_gltf.h" "..\..\..\..\..\AE-Bin\external\source\tinygltf\tiny_gltf.h"
copy /Y "temp\json.hpp" "..\..\..\..\..\AE-Bin\external\source\tinygltf\json.hpp"
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Bin\external\source\tinygltf\LICENSE"
rmdir /Q /S "temp"
pause
