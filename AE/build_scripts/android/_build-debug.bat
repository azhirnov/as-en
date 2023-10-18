cd ../../android
rmdir /Q /S ".gradle"
rmdir /Q /S ".idea"
rmdir /Q /S "build"
rmdir /Q /S "engine/build"
rmdir /Q /S "test/.cxx"
rmdir /Q /S "test/build"
rmdir /Q /S "demo/.cxx"
rmdir /Q /S "demo/build"

gradlew buildCMakeDebug[arm64-v8a]
::gradlew buildCMakeDebug[armeabi-v7a]
