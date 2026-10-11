echo "update 'tracy'..."
rm -rf "build"
rm -rf "../../../../../AE-Bin/external/linux-x64-clang20/tracy"
rm -rf "temp"
mkdir "build"
export CC=/usr/bin/clang-20
export CXX=/usr/bin/clang++-20
export CPP=/usr/bin/clang-cpp-20
export LD=/usr/bin/ld.lld-20
git clone --branch "v0.14.1" "https://github.com/wolfpld/tracy.git" "temp"
cp "temp/CMakeLists.txt" "temp/origin_CMakeLists.txt"
cp "tracy_CMakeLists.txt" "temp/CMakeLists.txt"
mkdir "../../../../../AE-Bin/external/linux-x64-clang20/tracy/bin"
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Debug -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-clang20/tracy"  || pauseOnError
cmake --build build --config Debug --target install -j 8 || pauseOnError
cmake -S temp -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release -DAE_SIMD_SSE=20 -DAE_SIMD_AES=0 -DCMAKE_INSTALL_PREFIX="../../../../../AE-Bin/external/linux-x64-clang20/tracy"  || pauseOnError
cmake --build build --config Release --target install -j 8 || pauseOnError

echo
build=0
while true; do
  read -r -n 1 -p "Build Tracy profiler? [Y/n] " answer
  echo
  case "$answer" in
    [Yy]) build=1; break ;;
    [Nn]) break ;;
  esac
done

if (( build )); then
	pushd "temp/profiler" >/dev/null || pause_on_error

	{
	  printf '%s\n' 'set(CMAKE_OBJECT_PATH_MAX 512)'
	  cat -- CMakeLists.txt
	} > CMakeLists.head
	mv -f -- CMakeLists.head CMakeLists.txt

	cmake -S . -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release
	cmake -S . -B build -G "Unix Makefiles" -DAE_ENABLE_COMPILER_WARNINGS=OFF -DCMAKE_BUILD_TYPE=Release || { popd >/dev/null || true; pauseOnError; }
	cmake --build build --config Release -j 8 || { popd >/dev/null || true; pauseOnError; }
	popd >/dev/null
fi
cp "temp/profiler/build/Release/tracy-profiler" "../../../../../AE-Bin/external/linux-x64-clang20/tracy/bin/tracy-profiler"
rm -rf "temp"
rm -rf "build"
read -p "press any key..."

exit 0

pauseOnError() {
	echo -e "\033[31m*** failed ***\033[0m"
	read -p "press any key..."
	exit 1
}
