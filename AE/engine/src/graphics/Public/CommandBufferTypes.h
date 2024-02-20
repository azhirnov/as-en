// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Queue.h"
#include "graphics/Public/ImageDesc.h"
#include "graphics/Public/BufferDesc.h"
#include "graphics/Public/PipelineDesc.h"
#include "graphics/Public/RenderPassDesc.h"
#include "graphics/Public/IDs.h"
#include "graphics/Public/EResourceState.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/RenderStateEnums.h"
#include "graphics/Public/ShaderEnums.h"
#include "graphics/Public/VertexEnums.h"
#include "graphics/Public/BufferMemView.h"
#include "graphics/Public/ImageMemView.h"

namespace AE::Graphics
{

    struct ImageSubresourceRange
    {
        EImageAspect    aspectMask      = Default;
        MipmapLevel     baseMipLevel;
        ushort          mipmapCount     = 1;
        ImageLayer      baseLayer;
        ushort          layerCount      = 1;

        ImageSubresourceRange ()                                                __NE___ = default;

        ImageSubresourceRange (EImageAspect aspectMask,
                               MipmapLevel baseMipLevel, ImageLayer baseLayer)  __NE___ :
            aspectMask{aspectMask}, baseMipLevel{baseMipLevel}, baseLayer{baseLayer} {}

        ImageSubresourceRange (EImageAspect aspectMask,
                               MipmapLevel baseMipLevel, uint mipmapCount,
                               ImageLayer baseLayer, uint layerCount)           __NE___ :
            aspectMask{aspectMask},
            baseMipLevel{baseMipLevel}, mipmapCount{ushort(mipmapCount)},
            baseLayer{baseLayer}, layerCount{ushort(layerCount)} {}
    };


    struct ImageSubresourceLayers
    {
        EImageAspect    aspectMask      = Default;
        MipmapLevel     mipLevel;
        ImageLayer      baseLayer;
        ushort          layerCount      = 1;

        ImageSubresourceLayers ()                                               __NE___ = default;

        ImageSubresourceLayers (EImageAspect aspectMask, MipmapLevel mipLevel,
                                ImageLayer baseLayer, uint layerCount = 1)      __NE___ :
            aspectMask{aspectMask}, mipLevel{mipLevel},
            baseLayer{baseLayer}, layerCount{ushort(layerCount)} {}
    };


    struct BufferCopy
    {
        Bytes           srcOffset;
        Bytes           dstOffset;
        Bytes           size;

        BufferCopy ()                                               __NE___ = default;

        BufferCopy (Bytes srcOffset, Bytes dstOffset, Bytes size)   __NE___ :
            srcOffset{srcOffset}, dstOffset{dstOffset}, size{size} {}
    };

    // TODO: ImageCopyRegion, ImageCopySlices
    struct ImageCopy
    {
        ImageSubresourceLayers  srcSubres;
        uint3                   srcOffset;
        ImageSubresourceLayers  dstSubres;
        uint3                   dstOffset;
        uint3                   extent;

        ImageCopy () __NE___ = default;
    };


    struct BufferImageCopy
    {
        Bytes                   bufferOffset;
        uint                    bufferRowLength     = 0;    // pixels, must be 0 or >= imageExtent.x
        uint                    bufferImageHeight   = 0;    // pixels, must be 0 or >= imageExtent.y
        ImageSubresourceLayers  imageSubres;
        uint3                   imageOffset;
        uint3                   imageExtent;

        BufferImageCopy () __NE___ = default;
    };

    struct BufferImageCopy2
    {
        Bytes                   bufferOffset;
        Bytes                   rowPitch;
        Bytes                   slicePitch;
        ImageSubresourceLayers  imageSubres;
        uint3                   imageOffset;
        uint3                   imageExtent;

        BufferImageCopy2 () __NE___ = default;
    };


    struct ImageBlit
    {
        ImageSubresourceLayers  srcSubres;
        uint3                   srcOffset0;
        uint3                   srcOffset1;
        ImageSubresourceLayers  dstSubres;
        uint3                   dstOffset0;
        uint3                   dstOffset1;

        ImageBlit () __NE___ = default;
    };


    struct ImageResolve
    {
        ImageSubresourceLayers  srcSubres;
        uint3                   srcOffset;
        ImageSubresourceLayers  dstSubres;
        uint3                   dstOffset;
        uint3                   extent;

        ImageResolve () __NE___ = default;
    };


    struct RTShaderBindingTable
    {
        // non-portable
        #ifdef AE_ENABLE_VULKAN
            VkStridedDeviceAddressRegionKHR     raygen      {};
            VkStridedDeviceAddressRegionKHR     miss        {};
            VkStridedDeviceAddressRegionKHR     hit         {};
            VkStridedDeviceAddressRegionKHR     callable    {};

        #elif defined(AE_ENABLE_METAL)
            MetalIntersectionFnTable            intersectionTable;
            MetalVisibleFnTable                 visibleTable;

        #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
            // TODO

        #else
        #   error not implemented
        #endif

        RTShaderBindingTable () __NE___ = default;
    };
//-----------------------------------------------------------------------------



    struct DrawCmd
    {
        uint    vertexCount     = 0;
        uint    instanceCount   = 1;
        uint    firstVertex     = 0;
        uint    firstInstance   = 0;

        DrawCmd () __NE___ = default;
    };


    struct DrawIndexedCmd
    {
        uint    indexCount      = 0;
        uint    instanceCount   = 1;
        uint    firstIndex      = 0;
        int     vertexOffset    = 0;
        uint    firstInstance   = 0;

        DrawIndexedCmd () __NE___ = default;
    };


    struct DrawIndirectCmd
    {
        BufferID    indirectBuffer;
        Bytes       indirectBufferOffset;
        uint        drawCount               = 1;
        Bytes       stride;                 // sizeof(DrawIndirectCommand)

        DrawIndirectCmd () __NE___ = default;
    };


    struct DrawIndexedIndirectCmd
    {
        BufferID    indirectBuffer;
        Bytes       indirectBufferOffset;
        uint        drawCount               = 1;
        Bytes       stride;                 // sizeof(DrawIndexedIndirectCommand)

        DrawIndexedIndirectCmd () __NE___ = default;
    };


    struct DrawMeshTasksIndirectCmd
    {
        BufferID    indirectBuffer;
        Bytes       indirectBufferOffset;
        uint        drawCount;
        Bytes       stride;                 // sizeof(DrawMeshTasksIndirectCommand)

        DrawMeshTasksIndirectCmd () __NE___ = default;
    };


    struct DrawIndirectCountCmd
    {
        BufferID    indirectBuffer;
        Bytes       indirectBufferOffset;
        BufferID    countBuffer;
        Bytes       countBufferOffset;
        uint        maxDrawCount;
        Bytes       stride;                 // sizeof(DrawIndirectCommand)

        DrawIndirectCountCmd () __NE___ = default;
    };


    struct DrawIndexedIndirectCountCmd
    {
        BufferID    indirectBuffer;
        Bytes       indirectBufferOffset;
        BufferID    countBuffer;
        Bytes       countBufferOffset;
        uint        maxDrawCount;
        Bytes       stride;                 // sizeof(DrawIndexedIndirectCommand)

        DrawIndexedIndirectCountCmd () __NE___ = default;
    };


    struct DrawMeshTasksIndirectCountCmd
    {
        BufferID    indirectBuffer;
        Bytes       indirectBufferOffset;
        BufferID    countBuffer;
        Bytes       countBufferOffset;
        uint        maxDrawCount;
        Bytes       stride;                 // sizeof(DrawMeshTasksIndirectCommand)

        DrawMeshTasksIndirectCountCmd () __NE___ = default;
    };
//-----------------------------------------------------------------------------



    struct DispatchIndirectCommand
    {
        packed_uint3    groupCount;
    };
    StaticAssert( sizeof(DispatchIndirectCommand) == 12 );
    StaticAssert( alignof(DispatchIndirectCommand) == 4 );


    struct DrawIndirectCommand
    {
        uint    vertexCount;
        uint    instanceCount;
        uint    firstVertex;
        uint    firstInstance;              // non zero value requires 'FeatureSet::drawIndirectFirstInstance'
    };
    StaticAssert( sizeof(DrawIndirectCommand) == 16 );
    StaticAssert( alignof(DrawIndirectCommand) == 4 );


    struct DrawIndexedIndirectCommand
    {
        uint    indexCount;
        uint    instanceCount;
        uint    firstIndex;
        int     vertexOffset;
        uint    firstInstance;              // non zero value requires 'FeatureSet::drawIndirectFirstInstance'
    };
    StaticAssert( sizeof(DrawIndexedIndirectCommand) == 20 );
    StaticAssert( alignof(DrawIndexedIndirectCommand) == 4 );


    struct DrawMeshTasksIndirectCommand
    {
        packed_uint3    taskCount;
    };
    StaticAssert( sizeof(DrawMeshTasksIndirectCommand) == 12 );
    StaticAssert( alignof(DrawMeshTasksIndirectCommand) == 4 );


    struct TraceRayIndirectCommand
    {
        packed_uint3    dim;
    };
    StaticAssert( sizeof(TraceRayIndirectCommand) == 12 );
    StaticAssert( alignof(TraceRayIndirectCommand) == 4 );


    struct TraceRayIndirectCommand2
    {
      #ifdef AE_ENABLE_VULKAN
        VkDeviceAddress     raygenShaderRecordAddress;
        VkDeviceSize        raygenShaderRecordSize;
        VkDeviceAddress     missShaderBindingTableAddress;
        VkDeviceSize        missShaderBindingTableSize;
        VkDeviceSize        missShaderBindingTableStride;
        VkDeviceAddress     hitShaderBindingTableAddress;
        VkDeviceSize        hitShaderBindingTableSize;
        VkDeviceSize        hitShaderBindingTableStride;
        VkDeviceAddress     callableShaderBindingTableAddress;
        VkDeviceSize        callableShaderBindingTableSize;
        VkDeviceSize        callableShaderBindingTableStride;
        uint                width;
        uint                height;
        uint                depth;

      #elif defined(AE_ENABLE_METAL)
        // TODO

      #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
        // TODO

      #else
      # error not implemented
      #endif
    };
    #ifdef AE_ENABLE_VULKAN
      StaticAssert( sizeof(TraceRayIndirectCommand2) == 104 );
      StaticAssert( alignof(TraceRayIndirectCommand2) == 8 );
    #endif


    struct ASBuildIndirectCommand
    {
        // Triangles count, AABBs count, Instances count
        uint        primitiveCount;

        uint        primitiveOffset;
        uint        firstVertex;
        uint        transformOffset;
    };
    StaticAssert( sizeof(ASBuildIndirectCommand) == 16 );
    StaticAssert( alignof(ASBuildIndirectCommand) == 4 );


//-----------------------------------------------------------------------------



    enum class EStagingHeapType : ubyte
    {
        Static      = 1 << 0,
        Dynamic     = 1 << 1,
        Any         = Static | Dynamic,     // try static, then dynamic
    };


    //
    // Upload Buffer Description
    //
    struct UploadBufferDesc
    {
        Bytes               offset;
        Bytes               size            = UMax;     // UMax - remaining size
        Bytes               blockSize;                  // 0 - auto
        EStagingHeapType    heapType        = EStagingHeapType::Static;

        UploadBufferDesc ()                                     __NE___ = default;

        UploadBufferDesc (Bytes offset, Bytes size, Bytes blockSize = 0_b,
                          EStagingHeapType heapType = EStagingHeapType::Static) __NE___ :
            offset{offset}, size{size}, blockSize{blockSize}, heapType{heapType} {}

        UploadBufferDesc&   Offset (Bytes value)                __NE___ { offset    = value;                        return *this; }
        UploadBufferDesc&   DataSize (Bytes value)              __NE___ { size      = value;                        return *this; }
        UploadBufferDesc&   HeapType (EStagingHeapType value)   __NE___ { heapType  = value;                        return *this; }
        UploadBufferDesc&   StaticHeap ()                       __NE___ { heapType  = EStagingHeapType::Static;     return *this; }
        UploadBufferDesc&   DynamicHeap ()                      __NE___ { heapType  = EStagingHeapType::Dynamic;    return *this; }
        UploadBufferDesc&   BlockSize (Bytes value)             __NE___ { blockSize = value;                        return *this; }
        UploadBufferDesc&   MaxBlockSize ()                     __NE___ { ASSERT( size != UMax );  blockSize = DivCeil( size, GraphicsConfig::MaxStagingBufferParts );  return *this; }
    };
    using ReadbackBufferDesc = UploadBufferDesc;


    //
    // Upload Image Description
    //
    struct UploadImageDesc
    {
        uint3               imageOffset     {0};
        uint3               imageDim        {~0u};  // UMax - remaining size
        ImageLayer          arrayLayer;
        MipmapLevel         mipLevel;
        Bytes               dataRowPitch;           // 0 - auto
        Bytes               dataSlicePitch;         // 0 - auto
        EImageAspect        aspectMask      = EImageAspect::Color;  // must only have a single bit set
        EStagingHeapType    heapType        = EStagingHeapType::Dynamic;

        // TODO: setter

        UploadImageDesc () __NE___ = default;
    };
    using ReadbackImageDesc = UploadImageDesc;
//-----------------------------------------------------------------------------



    //
    // Buffer Stream
    //
    struct BufferStream
    {
    // variables
    public:
        Bytes               pos;
    private:
        BufferID            _bufferId;
        UploadBufferDesc    _desc;

    // methods
    public:
        BufferStream ()                                         __NE___ {}
        BufferStream (BufferID id, const UploadBufferDesc &desc)__NE___ : _bufferId{id}, _desc{desc} {}

        BufferStream (BufferID id,
                      Bytes offset, Bytes size, Bytes blockSize = 0_b,
                      EStagingHeapType heapType = EStagingHeapType::Static) __NE___ :
            _bufferId{id}, _desc{ offset, size, blockSize, heapType} {}

        BufferStream (const BufferStream &)                     __NE___ = default;
        BufferStream&  operator = (const BufferStream &)        __NE___ = default;

        BufferStream&  SetHeapType (EStagingHeapType type)      __NE___ { _desc.heapType = type;    return *this; }
        BufferStream&  SetBlockSize (Bytes value)               __NE___ { _desc.blockSize = value;  return *this; }
        BufferStream&  MaxBlockSize ()                          __NE___ { _desc.MaxBlockSize();     return *this; }

        ND_ BufferID            BufferId ()                     C_NE___ { return _bufferId; }
        ND_ Bytes               DataSize ()                     C_NE___ { return _desc.size; }
        ND_ Bytes               OffsetAndPos ()                 C_NE___ { return _desc.offset + pos; }
        ND_ Bytes               Begin ()                        C_NE___ { return _desc.offset; }
        ND_ Bytes               End ()                          C_NE___ { return _desc.offset + _desc.size; }
        ND_ Bytes               RemainSize ()                   C_NE___ { return _desc.size - pos; }
        ND_ EStagingHeapType    HeapType ()                     C_NE___ { return _desc.heapType; }
        ND_ Bytes               BlockSize ()                    C_NE___ { return _desc.blockSize; }

        ND_ auto const&         ToUploadDesc ()                 C_NE___ { return _desc; }
        ND_ auto const&         ToReadbackDesc ()               C_NE___ { return _desc; }

        ND_ bool                IsInitialized ()                C_NE___ { return _bufferId != Default; }
        ND_ bool                IsCompleted ()                  C_NE___ { return pos >= _desc.size; }
    };


    //
    // Image Stream
    //
    struct ImageStream
    {
    // variables
    public:
        packed_uint2        posYZ;
    private:
        ImageID             _imageId;
        UploadImageDesc     _desc;

    // methods
    public:
        ImageStream ()                                          __NE___ {}
        ImageStream (ImageID id, const UploadImageDesc &desc)   __NE___ : _imageId{id}, _desc{desc} {}

        ImageStream (const ImageStream &)                       __NE___ = default;
        ImageStream&  operator = (const ImageStream &)          __NE___ = default;

        ImageStream&  SetHeapType (EStagingHeapType type)       __NE___ { _desc.heapType = type;  return *this; }

        ND_ ImageID             ImageId ()                      C_NE___ { return _imageId; }
        ND_ uint3 const&        Begin ()                        C_NE___ { return _desc.imageOffset; }
        ND_ uint3               End ()                          C_NE___ { return _desc.imageOffset + _desc.imageDim; }
        ND_ uint3 const&        RegionSize ()                   C_NE___ { return _desc.imageDim; }
        ND_ Bytes               DataOffset ()                   C_NE___ { return posYZ[0] * _desc.dataRowPitch + posYZ[1] * _desc.dataSlicePitch; }
        ND_ EStagingHeapType    HeapType ()                     C_NE___ { return _desc.heapType; }

        ND_ auto const&         ToUploadDesc ()                 C_NE___ { return _desc; }
        ND_ auto const&         ToReadbackDesc ()               C_NE___ { return _desc; }

        ND_ bool                IsInitialized ()                C_NE___ { return _imageId != Default; }
        ND_ bool                IsCompleted ()                  C_NE___ { return IsInitialized() and (posYZ[1] >= _desc.imageDim.z); }
    };


    //
    // Video Image Stream
    //
    struct VideoImageStream
    {
    // variables
    public:
        packed_uint2        posYZ;
    private:
        VideoImageID        _imageId;
        UploadImageDesc     _desc;

    // methods
    public:
        VideoImageStream ()                                             __NE___ {}
        VideoImageStream (VideoImageID id, const UploadImageDesc &desc) __NE___ : _imageId{id}, _desc{desc} {}

        VideoImageStream (const VideoImageStream &)                     __NE___ = default;
        VideoImageStream&  operator = (const VideoImageStream &)        __NE___ = default;

        VideoImageStream&  SetHeapType (EStagingHeapType type)          __NE___ { _desc.heapType = type;  return *this; }

        ND_ VideoImageID        ImageId ()                              C_NE___ { return _imageId; }
        ND_ uint3 const&        Begin ()                                C_NE___ { return _desc.imageOffset; }
        ND_ uint3               End ()                                  C_NE___ { return _desc.imageOffset + _desc.imageDim; }
        ND_ uint3 const&        RegionSize ()                           C_NE___ { return _desc.imageDim; }
        ND_ Bytes               DataOffset ()                           C_NE___ { return posYZ[0] * _desc.dataRowPitch + posYZ[1] * _desc.dataSlicePitch; }
        ND_ EStagingHeapType    HeapType ()                             C_NE___ { return _desc.heapType; }

        ND_ auto const&         ToUploadDesc ()                         C_NE___ { return _desc; }
        ND_ auto const&         ToReadbackDesc ()                       C_NE___ { return _desc; }

        ND_ bool                IsInitialized ()                        C_NE___ { return _imageId != Default; }
        ND_ bool                IsCompleted ()                          C_NE___ { return IsInitialized() and (posYZ[1] >= _desc.imageDim.z); }
    };
//-----------------------------------------------------------------------------



    //
    // Vertex Stream
    //
    struct VertexStream
    {
        void*       mappedPtr   = null;     // mapped memory for host visible memory
        BufferID    id;                     // single buffer for all, bind it once
        Bytes       offset;                 // offset in buffer
        Bytes       size;                   // same as in request

        VertexStream () __NE___ = default;
    };
//-----------------------------------------------------------------------------



    //
    // Video Decode command
    //
    struct VideoDecodeCmd
    {
        VideoDecodeCmd () __NE___ = default;
    };


    //
    // Video Encode command
    //
    struct VideoEncodeCmd
    {
        VideoEncodeCmd () __NE___ = default;
    };
//-----------------------------------------------------------------------------



    enum class ECommandBufferType : ubyte
    {
        Primary_OneTimeSubmit       = 0,
        Secondary_RenderCommands    = 1,
        _Count,
        Unknown                     = 0xFF,
    };


    enum class ESubmitMode : ubyte
    {
        Auto    = 0,

        // Allows RG to accumulate batches to minimize CPU overhead.
        Deferred,

        // Submit batch immediately when all render tasks are completed.
        Immediately,

        // Block until batch is not submitted
        Force,

        Unknown = Auto,
    };



    //
    // Begin Frame Config
    //
    struct BeginFrameConfig
    {
        // limit for dynamic staging buffers
        struct {
            Bytes32u        write   {UMax};
            Bytes32u        read    {UMax};
        }   stagingBufferPerFrameLimits;

        BeginFrameConfig () __NE___ = default;
    };


} // AE::Graphics

