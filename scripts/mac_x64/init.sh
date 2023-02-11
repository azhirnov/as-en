# warning: on ARM CPU use 'init_on_arm.sh'

cd ../..
rm -rf _build_x64

mkdir _build_x64
cd _build_x64
cmake -G "Xcode" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 -DCMAKE_OSX_ARCHITECTURES=x86_64 -DAE_SIMD_AVX=2 ".."
