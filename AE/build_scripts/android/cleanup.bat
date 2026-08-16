cd ../../android

rmdir /Q /S ".gradle"
rmdir /Q /S ".idea"

rmdir /Q /S "demo\.cxx"
rmdir /Q /S "demo\build"


rmdir /Q /S "test\.cxx"
rmdir /Q /S "test\build"
rmdir /Q /S "engine\build"
rmdir /Q /S "engine_java\build"

rmdir /Q /S "remote-ctrl\.cxx"
rmdir /Q /S "remote-ctrl\build"

rmdir /Q /S "cicd\.cxx"
rmdir /Q /S "cicd\build"

rmdir /Q /S "rg-device\.cxx"
rmdir /Q /S "rg-device\build"

rmdir /Q /S "res_editor\.cxx"
rmdir /Q /S "res_editor\build"

rmdir /Q /S "../engine/external/android-clang/AndroidTest/.gradle"
cd ../build_scripts/android
