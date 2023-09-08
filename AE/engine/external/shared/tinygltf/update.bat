rmdir /Q /S "..\..\..\..\..\AE-Data\external\source\tinygltf"
rmdir /Q /S "temp"
git clone --branch "v2.8.13" "..\..\..\..\..\3party\tinygltf" "temp"
mkdir "..\..\..\..\..\AE-Data\external\source\tinygltf"
copy /Y "temp\tiny_gltf.h" "..\..\..\..\..\AE-Data\external\source\tinygltf\tiny_gltf.h"
copy /Y "temp\json.hpp" "..\..\..\..\..\AE-Data\external\source\tinygltf\json.hpp"
copy /Y "temp\LICENSE" "..\..\..\..\..\AE-Data\external\source\tinygltf\LICENSE"
rmdir /Q /S "temp"
pause
