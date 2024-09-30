cd ../../..
rmdir /Q /S "_build_arm64"

mkdir _build_arm64
cd _build_arm64
cmake -G "Visual Studio 17 2022" -A ARM64 -DAE_ENABLE_VULKAN=ON "../AE"

pause

START AE.sln
