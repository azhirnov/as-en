# Clang 16 is required
# ninja-build is required

cd ../../..

# cleanup
rm -rf _build

export CC=/usr/bin/clang-16
export CPP=/usr/bin/clang-cpp-16
export CXX=/usr/bin/clang++-16
export LD=/usr/bin/ld.lld-16

mkdir _build
cd _build
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=42 -DAE_ENABLE_VULKAN=ON "../AE"
