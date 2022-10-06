cd ../../engine/external/android-clang/AndroidTest
rmdir /Q /S ".gradle"
rmdir /Q /S ".idea"
rmdir /Q /S "app/.cxx"
rmdir /Q /S "app/build"

gradlew build
