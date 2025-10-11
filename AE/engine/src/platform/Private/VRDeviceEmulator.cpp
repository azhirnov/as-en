// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Private/VRDeviceEmulator.h"
#include "platform/Private/ApplicationBase.h"

namespace AE::App
{
	using namespace AE::Graphics;

/*
=================================================
	_BlitImageTask
=================================================
*/
	RenderCoro  VRDeviceEmulator::VRRenderSurface::_BlitImageTask (VRRenderSurface &t) __NE___
	{
		RenderTargets_t	src_targets;
		CHECK_CE( t.GetTargets( OUT src_targets ) and src_targets.size() == 2 );

		RenderTargets_t	dst_targets;
		CHECK_CE( t._GetDstTargets( OUT dst_targets ) and dst_targets.size() == 1 );

		auto&	src_rt0	= src_targets[0];
		auto&	src_rt1	= src_targets[1];
		auto&	dst_rt	= dst_targets[0];

	//	ASSERT( src_rt0.colorSpace == dst_rt.colorSpace );
	//	ASSERT( src_rt1.colorSpace == dst_rt.colorSpace );

		DirectCtx::Transfer		ctx{ RenderCoro_Get() };

		ctx.AccumBarriers()
			.ImageBarrier( dst_rt.imageId,	dst_rt.initialState | EResourceState::Invalidate,	EResourceState::BlitDst )
			.ImageBarrier( src_rt0.imageId,	src_rt0.finalState,									EResourceState::BlitSrc )
			.ImageBarrier( src_rt1.imageId,	src_rt1.finalState,									EResourceState::BlitSrc );

		ImageBlit	region;
		region.srcOffset0	= int3{ src_rt0.region.left,		src_rt0.region.top,		0 };
		region.srcOffset1	= int3{ src_rt0.region.right,		src_rt0.region.bottom,	1 };
		region.dstOffset0	= int3{ dst_rt.region.left,			dst_rt.region.top,		0 };
		region.dstOffset1	= int3{ dst_rt.region.CenterX(),	dst_rt.region.bottom,	1 };
		region.srcSubres	= { EImageAspect::Color, 0_mipmap, src_rt0.layer, 1u };
		region.dstSubres	= { EImageAspect::Color, 0_mipmap, dst_rt.layer,  1u };

		ctx.BlitImage( src_rt0.imageId, dst_rt.imageId, EBlitFilter::Linear, ArrayView<ImageBlit>{ &region, 1 });

		region.srcOffset0	= int3{ src_rt1.region.left,		src_rt1.region.top,		0 };
		region.srcOffset1	= int3{ src_rt1.region.right,		src_rt1.region.bottom,	1 };
		region.dstOffset0	= int3{ dst_rt.region.CenterX(),	dst_rt.region.top,		0 };
		region.dstOffset1	= int3{ dst_rt.region.right,		dst_rt.region.bottom,	1 };
		region.srcSubres	= { EImageAspect::Color, 0_mipmap, src_rt1.layer, 1u };
		region.dstSubres	= { EImageAspect::Color, 0_mipmap, dst_rt.layer,  1u };

		ctx.BlitImage( src_rt1.imageId, dst_rt.imageId, EBlitFilter::Linear, ArrayView<ImageBlit>{ &region, 1 });

		ctx.AccumBarriers()
			.ImageBarrier( dst_rt.imageId,	EResourceState::BlitDst,	dst_rt.finalState  )
			.ImageBarrier( src_rt0.imageId,	EResourceState::BlitSrc,	src_rt0.finalState )
			.ImageBarrier( src_rt1.imageId,	EResourceState::BlitSrc,	src_rt1.finalState );

		RenderCoro_Execute( ctx );
	}

/*
=================================================
	Begin
=================================================
*/
	AsyncTask  VRDeviceEmulator::VRRenderSurface::Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps) __NE___
	{
		EXLOCK( _guard );
		CHECK_ERR( not _presentBatch );

		auto&	surf = _vrSession._window->GetSurface();
		if_unlikely( not surf.IsInitialized() )
			return null;

		_presentBatch = GraphicsScheduler().BeginCmdBatch( EQueueType::Graphics, 2, {"VR emulator present"} );
		CHECK_ERR( _presentBatch );

		CHECK_ERR( _presentBatch->AddInputDependency( endCmdBatch ));

		_acquireImg = surf.Begin( RVRef(beginCmdBatch), _presentBatch, deps );

		_projIdx.store( _presentBatch->GetFrameId().Remap2() * 2 );

		return _acquireImg;
	}

/*
=================================================
	End
=================================================
*/
	AsyncTask  VRDeviceEmulator::VRRenderSurface::End (ArrayView<AsyncTask> deps) __NE___
	{
		EXLOCK( _guard );
		CHECK_ERR( _presentBatch );

		AsyncTask	acquire		= RVRef(_acquireImg);
		AsyncTask	draw_task	= _presentBatch->Run( _BlitImageTask( *this ), Tuple{acquire}, True{"Last"}, {"VR emulator blit"} );
		AsyncTask	end_task	= _vrSession._window->GetSurface().End( deps );

		_presentBatch = null;
		return end_task;
	}

/*
=================================================
	GetTargets
=================================================
*/
	bool  VRDeviceEmulator::VRRenderSurface::GetTargets (OUT RenderTargets_t &targets) C_NE___
	{
		const usize	idx = _projIdx.load();

		if_likely( VRSurface::GetTargets( OUT targets ))
		{
			for (usize i = 0; i < targets.size(); ++i)
			{
				targets[i].initialState = EResourceState::BlitSrc;
				targets[i].finalState	= EResourceState::BlitSrc;
				targets[i].projection	= &_vrSession._projections[i + idx];
			}
			return true;
		}
		return false;
	}

/*
=================================================
	_GetDstTargets
=================================================
*/
	bool  VRDeviceEmulator::VRRenderSurface::_GetDstTargets (OUT RenderTargets_t &targets) C_NE___
	{
		return _vrSession._window->GetSurface().GetTargets( OUT targets );
	}

/*
=================================================
	SetSurfaceMode
=================================================
*/
	bool  VRDeviceEmulator::VRRenderSurface::SetSurfaceMode (const SurfaceInfo &info) __NE___
	{
		return _vrSession._window->GetSurface().SetSurfaceMode( info );
	}

/*
=================================================
	GetSurfaceFormats
=================================================
*/
	IOutputSurface::SurfaceFormats_t  VRDeviceEmulator::VRRenderSurface::GetSurfaceFormats () C_NE___
	{
		return _vrSession._window->GetSurface().GetSurfaceFormats();
	}

/*
=================================================
	GetPresentModes
=================================================
*/
	IOutputSurface::PresentModes_t  VRDeviceEmulator::VRRenderSurface::GetPresentModes () C_NE___
	{
		return _vrSession._window->GetSurface().GetPresentModes();
	}

/*
=================================================
	GetSurfaceInfo
=================================================
*/
	IOutputSurface::SurfaceInfo  VRDeviceEmulator::VRRenderSurface::GetSurfaceInfo () C_NE___
	{
		auto	result	= _vrSession._window->GetSurface().GetSurfaceInfo();
		result.type		= ESurfaceType::VR;
		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ReadInput
=================================================
*/
	VRDeviceEmulator::InputActions::ActionQueueReader  VRDeviceEmulator::InputActions::ReadInput (FrameUID frameId) C_NE___
	{
		if_likely( _vrSession._window )
		{
			auto	reader = _vrSession._window->InputActions().ReadInput( frameId );

			bool	update_rotation = false;
			{
				SHAREDLOCK( _vrSession._hmdRotationGuard );
				update_rotation = (_vrSession._lastFrameId != frameId);
			}

			if ( update_rotation )
			{
				float2						accum_rotation;
				ActionQueueReader::Header	hdr;

				for (; reader.ReadHeader( OUT hdr );)
				{
					if ( hdr.name == InputActionName{".VRE.Rotate"} )
						accum_rotation += reader.DataCopy<float2>( hdr.offset );
				}

				if ( Any( not IsZero( accum_rotation )))
				{
					EXLOCK( _vrSession._hmdRotationGuard );
					_vrSession._lastFrameId	 = frameId;
					_vrSession._hmdRotation	+= float2{accum_rotation.x, -accum_rotation.y};
					_vrSession._hmdRotation  = Wrap( _vrSession._hmdRotation, float(-Pi), float(Pi) );
				}
			}
			return reader;
		}
		else
			return InputActionsBase::ReadInput( frameId );
	}

/*
=================================================
	NextFrame
=================================================
*/
	void  VRDeviceEmulator::InputActions::NextFrame (FrameUID frameId) __NE___
	{
		if_likely( _vrSession._window )
			_vrSession._window->InputActions().NextFrame( frameId );
		else
			_dbQueue._NextFrame( frameId );
	}

/*
=================================================
	SetMode
=================================================
*/
	bool  VRDeviceEmulator::InputActions::SetMode (InputModeName::Ref value) __NE___
	{
		if_likely( _vrSession._window )
			return _vrSession._window->InputActions().SetMode( value );
		else
			return InputActionsBase::SetMode( value );
	}

/*
=================================================
	LoadSerialized
=================================================
*/
	bool  VRDeviceEmulator::InputActions::LoadSerialized (MemRefRStream &stream) __NE___
	{
		if_likely( _vrSession._window )
		{
			ASSERT( CastAllowed<InputActionsBase>( &_vrSession._window->InputActions() ));
			Cast<InputActionsBase>( &_vrSession._window->InputActions() )->EnableVREmulation();

			return _vrSession._window->InputActions().LoadSerialized( stream );
		}
		return false;
	}

/*
=================================================
	GetReflection
=================================================
*/
	bool  VRDeviceEmulator::InputActions::GetReflection (InputModeName::Ref mode, InputActionName::Ref action, OUT Reflection &result) C_NE___
	{
		if_likely( _vrSession._window )
			return _vrSession._window->InputActions().GetReflection( mode, action, OUT result );
		else
			return InputActionsBase::GetReflection( mode, action, OUT result );
	}

/*
=================================================
	BeginBindAction
=================================================
*/
	bool  VRDeviceEmulator::InputActions::BeginBindAction (InputModeName::Ref mode, InputActionName::Ref action, EValueType type, EGestureType gesture) __NE___
	{
		if_likely( _vrSession._window )
			return _vrSession._window->InputActions().BeginBindAction( mode, action, type, gesture );
		else
			return InputActionsBase::BeginBindAction( mode, action, type, gesture );
	}

/*
=================================================
	EndBindAction
=================================================
*/
	bool  VRDeviceEmulator::InputActions::EndBindAction () __NE___
	{
		if_likely( _vrSession._window )
			return _vrSession._window->InputActions().EndBindAction();
		else
			return InputActionsBase::EndBindAction();
	}

/*
=================================================
	IsBindActionActive
=================================================
*/
	bool  VRDeviceEmulator::InputActions::IsBindActionActive () C_NE___
	{
		if_likely( _vrSession._window )
			return _vrSession._window->InputActions().IsBindActionActive();
		else
			return InputActionsBase::IsBindActionActive();
	}

/*
=================================================
	Serialize
=================================================
*/
	bool  VRDeviceEmulator::InputActions::Serialize (Serializing::Serializer &ser) C_NE___
	{
		if_likely( _vrSession._window )
			return _vrSession._window->InputActions().Serialize( ser );
		else
			return false;
	}

/*
=================================================
	Deserialize
=================================================
*/
	bool  VRDeviceEmulator::InputActions::Deserialize (Serializing::Deserializer &des) __NE___
	{
		if_likely( _vrSession._window )
			return _vrSession._window->InputActions().Deserialize( des );
		else
			return false;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	OnStateChanged
=================================================
*/
	void  VRDeviceEmulator::WindowEventListener::OnStateChanged (IWindow &wnd, EState state) __NE___
	{
		if_unlikely( not _vrSession._window )
			return;

		ASSERT( &wnd == _vrSession._window.get() );

		if_likely( _vrSession._listener )
			_vrSession._listener->OnStateChanged( _vrSession, state );
	}
	
/*
=================================================
	OnSurfaceCreated
=================================================
*/
	void  VRDeviceEmulator::WindowEventListener::OnSurfaceCreated (IWindow &wnd) __NE___
	{
		if_unlikely( not _vrSession._window )
			return;
		
		ASSERT( &wnd == _vrSession._window.get() );

		if_likely( _vrSession._listener )
			_vrSession._listener->OnSurfaceCreated( _vrSession );
	}
	
/*
=================================================
	OnSurfaceDestroyed
=================================================
*/
	void  VRDeviceEmulator::WindowEventListener::OnSurfaceDestroyed (IWindow &wnd) __NE___
	{
		if_unlikely( not _vrSession._window )
			return;
		
		ASSERT( &wnd == _vrSession._window.get() );

		if_likely( _vrSession._listener )
			_vrSession._listener->OnSurfaceDestroyed( _vrSession );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	VRDeviceEmulator::VRDeviceEmulator (ApplicationBase &app, Unique<IWndListener> listener, IInputActions* dstActions) __NE___ :
		VRSessionBase{ app, RVRef(listener) },
		_surface{ *this },
		_input{ *this, InputActionsBase::GetQueue( dstActions )}
	{
		_view[0] = float4x4{
			float4{ 1.00000072f,    -0.000183293581f, -0.000353380980f, -0.000000000f },
			float4{ 0.000182049334f, 0.999995828f,    -0.00308410777f,   0.000000000f },
			float4{ 0.000353740237f, 0.00308382465f,   0.999995828f,    -0.000000000f },
			float4{ 0.0329737701f,  -0.000433419773f,  0.000178515897f,  1.000000000f }
		};
		_view[1] = float4x4{
			float4{  1.00000072f,      0.000182215153f,  0.000351947267f, -0.000000000f },
			float4{ -0.000183455661f,  0.999995947f,     0.00308232009f,   0.000000000f },
			float4{ -0.000351546332f, -0.00308261835f,   0.999995947f,    -0.000000000f },
			float4{ -0.0329739153f,    0.000422920042f, -0.000199772359f,  1.000000000f }
		};
	}

/*
=================================================
	destructor
=================================================
*/
	VRDeviceEmulator::~VRDeviceEmulator () __NE___
	{
		_Destroy();
	}

/*
=================================================
	Setup
=================================================
*/
	bool  VRDeviceEmulator::Setup (const Settings &settings) __NE___
	{
		DRC_EXLOCK( _drCheck );

		const float2	clip_planes = settings.cameraClipPlanes;

		_projections[0].view = _view[0];
		_projections[1].view = _view[1];

	#if 0
		_projections[0].proj = float4x4{
			float4{  0.881646931f,   0.0f,            0.0f,          0.0f },
			float4{  0.0f,           0.703921199f,    0.0f,          0.0f },
			float4{ -0.0761833116f, -0.00306562823f, -1.00100100f,  -1.0f },
			float4{  0.0f,           0.0f,           -0.100100100f,  0.0f }
		};
		_projections[1].proj = float4x4{
			float4{ 0.876052380f,   0.0f,            0.0f,          0.0f },
			float4{ 0.0f,           0.700941682f,    0.0f,          0.0f },
			float4{ 0.0850749388f,  0.00705791591f, -1.00100100f,  -1.0f },
			float4{ 0.0f,           0.0f,           -0.100100100f,  0.0f }
		};
	#elif 1
		{
			const float	scale			= 0.125f;
			const auto	left_eye_proj	= float4{-1.22065103f, 1.04783058f, -1.42496860f, 1.41625845f} * scale; // left, right, top, bottom
			const auto	right_eye_proj	= float4{-1.04437256f, 1.23859596f, -1.41658306f, 1.43672144f} * scale; // left, right, top, bottom

			if ( IsInfinity( clip_planes[1] ))
			{
				_projections[0].proj = float4x4::InfiniteFrustum( RectF{ left_eye_proj [0], left_eye_proj [3], left_eye_proj [1], left_eye_proj [2] }, clip_planes[0] );
				_projections[1].proj = float4x4::InfiniteFrustum( RectF{ right_eye_proj[0], right_eye_proj[3], right_eye_proj[1], right_eye_proj[2] }, clip_planes[0] );
			}
			else
			{
				_projections[0].proj = float4x4::Frustum( RectF{ left_eye_proj [0], left_eye_proj [3], left_eye_proj [1], left_eye_proj [2] }, clip_planes );
				_projections[1].proj = float4x4::Frustum( RectF{ right_eye_proj[0], right_eye_proj[3], right_eye_proj[1], right_eye_proj[2] }, clip_planes );
			}
		}
	#else
		{
			const float		fov_y         = 1.0f;
			const float		aspect        = 1.0f;
			const float		tan_half_fovy = tan( fov_y * 0.5f );
			float4x4		proj;

			proj[0][0] = 1.0f / (aspect * tan_half_fovy);
			proj[1][1] = 1.0f / tan_half_fovy;
			proj[2][2] = -clip_planes[1] / (clip_planes[1] - clip_planes[0]);
			proj[2][3] = -1.0f;
			proj[3][2] = -(clip_planes[1] * clip_planes[0]) / (clip_planes[1] - clip_planes[0]);

			_projections[0].proj = proj;
			_projections[1].proj = proj;
		}
	#endif

	//	_projections[0].invProj = _projections[0].proj.Inversed();
	//	_projections[1].invProj = _projections[1].proj.Inversed();

		_projections[2] = _projections[0];
		_projections[3] = _projections[1];

		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  VRDeviceEmulator::Create () __NE___
	{
		CHECK_ERR( not _window );

		WindowDesc	desc;
		desc.title	= "VRDeviceEmulator";
		desc.size	= {800,600};
		desc.mode	= EWindowMode::NonResizable;

		Unique<WindowEventListener>	listener{ new WindowEventListener{ *this }};

		_window = _app.CreateWindow( RVRef(listener), desc, &_input );
		CHECK_ERR( _window );

		for (uint s = uint(EState::Created), dst = uint(_window->GetState()); s <= dst; ++s)
		{
			_SetStateV2( EState(s) );
		}

		return true;
	}
	
/*
=================================================
	Close
=================================================
*/
	void  VRDeviceEmulator::Close () __NE___
	{
		if ( _window )
			_window->Close();
	}

/*
=================================================
	_Destroy
=================================================
*/
	void  VRDeviceEmulator::_Destroy () __NE___
	{
		DRC_EXLOCK( _drCheck );

		_window.reset();

		_surface.Destroy();

		_DestroyListener();
	}

/*
=================================================
	ProcessMessages
=================================================
*/
	bool  VRDeviceEmulator::ProcessMessages () __NE___
	{
		DRC_EXLOCK( _drCheck );

		if_unlikely( not _window )
			return false;

		// update state
		{
			auto	wnd_state = _window->GetState();
			if ( wnd_state != _wndState )
				_SetStateV2( wnd_state );

			if_unlikely( wnd_state >= EState::Destroyed )
			{
				_Destroy();
				return false;
			}
		}

		RadianVec<float,2>	angle;
		const usize			idx = GraphicsScheduler().GetFrameId().Remap2( 1 ) * 2;
		{
			EXLOCK( _hmdRotationGuard );
			angle.x	= Rad{_hmdRotation.x};
			angle.y	= Rad{_hmdRotation.y};
		}

		const float4x4	pose = float4x4::RotateX( -angle.y ) * float4x4::RotateY( -angle.x );

		for (usize i = 0; i < 2; ++i) {
			_projections[i + idx].view		= _view[i] * pose;
		//	_projections[i + idx].invView	= _projections[i + idx].view.Inversed();
		}

		_input.Update( _app.GetTimeSinceStart() );

		return true;
	}

/*
=================================================
	CreateRenderSurface
=================================================
*/
	bool  VRDeviceEmulator::CreateRenderSurface (const SwapchainDesc &desc) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _window );

		switch ( desc.colorFormat )
		{
			case EPixelFormat::RGBA8_UNorm :
			case EPixelFormat::sRGB8_A8 :
			case EPixelFormat::BGRA8_UNorm :
			case EPixelFormat::sBGR8_A8 :
			case EPixelFormat::RGBA32F :
			case EPixelFormat::RGBA16F :
			case EPixelFormat::RGB10_A2_UNorm :
				break;	// OK
			default :
				RETURN_ERR( "unsupported image format for VR emulator" );
		}

		CHECK_ERR( AllBits( desc.usage, EImageUsage::TransferSrc | EImageUsage::Sampled ));

		// create window render surface
		{
			SwapchainDesc		sw_desc;
			sw_desc.colorFormat	= desc.colorFormat;
			sw_desc.usage		= EImageUsage::ColorAttachment | EImageUsage::Sampled | EImageUsage::TransferDst;
			sw_desc.options		= EImageOpt::BlitDst;

			CHECK_ERR( _window->CreateRenderSurface( sw_desc ));
		}

		VRImageDesc		desc2 = desc;
		desc2.options	|= EImageOpt::BlitSrc;
		desc2.dimension	= ImageDim2_t{ 1024, 1024 };

		return _surface.Create( desc2 );
	}
	
/*
=================================================
	_CreateSwapchain / _DestroySwapchain
=================================================
*/
	void  VRDeviceEmulator::_CreateSwapchain () __NE___
	{
	}

	void  VRDeviceEmulator::_DestroySwapchain () __NE___
	{
	}


} // AE::App
