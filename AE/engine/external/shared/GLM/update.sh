rm -rf "../../../../../AE-Data/external/source/GLM"
rm -rf "temp"
git clone --branch "AE-version" "../../../../../3party/ae-glm" "temp"
mkdir "../../../../../AE-Data/external/source/GLM"
cp -R "temp/glm/**/*.h" "../../../../../AE-Data/external/source/GLM"
cp -R "temp/glm/**/*.hpp" "../../../../../AE-Data/external/source/GLM"
cp -R "temp/glm/**/*.inl" "../../../../../AE-Data/external/source/GLM"
cp "temp/copying.txt" "../../../../../AE-Data/external/source/GLM/LICENSE.txt"
rm -rf "temp"
read -p "press any key..."