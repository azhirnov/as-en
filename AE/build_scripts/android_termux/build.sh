#!/data/data/com.termux/files/usr/bin/bash

set -u

build_config="Debug"

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

run_utility() {
  target="$1"

  printf 'run %s? [y/N] ' "$target"
  read -r answ

  if is_yes "$answ"; then
    cd ~/AEbuild
    cmake --build . -j 8 --target "$target"
  fi
}

run_test() {
  target="$1"

  printf 'run %s? [y/N] ' "$target"
  read -r answ

  if is_yes "$answ"; then
	  cd ~/AEbuild
	  
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

		  read -r -p "Rebuild and run again? [y/N] " answer

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
  fi
  return 0
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

  run_test 'Tests.Base'

  run_utility 'Tests.GraphicsRHI.PackRes'
  if run_test 'Tests.GraphicsRHI'; then
    cd "/sdcard/Code/AE-Data/tests/graphics_rhi/vulkan"
    ls
  fi
	  
  run_test 'Tests.Scripting'
  run_test 'Tests.Networking'
  run_test 'Tests.CoopMat'
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

printf 'exit? [Y/n] '
read -r answ
answ="${answ:-y}"

if is_yes "$answ"; then
  exit 0
fi

exec bash
