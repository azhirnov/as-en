cd ../../..
rmdir /Q /S "_tmp_build"
rmdir /Q /S "AE-Temp\engine\precompiled"

mkdir _tmp_build
mkdir AE-Temp\engine\precompiled

cd _tmp_build
cmake -G "Visual Studio 17 2022" -A x64 -DAE_SIMD_AVX=2 -DAE_ENABLE_VULKAN=ON "../AE"

mkdir "..\AE-Temp\engine\precompiled\Debug"
cmake --build . --config Debug --target Base
copy /Y "bin\Debug\Base.lib" "..\AE-Temp\engine\precompiled\Debug\Base.lib"
copy /Y "bin\Debug\Base.pdb" "..\AE-Temp\engine\precompiled\Debug\Base.pdb"

cmake --build . --config Debug --target Scripting
copy /Y "bin\Debug\Scripting.lib" "..\AE-Temp\engine\precompiled\Debug\Scripting.lib"
copy /Y "bin\Debug\Scripting.pdb" "..\AE-Temp\engine\precompiled\Debug\Scripting.pdb"

cmake --build . --config Debug --target Serializing
copy /Y "bin\Debug\Serializing.lib" "..\AE-Temp\engine\precompiled\Debug\Serializing.lib"
copy /Y "bin\Debug\Serializing.pdb" "..\AE-Temp\engine\precompiled\Debug\Serializing.pdb"

cmake --build . --config Debug --target Threading
copy /Y "bin\Debug\Threading.lib" "..\AE-Temp\engine\precompiled\Debug\Threading.lib"
copy /Y "bin\Debug\Threading.pdb" "..\AE-Temp\engine\precompiled\Debug\Threading.pdb"

cmake --build . --config Debug --target Networking
copy /Y "bin\Debug\Networking.lib" "..\AE-Temp\engine\precompiled\Debug\Networking.lib"
copy /Y "bin\Debug\Networking.pdb" "..\AE-Temp\engine\precompiled\Debug\Networking.pdb"

cmake --build . --config Debug --target Graphics
copy /Y "bin\Debug\Graphics.lib" "..\AE-Temp\engine\precompiled\Debug\Graphics.lib"
copy /Y "bin\Debug\Graphics.pdb" "..\AE-Temp\engine\precompiled\Debug\Graphics.pdb"

cmake --build . --config Debug --target VFS
copy /Y "bin\Debug\VFS.lib" "..\AE-Temp\engine\precompiled\Debug\VFS.lib"
copy /Y "bin\Debug\VFS.pdb" "..\AE-Temp\engine\precompiled\Debug\VFS.pdb"

cmake --build . --config Debug --target ECS-st
copy /Y "bin\Debug\ECS-st.lib" "..\AE-Temp\engine\precompiled\Debug\ECS-st.lib"
copy /Y "bin\Debug\ECS-st.pdb" "..\AE-Temp\engine\precompiled\Debug\ECS-st.pdb"

cmake --build . --config Debug --target Platform
copy /Y "bin\Debug\Platform.lib" "..\AE-Temp\engine\precompiled\Debug\Platform.lib"
copy /Y "bin\Debug\Platform.pdb" "..\AE-Temp\engine\precompiled\Debug\Platform.pdb"

cmake --build . --config Debug --target Audio
copy /Y "bin\Debug\Audio.lib" "..\AE-Temp\engine\precompiled\Debug\Audio.lib"
copy /Y "bin\Debug\Audio.pdb" "..\AE-Temp\engine\precompiled\Debug\Audio.pdb"

cmake --build . --config Debug --target Video
copy /Y "bin\Debug\Video.lib" "..\AE-Temp\engine\precompiled\Debug\Video.lib"
copy /Y "bin\Debug\Video.pdb" "..\AE-Temp\engine\precompiled\Debug\Video.pdb"

cmake --build . --config Debug --target HuLang
copy /Y "bin\Debug\HuLang.lib" "..\AE-Temp\engine\precompiled\Debug\HuLang.lib"
copy /Y "bin\Debug\HuLang.pdb" "..\AE-Temp\engine\precompiled\Debug\HuLang.pdb"

cmake --build . --config Debug --target GraphicsHL
copy /Y "bin\Debug\GraphicsHL.lib" "..\AE-Temp\engine\precompiled\Debug\GraphicsHL.lib"
copy /Y "bin\Debug\GraphicsHL.pdb" "..\AE-Temp\engine\precompiled\Debug\GraphicsHL.pdb"

cmake --build . --config Debug --target Profiler
copy /Y "bin\Debug\Profiler.lib" "..\AE-Temp\engine\precompiled\Debug\Profiler.lib"
copy /Y "bin\Debug\Profiler.pdb" "..\AE-Temp\engine\precompiled\Debug\Profiler.pdb"

cmake --build . --config Debug --target AtlasTools
copy /Y "bin\Debug\AtlasTools.lib" "..\AE-Temp\engine\precompiled\Debug\AtlasTools.lib"
copy /Y "bin\Debug\AtlasTools.pdb" "..\AE-Temp\engine\precompiled\Debug\AtlasTools.pdb"

cmake --build . --config Debug --target GeometryTools
copy /Y "bin\Debug\GeometryTools.lib" "..\AE-Temp\engine\precompiled\Debug\GeometryTools.lib"
copy /Y "bin\Debug\GeometryTools.pdb" "..\AE-Temp\engine\precompiled\Debug\GeometryTools.pdb"

cmake --build . --config Debug --target GraphicsTestUtils
copy /Y "bin\Debug\GraphicsTestUtils.lib" "..\AE-Temp\engine\precompiled\Debug\GraphicsTestUtils.lib"
copy /Y "bin\Debug\GraphicsTestUtils.pdb" "..\AE-Temp\engine\precompiled\Debug\GraphicsTestUtils.pdb"

cmake --build . --config Debug --target ResourceLoaders
copy /Y "bin\Debug\ResourceLoaders.lib" "..\AE-Temp\engine\precompiled\Debug\ResourceLoaders.lib"
copy /Y "bin\Debug\ResourceLoaders.pdb" "..\AE-Temp\engine\precompiled\Debug\ResourceLoaders.pdb"

cmake --build . --config Debug --target VulkanExtEmulation
copy /Y "bin\Debug\VulkanExtEmulation.lib" "..\AE-Temp\engine\precompiled\Debug\VulkanExtEmulation.lib"
copy /Y "bin\Debug\VulkanExtEmulation.pdb" "..\AE-Temp\engine\precompiled\Debug\VulkanExtEmulation.pdb"

cmake --build . --config Debug --target AssetPacker
copy /Y "bin\Debug\AssetPacker.lib" "..\AE-Temp\engine\precompiled\Debug\AssetPacker.lib"
copy /Y "bin\Debug\AssetPacker.pdb" "..\AE-Temp\engine\precompiled\Debug\AssetPacker.pdb"

cmake --build . --config Debug --target InputActionsBinding
copy /Y "bin\Debug\InputActionsBinding.lib" "..\AE-Temp\engine\precompiled\Debug\InputActionsBinding.lib"
copy /Y "bin\Debug\InputActionsBinding.pdb" "..\AE-Temp\engine\precompiled\Debug\InputActionsBinding.pdb"

cmake --build . --config Debug --target PipelineCompiler
copy /Y "bin\Debug\PipelineCompiler.lib" "..\AE-Temp\engine\precompiled\Debug\PipelineCompiler.lib"
copy /Y "bin\Debug\PipelineCompiler.pdb" "..\AE-Temp\engine\precompiled\Debug\PipelineCompiler.pdb"

pause

mkdir "..\AE-Temp\engine\precompiled\Develop"
cmake --build . --config Develop --target Base
copy /Y "bin\Develop\Base.lib" "..\AE-Temp\engine\precompiled\Develop\Base.lib"
copy /Y "bin\Develop\Base.pdb" "..\AE-Temp\engine\precompiled\Develop\Base.pdb"

cmake --build . --config Develop --target Scripting
copy /Y "bin\Develop\Scripting.lib" "..\AE-Temp\engine\precompiled\Develop\Scripting.lib"
copy /Y "bin\Develop\Scripting.pdb" "..\AE-Temp\engine\precompiled\Develop\Scripting.pdb"

cmake --build . --config Develop --target Serializing
copy /Y "bin\Develop\Serializing.lib" "..\AE-Temp\engine\precompiled\Develop\Serializing.lib"
copy /Y "bin\Develop\Serializing.pdb" "..\AE-Temp\engine\precompiled\Develop\Serializing.pdb"

cmake --build . --config Develop --target Threading
copy /Y "bin\Develop\Threading.lib" "..\AE-Temp\engine\precompiled\Develop\Threading.lib"
copy /Y "bin\Develop\Threading.pdb" "..\AE-Temp\engine\precompiled\Develop\Threading.pdb"

cmake --build . --config Develop --target Networking
copy /Y "bin\Develop\Networking.lib" "..\AE-Temp\engine\precompiled\Develop\Networking.lib"
copy /Y "bin\Develop\Networking.pdb" "..\AE-Temp\engine\precompiled\Develop\Networking.pdb"

cmake --build . --config Develop --target Graphics
copy /Y "bin\Develop\Graphics.lib" "..\AE-Temp\engine\precompiled\Develop\Graphics.lib"
copy /Y "bin\Develop\Graphics.pdb" "..\AE-Temp\engine\precompiled\Develop\Graphics.pdb"

cmake --build . --config Develop --target VFS
copy /Y "bin\Develop\VFS.lib" "..\AE-Temp\engine\precompiled\Develop\VFS.lib"
copy /Y "bin\Develop\VFS.pdb" "..\AE-Temp\engine\precompiled\Develop\VFS.pdb"

cmake --build . --config Develop --target ECS-st
copy /Y "bin\Develop\ECS-st.lib" "..\AE-Temp\engine\precompiled\Develop\ECS-st.lib"
copy /Y "bin\Develop\ECS-st.pdb" "..\AE-Temp\engine\precompiled\Develop\ECS-st.pdb"

cmake --build . --config Develop --target Platform
copy /Y "bin\Develop\Platform.lib" "..\AE-Temp\engine\precompiled\Develop\Platform.lib"
copy /Y "bin\Develop\Platform.pdb" "..\AE-Temp\engine\precompiled\Develop\Platform.pdb"

cmake --build . --config Develop --target Audio
copy /Y "bin\Develop\Audio.lib" "..\AE-Temp\engine\precompiled\Develop\Audio.lib"
copy /Y "bin\Develop\Audio.pdb" "..\AE-Temp\engine\precompiled\Develop\Audio.pdb"

cmake --build . --config Develop --target Video
copy /Y "bin\Develop\Video.lib" "..\AE-Temp\engine\precompiled\Develop\Video.lib"
copy /Y "bin\Develop\Video.pdb" "..\AE-Temp\engine\precompiled\Develop\Video.pdb"

cmake --build . --config Develop --target HuLang
copy /Y "bin\Develop\HuLang.lib" "..\AE-Temp\engine\precompiled\Develop\HuLang.lib"
copy /Y "bin\Develop\HuLang.pdb" "..\AE-Temp\engine\precompiled\Develop\HuLang.pdb"

cmake --build . --config Develop --target GraphicsHL
copy /Y "bin\Develop\GraphicsHL.lib" "..\AE-Temp\engine\precompiled\Develop\GraphicsHL.lib"
copy /Y "bin\Develop\GraphicsHL.pdb" "..\AE-Temp\engine\precompiled\Develop\GraphicsHL.pdb"

cmake --build . --config Develop --target Profiler
copy /Y "bin\Develop\Profiler.lib" "..\AE-Temp\engine\precompiled\Develop\Profiler.lib"
copy /Y "bin\Develop\Profiler.pdb" "..\AE-Temp\engine\precompiled\Develop\Profiler.pdb"

cmake --build . --config Develop --target AtlasTools
copy /Y "bin\Develop\AtlasTools.lib" "..\AE-Temp\engine\precompiled\Develop\AtlasTools.lib"
copy /Y "bin\Develop\AtlasTools.pdb" "..\AE-Temp\engine\precompiled\Develop\AtlasTools.pdb"

cmake --build . --config Develop --target GeometryTools
copy /Y "bin\Develop\GeometryTools.lib" "..\AE-Temp\engine\precompiled\Develop\GeometryTools.lib"
copy /Y "bin\Develop\GeometryTools.pdb" "..\AE-Temp\engine\precompiled\Develop\GeometryTools.pdb"

cmake --build . --config Develop --target GraphicsTestUtils
copy /Y "bin\Develop\GraphicsTestUtils.lib" "..\AE-Temp\engine\precompiled\Develop\GraphicsTestUtils.lib"
copy /Y "bin\Develop\GraphicsTestUtils.pdb" "..\AE-Temp\engine\precompiled\Develop\GraphicsTestUtils.pdb"

cmake --build . --config Develop --target ResourceLoaders
copy /Y "bin\Develop\ResourceLoaders.lib" "..\AE-Temp\engine\precompiled\Develop\ResourceLoaders.lib"
copy /Y "bin\Develop\ResourceLoaders.pdb" "..\AE-Temp\engine\precompiled\Develop\ResourceLoaders.pdb"

cmake --build . --config Develop --target VulkanExtEmulation
copy /Y "bin\Develop\VulkanExtEmulation.lib" "..\AE-Temp\engine\precompiled\Develop\VulkanExtEmulation.lib"
copy /Y "bin\Develop\VulkanExtEmulation.pdb" "..\AE-Temp\engine\precompiled\Develop\VulkanExtEmulation.pdb"

cmake --build . --config Develop --target AssetPacker
copy /Y "bin\Develop\AssetPacker.lib" "..\AE-Temp\engine\precompiled\Develop\AssetPacker.lib"
copy /Y "bin\Develop\AssetPacker.pdb" "..\AE-Temp\engine\precompiled\Develop\AssetPacker.pdb"

cmake --build . --config Develop --target InputActionsBinding
copy /Y "bin\Develop\InputActionsBinding.lib" "..\AE-Temp\engine\precompiled\Develop\InputActionsBinding.lib"
copy /Y "bin\Develop\InputActionsBinding.pdb" "..\AE-Temp\engine\precompiled\Develop\InputActionsBinding.pdb"

cmake --build . --config Develop --target PipelineCompiler
copy /Y "bin\Develop\PipelineCompiler.lib" "..\AE-Temp\engine\precompiled\Develop\PipelineCompiler.lib"
copy /Y "bin\Develop\PipelineCompiler.pdb" "..\AE-Temp\engine\precompiled\Develop\PipelineCompiler.pdb"

pause

mkdir "..\AE-Temp\engine\precompiled\Profile"
cmake --build . --config Profile --target Base
copy /Y "bin\Profile\Base.lib" "..\AE-Temp\engine\precompiled\Profile\Base.lib"
copy /Y "bin\Profile\Base.pdb" "..\AE-Temp\engine\precompiled\Profile\Base.pdb"

cmake --build . --config Profile --target Scripting
copy /Y "bin\Profile\Scripting.lib" "..\AE-Temp\engine\precompiled\Profile\Scripting.lib"
copy /Y "bin\Profile\Scripting.pdb" "..\AE-Temp\engine\precompiled\Profile\Scripting.pdb"

cmake --build . --config Profile --target Serializing
copy /Y "bin\Profile\Serializing.lib" "..\AE-Temp\engine\precompiled\Profile\Serializing.lib"
copy /Y "bin\Profile\Serializing.pdb" "..\AE-Temp\engine\precompiled\Profile\Serializing.pdb"

cmake --build . --config Profile --target Threading
copy /Y "bin\Profile\Threading.lib" "..\AE-Temp\engine\precompiled\Profile\Threading.lib"
copy /Y "bin\Profile\Threading.pdb" "..\AE-Temp\engine\precompiled\Profile\Threading.pdb"

cmake --build . --config Profile --target Networking
copy /Y "bin\Profile\Networking.lib" "..\AE-Temp\engine\precompiled\Profile\Networking.lib"
copy /Y "bin\Profile\Networking.pdb" "..\AE-Temp\engine\precompiled\Profile\Networking.pdb"

cmake --build . --config Profile --target Graphics
copy /Y "bin\Profile\Graphics.lib" "..\AE-Temp\engine\precompiled\Profile\Graphics.lib"
copy /Y "bin\Profile\Graphics.pdb" "..\AE-Temp\engine\precompiled\Profile\Graphics.pdb"

cmake --build . --config Profile --target VFS
copy /Y "bin\Profile\VFS.lib" "..\AE-Temp\engine\precompiled\Profile\VFS.lib"
copy /Y "bin\Profile\VFS.pdb" "..\AE-Temp\engine\precompiled\Profile\VFS.pdb"

cmake --build . --config Profile --target ECS-st
copy /Y "bin\Profile\ECS-st.lib" "..\AE-Temp\engine\precompiled\Profile\ECS-st.lib"
copy /Y "bin\Profile\ECS-st.pdb" "..\AE-Temp\engine\precompiled\Profile\ECS-st.pdb"

cmake --build . --config Profile --target Platform
copy /Y "bin\Profile\Platform.lib" "..\AE-Temp\engine\precompiled\Profile\Platform.lib"
copy /Y "bin\Profile\Platform.pdb" "..\AE-Temp\engine\precompiled\Profile\Platform.pdb"

cmake --build . --config Profile --target Audio
copy /Y "bin\Profile\Audio.lib" "..\AE-Temp\engine\precompiled\Profile\Audio.lib"
copy /Y "bin\Profile\Audio.pdb" "..\AE-Temp\engine\precompiled\Profile\Audio.pdb"

cmake --build . --config Profile --target Video
copy /Y "bin\Profile\Video.lib" "..\AE-Temp\engine\precompiled\Profile\Video.lib"
copy /Y "bin\Profile\Video.pdb" "..\AE-Temp\engine\precompiled\Profile\Video.pdb"

cmake --build . --config Profile --target HuLang
copy /Y "bin\Profile\HuLang.lib" "..\AE-Temp\engine\precompiled\Profile\HuLang.lib"
copy /Y "bin\Profile\HuLang.pdb" "..\AE-Temp\engine\precompiled\Profile\HuLang.pdb"

cmake --build . --config Profile --target GraphicsHL
copy /Y "bin\Profile\GraphicsHL.lib" "..\AE-Temp\engine\precompiled\Profile\GraphicsHL.lib"
copy /Y "bin\Profile\GraphicsHL.pdb" "..\AE-Temp\engine\precompiled\Profile\GraphicsHL.pdb"

cmake --build . --config Profile --target Profiler
copy /Y "bin\Profile\Profiler.lib" "..\AE-Temp\engine\precompiled\Profile\Profiler.lib"
copy /Y "bin\Profile\Profiler.pdb" "..\AE-Temp\engine\precompiled\Profile\Profiler.pdb"

cmake --build . --config Profile --target AtlasTools
copy /Y "bin\Profile\AtlasTools.lib" "..\AE-Temp\engine\precompiled\Profile\AtlasTools.lib"
copy /Y "bin\Profile\AtlasTools.pdb" "..\AE-Temp\engine\precompiled\Profile\AtlasTools.pdb"

cmake --build . --config Profile --target GeometryTools
copy /Y "bin\Profile\GeometryTools.lib" "..\AE-Temp\engine\precompiled\Profile\GeometryTools.lib"
copy /Y "bin\Profile\GeometryTools.pdb" "..\AE-Temp\engine\precompiled\Profile\GeometryTools.pdb"

cmake --build . --config Profile --target GraphicsTestUtils
copy /Y "bin\Profile\GraphicsTestUtils.lib" "..\AE-Temp\engine\precompiled\Profile\GraphicsTestUtils.lib"
copy /Y "bin\Profile\GraphicsTestUtils.pdb" "..\AE-Temp\engine\precompiled\Profile\GraphicsTestUtils.pdb"

cmake --build . --config Profile --target ResourceLoaders
copy /Y "bin\Profile\ResourceLoaders.lib" "..\AE-Temp\engine\precompiled\Profile\ResourceLoaders.lib"
copy /Y "bin\Profile\ResourceLoaders.pdb" "..\AE-Temp\engine\precompiled\Profile\ResourceLoaders.pdb"

cmake --build . --config Profile --target VulkanExtEmulation
copy /Y "bin\Profile\VulkanExtEmulation.lib" "..\AE-Temp\engine\precompiled\Profile\VulkanExtEmulation.lib"
copy /Y "bin\Profile\VulkanExtEmulation.pdb" "..\AE-Temp\engine\precompiled\Profile\VulkanExtEmulation.pdb"

cmake --build . --config Profile --target AssetPacker
copy /Y "bin\Profile\AssetPacker.lib" "..\AE-Temp\engine\precompiled\Profile\AssetPacker.lib"
copy /Y "bin\Profile\AssetPacker.pdb" "..\AE-Temp\engine\precompiled\Profile\AssetPacker.pdb"

cmake --build . --config Profile --target InputActionsBinding
copy /Y "bin\Profile\InputActionsBinding.lib" "..\AE-Temp\engine\precompiled\Profile\InputActionsBinding.lib"
copy /Y "bin\Profile\InputActionsBinding.pdb" "..\AE-Temp\engine\precompiled\Profile\InputActionsBinding.pdb"

cmake --build . --config Profile --target PipelineCompiler
copy /Y "bin\Profile\PipelineCompiler.lib" "..\AE-Temp\engine\precompiled\Profile\PipelineCompiler.lib"
copy /Y "bin\Profile\PipelineCompiler.pdb" "..\AE-Temp\engine\precompiled\Profile\PipelineCompiler.pdb"

pause

mkdir "..\AE-Temp\engine\precompiled\Release"
cmake --build . --config Release --target Base
copy /Y "bin\Release\Base.lib" "..\AE-Temp\engine\precompiled\Release\Base.lib"
copy /Y "bin\Release\Base.pdb" "..\AE-Temp\engine\precompiled\Release\Base.pdb"

cmake --build . --config Release --target Scripting
copy /Y "bin\Release\Scripting.lib" "..\AE-Temp\engine\precompiled\Release\Scripting.lib"
copy /Y "bin\Release\Scripting.pdb" "..\AE-Temp\engine\precompiled\Release\Scripting.pdb"

cmake --build . --config Release --target Serializing
copy /Y "bin\Release\Serializing.lib" "..\AE-Temp\engine\precompiled\Release\Serializing.lib"
copy /Y "bin\Release\Serializing.pdb" "..\AE-Temp\engine\precompiled\Release\Serializing.pdb"

cmake --build . --config Release --target Threading
copy /Y "bin\Release\Threading.lib" "..\AE-Temp\engine\precompiled\Release\Threading.lib"
copy /Y "bin\Release\Threading.pdb" "..\AE-Temp\engine\precompiled\Release\Threading.pdb"

cmake --build . --config Release --target Networking
copy /Y "bin\Release\Networking.lib" "..\AE-Temp\engine\precompiled\Release\Networking.lib"
copy /Y "bin\Release\Networking.pdb" "..\AE-Temp\engine\precompiled\Release\Networking.pdb"

cmake --build . --config Release --target Graphics
copy /Y "bin\Release\Graphics.lib" "..\AE-Temp\engine\precompiled\Release\Graphics.lib"
copy /Y "bin\Release\Graphics.pdb" "..\AE-Temp\engine\precompiled\Release\Graphics.pdb"

cmake --build . --config Release --target VFS
copy /Y "bin\Release\VFS.lib" "..\AE-Temp\engine\precompiled\Release\VFS.lib"
copy /Y "bin\Release\VFS.pdb" "..\AE-Temp\engine\precompiled\Release\VFS.pdb"

cmake --build . --config Release --target ECS-st
copy /Y "bin\Release\ECS-st.lib" "..\AE-Temp\engine\precompiled\Release\ECS-st.lib"
copy /Y "bin\Release\ECS-st.pdb" "..\AE-Temp\engine\precompiled\Release\ECS-st.pdb"

cmake --build . --config Release --target Platform
copy /Y "bin\Release\Platform.lib" "..\AE-Temp\engine\precompiled\Release\Platform.lib"
copy /Y "bin\Release\Platform.pdb" "..\AE-Temp\engine\precompiled\Release\Platform.pdb"

cmake --build . --config Release --target Audio
copy /Y "bin\Release\Audio.lib" "..\AE-Temp\engine\precompiled\Release\Audio.lib"
copy /Y "bin\Release\Audio.pdb" "..\AE-Temp\engine\precompiled\Release\Audio.pdb"

cmake --build . --config Release --target Video
copy /Y "bin\Release\Video.lib" "..\AE-Temp\engine\precompiled\Release\Video.lib"
copy /Y "bin\Release\Video.pdb" "..\AE-Temp\engine\precompiled\Release\Video.pdb"

cmake --build . --config Release --target HuLang
copy /Y "bin\Release\HuLang.lib" "..\AE-Temp\engine\precompiled\Release\HuLang.lib"
copy /Y "bin\Release\HuLang.pdb" "..\AE-Temp\engine\precompiled\Release\HuLang.pdb"

cmake --build . --config Release --target GraphicsHL
copy /Y "bin\Release\GraphicsHL.lib" "..\AE-Temp\engine\precompiled\Release\GraphicsHL.lib"
copy /Y "bin\Release\GraphicsHL.pdb" "..\AE-Temp\engine\precompiled\Release\GraphicsHL.pdb"

cmake --build . --config Release --target Profiler
copy /Y "bin\Release\Profiler.lib" "..\AE-Temp\engine\precompiled\Release\Profiler.lib"
copy /Y "bin\Release\Profiler.pdb" "..\AE-Temp\engine\precompiled\Release\Profiler.pdb"

cmake --build . --config Release --target AtlasTools
copy /Y "bin\Release\AtlasTools.lib" "..\AE-Temp\engine\precompiled\Release\AtlasTools.lib"
copy /Y "bin\Release\AtlasTools.pdb" "..\AE-Temp\engine\precompiled\Release\AtlasTools.pdb"

cmake --build . --config Release --target GeometryTools
copy /Y "bin\Release\GeometryTools.lib" "..\AE-Temp\engine\precompiled\Release\GeometryTools.lib"
copy /Y "bin\Release\GeometryTools.pdb" "..\AE-Temp\engine\precompiled\Release\GeometryTools.pdb"

cmake --build . --config Release --target GraphicsTestUtils
copy /Y "bin\Release\GraphicsTestUtils.lib" "..\AE-Temp\engine\precompiled\Release\GraphicsTestUtils.lib"
copy /Y "bin\Release\GraphicsTestUtils.pdb" "..\AE-Temp\engine\precompiled\Release\GraphicsTestUtils.pdb"

cmake --build . --config Release --target ResourceLoaders
copy /Y "bin\Release\ResourceLoaders.lib" "..\AE-Temp\engine\precompiled\Release\ResourceLoaders.lib"
copy /Y "bin\Release\ResourceLoaders.pdb" "..\AE-Temp\engine\precompiled\Release\ResourceLoaders.pdb"

cmake --build . --config Release --target VulkanExtEmulation
copy /Y "bin\Release\VulkanExtEmulation.lib" "..\AE-Temp\engine\precompiled\Release\VulkanExtEmulation.lib"
copy /Y "bin\Release\VulkanExtEmulation.pdb" "..\AE-Temp\engine\precompiled\Release\VulkanExtEmulation.pdb"

cmake --build . --config Release --target AssetPacker
copy /Y "bin\Release\AssetPacker.lib" "..\AE-Temp\engine\precompiled\Release\AssetPacker.lib"
copy /Y "bin\Release\AssetPacker.pdb" "..\AE-Temp\engine\precompiled\Release\AssetPacker.pdb"

cmake --build . --config Release --target InputActionsBinding
copy /Y "bin\Release\InputActionsBinding.lib" "..\AE-Temp\engine\precompiled\Release\InputActionsBinding.lib"
copy /Y "bin\Release\InputActionsBinding.pdb" "..\AE-Temp\engine\precompiled\Release\InputActionsBinding.pdb"

cmake --build . --config Release --target PipelineCompiler
copy /Y "bin\Release\PipelineCompiler.lib" "..\AE-Temp\engine\precompiled\Release\PipelineCompiler.lib"
copy /Y "bin\Release\PipelineCompiler.pdb" "..\AE-Temp\engine\precompiled\Release\PipelineCompiler.pdb"

pause

