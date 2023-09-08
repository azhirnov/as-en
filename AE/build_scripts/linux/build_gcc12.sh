# GCC 12 is required

cd ../../..

# cleanup
rm -rf _build

export CC=/usr/bin/gcc-12
export CXX=/usr/bin/g++-12

mkdir _build
cd _build

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=42 -DAE_ENABLE_VULKAN=ON "../AE"
cmake --build . --config Debug
read -p "press any key..."

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=42 -DAE_ENABLE_VULKAN=ON "../AE"
cmake --build . --config Release
read -p "press any key..."
