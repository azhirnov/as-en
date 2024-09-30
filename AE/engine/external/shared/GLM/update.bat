rmdir /Q /S "..\..\..\..\..\AE-Bin\external\source\GLM"
rmdir /Q /S "temp"
git clone --branch "ae-24.08" "..\..\..\..\..\3party\ae-glm" "temp"
mkdir "..\..\..\..\..\AE-Bin\external\source\GLM"
robocopy "temp\glm" "..\..\..\..\..\AE-Bin\external\source\GLM" *.h /S 
robocopy "temp\glm" "..\..\..\..\..\AE-Bin\external\source\GLM" *.hpp /S 
robocopy "temp\glm" "..\..\..\..\..\AE-Bin\external\source\GLM" *.inl /S 
copy /Y "temp\copying.txt" "..\..\..\..\..\AE-Bin\external\source\GLM\LICENSE.txt"
rmdir /Q /S "temp"
pause
