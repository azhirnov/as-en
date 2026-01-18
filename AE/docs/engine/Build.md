Build instructions.

## Install dependencies

### Android (PC)

When building from PC:
* Install [Android Studio](https://developer.android.com/studio) with NDK.

### Android (device)

When building from Android device:
* Install Linux terminal: **termux** ([github](https://github.com/termux/termux-app), [f-droid](https://f-droid.org/en/packages/com.termux/), [play store](https://play.google.com/store/apps/details?id=com.termux))
* In terminal install: git, clang, cmake.<br/>
  ```
  pkg install git
  pkg install clang
  pkg install cmake
  ```
* Optional: github and f-droid version of **termux** can access to external storage, allow this and clone repository to public path like `/sdcard/Code`.
* Optional: install any code editor.

### Linux

* In terminal run:
	```
	sudo apt install build-essential pkg-config libx11-dev libxcursor-dev \
		libxinerama-dev libgl1-mesa-dev libasound2-dev \
		libxi-dev libxrandr-dev yasm liburing-dev libpng-dev libbz2-dev libwayland-dev \
		libxkbcommon-dev libc++-dev libc++abi-dev \
		mesa-vulkan-drivers libvulkan1 vulkan-validationlayers \
		libpng-dev libzip-dev libbz2-dev
	```

* Optional: install clang
	```
	sudo apt install clang libc++-dev libc++abi-dev
	```

* Install [CMake](https://cmake.org/download/).
* Install [VSCode](https://code.visualstudio.com/) with CMake and C++ plugins.

### MacOS

* Install [CMake](https://cmake.org/download/)
* Install XCode.

### Windows

* Install [CMake](https://cmake.org/download/).
* Install [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/).
	* Alternative: install [VSCode](https://code.visualstudio.com/) with CMake and C++ plugins.
* Optional: install ClangCL in Visual Studio components.


## Build third party dependencies

Open folder `AE/engine/external` and run scripts for required platforms and compiler version.

Libraries and headers will be installed to the `AE/../AE-Bin/external` folder.

**Alternative**<br/>
Use `download-<platform>-<compiler>` scripts in `AE-Bin` folder to download and extract binaries.<br/>
Then [Build engine and samples](#Build-engine-and-samples)

### Android (PC)

`update-all-win.bat` - to load header-only sources.<br/>
`update-all-android-clang.bat` - to compile static (.a) libs.

### Android (device)

You should build dependencies on PC or download them.

### Linux

`update-all-linux.sh` - to load header-only sources.<br/>
`update-all-linux-x64-clang.sh` or `update-all-linux-x64-gcc.sh` - to compile static (.a) libs.

### MacOS

`update-all-macos.sh` - to load header-only sources.<br/>

**ARM64:**<br/>
`update-all-macos-arm64-clang.sh` - to compile static (.a) libs.

**x64 on ARM64 platform:**
<details><summary>How to install x64 compiler on ARM64</summary>

From [stackoverflow](https://stackoverflow.com/questions/67386941/using-x86-libraries-and-openmp-on-macos-arm64-architecture/67418208#67418208) :

```
# launch x86_64 shell
arch -x86_64 zsh
# install x86_64 variant of brew
arch -x86_64 /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
# install x86_64 variant of clang
arch -x86_64 /usr/local/bin/brew install llvm
# compile using x86_64 variant of clang
/usr/local/opt/llvm/bin/clang++ -arch x86_64 omp_ex.cpp
```
</details>

`update-all-macos-x64-onarm-clang.sh` - to compile static (.a) libs.

**x64:**<br/>
`update-all-macos-x64-clang.sh` - to compile static (.a) libs.

### Windows

`update-all-win.bat` - to load header-only sources.<br/>
`update-all-win-x64-msvc.bat` or `update-all-win-x64-clang.bat` - to compile static (.lib) and dynamic (.dll) libs.

To compile Metal shaders put [MetalTools for Windows](https://developer.apple.com/download/all/?q=metal%20developer%20tools%20for%20windows) into the folder `AE-Bin/external/win-x64/MetalTools`:
```
bin
  air-lld.exe
  metal.exe
lib
  clang
    ...
```


## Build engine and samples

Open folder `AE/build_scripts`, select target platform and compiler, for example `win_x64_clang`.
Then call `init_*` script to create solution or call `build_*` script to build project from console.

Run `HeaderGen` (*VS: in 'Engine/ToolApps'*) to generate Vulkan and OpenXR loader from current header files.

### Android (PC)

Script `AE/build_scripts/build-*.bat` will build project from console.<br/>
To use Android IDE open project in folder `AE/android`.

### Android (device)

For example you have folder structure:
```
/sdcard/Code
  AE      -- from git
  AE-Bin  -- from git or downloaded, see 'Build third party dependencies'
  AE-Temp -- from git or downloaded, see 'Compile resources'
  build   -- empty
```

Then, in **termux**:
```
cd /sdcard/Code/build
cmake -DAE_ENABLE_VULKAN=ON "../AE"
cmake --build . --config Debug
```

**Note**: app launcher is in progress.

## Compile resources

Projects which requires precompiled resources has cmake targets with `.PackRes` suffixes.<br/>
You should run it first to compile resources.

From console: `cmake --build . --config <config> --target <project>.PackRes`.<br/>
From IDE: build target `<project>.PackRes`.

Compiled resources will be stored in `AE/../AE-Temp` folder. They can be used on all platforms.


## Update file paths (optional)

Visual Studio, VSCode, Notepad++ and some other IDEs allows to open file in URL format like a `file://absolute/path/to/a/file.txt`, but it requires absolute paths. Source code may contain links to other source files or docs which is written as URLs.<br/>
Use IDE or another tool to replace path `[](https://github.com/azhirnov/as-en/blob/dev/AE/)` to an absolute path like a `file://C:/Projects/AE/`.


## Setup IDE and environment (optional)

Associate file extensions `.as`, `.glsl` with Visual Studio, Notepad++ or other IDE or text editor. This text editor will be used to open files by links like `file://path`.

### Visual Studio

Open `TOOLS- > Options...`, select `Text Editor -> File Extension` and add file extensions as a C++ source:<br/>
`.as` - AngelScript file with AsEn preprocessor.<br/>
`.ppln` - same as `.as`, used for shader pipelines.<br/>
`.glsl`, `.msl` - shader language.<br/>
`.glsl_dbg` - for shader debugger.<br/>
`.mm` - for ObjC code.


### VSCode

Already setup, see `AE/.vscode/settings.json`.
