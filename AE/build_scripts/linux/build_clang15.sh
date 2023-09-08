# Clang 15 is required

cd ../../..

# cleanup
rm -rf _build

export CC=/usr/bin/clang-15
export CPP=/usr/bin/clang-cpp-15
export CXX=/usr/bin/clang++-15
export LD=/usr/bin/ld.lld-15

mkdir _build
cd _build

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=42 -DAE_ENABLE_VULKAN=ON "../AE"
cmake --build . --config Debug
read -p "press any key..."

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=42 -DAE_ENABLE_VULKAN=ON "../AE"
cmake --build . --config Release
read -p "press any key..."
