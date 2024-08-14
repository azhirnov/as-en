cd ../../..
rmdir /Q /S "_build_rem"

mkdir _build_rem
cd _build_rem
cmake -G "Visual Studio 17 2022" -A x64 -DAE_SIMD_AVX=2 -DAE_ENABLE_REMOTE_GRAPHICS=ON -DAE_ENABLE_OPENVR=OFF "../AE"

pause

START AE.sln
