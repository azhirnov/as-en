cd ../..

rmdir /Q /S "_build_mtl"

mkdir _build_mtl
cd _build_mtl
cmake -G "Visual Studio 16 2019" -A x64 -DAE_SIMD_AVX=2 -DAE_ENABLE_METAL=ON -DAE_ENABLE_VULKAN=OFF -DAE_ENABLE_OPENVR=OFF ".."

pause