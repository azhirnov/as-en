echo "update 'llama'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/macos-arm64/llama"
rm -rf "temp"
mkdir "build"
git clone --recurse-submodules --branch "b6615" "https://github.com/ggml-org/llama.cpp.git" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "llama_CMakeLists.txt" "temp/CMakeLists.txt"
mkdir "../../../../../AE-Bin/external/macos-arm64/llama"
mkdir "../../../../../AE-Bin/external/macos-arm64/llama/include"
mkdir "../../../../../AE-Bin/external/macos-arm64/llama/lib"
cp "temp/LICENSE" "../../../../../AE-Bin/external/macos-arm64/llama/llama-LICENSE.txt"
cp "temp/include/llama.h" "../../../../../AE-Bin/external/macos-arm64/llama/include/llama.h"
cd "temp/ggml/include"
find . -type d -exec mkdir -p "../../../../../../../../AE-Bin/external/macos-arm64/llama/include/"{} \;
find . -type f -name '*.h' -exec cp {} "../../../../../../../../AE-Bin/external/macos-arm64/llama/include/"{} \;
cd "../../.."
rm -rf "build"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64/llama"  -DCPU_TYPE=1
cmake --build build --config Release --target "ggml-cpu" -j 6
rm -rf "build"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64/llama"  -DCPU_TYPE=2
cmake --build build --config Release --target "ggml-cpu" -j 6
rm -rf "build"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64/llama"  -DCPU_TYPE=3
cmake --build build --config Release --target "ggml-cpu" -j 6
rm -rf "build"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64/llama"  -DCPU_TYPE=4
cmake --build build --config Release --target "ggml-cpu" -j 6
rm -rf "build"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64/llama"  -DCPU_TYPE=5
cmake --build build --config Release --target "ggml-cpu" -j 6
rm -rf "build"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64/llama"  -DCPU_TYPE=6
cmake --build build --config Release --target "ggml-cpu" -j 6
rm -rf "build"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64/llama"  -DCPU_TYPE=1 -DENABLE_VULKAN=ON
cmake --build build --config Release --target "ggml-vulkan" -j 6
rm -rf "build"
cmake -S temp -B build  -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/macos-arm64/llama"  -DCPU_TYPE=1
cmake --build build --config Release --target "llama" -j 6
rm -rf "temp"
rm -rf "build"
read -p "press any key..."
