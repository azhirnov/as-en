// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    TransferCtx --> DirectTransferCtx   --> BarrierMngr --> Vulkan device
                \-> IndirectTransferCtx --> BarrierMngr --> Backed commands
*/

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Vulkan/Commands/VBaseIndirectContext.h"
# include "graphics/Vulkan/Commands/VBaseDirectContext.h"
# include "graphics/Vulkan/Commands/VAccumBarriers.h"

namespace AE::Graphics::_hidden_
{

    //
    // Vulkan Direct Transfer Context implementation
    //

    class _VDirectTransferCtx : public VBaseDirectContext
    {
    private:
        using Validator_t   = TransferContextValidation;

    // methods
    public:
        void  ClearColorImage (VkImage image, const VkClearColorValue &color, ArrayView<VkImageSubresourceRange> ranges)                    __Th___;
        void  ClearDepthStencilImage (VkImage image, const VkClearDepthStencilValue &depthStencil, ArrayView<VkImageSubresourceRange> ranges)__Th___;

        void  FillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data)                                                             __Th___;
        void  UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data)                                                    __Th___;

        void  CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges)                                           __Th___;
        void  CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges)                                                 __Th___;

        void  CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges)                                 __Th___;
        void  CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges)                                 __Th___;

        void  BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions)                               __Th___;
        void  ResolveImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageResolve> regions)                                          __Th___;

        void  GenerateMipmaps (VkImage image, const uint3 &dimension, ArrayView<ImageSubresourceRange> ranges)                              __Th___;

        ND_ VkCommandBuffer EndCommandBuffer ()                                                                                             __Th___;
        ND_ VCommandBuffer  ReleaseCommandBuffer ()                                                                                         __Th___;

        VBARRIERMNGR_INHERIT_VKBARRIERS

    protected:
        _VDirectTransferCtx (const RenderTask &task, VCommandBuffer cmdbuf, DebugLabel dbg)                                                 __Th___ : VBaseDirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::Transfer } {}
    };



    //
    // Vulkan Indirect Transfer Context implementation
    //

    class _VIndirectTransferCtx : public VBaseIndirectContext
    {
    private:
        using Validator_t   = TransferContextValidation;

    // methods
    public:
        void  ClearColorImage (VkImage image, const VkClearColorValue &color, ArrayView<VkImageSubresourceRange> ranges)                    __Th___;
        void  ClearDepthStencilImage (VkImage image, const VkClearDepthStencilValue &depthStencil, ArrayView<VkImageSubresourceRange> ranges)__Th___;

        void  FillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data)                                                             __Th___;
        void  UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data)                                                    __Th___;

        void  CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges)                                           __Th___;
        void  CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges)                                                 __Th___;

        void  CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges)                                 __Th___;
        void  CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges)                                 __Th___;

        void  BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions)                               __Th___;
        void  ResolveImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageResolve> regions)                                          __Th___;

        void  GenerateMipmaps (VkImage image, const uint3 &dimension, ArrayView<ImageSubresourceRange> ranges)                              __Th___;

        ND_ VBakedCommands      EndCommandBuffer ()                                                                                         __Th___;
        ND_ VSoftwareCmdBufPtr  ReleaseCommandBuffer ()                                                                                     __Th___;

        VBARRIERMNGR_INHERIT_VKBARRIERS

    protected:
        _VIndirectTransferCtx (const RenderTask &task, VSoftwareCmdBufPtr cmdbuf, DebugLabel dbg)                                           __Th___ : VBaseIndirectContext{ task, RVRef(cmdbuf), dbg, ECtxType::Transfer } {}
    };



    //
    // Vulkan Transfer Context implementation
    //

    template <typename CtxImpl>
    class _VTransferContextImpl : public CtxImpl, public ITransferContext
    {
    // types
    public:
        using CmdBuf_t          = typename CtxImpl::CmdBuf_t;
    private:
        static constexpr uint   _LocalArraySize         = 16;

        using RawCtx            = CtxImpl;
        using AccumBar          = VAccumBarriers< _VTransferContextImpl< CtxImpl >>;
        using DeferredBar       = VAccumDeferredBarriersForCtx< _VTransferContextImpl< CtxImpl >>;
        using Validator_t       = TransferContextValidation;


    // methods
    public:
        explicit _VTransferContextImpl (const RenderTask &task, CmdBuf_t cmdbuf = Default, DebugLabel dbg = Default)    __Th___;

        _VTransferContextImpl ()                                                                                        = delete;
        _VTransferContextImpl (const _VTransferContextImpl &)                                                           = delete;

        using RawCtx::ClearColorImage;
        using RawCtx::ClearDepthStencilImage;

        void  ClearColorImage (ImageID image, const RGBA32f &color, ArrayView<ImageSubresourceRange> ranges)            __Th___ { _ClearColorImage( image, color, ranges ); }
        void  ClearColorImage (ImageID image, const RGBA32i &color, ArrayView<ImageSubresourceRange> ranges)            __Th___ { _ClearColorImage( image, color, ranges ); }
        void  ClearColorImage (ImageID image, const RGBA32u &color, ArrayView<ImageSubresourceRange> ranges)            __Th___ { _ClearColorImage( image, color, ranges ); }
        void  ClearDepthStencilImage (ImageID image, const DepthStencil &, ArrayView<ImageSubresourceRange> ranges)     __Th___;

        using RawCtx::UpdateBuffer;
        void  UpdateBuffer (BufferID buffer, Bytes offset, Bytes size, const void* data)                                __Th_OV;

        using RawCtx::FillBuffer;
        void  FillBuffer (BufferID buffer, Bytes offset, Bytes size, uint data)                                         __Th_OV;

        using RawCtx::CopyBuffer;
        using RawCtx::CopyImage;

        void  CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges)                         __Th_OV;
        void  CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges)                               __Th_OV;

        using RawCtx::CopyBufferToImage;
        using RawCtx::CopyImageToBuffer;

        void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges)               __Th_OV;
        void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges)               __Th_OV;

        void  CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges)              __Th_OV;
        void  CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges)              __Th_OV;

        using RawCtx::BlitImage;

        void  BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter filter, ArrayView<ImageBlit> regions)          __Th_OV;

        void  ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions)                        __Th___;

        void  UploadBuffer (BufferID buffer, const UploadBufferDesc &desc, OUT BufferMemView &memView)                  __Th_OV;
        void  UploadImage  (ImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)                      __Th_OV { _UploadImage( image, desc, OUT memView ); }
        void  UploadImage  (VideoImageID image, const UploadImageDesc &desc, OUT ImageMemView &memView)                 __Th_OV { _UploadImage( image, desc, OUT memView ); }

        void  UploadBuffer (BufferStream &stream, OUT BufferMemView &memView)                                           __Th_OV;
        void  UploadImage (ImageStream &stream, OUT ImageMemView &memView)                                              __Th_OV { _UploadImage( stream, OUT memView ); }
        void  UploadImage (VideoImageStream &stream, OUT ImageMemView &memView)                                         __Th_OV { _UploadImage( stream, OUT memView ); }

        ND_ Promise<BufferMemView>  ReadbackBuffer (BufferID buffer, const ReadbackBufferDesc &desc)                    __Th_OV;
        ND_ Promise<ImageMemView>   ReadbackImage (ImageID image, const ReadbackImageDesc &desc)                        __Th_OV { return _ReadbackImage( image, desc ); }
        ND_ Promise<ImageMemView>   ReadbackImage (VideoImageID image, const ReadbackImageDesc &desc)                   __Th_OV { return _ReadbackImage( image, desc ); }

        ND_ Promise<BufferMemView>  ReadbackBuffer (INOUT BufferStream &stream)                                         __Th_OV;
        ND_ Promise<ImageMemView>   ReadbackImage (INOUT ImageStream &stream)                                           __Th_OV { return _ReadbackImage( stream ); }
        ND_ Promise<ImageMemView>   ReadbackImage (INOUT VideoImageStream &stream)                                      __Th_OV { return _ReadbackImage( stream ); }

        ND_ bool  MapHostBuffer (BufferID buffer, Bytes offset, INOUT Bytes &size, OUT void* &mapped)                   __Th___;
        ND_ bool  UpdateHostBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data)                      __Th_OV;

        ND_ Promise<ArrayView<ubyte>>  ReadHostBuffer (BufferID buffer, Bytes offset, Bytes size)                       __Th_OV;

        using RawCtx::GenerateMipmaps;

        void  GenerateMipmaps (ImageID image, EResourceState state)                                                     __Th_OV;
        void  GenerateMipmaps (ImageID image, ArrayView<ImageSubresourceRange> ranges, EResourceState state)            __Th_OV;

        using ITransferContext::UpdateHostBuffer;
        using ITransferContext::UploadBuffer;
        using ITransferContext::UploadImage;
        using ITransferContext::UpdateBuffer;

        uint3  MinImageTransferGranularity ()                                                                           C_NE_OF;

        VBARRIERMNGR_INHERIT_BARRIERS

    private:
        template <typename ID>
        void  _UploadImage (ID image, const UploadImageDesc &desc, OUT ImageMemView &memView)                           __Th___;

        template <typename StreamType>
        void  _UploadImage (INOUT StreamType &stream, OUT ImageMemView &memView)                                        __Th___;

        template <typename ID>
        ND_ Promise<ImageMemView>  _ReadbackImage (ID image, const ReadbackImageDesc &desc)                             __Th___;

        template <typename StreamType>
        ND_ Promise<ImageMemView>  _ReadbackImage (INOUT StreamType &stream)                                            __Th___;

        template <typename ColType>
        void  _ClearColorImage (ImageID image, const ColType &color, ArrayView<ImageSubresourceRange> ranges)                                           __Th___;

        static void  _ConvertImageSubresourceRange (OUT VkImageSubresourceRange& dst, const ImageSubresourceRange& src, const ImageDesc &desc)          __NE___;
        static void  _ConvertImageSubresourceLayer (OUT VkImageSubresourceLayers &dst, const ImageSubresourceLayers &src, const ImageDesc &desc)        __NE___;
        static void  _ConvertBufferImageCopy (OUT VkBufferImageCopy& dst, const BufferImageCopy& src, const ImageDesc &desc)                            __NE___;
        static void  _ConvertBufferImageCopy (OUT VkBufferImageCopy& dst, const BufferImageCopy2& src, const ImageDesc &desc, const PixelFormatInfo &)  __Th___;

        ND_ bool  _MapHostBuffer (BufferID bufferId, INOUT Bytes &offset, INOUT Bytes &size, OUT VulkanMemoryObjInfo &memInfo)                          __Th___;
    };

} // AE::Graphics::_hidden_
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
    using VDirectTransferContext    = Graphics::_hidden_::_VTransferContextImpl< Graphics::_hidden_::_VDirectTransferCtx >;
    using VIndirectTransferContext  = Graphics::_hidden_::_VTransferContextImpl< Graphics::_hidden_::_VIndirectTransferCtx >;

} // AE::Graphics


namespace AE::Graphics::_hidden_
{
/*
=================================================
    constructor
=================================================
*/
    template <typename C>
    _VTransferContextImpl<C>::_VTransferContextImpl (const RenderTask &task, CmdBuf_t cmdbuf, DebugLabel dbg) __Th___ :
        RawCtx{ task, RVRef(cmdbuf), dbg }
    {
        Validator_t::CtxInit( task.GetQueueMask() );
    }

/*
=================================================
    _ClearColorImage
=================================================
*/
    template <typename C>
    template <typename ColType>
    void  _VTransferContextImpl<C>::_ClearColorImage (ImageID imageId, const ColType &color, ArrayView<ImageSubresourceRange> ranges) __Th___
    {
        auto&   img = _GetResourcesOrThrow( imageId );
        VALIDATE_GCTX( ClearColorImage( img.Description(), ranges ));

        VkClearColorValue                                       clear_value;
        FixedArray<VkImageSubresourceRange, _LocalArraySize>    vk_ranges;
        const ImageDesc &                                       desc    = img.Description();

        MemCopy( OUT clear_value, color );

        for (usize i = 0; i < ranges.size(); ++i)
        {
            auto&   src = ranges[i];
            auto&   dst = vk_ranges.emplace_back();

            _ConvertImageSubresourceRange( OUT dst, src, desc );

            if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
            {
                RawCtx::ClearColorImage( img.Handle(), clear_value, vk_ranges );
                vk_ranges.clear();
            }
        }

        if ( vk_ranges.size() )
            RawCtx::ClearColorImage( img.Handle(), clear_value, vk_ranges );
    }

/*
=================================================
    ClearDepthStencilImage
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::ClearDepthStencilImage (ImageID imageId, const DepthStencil &depthStencil, ArrayView<ImageSubresourceRange> ranges) __Th___
    {
        auto&   img = _GetResourcesOrThrow( imageId );
        VALIDATE_GCTX( ClearDepthStencilImage( img.Description(), ranges ));

        VkClearDepthStencilValue                                clear_value;
        FixedArray<VkImageSubresourceRange, _LocalArraySize>    vk_ranges;
        const ImageDesc &                                       desc    = img.Description();

        clear_value.depth   = depthStencil.depth;
        clear_value.stencil = depthStencil.stencil;

        for (usize i = 0; i < ranges.size(); ++i)
        {
            auto&   src = ranges[i];
            auto&   dst = vk_ranges.emplace_back();

            _ConvertImageSubresourceRange( OUT dst, src, desc );

            if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
            {
                RawCtx::ClearDepthStencilImage( img.Handle(), clear_value, vk_ranges );
                vk_ranges.clear();
            }
        }

        if ( vk_ranges.size() )
            RawCtx::ClearDepthStencilImage( img.Handle(), clear_value, vk_ranges );
    }

/*
=================================================
    FillBuffer
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::FillBuffer (BufferID bufferId, Bytes offset, Bytes size, uint data) __Th___
    {
        auto&   buf = _GetResourcesOrThrow( bufferId );
        VALIDATE_GCTX( FillBuffer( buf.Description(), offset, size ));

        const Bytes buf_size = buf.Size();
        offset  = Min( offset, buf_size );
        size    = Min( size, buf_size - offset );

        RawCtx::FillBuffer( buf.Handle(), offset, size, data );
    }

/*
=================================================
    UpdateBuffer
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::UpdateBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data) __Th___
    {
        auto&   buf = _GetResourcesOrThrow( bufferId );
        VALIDATE_GCTX( UpdateBuffer( buf.Description(), offset, size, data ));

        RawCtx::UpdateBuffer( buf.Handle(), offset, size, data );
    }

/*
=================================================
    ReadHostBuffer
=================================================
*/
    template <typename C>
    Promise<ArrayView<ubyte>>  _VTransferContextImpl<C>::ReadHostBuffer (BufferID bufferId, Bytes offset, Bytes size) __Th___
    {
        VulkanMemoryObjInfo mem_info;
        CHECK_ERR( _MapHostBuffer( bufferId, INOUT offset, INOUT size, OUT mem_info ));

        ArrayView<ubyte>    mem_view = ArrayView<ubyte>{ Cast<ubyte>(mem_info.mappedPtr + offset), usize(size) };

        if_unlikely( not AllBits( mem_info.flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ))
        {
            GCTX_CHECK( offset + size <= mem_info.size );
            this->_mngr.GetStagingManager().AcquireMappedMemory( GetFrameId(), mem_info.memory, mem_info.offset, size );
        }

        return Threading::MakePromiseFromValue( mem_view,
                                                Tuple{ this->_mngr.GetBatchRC() },
                                                "VTransferContext::ReadHostBuffer",
                                                ETaskQueue::PerFrame
                                               );
    }

/*
=================================================
    MapHostBuffer
=================================================
*/
    template <typename C>
    bool  _VTransferContextImpl<C>::MapHostBuffer (BufferID bufferId, Bytes offset, INOUT Bytes &size, OUT void* &mapped) __Th___
    {
        VulkanMemoryObjInfo mem_info;
        bool    res = _MapHostBuffer( bufferId, offset, INOUT size, OUT mem_info );
        mapped = mem_info.mappedPtr + offset;
        return res;
    }

    template <typename C>
    bool  _VTransferContextImpl<C>::_MapHostBuffer (BufferID bufferId, INOUT Bytes &offset, INOUT Bytes &size, OUT VulkanMemoryObjInfo &memInfo) __Th___
    {
        auto&   buf = _GetResourcesOrThrow( bufferId );
        VALIDATE_GCTX( MapHostBuffer( buf.Description(), offset, size ));

        auto&   mem = _GetResourcesOrThrow( buf.MemoryId() );
        CHECK_ERR( mem.GetMemoryInfo( OUT memInfo ));
        CHECK_ERR( memInfo.mappedPtr != null );

        const Bytes buf_size = buf.Size();
        offset  = Min( offset, buf_size );
        size    = Min( size, buf_size - offset );

        ASSERT( size > 0 );
        return true;
    }

/*
=================================================
    UpdateHostBuffer
=================================================
*/
    template <typename C>
    bool  _VTransferContextImpl<C>::UpdateHostBuffer (BufferID bufferId, Bytes offset, Bytes size, const void* data) __Th___
    {
        VulkanMemoryObjInfo mem_info;
        CHECK_ERR( _MapHostBuffer( bufferId, INOUT offset, INOUT size, OUT mem_info ));

        MemCopy( OUT mem_info.mappedPtr + offset, data, size );

        if_unlikely( not AllBits( mem_info.flags, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT ))
        {
            VkMappedMemoryRange range;
            range.sType     = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
            range.pNext     = null;
            range.memory    = mem_info.memory;
            range.offset    = VkDeviceSize(mem_info.offset + offset);
            range.size      = VkDeviceSize(size);

            GCTX_CHECK( offset + size <= mem_info.size );

            auto&   dev = this->_mngr.GetDevice();
            VK_CHECK( dev.vkFlushMappedMemoryRanges( dev.GetVkDevice(), 1, &range ));
        }
        return true;
    }

/*
=================================================
    CopyBuffer
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::CopyBuffer (BufferID srcBuffer, BufferID dstBuffer, ArrayView<BufferCopy> ranges) __Th___
    {
        auto  [src_buf, dst_buf] = _GetResourcesOrThrow( srcBuffer, dstBuffer );
        VALIDATE_GCTX( CopyBuffer( src_buf.Description(), dst_buf.Description(), ranges ));

        FixedArray<VkBufferCopy, _LocalArraySize>   vk_ranges;
        const Bytes                                 src_size    = src_buf.Size();
        const Bytes                                 dst_size    = dst_buf.Size();

        for (usize i = 0; i < ranges.size(); ++i)
        {
            auto&   src = ranges[i];
            auto&   dst = vk_ranges.emplace_back();

            dst.srcOffset   = VkDeviceSize(Min( src.srcOffset, src_size ));
            dst.dstOffset   = VkDeviceSize(Min( src.dstOffset, dst_size ));
            dst.size        = VkDeviceSize(Min( src.size, src_size - src.srcOffset, dst_size - src.dstOffset ));

            if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
            {
                RawCtx::CopyBuffer( src_buf.Handle(), dst_buf.Handle(), vk_ranges );
                vk_ranges.clear();
            }
        }

        if ( vk_ranges.size() )
            RawCtx::CopyBuffer( src_buf.Handle(), dst_buf.Handle(), vk_ranges );
    }

/*
=================================================
    CopyImage
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::CopyImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageCopy> ranges) __Th___
    {
        auto  [src_img, dst_img] = _GetResourcesOrThrow( srcImage, dstImage );
        VALIDATE_GCTX( CopyImage( src_img.Description(), dst_img.Description(), ranges ));

        FixedArray<VkImageCopy, _LocalArraySize>    vk_ranges;
        const ImageDesc &                           src_desc    = src_img.Description();
        const ImageDesc &                           dst_desc    = dst_img.Description();

        for (usize i = 0; i < ranges.size(); ++i)
        {
            auto&   src = ranges[i];
            auto&   dst = vk_ranges.emplace_back();

            dst.srcOffset   = { int(src.srcOffset.x), int(src.srcOffset.y), int(src.srcOffset.z) };
            dst.dstOffset   = { int(src.dstOffset.x), int(src.dstOffset.y), int(src.dstOffset.z) };
            dst.extent      = { src.extent.x,         src.extent.y,         src.extent.z };

            _ConvertImageSubresourceLayer( OUT dst.srcSubresource, src.srcSubres, src_desc );
            _ConvertImageSubresourceLayer( OUT dst.dstSubresource, src.dstSubres, dst_desc );

            if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
            {
                RawCtx::CopyImage( src_img.Handle(), dst_img.Handle(), vk_ranges );
                vk_ranges.clear();
            }
        }

        if ( vk_ranges.size() )
            RawCtx::CopyImage( src_img.Handle(), dst_img.Handle(), vk_ranges );
    }

/*
=================================================
    CopyBufferToImage
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy> ranges) __Th___
    {
        auto  [src_buf, dst_img] = _GetResourcesOrThrow( srcBuffer, dstImage );
        VALIDATE_GCTX( CopyBufferToImage( src_buf.Description(), dst_img.Description(), ranges ));

        FixedArray<VkBufferImageCopy, _LocalArraySize>  vk_ranges;
        const ImageDesc &                               img_desc    = dst_img.Description();

        for (usize i = 0; i < ranges.size(); ++i)
        {
            auto&   src = ranges[i];
            auto&   dst = vk_ranges.emplace_back();

            _ConvertBufferImageCopy( OUT dst, src, img_desc );

            if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
            {
                RawCtx::CopyBufferToImage( src_buf.Handle(), dst_img.Handle(), vk_ranges );
                vk_ranges.clear();
            }
        }

        if ( vk_ranges.size() )
            RawCtx::CopyBufferToImage( src_buf.Handle(), dst_img.Handle(), vk_ranges );
    }

/*
=================================================
    CopyBufferToImage
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::CopyBufferToImage (BufferID srcBuffer, ImageID dstImage, ArrayView<BufferImageCopy2> ranges) __Th___
    {
        auto  [src_buf, dst_img] = _GetResourcesOrThrow( srcBuffer, dstImage );
        VALIDATE_GCTX( CopyBufferToImage( src_buf.Description(), dst_img.Description(), ranges ));

        FixedArray<VkBufferImageCopy, _LocalArraySize>  vk_ranges;
        const ImageDesc &                               img_desc    = dst_img.Description();
        const PixelFormatInfo &                         fmt_info    = EPixelFormat_GetInfo( img_desc.format );

        for (usize i = 0; i < ranges.size(); ++i)
        {
            auto&   src = ranges[i];
            auto&   dst = vk_ranges.emplace_back();

            _ConvertBufferImageCopy( OUT dst, src, img_desc, fmt_info );

            if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
            {
                RawCtx::CopyBufferToImage( src_buf.Handle(), dst_img.Handle(), vk_ranges );
                vk_ranges.clear();
            }
        }

        if ( vk_ranges.size() )
            RawCtx::CopyBufferToImage( src_buf.Handle(), dst_img.Handle(), vk_ranges );
    }

/*
=================================================
    CopyImageToBuffer
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy> ranges) __Th___
    {
        auto  [src_img, dst_buf] = _GetResourcesOrThrow( srcImage, dstBuffer );
        VALIDATE_GCTX( CopyImageToBuffer( src_img.Description(), dst_buf.Description(), ranges ));

        FixedArray<VkBufferImageCopy, _LocalArraySize>  vk_ranges;
        const ImageDesc &                               img_desc    = src_img.Description();

        for (usize i = 0; i < ranges.size(); ++i)
        {
            auto&   src = ranges[i];
            auto&   dst = vk_ranges.emplace_back();

            _ConvertBufferImageCopy( OUT dst, src, img_desc );

            if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
            {
                RawCtx::CopyImageToBuffer( src_img.Handle(), dst_buf.Handle(), vk_ranges );
                vk_ranges.clear();
            }
        }

        if ( vk_ranges.size() )
            RawCtx::CopyImageToBuffer( src_img.Handle(), dst_buf.Handle(), vk_ranges );
    }

/*
=================================================
    CopyImageToBuffer
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::CopyImageToBuffer (ImageID srcImage, BufferID dstBuffer, ArrayView<BufferImageCopy2> ranges) __Th___
    {
        auto  [src_img, dst_buf] = _GetResourcesOrThrow( srcImage, dstBuffer );
        VALIDATE_GCTX( CopyImageToBuffer( src_img.Description(), dst_buf.Description(), ranges ));

        FixedArray<VkBufferImageCopy, _LocalArraySize>  vk_ranges;
        const ImageDesc &                               img_desc    = src_img.Description();
        const PixelFormatInfo &                         fmt_info    = EPixelFormat_GetInfo( img_desc.format );

        for (usize i = 0; i < ranges.size(); ++i)
        {
            auto&   src = ranges[i];
            auto&   dst = vk_ranges.emplace_back();

            _ConvertBufferImageCopy( OUT dst, src, img_desc, fmt_info );

            if_unlikely( vk_ranges.size() == vk_ranges.capacity() )
            {
                RawCtx::CopyImageToBuffer( src_img.Handle(), dst_buf.Handle(), vk_ranges );
                vk_ranges.clear();
            }
        }

        if ( vk_ranges.size() )
            RawCtx::CopyImageToBuffer( src_img.Handle(), dst_buf.Handle(), vk_ranges );
    }

/*
=================================================
    BlitImage
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::BlitImage (ImageID srcImage, ImageID dstImage, EBlitFilter blitFilter, ArrayView<ImageBlit> regions) __Th___
    {
        auto  [src_img, dst_img] = _GetResourcesOrThrow( srcImage, dstImage );
        VALIDATE_GCTX( BlitImage( src_img.Description(), dst_img.Description(), blitFilter, regions ));

        FixedArray<VkImageBlit, _LocalArraySize>    vk_regions;
        const ImageDesc &                           src_desc    = src_img.Description();
        const ImageDesc &                           dst_desc    = dst_img.Description();
        const VkFilter                              filter      = VEnumCast( blitFilter );

        for (usize i = 0; i < regions.size(); ++i)
        {
            auto&   src = regions[i];
            auto&   dst = vk_regions.emplace_back();

            dst.srcOffsets[0] = { int(src.srcOffset0.x), int(src.srcOffset0.y), int(src.srcOffset0.z) };
            dst.srcOffsets[1] = { int(src.srcOffset1.x), int(src.srcOffset1.y), int(src.srcOffset1.z) };
            dst.dstOffsets[0] = { int(src.dstOffset0.x), int(src.dstOffset0.y), int(src.dstOffset0.z) };
            dst.dstOffsets[1] = { int(src.dstOffset1.x), int(src.dstOffset1.y), int(src.dstOffset1.z) };
            this->_ConvertImageSubresourceLayer( OUT dst.srcSubresource, src.srcSubres, src_desc );
            this->_ConvertImageSubresourceLayer( OUT dst.dstSubresource, src.dstSubres, dst_desc );

            if_unlikely( vk_regions.size() == vk_regions.capacity() )
            {
                RawCtx::BlitImage( src_img.Handle(), dst_img.Handle(), filter, vk_regions );
                vk_regions.clear();
            }
        }

        if ( vk_regions.size() )
            RawCtx::BlitImage( src_img.Handle(), dst_img.Handle(), filter, vk_regions );
    }

/*
=================================================
    ResolveImage
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::ResolveImage (ImageID srcImage, ImageID dstImage, ArrayView<ImageResolve> regions) __Th___
    {
        auto  [src_img, dst_img] = _GetResourcesOrThrow( srcImage, dstImage );
        VALIDATE_GCTX( ResolveImage( src_img.Description(), dst_img.Description(), regions ));

        FixedArray<VkImageResolve, _LocalArraySize> vk_regions;
        const ImageDesc &                           src_desc    = src_img.Description();
        const ImageDesc &                           dst_desc    = dst_img.Description();

        for (usize i = 0; i < regions.size(); ++i)
        {
            auto&   src = regions[i];
            auto&   dst = vk_regions.emplace_back();

            dst.srcOffset   = { int(src.srcOffset.x), int(src.srcOffset.y), int(src.srcOffset.z) };
            dst.dstOffset   = { int(src.dstOffset.x), int(src.dstOffset.y), int(src.dstOffset.z) };
            dst.extent      = { src.extent.x,         src.extent.y,         src.extent.z         };

            this->_ConvertImageSubresourceLayer( OUT dst.srcSubresource, src.srcSubres, src_desc );
            this->_ConvertImageSubresourceLayer( OUT dst.dstSubresource, src.dstSubres, dst_desc );

            if_unlikely( vk_regions.size() == vk_regions.capacity() )
            {
                RawCtx::ResolveImage( src_img.Handle(), dst_img.Handle(), vk_regions );
                vk_regions.clear();
            }
        }

        if ( vk_regions.size() )
            RawCtx::ResolveImage( src_img.Handle(), dst_img.Handle(), vk_regions );
    }

/*
=================================================
    GenerateMipmaps
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::GenerateMipmaps (ImageID image, EResourceState state) __Th___
    {
        auto&                   img = _GetResourcesOrThrow( image );
        ImageDesc const&        desc = img.Description();
        ImageSubresourceRange   range;

        range.aspectMask    = EPixelFormat_ToImageAspect( desc.format );
        range.baseMipLevel  = 0_mipmap;
        range.baseLayer     = 0_layer;
        range.layerCount    = ushort(desc.arrayLayers.Get());

        if ( state != Default and state != EResourceState::BlitSrc )
        {
            range.mipmapCount = 1;
            this->ImageBarrier( image, state, EResourceState::BlitSrc, range );
            this->CommitBarriers();
        }

        range.mipmapCount = ushort(desc.maxLevel.Get());

        VALIDATE_GCTX( GenerateMipmaps( img.Description(), {range} ));
        RawCtx::GenerateMipmaps( img.Handle(), desc.dimension, {range} );
    }

    template <typename C>
    void  _VTransferContextImpl<C>::GenerateMipmaps (ImageID image, ArrayView<ImageSubresourceRange> ranges, EResourceState state) __Th___
    {
        auto&               img = _GetResourcesOrThrow( image );
        ImageDesc const&    desc = img.Description();

        if ( state != Default and state != EResourceState::BlitSrc )
        {
            for (auto range : ranges) {
                range.mipmapCount = 1;
                this->ImageBarrier( image, state, EResourceState::BlitSrc, range );
            }
            this->CommitBarriers();
        }

        VALIDATE_GCTX( GenerateMipmaps( img.Description(), ranges ));
        RawCtx::GenerateMipmaps( img.Handle(), desc.dimension, ranges );
    }

/*
=================================================
    MinImageTransferGranularity
=================================================
*/
    template <typename C>
    uint3  _VTransferContextImpl<C>::MinImageTransferGranularity () C_NE___
    {
        return uint3{this->_mngr.GetQueue()->minImageTransferGranularity};
    }

/*
=================================================
    _ConvertImageSubresourceRange
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::_ConvertImageSubresourceRange (OUT VkImageSubresourceRange& dst, const ImageSubresourceRange& src, const ImageDesc &desc) __NE___
    {
        dst.aspectMask      = VEnumCast( src.aspectMask );
        dst.baseMipLevel    = Min( src.baseMipLevel.Get(), desc.maxLevel.Get()-1 );
        dst.levelCount      = Min( src.mipmapCount, desc.maxLevel.Get() - src.baseMipLevel.Get() );
        dst.baseArrayLayer  = Min( src.baseLayer.Get(), desc.arrayLayers.Get()-1 );
        dst.layerCount      = Min( src.layerCount, desc.arrayLayers.Get() - src.baseLayer.Get() );
    }

/*
=================================================
    _ConvertBufferImageCopy
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::_ConvertBufferImageCopy (OUT VkBufferImageCopy& dst, const BufferImageCopy& src, const ImageDesc &desc) __NE___
    {
        dst.bufferOffset        = VkDeviceSize(src.bufferOffset);
        dst.bufferRowLength     = src.bufferRowLength;
        dst.bufferImageHeight   = src.bufferImageHeight;
        _ConvertImageSubresourceLayer( OUT dst.imageSubresource, src.imageSubres, desc );
        dst.imageOffset         = { int(src.imageOffset.x), int(src.imageOffset.y), int(src.imageOffset.z) };
        dst.imageExtent         = { src.imageExtent.x, src.imageExtent.y, src.imageExtent.z };
    }

/*
=================================================
    _ConvertBufferImageCopy
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::_ConvertBufferImageCopy (OUT VkBufferImageCopy& dst, const BufferImageCopy2& src, const ImageDesc &desc, const PixelFormatInfo &fmtInfo) __Th___
    {
        const uint  bits_per_block = AllBits( src.imageSubres.aspectMask, EImageAspect::Stencil ) ?
                                        fmtInfo.bitsPerBlock2 : fmtInfo.bitsPerBlock;

        dst.bufferOffset        = VkDeviceSize(src.bufferOffset);
        dst.bufferRowLength     = ImageUtils::RowLength( src.rowPitch, bits_per_block, fmtInfo.TexBlockDim() );
        dst.bufferImageHeight   = ImageUtils::ImageHeight( src.slicePitch, src.rowPitch, fmtInfo.TexBlockDim() );
        _ConvertImageSubresourceLayer( OUT dst.imageSubresource, src.imageSubres, desc );
        dst.imageOffset         = { int(src.imageOffset.x), int(src.imageOffset.y), int(src.imageOffset.z) };
        dst.imageExtent         = { src.imageExtent.x, src.imageExtent.y, src.imageExtent.z };

        GCTX_CHECK( dst.bufferRowLength == 0 or dst.bufferRowLength >= dst.imageExtent.width );
        GCTX_CHECK( dst.bufferImageHeight == 0 or dst.bufferImageHeight >= dst.imageExtent.height );
    }

/*
=================================================
    _ConvertImageSubresourceLayer
=================================================
*/
    template <typename C>
    void  _VTransferContextImpl<C>::_ConvertImageSubresourceLayer (OUT VkImageSubresourceLayers &dst, const ImageSubresourceLayers &src, const ImageDesc &desc) __NE___
    {
        dst.aspectMask      = VEnumCast( src.aspectMask );
        dst.mipLevel        = Min( src.mipLevel.Get(), desc.maxLevel.Get()-1 );
        dst.baseArrayLayer  = Min( src.baseLayer.Get(), desc.arrayLayers.Get()-1 );
        dst.layerCount      = Min( src.layerCount, desc.arrayLayers.Get() - src.baseLayer.Get() );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    ClearColorImage
=================================================
*/
    inline void  _VDirectTransferCtx::ClearColorImage (VkImage image, const VkClearColorValue &color, ArrayView<VkImageSubresourceRange> ranges) __Th___
    {
        ASSERT( _NoPendingBarriers() );
        VALIDATE_GCTX( ClearColorImage( image, ranges ));

        vkCmdClearColorImage( _cmdbuf.Get(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, uint(ranges.size()), ranges.data() );
    }

/*
=================================================
    ClearDepthStencilImage
=================================================
*/
    inline void  _VDirectTransferCtx::ClearDepthStencilImage (VkImage image, const VkClearDepthStencilValue &depthStencil, ArrayView<VkImageSubresourceRange> ranges) __Th___
    {
        ASSERT( _NoPendingBarriers() );
        VALIDATE_GCTX( ClearDepthStencilImage( image, ranges ));

        vkCmdClearDepthStencilImage( _cmdbuf.Get(), image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &depthStencil, uint(ranges.size()), ranges.data() );
    }

/*
=================================================
    FillBuffer
=================================================
*/
    inline void  _VDirectTransferCtx::FillBuffer (VkBuffer buffer, Bytes offset, Bytes size, uint data) __Th___
    {
        ASSERT( _NoPendingBarriers() );
        VALIDATE_GCTX( FillBuffer( buffer, offset, size ));

        vkCmdFillBuffer( _cmdbuf.Get(), buffer, VkDeviceSize(offset), VkDeviceSize(size), data );
    }

/*
=================================================
    UpdateBuffer
=================================================
*/
    inline void  _VDirectTransferCtx::UpdateBuffer (VkBuffer buffer, Bytes offset, Bytes size, const void* data) __Th___
    {
        ASSERT( _NoPendingBarriers() );
        VALIDATE_GCTX( UpdateBuffer( buffer, offset, size, data ));

        vkCmdUpdateBuffer( _cmdbuf.Get(), buffer, VkDeviceSize(offset), VkDeviceSize(size), data );
    }

/*
=================================================
    CopyBuffer
=================================================
*/
    inline void  _VDirectTransferCtx::CopyBuffer (VkBuffer srcBuffer, VkBuffer dstBuffer, ArrayView<VkBufferCopy> ranges) __Th___
    {
        ASSERT( _NoPendingBarriers() );
        VALIDATE_GCTX( CopyBuffer( srcBuffer, dstBuffer, ranges ));

        vkCmdCopyBuffer( _cmdbuf.Get(), srcBuffer, dstBuffer, uint(ranges.size()), ranges.data() );
        // TODO vkCmdCopyBuffer2KHR
    }

/*
=================================================
    CopyImage
=================================================
*/
    inline void  _VDirectTransferCtx::CopyImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageCopy> ranges) __Th___
    {
        ASSERT( _NoPendingBarriers() );
        VALIDATE_GCTX( CopyImage( srcImage, dstImage, ranges ));

        vkCmdCopyImage( _cmdbuf.Get(), srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                        dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        uint(ranges.size()), ranges.data() );
        // TODO vkCmdCopyImage2KHR
    }

/*
=================================================
    CopyBufferToImage
=================================================
*/
    inline void  _VDirectTransferCtx::CopyBufferToImage (VkBuffer srcBuffer, VkImage dstImage, ArrayView<VkBufferImageCopy> ranges) __Th___
    {
        ASSERT( _NoPendingBarriers() );
        VALIDATE_GCTX( CopyBufferToImage( srcBuffer, dstImage, ranges ));

        vkCmdCopyBufferToImage( _cmdbuf.Get(), srcBuffer, dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, uint(ranges.size()), ranges.data() );
        // TODO vkCmdCopyBufferToImage2KHR
    }

/*
=================================================
    CopyImageToBuffer
=================================================
*/
    inline void  _VDirectTransferCtx::CopyImageToBuffer (VkImage srcImage, VkBuffer dstBuffer, ArrayView<VkBufferImageCopy> ranges) __Th___
    {
        ASSERT( _NoPendingBarriers() );
        VALIDATE_GCTX( CopyImageToBuffer( srcImage, dstBuffer, ranges ));

        vkCmdCopyImageToBuffer( _cmdbuf.Get(), srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dstBuffer, uint(ranges.size()), ranges.data() );
        // TODO vkCmdCopyImageToBuffer2KHR
    }

/*
=================================================
    BlitImage
=================================================
*/
    inline void  _VDirectTransferCtx::BlitImage (VkImage srcImage, VkImage dstImage, VkFilter filter, ArrayView<VkImageBlit> regions) __Th___
    {
        ASSERT( _NoPendingBarriers() );
        VALIDATE_GCTX( BlitImage( srcImage, dstImage, filter, regions ));

        vkCmdBlitImage( _cmdbuf.Get(), srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                        dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        uint(regions.size()), regions.data(), filter );
        // TODO vkCmdBlitImage2KHR
    }

/*
=================================================
    ResolveImage
=================================================
*/
    inline void  _VDirectTransferCtx::ResolveImage (VkImage srcImage, VkImage dstImage, ArrayView<VkImageResolve> regions) __Th___
    {
        ASSERT( _NoPendingBarriers() );
        VALIDATE_GCTX( ResolveImage( srcImage, dstImage, regions ));

        vkCmdResolveImage( _cmdbuf.Get(), srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                            dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                            uint(regions.size()), regions.data() );
        // TODO vkCmdResolveImage2KHR
    }


} // AE::Graphics::_hidden_

# include "graphics/Private/TransferContextImpl.h"

#endif // AE_ENABLE_VULKAN
