# Clang 12 is required
# ninja-build is required

cd ../..

# cleanup
rm -rf _build
rm -rf _build_res

export CC=/usr/bin/clang-12
export CXX=/usr/bin/clang++-12

mkdir _build
cd _build
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Debug ".." -DAE_SIMD_SSE=42
