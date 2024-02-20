// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#   define STBUFMNGR        VStagingBufferManager

#elif defined(AE_ENABLE_METAL)
#   define STBUFMNGR        MStagingBufferManager

#elif defined(AE_ENABLE_REMOTE_GRAPHICS)
#   define STBUFMNGR        RStagingBufferManager

#else
#   error not implemented
#endif
//-----------------------------------------------------------------------------


/*
=================================================
    constructor
=================================================
*/
    STBUFMNGR::STBUFMNGR (ResourceManager_t &resMngr) __NE___ :
        _resMngr{ resMngr }
    {
        StaticAssert( SizePerQueue_t{}.max_size() == _QueueCount );
    }

/*
=================================================
    destructor
=================================================
*/
    STBUFMNGR::~STBUFMNGR () __NE___
    {
      #if defined(AE_ENABLE_VULKAN)
        ASSERT( _static.memoryForWrite == Default );
        ASSERT( _static.memoryForRead  == Default );
        ASSERT( _vstream.memory == Default );

        ASSERT( _memRanges.ranges.empty() );

      #elif defined(AE_ENABLE_METAL)
        ASSERT( not _static.memory );

      #elif defined(AE_ENABLE_REMOTE_GRAPHICS)

      #else
      # error not implemented
      #endif

        ASSERT( _static.buffersForWrite.empty() );
        ASSERT( _static.buffersForRead.empty() );

        ASSERT( _dynamic.write.buffers.available.empty() );
        ASSERT( _dynamic.write.buffers.current.empty() );
        ASSERT( _dynamic.read.buffers.available.empty() );
        ASSERT( _dynamic.read.buffers.current.empty() );
    }

/*
=================================================
    Initialize
=================================================
*/
    bool  STBUFMNGR::Initialize (const GraphicsCreateInfo &info) __NE___
    {
        Deinitialize();

        _memSizeAlign = Bytes16u{_resMngr.GetDevice().GetDeviceProperties().res.minNonCoherentAtomSize};

        // staging buffer for host to device requires coherent memory
        CHECK_ERR( _resMngr.IsSupported( EMemoryType::Unified ) or _resMngr.IsSupported( EMemoryType::HostCoherent ));

        CHECK_ERR( _CreateStaticBuffers( info ));
        CHECK_ERR( _InitDynamicBuffers( info ));
        CHECK_ERR( _InitVertexStream( info ));

        return true;
    }

/*
=================================================
    Deinitialize
=================================================
*/
    void  STBUFMNGR::Deinitialize () __NE___
    {
      #ifdef AE_ENABLE_VULKAN
        _static.memoryForWrite  = Default;
        _static.memoryForRead   = Default;
        _vstream.memory         = Default;
        {
            EXLOCK( _memRanges.guard );
            _memRanges.ranges.clear();
        }

      #elif defined(AE_ENABLE_METAL)
        _static.memory = null;
        _vstream.memory = null;

      #elif defined(AE_ENABLE_REMOTE_GRAPHICS)

      #else
      # error not implemented
      #endif

        for (auto& sb : _static.buffersForWrite) {
            _resMngr.ImmediatelyRelease( sb.buffer );
        }
        for (auto& sb : _static.buffersForRead) {
            _resMngr.ImmediatelyRelease( sb.buffer );
        }
        _static.buffersForWrite.clear();
        _static.buffersForRead.clear();

        _static.writeSize   = Default;
        _static.readSize    = Default;


        const auto  ReleaseDynBuffers = [this] (DynamicPerType &dyn)
        {{
            for (auto& sb : dyn.buffers.current) {
                _resMngr.ImmediatelyRelease( sb.buffer );
            }
            dyn.buffers.current.clear();

            for (auto& db : dyn.buffers.available) {
                _resMngr.ImmediatelyRelease( db.id );
            }
            dyn.buffers.available.clear();

            dyn.maxSize = 0_b;
        }};

        ReleaseDynBuffers( _dynamic.write );
        ReleaseDynBuffers( _dynamic.read );

        _dynamic.gfxAllocator   = null;
        _dynamic.blockSize      = 0_b;

        for (auto& sb : _vstream.buffers) {
            _resMngr.ImmediatelyRelease( sb.buffer );
        }
    }

/*
=================================================
    OnBeginFrame
=================================================
*/
    void  STBUFMNGR::OnBeginFrame (FrameUID frameId, const BeginFrameConfig &cfg) __NE___
    {
        DEBUG_ONLY( _frameId.store( frameId ); )

        // reset static buffers
        for (uint q = 0; q < _QueueCount; ++q)
        {
            const uint  i = frameId.Index() * _QueueCount + q;

            _static.buffersForWrite[i].size.store( 0_b );
            _static.buffersForRead[i].size.store( 0_b );
        }

        _vstream.buffers[frameId.Index()].size.store( 0_b );


        // reset dynamic buffers
        _dynamic.write.maxPerFrame.store( cfg.stagingBufferPerFrameLimits.write );
        _dynamic.write.usedPerFrame.store( 0_b );

        _dynamic.read.maxPerFrame.store( cfg.stagingBufferPerFrameLimits.read );
        _dynamic.read.usedPerFrame.store( 0_b );


        // invalidate mapped memory
      #ifdef AE_ENABLE_VULKAN
        {
            EXLOCK( _memRanges.guard );
            auto&   ranges = _memRanges.ranges[ frameId.Index() ];

            if ( not ranges.empty() )
            {
                auto&   dev = _resMngr.GetDevice();
                VK_CHECK( dev.vkInvalidateMappedMemoryRanges( dev.GetVkDevice(), uint(ranges.size()), ranges.data() ));
                ranges.clear();
            }
        }
      #endif
    }

/*
=================================================
    OnEndFrame
----
    don't reset buffer size - they may be used between 'EndFrame' and 'BeginFrame' tasks
=================================================
*/
    void  STBUFMNGR::OnEndFrame (FrameUID frameId) __NE___
    {
        ASSERT( _frameId.load() == frameId );

      #ifdef AE_ENABLE_VULKAN
        // invalidate static staging buffer memory
        if_unlikely( not _static.isReadCoherent )
        {
            for (uint q = 0; q < _QueueCount; ++q)
            {
                const uint  i = frameId.Index() * _QueueCount + q;

                AcquireMappedMemory( frameId, _static.memoryForRead, 0_b, AlignUp( _static.buffersForRead[i].size.load(), _memSizeAlign ));
            }
        }

        // invalidate dynamic staging buffers memory
        if_unlikely( EMemoryType_IsNonCoherent( _dynamic.read.memType ))
        {
            EXLOCK( _dynamic.read.buffers.currentGuard );

            for (auto& sb : _dynamic.read.buffers.current)
            {
                AcquireMappedMemory( frameId, sb.memory, sb.memOffset, AlignUp( sb.size.load(), _memSizeAlign ));
            }
        }
      #endif

        const auto  RecycleBuffers = [this, frameId] (DynamicPerType &dyn)
        {{
            // recycle current buffers
            {
                EXLOCK( dyn.buffers.currentGuard );

                for (auto& sb : dyn.buffers.current) {
                    dyn.buffers.available.push_back( AvailableBuffer{ RVRef(sb.buffer), frameId });
                }
                dyn.buffers.current.clear();
            }

            // release dynamic buffers
            uint    num_released = 0;
            {
                const slong     max_diff = _dynamic.maxFramesToRelease;

                EXLOCK( dyn.buffers.availableGuard );

                for (; not dyn.buffers.available.empty();)
                {
                    auto&   av_buf = dyn.buffers.available.front();

                    if_likely( frameId.Diff( av_buf.lastUsage ) < max_diff )
                        break;

                    _resMngr.ImmediatelyRelease( av_buf.id );
                    dyn.buffers.available.pop_front();
                    ++num_released;
                }
            }
            dyn.allocated.fetch_sub( num_released * _dynamic.blockSize );
        }};

        RecycleBuffers( _dynamic.write );
        RecycleBuffers( _dynamic.read );
    }

/*
=================================================
    _CalcBlockSize
=================================================
*/
    Bytes  STBUFMNGR::_CalcBlockSize (Bytes reqSize, EStagingHeapType heap, EQueueType queue, bool upload) const
    {
        ASSERT( uint(queue) < _QueueCount );

        switch_enum( heap )
        {
            case EStagingHeapType::Static :
            {
                const uint      q           = uint(queue);
                const Bytes     max_size    = DivCeil( upload ? _static.writeSize[q] : _static.readSize[q], GraphicsConfig::MaxStagingBufferParts );
                const Bytes     min_size    = DivCeil( reqSize, GraphicsConfig::MaxStagingBufferParts );
                return AlignUp( Min( min_size, max_size ), _BlockAlign );
            }
            case EStagingHeapType::Dynamic :
            case EStagingHeapType::Any :
            {
                const Bytes     avail_size  = Min( (upload ? _dynamic.write.maxPerFrame : _dynamic.read.maxPerFrame).load(), _dynamic.blockSize );
                const Bytes     max_size    = DivCeil( avail_size, GraphicsConfig::MaxStagingBufferParts );
                const Bytes     min_size    = DivCeil( reqSize, GraphicsConfig::MaxStagingBufferParts );
                return AlignUp( Min( min_size, max_size ), _BlockAlign );
            }
            default_unlikely:
                RETURN_ERR( "unsupported heap type" );
        }
        switch_end
    }

/*
=================================================
    _AllocStatic
----
    memOffsetAlign - alignment of start address in staging buffer
=================================================
*/
    template <typename RangeType, typename BufferType>
    bool  STBUFMNGR::_AllocStatic (const Bytes32u reqSize, const Bytes32u blockSize, const Bytes32u memOffsetAlign,
                                   INOUT RangeType &result, BufferType& sb)
    {
        Bytes32u    expected    = 0_b;
        Bytes32u    new_size    = 0_b;
        Bytes32u    offset      = 0_b;

        for (;;)
        {
            offset = AlignUp( expected, memOffsetAlign );

            if_unlikely( offset + blockSize > sb.capacity )
                return false;

            new_size = Min( reqSize, AlignDown( sb.capacity - offset, blockSize ));

            if_unlikely( new_size == 0 )
                return false;

            if_likely( sb.size.CAS( INOUT expected, offset + new_size ))
                break;

            ThreadUtils::Pause();
        }

        auto&   res         = result.emplace_back();
        res.buffer          = sb.bufferHandle;
        res.bufferOffset    = offset;
        res.size            = new_size;
        res.mapped          = sb.mapped + res.bufferOffset;

        ASSERT( IsMultipleOf( res.mapped, memOffsetAlign ));

        return true;
    }

/*
=================================================
    _AllocStaticImage
----
    memOffsetAlign - alignment of start address in staging buffer
=================================================
*/
    bool  STBUFMNGR::_AllocStaticImage (const Bytes reqSize, const Bytes rowPitch, const Bytes slicePitch, const Bytes memOffsetAlign,
                                        const uint2 &texelBlockDim, const uint3 &imageOffset, const uint3 &imageDim,
                                        INOUT StagingImageResultRanges &result, StaticBuffer& sb)
    {
        ASSERT( rowPitch > 0 );
        ASSERT( slicePitch > 0 );

        // try to allocate some slices
        if_likely( _AllocStatic( reqSize, slicePitch, memOffsetAlign, INOUT result.buffers, sb ))
        {
            auto&       res     = result.buffers.back();
            const uint  z_size  = Max( 1u, CheckCast<uint>( res.size / slicePitch ));
            ASSERT( imageDim.z >= z_size );

          #if defined(AE_ENABLE_VULKAN)
            res.bufferImageHeight   = Max( 1u, CheckCast<uint>( (slicePitch * texelBlockDim.y) / rowPitch ));

          #elif defined(AE_ENABLE_METAL) or defined(AE_ENABLE_REMOTE_GRAPHICS)
            res.bufferSlicePitch    = slicePitch;

          #else
          # error not implemented
          #endif

            res.imageOffset = imageOffset;

            res.imageDim.x  = imageDim.x;
            res.imageDim.y  = imageDim.y;
            res.imageDim.z  = z_size;
            return true;
        }

        // try to allocate some rows
        if_likely( _AllocStatic( reqSize, rowPitch, memOffsetAlign, INOUT result.buffers, sb ))
        {
            auto&       res     = result.buffers.back();
            const uint  y_size  = Max( 1u, CheckCast<uint>( (res.size * texelBlockDim.y) / rowPitch ));

            ASSERT( imageDim.y >= y_size );
            ASSERT( IsMultipleOf( y_size, texelBlockDim.y ));

          #if defined(AE_ENABLE_VULKAN)
            res.bufferImageHeight   = y_size;

          #elif defined(AE_ENABLE_METAL) or defined(AE_ENABLE_REMOTE_GRAPHICS)
            res.bufferSlicePitch    = slicePitch;

          #else
          # error not implemented
          #endif

            res.imageOffset = imageOffset;

            res.imageDim.x  = imageDim.x;
            res.imageDim.y  = y_size;
            res.imageDim.z  = 1;
            return true;
        }

        return false;
    }

/*
=================================================
    _AddToCurrent
=================================================
*/
    template <typename RangeType>
    bool  STBUFMNGR::_AddToCurrent (INOUT Bytes &reqSize, const Bytes blockSize, const Bytes memOffsetAlign,
                                    Strong<BufferID> id, INOUT RangeType& buffers, DynamicBuffers &db) const
    {
        auto*       buf         = _resMngr.GetResource( id );
        auto*       mem         = _resMngr.GetResource( buf->MemoryId() );
        const auto& buf_desc    = buf->Description();

        ASSERT( blockSize <= buf_desc.size );

        NativeMemObjInfo_t  mem_desc;
        CHECK_ERR( mem->GetMemoryInfo( OUT mem_desc ));

        EXLOCK( db.currentGuard );
        auto&   sb = db.current.emplace_back();

        sb.size.store( 0_b );
        sb.capacity     = buf_desc.size;
        sb.buffer       = RVRef(id);
        sb.bufferHandle = buf->Handle();
        sb.mapped       = mem_desc.mappedPtr;

      #ifdef AE_ENABLE_VULKAN
        sb.memOffset    = mem_desc.offset;
        sb.memory       = mem_desc.memory;
      #endif

        if_likely( _AllocStatic( reqSize, blockSize, memOffsetAlign, INOUT buffers, sb ))
        {
            auto&   res = buffers.back();

            ASSERT( res.size <= reqSize );
            reqSize -= res.size;

            return true;
        }
        return false;
    }

/*
=================================================
    _AllocDynamic
----
    memOffsetAlign - alignment of start address in staging buffer
=================================================
*/
    template <bool SingleAlloc, typename RangeType>
    bool  STBUFMNGR::_AllocDynamic (FrameUID frameId, INOUT Bytes &reqSize, const Bytes blockSize, const Bytes memOffsetAlign, const bool upload,
                                    INOUT RangeType& buffers, DynamicBuffers &db) const
    {
        if_unlikely( blockSize > _dynamic.blockSize )
            return false;

        auto&       allocated_size  = upload ? _dynamic.write.allocated     : _dynamic.read.allocated;
        const auto  max_size        = upload ? _dynamic.write.maxSize       : _dynamic.read.maxSize;
        const auto  max_per_frame   = (upload ? _dynamic.write.maxPerFrame  : _dynamic.read.maxPerFrame).load();
        auto&       used_per_frame  = upload ? _dynamic.write.usedPerFrame  : _dynamic.read.usedPerFrame;
        const auto  used_mem        = used_per_frame.load();
        uint        num_allocs      = 0;

        if_unlikely( used_mem + blockSize > max_per_frame )
            return false;

        reqSize = Min( reqSize, AlignDown( max_per_frame - used_mem, blockSize ));

        // search in currently used buffers
        {
            SHAREDLOCK( db.currentGuard );
            for (auto& sb : db.current)
            {
                if_likely( _AllocStatic( reqSize, blockSize, memOffsetAlign, INOUT buffers, sb ))
                {
                    auto&   res = buffers.back();
                    ASSERT( res.size <= reqSize );

                    reqSize -= res.size;
                    ++num_allocs;

                    if_unlikely( used_per_frame.Add( res.size ) >= max_per_frame )
                        return true;

                    if constexpr( SingleAlloc )
                        return true;
                }

                if ( reqSize == 0 )
                    break;
            }

            // fixed-size array is full
            if_unlikely( db.current.size() == db.current.capacity() )
                return num_allocs > 0;
        }

        // search in available dynamic buffers
        for (; reqSize != 0;)
        {
            Strong<BufferID>    buf_id;
            {
                EXLOCK( db.availableGuard );

                if ( db.available.empty() )
                    break;

                auto&   av_buf = db.available.front();

                if_unlikely( frameId.Diff( av_buf.lastUsage ) <= frameId.MaxFrames() )
                    break;

                buf_id = RVRef(av_buf.id);
                db.available.pop_front();
            }

            CHECK_ERR( _AddToCurrent( INOUT reqSize, blockSize, memOffsetAlign, RVRef(buf_id), INOUT buffers, db ));
            ++num_allocs;

            if_unlikely( used_per_frame.Add( buffers.back().size ) >= max_per_frame )
                return true;

            if constexpr( SingleAlloc )
                return true;
        }

        // create new dynamic buffer
        if_unlikely( reqSize > 0 and allocated_size.load() < max_size )
        {
            Strong<BufferID>    buf_id =
                _resMngr.CreateBuffer( BufferDesc{  _dynamic.blockSize,
                                                    upload ? EBufferUsage::TransferSrc : EBufferUsage::TransferDst,
                                                    EBufferOpt::Unknown,
                                                    EQueueMask::Unknown,
                                                    upload ? _dynamic.write.memType : _dynamic.read.memType },
                                        "dynamic staging buffer",
                                        _dynamic.gfxAllocator );
            CHECK_ERR( buf_id );

            allocated_size.fetch_add( _dynamic.blockSize );

            CHECK_ERR( _AddToCurrent( INOUT reqSize, blockSize, memOffsetAlign, RVRef(buf_id), INOUT buffers, db ));
            ++num_allocs;

            if_unlikely( used_per_frame.Add( buffers.back().size ) >= max_per_frame )
                return true;

            if constexpr( SingleAlloc )
                return true;
        }

        return num_allocs > 0;
    }

/*
=================================================
    _AllocDynamicImage
----
    memOffsetAlign - alignment of start address in staging buffer
=================================================
*/
    void  STBUFMNGR::_AllocDynamicImage (FrameUID frameId, Bytes reqSize, const Bytes rowPitch, const Bytes slicePitch, const Bytes memOffsetAlign,
                                         const uint2 &texelBlockDim, const uint3 &imageOffset, const uint3 &regionDim, bool upload,
                                         INOUT StagingImageResultRanges &result, DynamicBuffers& db) const
    {
        ASSERT( rowPitch > 0 );
        ASSERT( slicePitch > 0 );
        ASSERT( rowPitch <= _dynamic.blockSize );

        uint3   local_offset {0};

        // try to allocate some slices
        for (; (reqSize > 0) and (result.buffers.size() < ImageMemView::Count);)
        {
            if_likely( _AllocDynamic<true>( frameId, INOUT reqSize, slicePitch, memOffsetAlign, upload, INOUT result.buffers, db ))
            {
                auto&       res     = result.buffers.back();
                const uint  z_size  = Max( 1u, CheckCast<uint>( res.size / slicePitch ));

                ASSERT( res.size >= slicePitch );
                ASSERT( regionDim.z >= z_size );
                ASSERT( IsMultipleOf( res.size, slicePitch ));

              #if defined(AE_ENABLE_VULKAN)
                res.bufferImageHeight   = Max( 1u, CheckCast<uint>( (slicePitch * texelBlockDim.y) / rowPitch ));

              #elif defined(AE_ENABLE_METAL) or defined(AE_ENABLE_REMOTE_GRAPHICS)
                res.bufferSlicePitch    = slicePitch;

              #else
              # error not implemented
              #endif

                res.imageOffset = imageOffset + local_offset;

                res.imageDim.x  = regionDim.x;
                res.imageDim.y  = regionDim.y;
                res.imageDim.z  = z_size;

                local_offset.z += z_size;

                ASSERT( All( local_offset <= regionDim ));
                continue;
            }
            break;
        }

        // try to allocate some rows
        for (; (reqSize > 0) and (result.buffers.size() < ImageMemView::Count);)
        {
            if_likely( _AllocDynamic<true>( frameId, INOUT reqSize, rowPitch, memOffsetAlign, upload, INOUT result.buffers, db ))
            {
                auto&       res     = result.buffers.back();
                const uint  y_size  = Max( 1u, CheckCast<uint>( (res.size * texelBlockDim.y) / rowPitch ));

                ASSERT( res.size >= rowPitch );
                ASSERT( regionDim.y >= y_size );
                ASSERT( IsMultipleOf( y_size, texelBlockDim.y ));

              #if defined(AE_ENABLE_VULKAN)
                res.bufferImageHeight   = y_size;

              #elif defined(AE_ENABLE_METAL) or defined(AE_ENABLE_REMOTE_GRAPHICS)
                res.bufferSlicePitch    = slicePitch;

              #else
              # error not implemented
              #endif

                res.imageOffset = imageOffset + local_offset;

                res.imageDim.x  = regionDim.x;
                res.imageDim.y  = y_size;
                res.imageDim.z  = 1;

                local_offset.y += y_size;
                ASSERT( All( local_offset <= regionDim ));

                if_unlikely( local_offset.y >= regionDim.y )
                    return; // can not merge slices into ImageMemView

                continue;
            }
            break;
        }
    }

/*
=================================================
    GetBufferRanges
=================================================
*/
    void  STBUFMNGR::GetBufferRanges (OUT BufferRanges_t &buffers, const Bytes reqSize, const Bytes blockSize, const Bytes memOffsetAlign,
                                      const FrameUID frameId, const EStagingHeapType heap, const EQueueType queue, const Bool upload) __NE___
    {
        ASSERT( buffers.empty() );
        ASSERT( _frameId.load() == frameId );

        const Bytes block_size = (blockSize == 0_b ? _CalcBlockSize( reqSize, heap, queue, upload ) : Min( reqSize, blockSize ));

        switch_enum( heap )
        {
            // static heap has only one staging buffer so loop is not needed
            case EStagingHeapType::Static :
            {
                const uint  i   = frameId.Index() * _QueueCount + uint(queue);
                auto&       sb  = upload ? _static.buffersForWrite[i] : _static.buffersForRead[i];

                Unused( _AllocStatic( reqSize, block_size, memOffsetAlign, OUT buffers, sb ));
                ASSERT( buffers.size() <= 1 );
                return;
            }

            case EStagingHeapType::Dynamic :
            case EStagingHeapType::Any :        // TODO: try static & dynamic
            {
                Bytes   req_size = reqSize;
                auto&   db       = upload ? _dynamic.write.buffers : _dynamic.read.buffers;

                Unused( _AllocDynamic<false>( frameId, req_size, block_size, memOffsetAlign, upload, INOUT buffers, db ));
                return;
            }

            default_unlikely:
                AE_LOGE( "unknown staging heap type" );
        }
        switch_end
    }

/*
=================================================
    AllocVStream
=================================================
*/
    bool  STBUFMNGR::AllocVStream (FrameUID frameId, const Bytes reqSize, OUT VertexStream &result) __NE___
    {
        ASSERT( _frameId.load() == frameId );

        auto&           vb              = _vstream.buffers[ frameId.Index() ];
        const Bytes32u  offset_align    = 64_b;     // TODO
        Bytes32u        expected        = 0_b;
        const Bytes32u  new_size        = reqSize;
        Bytes32u        offset          = 0_b;

        for (;;)
        {
            offset = AlignUp( expected, offset_align );

            if_unlikely( offset + new_size > vb.capacity )
                return false;   // overflow

            if_likely( vb.size.CAS( INOUT expected, offset + new_size ))
                break;

            ThreadUtils::Pause();
        }

        result.id           = vb.buffer;
        result.offset       = offset;
        result.size         = new_size;
        result.mappedPtr    = vb.mapped + offset;

        return true;
    }

/*
=================================================
    GetImageRanges
=================================================
*/
    void  STBUFMNGR::GetImageRanges (OUT StagingImageResultRanges &result, const UploadImageDesc &uploadDesc, const ImageDesc &imgDesc,
                                     const uint3 &imageGranularity, const FrameUID frameId, const EQueueType queue, const Bool upload) __NE___
    {
        ASSERT( _frameId.load() == frameId );
        ASSERT( All( uploadDesc.imageDim > Zero ));
        ASSERT( result.buffers.empty() );
        ASSERT( All( imageGranularity > Zero ));

        CHECK_ERRV( IsSingleBitSet( uploadDesc.aspectMask ));
        CHECK_ERRV( AnyBits( uploadDesc.aspectMask, EImageAspect::Color | EImageAspect::Depth | EImageAspect::Stencil ));
        CHECK_ERRV( uint(queue) < _QueueCount );
        CHECK_ERRV( uploadDesc.mipLevel < imgDesc.maxLevel );
        CHECK_ERRV( uploadDesc.arrayLayer < imgDesc.arrayLayers );

        const auto&     fmt_info            = EPixelFormat_GetInfo( imgDesc.format );
        const uint2     texblock_dim        = fmt_info.TexBlockDim();   // TODO: use imageGranularity
        const uint3     mip_dim             = ImageUtils::MipmapDimension( imgDesc.dimension, uploadDesc.mipLevel.Get(), texblock_dim );
        const uint3     region_dim          = Min( mip_dim, Max( uploadDesc.imageDim, 1u ));
        const uint      texblock_bits       = uploadDesc.aspectMask != EImageAspect::Stencil ? fmt_info.bitsPerBlock : fmt_info.bitsPerBlock2;
        const Bytes     row_pitch           = Max( uploadDesc.dataRowPitch, Bytes{region_dim.x * texblock_bits + texblock_dim.x-1} / (texblock_dim.x * 8) );
        const Bytes     min_slice_pitch     = (region_dim.y * row_pitch + texblock_dim.y-1) / texblock_dim.y;
        const Bytes     slice_pitch         = Max( uploadDesc.dataSlicePitch, min_slice_pitch );
        const Bytes     total_size          = region_dim.z > 1 ? slice_pitch * region_dim.z : min_slice_pitch;
        const uint      row_length          = CheckCast<uint>((row_pitch * texblock_dim.x * 8) / texblock_bits);
        const Bytes     mem_offset_align    = Bytes{ (texblock_bits + 7) / 8 };

        ASSERT( IsPowerOfTwo( mem_offset_align ));

        CHECK_ERRV( All( uploadDesc.imageOffset < mip_dim ));
        CHECK_ERRV( All( uploadDesc.imageOffset + region_dim <= mip_dim ));
        CHECK_ERRV( All( IsMultipleOf( uint2{uploadDesc.imageOffset}, texblock_dim )));
        CHECK_ERRV( All( IsMultipleOf( uint2{region_dim}, texblock_dim )));
        CHECK_ERRV( All( IsMultipleOf( region_dim, imageGranularity )));
        CHECK_ERRV( total_size >= slice_pitch );

        result.dataRowPitch     = row_pitch;
        result.dataSlicePitch   = slice_pitch;
        result.bufferRowLength  = row_length;
        result.format           = imgDesc.format;
        result.planeScaleY      = 1;

        switch_enum( uploadDesc.heapType )
        {
            // static heap has only one staging buffer so loop is not needed
            case EStagingHeapType::Static :
            {
                const uint  i   = frameId.Index() * _QueueCount + uint(queue);
                auto&       sb  = upload ? _static.buffersForWrite[i] : _static.buffersForRead[i];

                Unused( _AllocStaticImage( total_size, row_pitch, slice_pitch, mem_offset_align, texblock_dim,
                                           uploadDesc.imageOffset, region_dim, INOUT result, sb ));
                ASSERT( result.buffers.size() <= 1 );
                return;
            }

            case EStagingHeapType::Dynamic :
            case EStagingHeapType::Any :        // TODO: try static & dynamic
            {
                auto&   db  = upload ? _dynamic.write.buffers : _dynamic.read.buffers;

                _AllocDynamicImage( frameId, total_size, row_pitch, slice_pitch, mem_offset_align, texblock_dim,
                                    uploadDesc.imageOffset, region_dim, upload, INOUT result, db );
                return;
            }

            default_unlikely:
                AE_LOGE( "unknown staging heap type" );
        }
        switch_end
    }

/*
=================================================
    GetImageRanges (multiplanar)
=================================================
*/
    void  STBUFMNGR::GetImageRanges (OUT StagingImageResultRanges &result, const UploadImageDesc &uploadDesc, const VideoImageDesc &imgDesc,
                                     const uint3 &imageGranularity, FrameUID frameId, EQueueType queue, Bool upload) __NE___
    {
        ASSERT( _frameId.load() == frameId );
        ASSERT( All( uploadDesc.imageDim > Zero ));
        ASSERT( result.buffers.empty() );
        ASSERT( All( imageGranularity > Zero ));

        CHECK_ERRV( IsSingleBitSet( uploadDesc.aspectMask ));
        CHECK_ERRV( AnyBits( uploadDesc.aspectMask, EImageAspect::_PlaneMask ));
        CHECK_ERRV( uint(queue) < _QueueCount );
        CHECK_ERRV( uploadDesc.mipLevel == 0_mipmap );
        CHECK_ERRV( uploadDesc.arrayLayer < imgDesc.arrayLayers );

        EPixelFormat    plane_fmt;
        uint2           plane_scale;
        CHECK_ERRV( EPixelFormat_GetPlaneInfo( imgDesc.format, uploadDesc.aspectMask, OUT plane_fmt, OUT plane_scale ));

        const auto&     fmt_info            = EPixelFormat_GetInfo( plane_fmt );
        const uint2     plane_dim           = imgDesc.dimension / plane_scale;
        const uint2     region_dim          = Min( plane_dim, Max( uint2{uploadDesc.imageDim}, 1u ));
        const Bytes     texblock_bytes      = fmt_info.BytesPerBlock();
        const Bytes     row_pitch           = Max( uploadDesc.dataRowPitch, Bytes{region_dim.x * texblock_bytes} );
        const Bytes     min_slice_pitch     = region_dim.y * row_pitch;
        const Bytes     slice_pitch         = Max( uploadDesc.dataSlicePitch, min_slice_pitch );
        const Bytes     total_size          = min_slice_pitch;
        const uint      row_length          = CheckCast<uint>( row_pitch / texblock_bytes );
        const Bytes     mem_offset_align    = texblock_bytes;

        ASSERT( IsPowerOfTwo( mem_offset_align ));
        ASSERT( All( fmt_info.TexBlockDim() == uint2{1} ));
        ASSERT( fmt_info.bitsPerBlock == texblock_bytes*8 );

        CHECK_ERRV( All( uploadDesc.imageOffset < uint3{plane_dim,1} ));
        CHECK_ERRV( All( uploadDesc.imageOffset + uint3{region_dim,1} <= uint3{plane_dim,1} ));
        CHECK_ERRV( All( IsMultipleOf( region_dim, uint2{imageGranularity} )));
        CHECK_ERRV( total_size >= slice_pitch );

        result.dataRowPitch     = row_pitch;
        result.dataSlicePitch   = slice_pitch;
        result.bufferRowLength  = row_length;
        result.format           = plane_fmt;
        result.planeScaleY      = plane_scale.y;

        switch_enum( uploadDesc.heapType )
        {
            // static heap has only one staging buffer so loop is not needed
            case EStagingHeapType::Static :
            {
                const uint  i   = frameId.Index() * _QueueCount + uint(queue);
                auto&       sb  = upload ? _static.buffersForWrite[i] : _static.buffersForRead[i];

                Unused( _AllocStaticImage( total_size, row_pitch, slice_pitch, mem_offset_align, uint2{1},
                                           uploadDesc.imageOffset, uint3{region_dim,1}, INOUT result, sb ));
                ASSERT( result.buffers.size() <= 1 );
                return;
            }

            case EStagingHeapType::Dynamic :
            case EStagingHeapType::Any :        // TODO: try static & dynamic
            {
                auto&   db  = upload ? _dynamic.write.buffers : _dynamic.read.buffers;

                _AllocDynamicImage( frameId, total_size, row_pitch, slice_pitch, mem_offset_align, uint2{1},
                                    uploadDesc.imageOffset, uint3{region_dim,1}, upload, INOUT result, db );
                return;
            }

            default_unlikely:
                AE_LOGE( "unknown staging heap type" );
        }
        switch_end
    }

/*
=================================================
    _InitDynamicBuffers
=================================================
*/
    bool  STBUFMNGR::_InitDynamicBuffers (const GraphicsCreateInfo &info)
    {
        _dynamic.maxFramesToRelease = Max( info.staging.maxFramesToRelease, GraphicsConfig::MaxFrames );
        _dynamic.blockSize          = Max( CeilPOT( info.staging.dynamicBlockSize ), 4_Kb );
        _dynamic.write.maxSize      = AlignUp( info.staging.maxWriteDynamicSize, _dynamic.blockSize );
        _dynamic.read.maxSize       = AlignUp( info.staging.maxReadDynamicSize,  _dynamic.blockSize );

        _dynamic.write.maxPerFrame.store( UMax );
        _dynamic.read.maxPerFrame.store( UMax );

        _dynamic.write.buffers.available.reserve( 16 );
        _dynamic.read.buffers.available.reserve( 16 );

      #if defined(AE_ENABLE_VULKAN)
        {
            Bytes   page_size = _dynamic.blockSize * 8;
            if ( info.staging.maxWriteDynamicSize > 0 ) page_size = Min( page_size, info.staging.maxWriteDynamicSize );
            if ( info.staging.maxReadDynamicSize > 0 )  page_size = Min( page_size, info.staging.maxReadDynamicSize );
            page_size = Max( page_size, _dynamic.blockSize );

            _dynamic.gfxAllocator = MakeRC<VBlockMemAllocator>( _dynamic.blockSize, page_size );
            _memRanges.ranges.resize( info.maxFrames );
        }
      #elif defined(AE_ENABLE_METAL) or defined(AE_ENABLE_REMOTE_GRAPHICS)
        // TODO: _dynamic.gfxAllocator = MakeRC<MBlockMemAllocator>();

      #else
      # error not implemented
      #endif

        if ( _resMngr.IsSupported( EMemoryType::HostCoherent ))
            _dynamic.write.memType = EMemoryType::HostCoherent;
        else
        if ( _resMngr.IsSupported( EMemoryType::Unified ))
            _dynamic.write.memType = EMemoryType::Unified;

        if ( _resMngr.IsSupported( EMemoryType::HostCachedCoherent ))
            _dynamic.read.memType = EMemoryType::HostCachedCoherent;
        else
        if ( _resMngr.IsSupported( EMemoryType::HostCached ))
        {
            _dynamic.read.memType = EMemoryType::HostCached;

          #if defined(AE_ENABLE_VULKAN)
            for (auto& ranges : _memRanges.ranges) {
                ranges.reserve( 64 );
            }
          #endif
        }
        else
        if ( _resMngr.IsSupported( EMemoryType::UnifiedCached ))
            _dynamic.read.memType = EMemoryType::UnifiedCached;
        else
        if ( _resMngr.IsSupported( EMemoryType::Unified ))
            _dynamic.read.memType = EMemoryType::Unified;

        CHECK_ERR( _dynamic.write.memType != Default );
        CHECK_ERR( _dynamic.read.memType != Default );

        return true;
    }

/*
=================================================
    GetFrameStat
=================================================
*/
    STBUFMNGR::FrameStat_t  STBUFMNGR::GetFrameStat (FrameUID frameId) C_NE___
    {
        FrameStat_t     res;
        res.dynamicWrite    = _dynamic.write.usedPerFrame.load();
        res.dynamicRead     = _dynamic.read.usedPerFrame.load();
        res.staticWrite     = _vstream.buffers[ frameId.Index() ].size.load();

        for (uint q = 0; q < _QueueCount; ++q)
        {
            const uint  i = frameId.Index() * _QueueCount + q;

            res.staticWrite += _static.buffersForWrite[i].size.load();
            res.staticRead  += _static.buffersForRead[i].size.load();
        }
        return res;
    }
