// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Core/RenderGraph.h"
#include "res_editor/Resources/VideoImage2.h"
#include "res_editor/Resources/Buffer.h"
#include "res_editor/Resources/Image.h"
#include "res_editor/Resources/RTScene.h"
#include "res_editor/Passes/Renderer.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	VideoImage2::VideoImage2 (Renderer &			renderer,
							  const ImageDesc &		inDesc,
							  const VFS::FileName	&filename,
							  RC<DynamicDim>		outDynSize,
							  const SamplerName		&ycbcrConversion,
							  PipelinePackID		packId,
							  StringView			dbgName) __Th___ :
		IResource{ renderer },
		_outDynSize{ RVRef(outDynSize) },
		_dbgName{ dbgName }
	{
		_states.store( States{} );

		_decoder = Video::VideoFactory::CreateFFmpegDecoder();
		CHECK_THROW( _decoder );

		auto	rstream = GetVFS().Open<RStream>( filename );
		CHECK_THROW( rstream );

		Video::IVideoDecoder::Config	in_cfg;
		in_cfg.filter		= Video::EFilter::Bilinear;
		in_cfg.threadCount	= 4;

		CHECK_THROW( _decoder->Begin( in_cfg, rstream ));

		const auto	config	= _decoder->GetConfig();
		const auto	props	= _decoder->GetProperties();
		const auto*	vstream	= props.GetStream( config.videoStreamIdx );
		CHECK_THROW( vstream != null );

		VideoImageDesc	desc;
		desc.dimension		= vstream->dimension;
		desc.arrayLayers	= 1_layer;
		desc.format			= vstream->pixFormat;
		desc.options		= inDesc.options;
		desc.usage			= inDesc.usage;
		desc.memType		= inDesc.memType;
		desc.ycbcrConversion= ycbcrConversion;
		desc.ycbcrConvPack	= packId;
		_dimension			= vstream->dimension;

		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();
		auto&	rstate		= RenderGraph().GetStateTracker();

		CHECK_THROW_MSG( res_mngr.IsSupported( desc ),
			"VideoImage2 '"s << _dbgName << "' description is not supported by GPU device" );

		for (usize i = 0; i < _ids.size(); ++i)
		{
			_ids[i] = res_mngr.CreateVideoImage( desc, _dbgName + ToString(i), _GfxAllocator() );
			CHECK_THROW( _ids[i] );

			rstate.AddResource( _ids[i].Get(),
								EResourceState::Invalidate,									// current state is not used
								EResourceState::ShaderSample | EResourceState::AllShaders,	// default
								EQueueType::Graphics );

			_DtTrQueue().EnqueueImageTransition( _ids[i] );
		}

		_allocator.SetBlockSize( EPixelFormat_ImageSize( config.dstFormat, config.dstDim ));

		for (usize i = 0; i < _imageMemView.size(); ++i) {
			CHECK_THROW( Video::IVideoDecoder::AllocMemView( config, OUT _imageMemView[i], _allocator ));
		}
		_streamArr.resize( _imageMemView[0].size() );

		_uploadStatus.store( EUploadStatus::InProgress );

		_DtTrQueue().EnqueueForUpload( GetRC() );

		_lastDecoding = Scheduler().Run( ETaskQueue::Background, _DecodeFrameTask( GetRC<VideoImage2>() ), Tuple{}, "Video decoding" );
	}

/*
=================================================
	destructor
=================================================
*/
	VideoImage2::~VideoImage2 () __NE___
	{
		auto&	rstate = RenderGraph().GetStateTracker();
		rstate.ReleaseResourceArray( _ids );
	}

/*
=================================================
	Upload
=================================================
*/
	IResource::EUploadStatus  VideoImage2::Upload (TransferCtx_t &ctx) __Th___
	{
		if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
			return stat;

		// invalidate cache to load '_frameTimes'
		States	states = _states.load( EMemoryOrder::Acquire );
		_Validate( states );

		if ( (states.emptyBits != 0) and (not _lastDecoding or not _lastDecoding->IsInQueue()) )
			_lastDecoding = Scheduler().Run( ETaskQueue::Background, _DecodeFrameTask( GetRC<VideoImage2>() ), Tuple{_lastDecoding}, "Video decoding" );


		// variants:
		//	- decoding in progress	-> skip
		//	- frame ready			-> begin uploading
		//	- uploading in progress	-> continue uploading
		const uint	mem_idx	= states.pos;

		if ( not HasBit( states.decodedBits, mem_idx ))
			return _uploadStatus.load();  // decoding in progress

		const auto	cur_time = Seconds_t{_curTime.Add( GraphicsScheduler().GetFrameTimeDelta().count() )};

		if ( cur_time < _frameTimes[mem_idx] )
			return _uploadStatus.load();  // skip

		const uint	idx = (_imageIdx.load()+1) % _ids.size();

		// init image stream
		for (usize i = 0; i < _streamArr.size(); ++i)
		{
			auto&	stream = _streamArr[i];
			if ( not stream.IsInitialized() )
			{
				UploadImageDesc		upload;
				upload.imageDim		= uint3{ _dimension, 1u };
				upload.heapType		= EStagingHeapType::Dynamic;
				upload.aspectMask	= EImageAspect_Plane( i );
				stream				= VideoImageStream{ _ids[idx], upload };

				ctx.ResourceState( _ids[idx], EResourceState::Invalidate );
			}
			ASSERT( stream.ImageId() == _ids[idx] );
		}

		// upload
		bool	all_complete = true;

		for (usize i = 0; i < _streamArr.size(); ++i)
		{
			auto&	stream = _streamArr[i];
			if ( stream.IsCompleted() )
				continue;

			ImageMemView&	src_mem = _imageMemView[ mem_idx ][i];
			ImageMemView	dst_mem;
			ctx.UploadImage( stream, OUT dst_mem );

			if ( not dst_mem.Empty() )
			{
				CHECK( dst_mem.CopyFrom( uint3{}, dst_mem.Offset(), src_mem, dst_mem.Dimension() ));
			}

			all_complete &= stream.IsCompleted();
		}

		if ( all_complete )
		{
			for (auto& stream : _streamArr) {
				stream = VideoImageStream{};
			}

			_imageIdx.store( idx );

			// update 'pos', 'decodedBits'
			for (States exp = states;;)
			{
				states.pos			= mem_idx+1;
				states.emptyBits	|= ToBit<uint>( mem_idx );
				states.decodedBits	&= ~ToBit<uint>( mem_idx );

				if_likely( _states.CAS( INOUT exp, states ))
					break;

				states = exp;
				ThreadUtils::Pause();
			}
		}

		return _uploadStatus.load();
	}

/*
=================================================
	Cancel
=================================================
*/
	void  VideoImage2::Cancel () __NE___
	{
		IResource::Cancel();

		Unused( Scheduler().Wait( {_lastDecoding}, seconds{1} ));
		_lastDecoding = null;
	}

/*
=================================================
	_DecodeFrameTask
=================================================
*/
	CoroTask  VideoImage2::_DecodeFrameTask (RC<VideoImage2> self) __NE___
	{
		for (uint i = 0; i < _MaxCpuImages/2; ++i)
		{
			if ( self->_uploadStatus.load() != EUploadStatus::InProgress )
				break;

			uint	cnt = self->_DecodeFrame();
			if ( cnt == 0 )
				break;
		}
		co_return;
	}

/*
=================================================
	_DecodeFrame
=================================================
*/
	uint  VideoImage2::_DecodeFrame () __NE___
	{
		using FrameInfo = Video::IVideoDecoder::FrameInfo;

		// find empty image
		States		states	= _states.load();
		uint		idx		= states.pos;

		_Validate( states );

		if_unlikely( states.emptyBits == 0 )
			return 0;

		for (uint i = 0; i < _MaxCpuImages; ++i)
		{
			if ( HasBit( states.emptyBits, idx ))
				break;

			idx = (idx+1) % _MaxCpuImages;
		}
		ASSERT( not HasBit( states.decodedBits, idx ));

		if_unlikely( not _decoder )
			return 0;

		uint	result = 0;

		// get next video frame
		if ( FrameInfo info;  _decoder->GetNextFrame( INOUT _imageMemView[idx], OUT info ))
		{
			_frameTimes[idx] = info.timestamp;

			for (States exp = states;;)
			{
				states.emptyBits	&= ~ToBit<uint>( idx );
				states.decodedBits	|= ToBit<uint>( idx );

				// flush cache to make '_frameTimes[idx]' visible
				if_likely( _states.CAS( INOUT exp, states, EMemoryOrder::Release, EMemoryOrder::Relaxed ))
					break;

				states = exp;
				ThreadUtils::Pause();
			}

			result = uint(BitCount( states.emptyBits ));
		}
		else
		// restart
		{
			_frameTimes.fill( Seconds_t{0.0} );
			_states.store( States{}, EMemoryOrder::Release );
			_curTime.store( 0.0 );

			if ( _decoder->SeekTo( 0 )) {
				result = UMax;
			}else{
				_SetUploadStatus( EUploadStatus::Canceled );
			}
		}

		return result;
	}

/*
=================================================
	_Validate
=================================================
*/
	void  VideoImage2::_Validate (const States s) __NE___
	{
		Unused( s );
		ASSERT( not AnyBits( s.emptyBits, s.decodedBits ));
		ASSERT( (s.emptyBits | s.decodedBits) == ToBitMask<uint>(_MaxCpuImages) );
		ASSERT( s.decodedBits == 0 or HasBit( s.decodedBits, s.pos ));
	}


} // AE::ResEditor
