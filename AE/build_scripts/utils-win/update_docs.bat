cd ../../../AE-Data
mkdir _docs
cd _docs

powershell -Command "(New-Object Net.WebClient).DownloadFile('https://registry.khronos.org/vulkan/specs/latest/pdf/vkspec.pdf', 'vkspec.pdf' )"

powershell -Command "(New-Object Net.WebClient).DownloadFile('https://registry.khronos.org/SPIR-V/specs/unified1/SPIRV.pdf', 'spirv.pdf' )"

powershell -Command "(New-Object Net.WebClient).DownloadFile('https://registry.khronos.org/OpenGL/specs/gl/GLSLangSpec.4.60.pdf', 'glsl.pdf' )"

powershell -Command "(New-Object Net.WebClient).DownloadFile('https://developer.apple.com/metal/Metal-Shading-Language-Specification.pdf', 'msl.pdf' )"

powershell -Command "(New-Object Net.WebClient).DownloadFile('https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf', 'Metal-Feature-Set-Tables.pdf' )"

powershell -Command "(New-Object Net.WebClient).DownloadFile('https://registry.khronos.org/OpenXR/specs/1.1-khr/pdf/xrspec.pdf', 'xrspec.pdf' )"

pause