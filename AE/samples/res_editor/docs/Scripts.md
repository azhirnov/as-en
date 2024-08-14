
## ResourceEditor Scripts

First type of scripts is used to define render passes, camera, scene, render targets, etc.
The [`scripts`](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/scripts) folder contains this type of scripts.

The script is executed once when loaded, then a sequence of the graphics/compute passes is created and rendering begins.

See [`res_editor.as`](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/res_editor.as) file with built-in docs and see [Samples](Samples.md) with examples.


## Pipeline Scripts

Second type of script is used to define pipelines which will be selected by the main script.
The [`pipelines`](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines) folder contains this type of scripts.

See 'Pipelines' tab in [Resource Compilation](https://github.com/azhirnov/as-en/blob/dev/AE/docs/engine/ResourceCompilation.md) docs and '[pipeline_compiler.as](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/pipeline_compiler.as)' file with built-in docs.


## Startup Config

`res_editor_cfg.as` file is auto-generated on first start.

It is used to configure ResEditor folders.

All documentation are in comments to the methods.
