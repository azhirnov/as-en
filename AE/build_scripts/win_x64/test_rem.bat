cd ../../..
rmdir /Q /S "_test_rem"

mkdir _test_rem
cd _test_rem
cmake -G "Visual Studio 17 2022" -A x64 -DAE_SIMD_AVX=2 -DAE_ENABLE_VULKAN=ON -DAE_CI_BUILD_TEST=ON "../AE"

cmake --build . --config Develop --target Tests.GraphicsRHI
cmake --build . --config Develop --target RemoteGraphicsDevice
cmake --build . --config Develop --target Tests.GraphicsRHI.PackRes

copy /Y "..\AE-Temp\engine\graphics_rhi\Tests.GraphicsRHI.vkPipelines.bin" "bin/Develop"

ctest -R Tests.GraphicsRHI -C Develop -V

robocopy "bin/Develop/tests_graphics_rhi_ref" "../_test_rem_temp/ref" /S
copy /Y "bin\Develop\RemoteGraphicsDevice.exe" "..\_test_rem_temp"
		
pause

cd ..
rmdir /Q /S "_test_rem"
mkdir _test_rem
cd _test_rem
cmake -G "Visual Studio 17 2022" -A x64 -DAE_SIMD_AVX=2 -DAE_ENABLE_REMOTE_GRAPHICS=ON -DAE_CI_BUILD_TEST=ON "../AE"

cmake --build . --config Develop --target Tests.GraphicsRHI

robocopy "../_test_rem_temp/ref" "bin/Develop/tests_graphics_rhi_ref" /S
START "" /B "..\_test_rem_temp\RemoteGraphicsDevice.exe"

pause

copy /Y "..\AE-Temp\engine\graphics_rhi\Tests.GraphicsRHI.vkPipelines.bin" "bin/Develop"

ctest -R Tests.GraphicsRHI -C Develop -V

pause

taskkill /IM RemoteGraphicsDevice.exe
