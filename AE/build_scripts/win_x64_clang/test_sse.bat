cd ../../..
rmdir /Q /S "_build_clang_sse"
mkdir _build_clang_sse
cd _build_clang_sse

cmake -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_SIMD_AVX=2 -DAE_USE_PCH=OFF -DAE_CI_BUILD_TEST=ON "../AE"
cmake --build . --config Debug --target Tests.Base
ctest -R "Tests.Base" -C Debug -V
pause

cd ..
rmdir /Q /S "_build_clang_sse"
mkdir _build_clang_sse
cd _build_clang_sse

cmake -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_SIMD_AVX=1 -DAE_USE_PCH=OFF -DAE_CI_BUILD_TEST=ON "../AE"
cmake --build . --config Debug --target Tests.Base
ctest -R "Tests.Base" -C Debug -V
pause

cd ..
rmdir /Q /S "_build_clang_sse"
mkdir _build_clang_sse
cd _build_clang_sse

cmake -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_SIMD_SSE=42 -DAE_USE_PCH=OFF -DAE_CI_BUILD_TEST=ON "../AE"
cmake --build . --config Debug --target Tests.Base
ctest -R "Tests.Base" -C Debug -V
pause

cd ..
rmdir /Q /S "_build_clang_sse"
mkdir _build_clang_sse
cd _build_clang_sse

cmake -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_SIMD_SSE=41 -DAE_USE_PCH=OFF -DAE_CI_BUILD_TEST=ON "../AE"
cmake --build . --config Debug --target Tests.Base
ctest -R "Tests.Base" -C Debug -V
pause

cd ..
rmdir /Q /S "_build_clang_sse"
mkdir _build_clang_sse
cd _build_clang_sse

cmake -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_SIMD_SSE=31 -DAE_USE_PCH=OFF -DAE_CI_BUILD_TEST=ON "../AE"
cmake --build . --config Debug --target Tests.Base
ctest -R "Tests.Base" -C Debug -V
pause

cd ..
rmdir /Q /S "_build_clang_sse"
mkdir _build_clang_sse
cd _build_clang_sse

cmake -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_SIMD_SSE=30 -DAE_USE_PCH=OFF -DAE_CI_BUILD_TEST=ON "../AE"
cmake --build . --config Debug --target Tests.Base
ctest -R "Tests.Base" -C Debug -V
pause

cd ..
rmdir /Q /S "_build_clang_sse"
mkdir _build_clang_sse
cd _build_clang_sse

cmake -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_SIMD_SSE=20 -DAE_USE_PCH=OFF -DAE_CI_BUILD_TEST=ON "../AE"
cmake --build . --config Debug --target Tests.Base
ctest -R "Tests.Base" -C Debug -V
pause

cd ..
rmdir /Q /S "_build_clang_sse"
mkdir _build_clang_sse
cd _build_clang_sse

cmake -G "Visual Studio 17 2022" -T ClangCL -A x64 -DAE_SIMD_SSE=20 -DAE_SIMD_SHA=1 -DAE_USE_PCH=OFF -DAE_CI_BUILD_TEST=ON "../AE"
cmake --build . --config Debug --target Tests.Base
ctest -R "Tests.Base" -C Debug -V
pause

cd ..
rmdir /Q /S "_build_clang_sse"
