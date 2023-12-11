rmdir /Q /S "..\..\..\..\..\AE-Data\external\source\GLM"
rmdir /Q /S "temp"
git clone --branch "AE-version" "..\..\..\..\..\3party\ae-glm" "temp"
mkdir "..\..\..\..\..\AE-Data\external\source\GLM"
robocopy "temp\glm" "..\..\..\..\..\AE-Data\external\source\GLM" *.h /S 
robocopy "temp\glm" "..\..\..\..\..\AE-Data\external\source\GLM" *.hpp /S 
robocopy "temp\glm" "..\..\..\..\..\AE-Data\external\source\GLM" *.inl /S 
copy /Y "temp\copying.txt" "..\..\..\..\..\AE-Data\external\source\GLM\LICENSE.txt"
rmdir /Q /S "temp"
pause
