Build instructions.

## Download third party dependencies

Script `AE/build_scripts/clone_3party.bat` will copy git repositories to local folder `AE/../3party`.

You can replace path to your local git server to get stable and safe connection.

Currently script is written only for Windows platform, on other platforms you can get access through the git-server.


## Build third party dependencies

Open folder `AE/engine/external` and run scripts for required platforms and compiler version.

Libraries and headers will be installed to `AE/../AE-Data/external` folder.


### Android

`update-all-win.bat` - to load header-only sources.<br/>
`update-all-android-clang.bat` - to compile static (.a) libs.

### Windows

`update-all-win.bat` - to load header-only sources.<br/>
`update-all-win-x64-msvc143.bat` - to compile static (.lib) and dynamic (.dll) libs.

To compile Metal shaders put [MetalTools for Windows](https://developer.apple.com/download/all/?q=metal%20developer%20tools%20for%20windows) into the folder `AE-Data/external/macos/MetalTools`:
```
bin
  air-lld.exe
  metal.exe
lib
  clang
    ...
```

### Linux

`update-all-linux.sh` - to load header-only sources.<br/>
`update-all-linux-x64-clang15.sh` or `update-all-linux-x64-gcc12.sh` - to compile static (.a) libs.

### MacOS

`update-all-macos.sh` - to load header-only sources.<br/>

**ARM64:**<br/>
`update-all-macos-arm64-clang15.sh` - to compile static (.a) libs.

**x64 on ARM64 platform:**<br/>
`update-all-macos-x64-onarm-clang15.sh` - to compile static (.a) libs.

**x64:**<br/>
`update-all-macos-x64-clang15.sh` - to compile static (.a) libs.


## Build engine and samples

Open folder `AE/build_scripts`, select target platform and compiler, then call `init` script to create solution or call `build` script to build project from console.

### Android

Script `AE/build_scripts/build-*.bat` will build project from console.<br/>
To use Android IDE open project in folder `AE/android` from IDE.


## Compile resources

Projects which is require precompiled resources has cmake targets with `.PackRes` suffixes.<br/>
You should run it first to compile resources.

From console: `cmake --build . --config <config> --target <project>.PackRes`.<br/>
From IDE: build target `<project>.PackRes`. 

Compiled resources will be stored in `AE/../AE-Temp` folder. They can be used on all platforms.


## Update file paths (optional)

Visual Studio and some other IDEs and programs (Notepad++) allows to open file in URL format like a `file://absolute/path/to/a/file.txt`, but it requires absolute paths. Source code may contain links to other source files or docs which is written as URLs.<br/>
Use IDE or another tool to replace placeholder `file://<path>/...` to a valid absolute path like a `file://C:/Projects/AE/...`.

