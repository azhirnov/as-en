
cd ../../..

# cleanup
rm -rf _build_e2k

export PATH=$PATH:/opt/mcst/lcc-1.27.21.e2k-v6.5.10-e2c3/bin/
export CC=lcc
export CXX=l++

mkdir _build_e2k
cd _build_e2k

cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DAE_ENABLE_VULKAN=ON "../AE"
cmake --build . --config Debug
read -p "press any key..."

