// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Core/RenderGraph.h"
#include "res_editor/Resources/VideoImage.h"
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
	VideoImage::VideoImage (Renderer &			renderer,
							const ImageDesc &	inDesc,
							const VFS::FileName	&filename,
							RC<DynamicDim>		outDynSize,
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
		in_cfg.dstFormat	= inDesc.format;
		in_cfg.filter		= Video::EFilter::Bilinear;
		in_cfg.threadCount	= 4;

		CHECK_THROW( _decoder->Begin( in_cfg, rstream ));

		const auto	config	= _decoder->GetConfig();
		const auto	props	= _decoder->GetProperties();
		const auto*	vstream	= props.GetStream( config.videoStreamIdx );
		CHECK_THROW( vstream != null );

		ImageDesc	desc	= inDesc;
		desc.dimension	= uint3{ vstream->dimension, 1u };
		_dimension		= vstream->dimension;

		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();
		auto&	rstate		= RenderGraph().GetStateTracker();

		CHECK_THROW_MSG( res_mngr.IsSupported( desc ),
			"VideoImage '"s << _dbgName << "' description is not supported by GPU device" );

		for (usize i = 0; i < _ids.size(); ++i)
		{
			_ids[i] = res_mngr.CreateImage( desc, _dbgName + ToString(i), _GfxAllocator() );
			CHECK_THROW( _ids[i] );

			_views[i] = res_mngr.CreateImageView( ImageViewDesc{desc}, _ids[i], _dbgName + ToString(i) );
			CHECK_THROW( _views[i] );

			rstate.AddResource( _ids[i].Get(),
								EResourceState::Invalidate,									// current state is not used
								EResourceState::ShaderSample | EResourceState::AllShaders,	// default
								EQueueType::Graphics );

			_DtTrQueue().EnqueueImageTransition( _ids[i] );
		}

		_allocator.SetBlockSize( EPixelFormat_ImageSize( desc.format, desc.dimension ));

		for (usize i = 0; i < _imageMemView.size(); ++i) {
			CHECK_THROW( Video::IVideoDecoder::AllocMemView( config, OUT _imageMemView[i], _allocator ));
		}

		_uploadStatus.store( EUploadStatus::InProgress );

		_DtTrQueue().EnqueueForUpload( GetRC() );

		_lastDecoding = Scheduler().Run( ETaskQueue::Background, _DecodeFrameTask( GetRC<VideoImage>() ), Tuple{}, "Video decoding" );
	}

/*
=================================================
	destructor
=================================================
*/
	VideoImage::~VideoImage () __NE___
	{
		auto&	rstate = RenderGraph().GetStateTracker();
		rstate.ReleaseResourceArray( _ids );
		rstate.ReleaseResourceArray( _views );
	}

/*
=================================================
	Upload
=================================================
*/
	IResource::EUploadStatus  VideoImage::Upload (TransferCtx_t &ctx) __Th___
	{
		if ( auto stat = _uploadStatus.load();  stat != EUploadStatus::InProgress )
			return stat;

		// invalidate cache to load '_frameTimes'
		States	states = _states.load( EMemoryOrder::Acquire );
		_Validate( states );

		if ( (states.emptyBits != 0) and (not _lastDecoding or not _lastDecoding->IsInQueue()) )
			_lastDecoding = Scheduler().Run( ETaskQueue::Background, _DecodeFrameTask( GetRC<VideoImage>() ), Tuple{_lastDecoding}, "Video decoding" );


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
		if ( not _stream.IsInitialized() )
		{
			UploadImageDesc		upload;
			upload.imageDim		= uint3{ _dimension, 1u };
			upload.heapType		= EStagingHeapType::Dynamic;
			upload.aspectMask	= EImageAspect::Color;
			_stream				= ImageStream{ _ids[idx], upload };

			ctx.ResourceState( _ids[idx], EResourceState::Invalidate );
		}
		ASSERT( _stream.ImageId() == _ids[idx] );


		// upload
		ImageMemView&	src_mem = _imageMemView[ mem_idx ];
		ImageMemView	dst_mem;
		ctx.UploadImage( _stream, OUT dst_mem );

		if ( not dst_mem.Empty() )
		{
			CHECK( dst_mem.CopyFrom( uint3{}, dst_mem.Offset(), src_mem, dst_mem.Dimension() ));
		}

		if ( _stream.IsCompleted() )
		{
			_imageIdx.store( idx );
			_stream = ImageStream{};

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
	void  VideoImage::Cancel () __NE___
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
	CoroTask  VideoImage::_DecodeFrameTask (RC<VideoImage> self) __NE___
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
	uint  VideoImage::_DecodeFrame () __NE___
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
	void  VideoImage::_Validate (const States s) __NE___
	{
		Unused( s );
		ASSERT( not AnyBits( s.emptyBits, s.decodedBits ));
		ASSERT( (s.emptyBits | s.decodedBits) == ToBitMask<uint>(_MaxCpuImages) );
		ASSERT( s.decodedBits == 0 or HasBit( s.decodedBits, s.pos ));
	}


} // AE::ResEditor
