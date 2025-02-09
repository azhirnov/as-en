
## Setup Remote Rendering

1 - Compile with `AE_ENABLE_REMOTE_GRAPHICS` (in cmake):
* [ResourceEditor](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor)

2 - Compile without `AE_ENABLE_REMOTE_GRAPHICS` (in cmake):
* [RemoteGraphicsDevice](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/remote_graphics_device) - for desktop
* [RG-Device](https://github.com/azhirnov/as-en/blob/dev/AE/android/rg-device) - for android
* [GraphicsLib](https://github.com/azhirnov/as-en/blob/dev/AE/engine/tools/graphics_rhi_shared) - optional, used for UI rendering

3 - Run `ResourceEditor`, wait until 'res_editor_cfg.as' was generated, then close it.

4 - Run `RemoteGraphicsDevice` on desktop or `RG-Device` on Android, you will see device IP address in your local network, save it.

5 - Modify 'res_editor_cfg.as':<br/>
`cfg.RemoteDeviceIpAddress( ... )` - write remote device IP address.<br/>
`cfg.GraphicsLibPath( "..." )` - write path to `GraphicsLib`, it will be used to render UI on host.<br/>

6 - Run `ResourceEditor` again, it will connect to remote device.


## Setup Remote Input

1 - Compile [Remote-ctrl](https://github.com/azhirnov/as-en/blob/dev/AE/android/remote-ctrl) Android project. In [file](https://github.com/azhirnov/as-en/blob/dev/AE/samples/android_remote_control/Main.cpp) set server address and port.

2 - Run `ResourceEditor`, wait until 'res_editor_cfg.as' was generated, then close it.

3 - Modify 'res_editor_cfg.as':<br/>
`cfg.RemoteInputServerPort( 0 )` - set port which is used by input server.

4 - Run `Remote-ctrl` app and `ResourceEditor`, they connect to each other and Android sensors will be used as input source.
