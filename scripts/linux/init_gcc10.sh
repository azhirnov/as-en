# GCC 10 is required

cd ../..

# cleanup
rm -rf _build

export CC=/usr/bin/gcc-10
export CXX=/usr/bin/g++-10

mkdir _build
cd _build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ".." -DAE_SIMD_SSE=42
