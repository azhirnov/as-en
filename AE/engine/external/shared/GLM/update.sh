rm -rf "../../../../../AE-Bin/external/source/GLM"
rm -rf "temp"
git clone --branch "ae-24.08" "https://github.com/azhirnov/glm.git" "temp"
mkdir "../../../../../AE-Bin/external/source/GLM"
cd "temp/glm"
find . -type d -exec mkdir -p "../../../../../../../AE-Bin/external/source/GLM/"{} \;
find . -type f -name '*.h' -exec cp {} "../../../../../../../AE-Bin/external/source/GLM/"{} \;
cd "../.."
cd "temp/glm"
find . -type d -exec mkdir -p "../../../../../../../AE-Bin/external/source/GLM/"{} \;
find . -type f -name '*.hpp' -exec cp {} "../../../../../../../AE-Bin/external/source/GLM/"{} \;
cd "../.."
cd "temp/glm"
find . -type d -exec mkdir -p "../../../../../../../AE-Bin/external/source/GLM/"{} \;
find . -type f -name '*.inl' -exec cp {} "../../../../../../../AE-Bin/external/source/GLM/"{} \;
cd "../.."
cp "temp/copying.txt" "../../../../../AE-Bin/external/source/GLM/LICENSE.txt"
rm -rf "temp"
read -p "press any key..."
