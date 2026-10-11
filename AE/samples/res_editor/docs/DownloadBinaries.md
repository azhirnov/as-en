
## Release

AsEn-ShaderEditor project has precompiled binaries in [release](https://github.com/azhirnov/AsEn-ShaderEditor/releases) tab.


## Folder structure

When ResourceEditor compiled with `AE_PORTABLE_APP=ON` it requires different folder structure than in [main project git](https://github.com/azhirnov/as-en/tree/dev/AE/samples/res_editor).

```
src                 -- git: AE/samples/res_editor/_data
docs                -- git: AE/samples/res_editor/docs  -- optional
res_editor_data     -- git: AE-Data/res_editor_data  -- optional
shadertoy_data      -- git: AE-Data/shadertoy_data  -- optional
shared_data         -- git: AE/engine/shared_data
ui                  -- unpack resources from CI (or AE-Temp/samples/res_editor)
- vk_ui_pipelines.bin
- controls.bin
bin
- ResourceEditor      -- executable from CI (or build/bin/<config>/ResourceEditor)
- GraphicsRHI-shared  -- shared lib from CI, optional, see 'Remote version'
```

## Android folder structure

Android version targets API28 with unlimited storage access.

* Install apk.
* In AppInfo grand Storage permission.
* On first run app will try to create config in `/sdcard/as-en/res_editor_cfg.as` or use existing.
* Without storage permission used local path: `/sdcard/Android/data/AE.ShaderEditor/files/res_editor_cfg.as`.
* In `res_editor_cfg.as` set `vfs_path` to folder with resources like described in [Folder structure](#Folder-structure).<br/>
  You can use sources from `/sdcard/Core/AE/samples/res_editor` but it requires to update other pathes:
	- `local_path` to `/sdcard/Core/AE/samples/res_editor/_data/`
	- `vfs_path` to `/sdcard/Core/AE-Data/`
	- `shader_data_path` to `/sdcard/Core/AE/engine/shared_data/`
* Run app.


## Download resources

Minimal set of resources which required to run all samples.
Many samples can run without it.

Use script `AE-Data/download-resources.*`.

For AsEn-ShaderEditor project run `download-resources.*` in project root directory.
