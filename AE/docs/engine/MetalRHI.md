Source folder: [Metal backend](https://github.com/azhirnov/as-en/blob/dev/AE/engine/src/graphics_rhi/Metal/)

Supported features:
- [x] Argument buffer
- [x] Ray query
- [x] Push constant emulation
- [x] GPU address
- [x] Metal 3
- [x] Async draw

TODO:
- [ ] variable rasterization rate
- [ ] non-tracked resources
- [ ] sparse memory
- [ ] ray tracing curves and other https://developer.apple.com/videos/play/wwdc2023/10128/, https://developer.apple.com/videos/play/wwdc2022/10105/
- [ ] mem profiling https://developer.apple.com/videos/play/wwdc2022/10106/
- [ ] offline compilation and optimization https://developer.apple.com/videos/play/wwdc2022/10102/
- [ ] optimization https://developer.apple.com/videos/play/wwdc2023/10127/
- [ ] indirect command buffer
- [ ] minimize useResources/useHeap calls
- [ ] upscaling
- [ ] tensor https://developer.apple.com/documentation/metal/MTLTensor
- [ ] min/max sampler https://developer.apple.com/documentation/metal/mtlsamplerreductionmode
- [ ] ml commands https://developer.apple.com/documentation/metal/mtl4machinelearningcommandencoder
- [ ] https://developer.apple.com/documentation/metal/resource-synchronization
