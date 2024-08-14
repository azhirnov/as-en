Remote Graphics Device

## Overview

Implements graphics device for remote graphics backend in `Graphics` module.

[Features](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/RemoteGraphicsFeatures.md)


## How to use

Supported only local network!

### Desktop device

* Run executable.
* In log will be printed device IP, use it to connect to the remote graphics device.

### Android device

* Install APK ([project](https://github.com/azhirnov/as-en/blob/dev/AE/android/rg-device)).
* Run application.
* When initialized toast will show device IP, use it to connect to the remote graphics device.

### Host

* Compile project with `AE_ENABLE_REMOTE_GRAPHICS` (in cmake).
* Setup IP address for remote graphics device in `GraphicsCreateInfo::deviceAddr`.
* (optional) Compile `GraphicsLib` project without `AE_ENABLE_REMOTE_GRAPHICS` and set path to shared library to `GraphicsCreateInfo::graphicsLibPath`. It allows to draw UI on host.


## Troubleshooting

To avoid some problems RmG client must be successfully disconnected (with graphics deinitialization), otherwise it may cause some unexpected problems. To fix this restart the app.


