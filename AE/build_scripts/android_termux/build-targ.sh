#!/data/data/com.termux/files/usr/bin/bash

set -u

build_config="Debug"
target="Tests.Base"

ask_build_config() {
  printf 'build config [dbg/dev/pf/rel]: '
  read -r cfg

  case "$cfg" in
    dev|Develop|develop)
      build_config="Develop"
      ;;
    pf|Profile|profile)
      build_config="Profile"
      ;;
    rel|Release|release)
      build_config="Release"
      ;;
    dbg|Debug|debug|*)
      build_config="Debug"
      ;;
  esac
}

ask_target() {
  printf 'target (default Tests.Base): '
  read -r input_target
  target="${input_target:-Tests.Base}"
}

is_yes() {
  case "$1" in
    Y|y)
      return 0
      ;;
    *)
      return 1
      ;;
  esac
}

build_test_loop() {
  ask_target
  echo "target: $target"

  while true; do
    while ! cmake --build . -j 8 --target "$target"; do
      read -r -p "Build failed. Retry? [Y/n] " answer
      answer="${answer:-y}"

      case "$answer" in
        y|Y|yes|YES|Yes)
          echo "Retrying build..."
          continue
          ;;
        *)
          echo "Build aborted."
          return 1
          ;;
      esac
    done
		  
    while true; do
      ctest -R "$target" -V

      read -r -p "Rebuild and run again? [Y/n] " answer
      answer="${answer:-y}"

      case "$answer" in
        y|Y|yes|YES|Yes)
          echo "Continuing..."
          continue
          ;;
        *)
          echo "Loop aborted."
          return 1
          ;;
      esac
    done

    return 0
  done
}

main() {
  mkdir -p ~/AEbuild
  cd ~/AEbuild

  while true; do
    if cmake -DCMAKE_BUILD_TYPE="${build_config}" \
             -DAE_ENABLE_VULKAN=ON                \
             -DAE_ENGINE_BASE_PCH=ON			  \
             -DAE_ANDROID_CONSOLE_MODE=ON         \
			 -DAE_SIMD_NEON_HALF=1				  \
			 -DAE_SIMD_NEON_BF16=0				  \
             -DAE_ARM_ARCH=82                     \
             -DAE_SIMD_SVE=0                      \
             -DAE_SIMD_SME=0                      \
             -S "/sdcard/Code/AE"                 \
             -B .; then
        break
    fi

    read -r -p "Retry? [Y/n] " answer
    answer="${answer:-y}"
        
    case "$answer" in
      y|Y|yes|YES|Yes)
        echo "Continuing..."
        continue
        ;;
      *)
        echo "Loop aborted."
        return 1
        ;;
    esac
  done
		
  ctest -N | sed -nE 's/^[[:space:]]*Test[[:space:]]+#[0-9]+:[[:space:]]*//p'

  build_test_loop || return 1
}

ask_build_config

echo "build config: $build_config"

main
status=$?

echo
if [ "$status" -eq 0 ]; then
  echo "finished successfully"
else
  echo "FAILED with code $status"
fi

if [ "$target" = "Tests.GraphicsRHI" ]; then
    cd /sdcard/Code/AE-Data/tests/graphics_rhi/vulkan
    ls
fi

printf 'exit? [Y/n] '
read -r answ
answ="${answ:-y}"

if is_yes "$answ"; then
  exit 0
fi

exec bash
