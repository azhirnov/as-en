:: PCH include Base module, use it for Engine development

cd ../../..
rmdir /Q /S "_build"

mkdir _build
cd _build
cmake -G "Visual Studio 17 2022" -A x64 -DAE_SIMD_AVX=2 -DAE_ENABLE_VULKAN=ON -DAE_ENGINE_BASE_PCH=ON -DAE_EXCLUDE_PACK_RES=ON "../AE"

pause

START AE.sln
