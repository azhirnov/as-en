cd ../../android

rmdir /Q /S ".gradle"
rmdir /Q /S ".idea"

rmdir /Q /S "demo\.cxx"
rmdir /Q /S "demo\build"

rmdir /Q /S "engine\.cxx"
rmdir /Q /S "engine\build"

rmdir /Q /S "test\.cxx"
rmdir /Q /S "test\build"

rmdir /Q /S "cicd\.cxx"
rmdir /Q /S "cicd\build"

rmdir /Q /S "rg-device\.cxx"
rmdir /Q /S "rg-device\build"

cd ../build_scripts/android
