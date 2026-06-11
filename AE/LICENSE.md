
## Source code

The source code (including scripts, cmake files, shader code) with the header `Copyright (c) Zhirnov Andrey` is licensed under Apache-2.0.<br/>
See `Apache2-license.txt` or https://www.apache.org/licenses/LICENSE-2.0.txt.

The final license depends on third-party libraries. The build system contains definitions of which licenses are used in attached libs (`AE_LICENSE_*`).<br/>
Add the following code to your project to determine which license you can use.
```cpp
#define REQUIRE_APACHE_2
#include "base/Defines/DetectLicense.inl.h"
```
Compilation will fail if the required license is not compatible with third-party dependencies.<br/>
**There are no warranty that [license detection](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/base/Defines/DetectLicense.inl.h) works correctly or that license information is valid for third-party libraries.**

**Warning**: In cmake use only PUBLIC visibility for `target_link_libraries()`, otherwise it breaks license detection.

**Warning**: Shader source code require to enable allowed licenses by adding `#define AE_LICENSE_*` macros to pipeline script or into your source code before `#include`.


## Content

Binary data, such as a images, models, etc., should contain license information in a folder or in a file (if supported), otherwise it is third-party content without any license information.

File formats which has internal license information:
 * not implemented yet


## Generated content

Licensing rules for generated content are still under consideration.
