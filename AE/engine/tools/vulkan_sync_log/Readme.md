## Vulkan Synchronizations Logger

### Features

* Capture Vulkan functions which transit resource state or use resource for read/write access (see examples).
* Capture resource usage on each draw call (`PRINT_ALL_DS` macros).
* Capture debug markers and debug groups (`ENABLE_DBG_LABEL` macros).
* Converts a timeline semaphore with a value to a unique ID, this improve readability.
* Result is stable on multiple executions and can be used in tests.


### How to use

```
#include "VulkanSyncLog.h"

void  Init ()
{
    ...
    VulkanSyncLog::Initialize( ... );
    VulkanSyncLog::Enable();
}

void  RenderFrame ()
{
    // get log of previous frame
    String  log;
    VulkanSyncLog::GetLog( OUT log );
    ...
]
```

### Example

**Copy command**
```
CopyBufferToImage
  src:       'SSWB {f:0} {q:Graphics}'
  dst:       'SrcImage'
  dstLayout: TRANSFER_DST_OPTIMAL
    copy { [0, 16384) } ---> { off:(0, 0, 0), ext:(64, 64, 1), mip:0, layers:[0, 1), COLOR }
```

**Draw/Dispatch commands**
```
Dispatch
  descriptors:
    image: 'Image-1', layout: GENERAL
```

**Queue family acquire/release operation**
```
// in queue 0 (graphics)
PipelineBarrier2
  dependencyFlags: 0
  ImageBarrier:
    name:    'Image-0'
    stage:   FRAGMENT_SHADER ---> COMPUTE_SHADER
    access:  SHADER_SAMPLED_READ ---> SHADER_STORAGE_READ | SHADER_STORAGE_WRITE
    layout:  SHADER_READ_ONLY_OPTIMAL ---> GENERAL
    queue:   0 ---> 2  (release)

// in queue 2 (async compute)
PipelineBarrier2
  dependencyFlags: 0
  ImageBarrier:
    name:    'Image-0'
    stage:   FRAGMENT_SHADER ---> COMPUTE_SHADER
    access:  SHADER_SAMPLED_READ ---> SHADER_STORAGE_READ | SHADER_STORAGE_WRITE
    layout:  SHADER_READ_ONLY_OPTIMAL ---> GENERAL
    queue:   0 ---> 2  (acquire)
```

**Synchronizations inside RenderPass**
```
BeginRenderPass2
  renderPass:  'render-pass'
  color attachment:
    view:    'ImageView'
    image:   'Image'
    layout:  SHADER_READ_ONLY_OPTIMAL ---> COLOR_ATTACHMENT_OPTIMAL
    loadOp:  CLEAR
  dependency:
    subpass: External ---> 0
    stage:   FRAGMENT_SHADER ---> COLOR_ATTACHMENT_OUTPUT
    access:  SHADER_SAMPLED_READ ---> COLOR_ATTACHMENT_READ | COLOR_ATTACHMENT_WRITE

EndRenderPass2
  attachment:
    view:    'ImageView'
    image:   'Image'
    layout:  COLOR_ATTACHMENT_OPTIMAL ---> SHADER_READ_ONLY_OPTIMAL
    storeOp: STORE
  dependency:
    subpass: 0 ---> External
    stage:   COLOR_ATTACHMENT_OUTPUT ---> FRAGMENT_SHADER
    access:  COLOR_ATTACHMENT_READ | COLOR_ATTACHMENT_WRITE ---> SHADER_SAMPLED_READ
```

**Timeline semaphore**
```
// in cmdbuf-1
signalSemaphore = {
  'sync-3',  stage: ALL_COMMANDS
}

// in cmdbuf-2
waitSemaphore = {
  'sync-3',  stage: ALL_COMMANDS
}
```
