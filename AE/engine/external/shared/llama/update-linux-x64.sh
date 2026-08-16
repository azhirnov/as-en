echo "update 'llama'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-x64/llama"
rm -rf "temp"
mkdir "build"
git clone --recurse-submodules --branch "b8665" "https://github.com/ggml-org/llama.cpp.git" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "llama_CMakeLists.txt" "temp/CMakeLists.txt"
mkdir "../../../../../AE-Bin/external/linux-x64/llama"
mkdir "../../../../../AE-Bin/external/linux-x64/llama/include"
mkdir "../../../../../AE-Bin/external/linux-x64/llama/lib"
cp "temp/LICENSE" "../../../../../AE-Bin/external/linux-x64/llama/llama-LICENSE.txt"
cp "temp/include/llama.h" "../../../../../AE-Bin/external/linux-x64/llama/include/llama.h"
cd "temp/ggml/include"
find . -type d -exec mkdir -p "../../../../../../../../AE-Bin/external/linux-x64/llama/include/"{} \;
find . -type f -name '*.h' -exec cp {} "../../../../../../../../AE-Bin/external/linux-x64/llama/include/"{} \;
cd "../../.."
rm -rf "build"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64/llama"  -DCPU_TYPE=1 || pauseOnError
cmake --build build --config Release --target "ggml-cpu" -j 8 || pauseOnError
cp "build/bin/libggml-cpu.so" "../../../../../AE-Bin/external/linux-x64/llama/lib/libggml-cpu-sse4.so"
rm -rf "build"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64/llama"  -DCPU_TYPE=2 || pauseOnError
cmake --build build --config Release --target "ggml-cpu" -j 8 || pauseOnError
cp "build/bin/libggml-cpu.so" "../../../../../AE-Bin/external/linux-x64/llama/lib/libggml-cpu-avx.so"
rm -rf "build"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64/llama"  -DCPU_TYPE=3 || pauseOnError
cmake --build build --config Release --target "ggml-cpu" -j 8 || pauseOnError
cp "build/bin/libggml-cpu.so" "../../../../../AE-Bin/external/linux-x64/llama/lib/libggml-cpu-avx2.so"
rm -rf "build"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64/llama"  -DCPU_TYPE=4 || pauseOnError
cmake --build build --config Release --target "ggml-cpu" -j 8 || pauseOnError
cp "build/bin/libggml-cpu.so" "../../../../../AE-Bin/external/linux-x64/llama/lib/libggml-cpu-avx512_bf16.so"
rm -rf "build"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64/llama"  -DCPU_TYPE=5 || pauseOnError
cmake --build build --config Release --target "ggml-cpu" -j 8 || pauseOnError
cp "build/bin/libggml-cpu.so" "../../../../../AE-Bin/external/linux-x64/llama/lib/libggml-cpu-avx512_bf16_vbmi_vnni.so"
rm -rf "build"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64/llama"  -DCPU_TYPE=6 || pauseOnError
cmake --build build --config Release --target "ggml-cpu" -j 8 || pauseOnError
cp "build/bin/libggml-cpu.so" "../../../../../AE-Bin/external/linux-x64/llama/lib/libggml-cpu-avx2_vnni.so"
rm -rf "build"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64/llama"  -DCPU_TYPE=1 -DENABLE_VULKAN=ON || pauseOnError
cmake --build build --config Release --target "ggml-vulkan" -j 8 || pauseOnError
cp "build/bin/libggml-vulkan.so" "../../../../../AE-Bin/external/linux-x64/llama/lib/libggml-vulkan.so"
rm -rf "build"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64/llama"  -DCPU_TYPE=1 || pauseOnError
cmake --build build --config Release --target "llama" -j 8 || pauseOnError
cp "build/bin/libllama.so" "../../../../../AE-Bin/external/linux-x64/llama/lib/libllama.so"
cp "build/bin/libggml.so" "../../../../../AE-Bin/external/linux-x64/llama/lib/libggml.so"
cp "build/bin/libggml-base.so" "../../../../../AE-Bin/external/linux-x64/llama/lib/libggml-base.so"
rm -rf "temp"
rm -rf "build"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
