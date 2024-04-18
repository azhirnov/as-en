// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_OPENVR

# if not defined(AE_ENABLE_VULKAN)
#	error OpenVR requires Vulkan API.
# endif

# include "graphics/Vulkan/VEnumCast.h"
# include "platform/OpenVR/OpenVRDevice.h"

namespace AE::App
{
namespace
{
	using namespace AE::Graphics;

/*
=================================================
	OpenVRMatToMat4
=================================================
*/
	ND_ static float4x4  OpenVRMatToMat4 (const HmdMatrix44_t &mat) __NE___
	{
		return float4x4{
				float4{ mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0] },
				float4{ mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1] },
				float4{ mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2] },
				float4{ mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3] }};
	}

	ND_ static float4x4  OpenVRMatToMat4 (const HmdMatrix34_t &mat) __NE___
	{
		return float4x4{
				float4{ mat.m[0][0], mat.m[1][0], mat.m[2][0], 0.0f },
				float4{ mat.m[0][1], mat.m[1][1], mat.m[2][1], 0.0f },
				float4{ mat.m[0][2], mat.m[1][2], mat.m[2][2], 0.0f },
				float4{ mat.m[0][3], mat.m[1][3], mat.m[2][3], 1.0f }};
	}

	ND_ static float3x3  OpenVRMatToMat3 (const HmdMatrix34_t &mat) __NE___
	{
		return float3x3{
				float3{ mat.m[0][0], mat.m[1][0], mat.m[2][0] },
				float3{ mat.m[0][1], mat.m[1][1], mat.m[2][1] },
				float3{ mat.m[0][2], mat.m[1][2], mat.m[2][2] }};
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	GetTargets
=================================================
*/
	bool  OpenVRDevice::VRRenderSurface::GetTargets (OUT RenderTargets_t &targets) C_NE___
	{
		const usize	idx = 0;//_projIdx.load();

		if_likely( VRSurface::GetTargets( targets ))
		{
			for (usize i = 0; i < targets.size(); ++i)
			{
				targets[i].finalState	= EResourceState::BlitSrc;
				targets[i].projection	= &_vrDev._projections[i + idx];
			}
			return true;
		}
		return false;
	}

/*
=================================================
	SubmitImageTask
=================================================
*/
	class OpenVRDevice::VRRenderSurface::SubmitImageTask final : public Threading::IAsyncTask
	{
	private:
		VRRenderSurface &	_surface;
		const EQueueType	_lastQueue;

	public:
		SubmitImageTask (VRRenderSurface* surf, EQueueType lastQueue) :
			IAsyncTask{ ETaskQueue::PerFrame },
			_surface{ *surf },
			_lastQueue{ lastQueue }
		{}

	  #ifdef AE_ENABLE_VULKAN
		void  Run () __Th_OV
		{
			EXLOCK( _surface._guard );

			auto&			rts			= GraphicsScheduler();
			const auto&		res_mngr	= rts.GetResourceManager();
			const auto&		dev			= rts.GetDevice();
			const auto&		vr_dev		= _surface._vrDev;
			auto			q			= dev.GetQueue( _lastQueue );
			const auto&		desc		= _surface._desc;
			const EVREye	vr_eye[]	= { EVREye_Eye_Left, EVREye_Eye_Right };

			VRTextureBounds_t	bounds;
			bounds.uMin = 0.0f;
			bounds.uMax = 1.0f;
			bounds.vMin = 0.0f;
			bounds.vMax = 1.0f;

			VRVulkanTextureData_t	vk_data;
			vk_data.m_pDevice			= dev.GetVkDevice();
			vk_data.m_pPhysicalDevice	= dev.GetVkPhysicalDevice();
			vk_data.m_pInstance			= dev.GetVkInstance();
			vk_data.m_pQueue			= q->handle;
			vk_data.m_nQueueFamilyIndex	= uint(q->familyIndex);
			vk_data.m_nWidth			= desc.dimension.x;
			vk_data.m_nHeight			= desc.dimension.y;
			vk_data.m_nFormat			= BitCast<uint>( VEnumCast( desc.format ));
			vk_data.m_nSampleCount		= desc.samples.Get();

			for (usize i = 0; i < _surface._images.size(); ++i)
			{
				auto*	img = res_mngr.GetResource( _surface._images[i] );
				ASSERT( img != null );

				vk_data.m_nImage	= BitCast<ulong>( img->Handle() );

				Texture_t	texture	= { &vk_data, ETextureType_TextureType_Vulkan, EColorSpace_ColorSpace_Auto };
				auto		err		= vr_dev._vrCompositor->Submit( vr_eye[i], &texture, &bounds, EVRSubmitFlags_Submit_Default );
				Unused( err );
			}
		}
	  #endif

		StringView  DbgName ()	C_NE_OV	{ return "VRRenderSurface::SubmitImage"; }
	};

/*
=================================================
	Begin
=================================================
*/
	AsyncTask  OpenVRDevice::VRRenderSurface::Begin (CommandBatchPtr, CommandBatchPtr, ArrayView<AsyncTask>) __NE___
	{
		EXLOCK( _guard );

		return null;
	}

/*
=================================================
	End
=================================================
*/
	AsyncTask  OpenVRDevice::VRRenderSurface::End (ArrayView<AsyncTask>) __NE___
	{/*
		EXLOCK( _guard );

		AsyncTask	task = Scheduler().Run<SubmitImageTask>( Tuple{ this, cmdBatch->GetQueueType() },
															 Tuple{ CmdBatchOnSubmit{cmdBatch}, deps });

		// next frame will wait for present task
		//GraphicsScheduler().AddNextFrameDeps( task );

		return task;*/
		return null;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	OpenVRDevice::OpenVRDevice (VRDeviceListener listener, IInputActions* dst) __NE___ :
		VRDeviceBase{ RVRef(listener) },
		_input{ InputActionsBase::GetQueue( dst )},
		_surface{ *this }
	{}

/*
=================================================
	destructor
=================================================
*/
	OpenVRDevice::~OpenVRDevice () __NE___
	{
		_Destroy();
	}

/*
=================================================
	_GetTrackedDeviceString
=================================================
*/
	String  OpenVRDevice::_GetTrackedDeviceString (TrackedDeviceIndex_t device, TrackedDeviceProperty prop, OUT TrackedPropertyError* peError) C_NE___
	{
		uint	req_len = _vrSystem->GetStringTrackedDeviceProperty( device, prop, null, 0, OUT peError );
		if_unlikely( req_len == 0 )
			return "";

		String	result;
		NOTHROW_ERR( result.resize( req_len );)

		req_len = _vrSystem->GetStringTrackedDeviceProperty( device, prop, OUT result.data(), uint(result.size()), OUT peError );
		ASSERT( req_len == result.size() );

		return result;
	}

/*
=================================================
	Create
=================================================
*/
	bool  OpenVRDevice::Create () __NE___
	{
		DRC_EXLOCK( _drCheck );

		if_unlikely( not _LoadLib() )
		{
			AE_LOGI( "failed to load OpenVR library" );
			return false;
		}

		if_unlikely( not _ovr.IsHmdPresent() )
		{
			AE_LOGI( "VR Headset is not present" );
			return false;
		}

		if_unlikely( not _ovr.IsRuntimeInstalled() )
		{
			AE_LOGI( "VR Runtime is not installed" );
			return false;
		}

		EVRInitError	err = EVRInitError_VRInitError_None;
		_hmd = _ovr.InitInternal( OUT &err, EVRApplicationType_VRApplication_Scene );

		if_unlikely( err != EVRInitError_VRInitError_None )
			RETURN_ERR( "VR_Init error: "s << _ovr.GetVRInitErrorAsEnglishDescription( err ));

		_vrSystem = BitCast<IVRSystemTable>( _ovr.GetGenericInterface( ("FnTable:"s << IVRSystem_Version).c_str(), OUT &err ));
		CHECK_ERR( _vrSystem and err == EVRInitError_VRInitError_None );

		AE_LOGI( "driver:  "s << _GetTrackedDeviceString( k_unTrackedDeviceIndex_Hmd, ETrackedDeviceProperty_Prop_TrackingSystemName_String ) <<
				 "display: " << _GetTrackedDeviceString( k_unTrackedDeviceIndex_Hmd, ETrackedDeviceProperty_Prop_SerialNumber_String ));

		_vrCompositor = BitCast<IVRCompositorTable>( _ovr.GetGenericInterface( ("FnTable:"s << IVRCompositor_Version).c_str(), OUT &err ));
		CHECK_ERR( _vrCompositor and err == EVRInitError_VRInitError_None );

		_vrCompositor->SetTrackingSpace( ETrackingUniverseOrigin_TrackingUniverseStanding );	// TODO: move to config

		EDeviceActivityLevel level = _vrSystem->GetTrackedDeviceActivityLevel( k_unTrackedDeviceIndex_Hmd );
		switch_enum( level )
		{
			case EDeviceActivityLevel_k_EDeviceActivityLevel_Unknown :					_hmdState = EState::Created;		break;
			case EDeviceActivityLevel_k_EDeviceActivityLevel_Idle :						_hmdState = EState::Started;		break;
			case EDeviceActivityLevel_k_EDeviceActivityLevel_UserInteraction :			_hmdState = EState::InForeground;	break;
			case EDeviceActivityLevel_k_EDeviceActivityLevel_UserInteraction_Timeout :	_hmdState = EState::InBackground;	break;
			case EDeviceActivityLevel_k_EDeviceActivityLevel_Standby :					_hmdState = EState::InBackground;	break;
			case EDeviceActivityLevel_k_EDeviceActivityLevel_Idle_Timeout :				_hmdState = EState::InBackground;	break;
		}
		switch_end

		for (uint st = uint(EState::Created); st <= uint(_hmdState); ++st)
			_listener->OnStateChanged( *this, EState(st) );

		_InitControllers();

		return true;
	}

/*
=================================================
	_Destroy
=================================================
*/
	void  OpenVRDevice::_Destroy ()
	{
		DRC_EXLOCK( _drCheck );

		_isRunning.store( false );

		if ( _listener )
		{
			if ( _hmdState == EState::InForeground or _hmdState == EState::Focused )
				_SetState( EState::InBackground );

			if ( _hmdState == EState::InBackground )
				_SetState( EState::Stopped );

			_SetState( EState::Destroyed );

			_listener.reset();
		}

		if ( _hmd )
		{
			_ovr.ShutdownInternal();
			_hmd			= Zero;
			_vrSystem		= null;
			_vrCompositor	= null;
		}

		_openVRLib.Unload();
	}

/*
=================================================
	_LoadLib
=================================================
*/
	bool  OpenVRDevice::_LoadLib ()
	{
		#ifdef AE_PLATFORM_WINDOWS
		constexpr char	lib_name[] = "openvr_api.dll";
		#else
		constexpr char	lib_name[] = "libopenvr_api.so";
		#endif
		#define VR_LOAD( _name_ )	res &= _openVRLib.GetProcAddr( "VR_" #_name_, OUT _ovr._name_ )

		if ( not _openVRLib.Load( lib_name ))
			return false;

		bool	res = true;
		res &= VR_LOAD( InitInternal );
		res &= VR_LOAD( ShutdownInternal );
		res &= VR_LOAD( IsHmdPresent );
		res &= VR_LOAD( GetGenericInterface );
		res &= VR_LOAD( IsRuntimeInstalled );
		res &= VR_LOAD( GetVRInitErrorAsSymbol );
		res &= VR_LOAD( GetVRInitErrorAsEnglishDescription );
		return res;

	#undef VR_LOAD
	}

/*
=================================================
	Setup
=================================================
*/
	bool  OpenVRDevice::Setup (const Settings &) __NE___
	{
		DRC_EXLOCK( _drCheck );

		_SetupCamera();
		return false;
	}

/*
=================================================
	_SetupCamera
=================================================
*/
	void  OpenVRDevice::_SetupCamera ()
	{
		const float2	clip_planes;

		_projections[0].proj = OpenVRMatToMat4( _vrSystem->GetProjectionMatrix( EVREye_Eye_Left, clip_planes[0], clip_planes[1] ));
		_projections[1].proj = OpenVRMatToMat4( _vrSystem->GetProjectionMatrix( EVREye_Eye_Right, clip_planes[0], clip_planes[1] ));
		_projections[0].view = OpenVRMatToMat4( _vrSystem->GetEyeToHeadTransform( EVREye_Eye_Left )).Inverse();
		_projections[1].view = OpenVRMatToMat4( _vrSystem->GetEyeToHeadTransform( EVREye_Eye_Right )).Inverse();

		_projections[2] = _projections[0];
		_projections[3] = _projections[1];
	}

/*
=================================================
	Update
=================================================
*/
	bool  OpenVRDevice::Update (Duration_t timeSinceStart) __NE___
	{
		if_unlikely( not _isRunning.load() )
			return false;

		DRC_EXLOCK( _drCheck );

		if_unlikely( _vrSystem == null or _vrCompositor == null )
			return false;

		VREvent_t	ev;
		while ( _vrSystem->PollNextEvent( OUT &ev, sizeof(ev) ))
		{
			if ( ev.trackedDeviceIndex == k_unTrackedDeviceIndex_Hmd )
				_ProcessHmdEvents( ev );

			auto	iter = _controllers.find( ev.trackedDeviceIndex );
			if ( iter != _controllers.end() )
				_ProcessControllerEvents( iter->second, ev );
		}

		const auto	now = TimePoint_t::clock::now();

		for (auto [idx, cont] : _controllers)
		{
			VRControllerState_t	state;
			if ( _vrSystem->GetControllerState( idx, OUT &state, sizeof(state) ))
			{
				if ( state.unPacketNum == cont.lastPacket )
					continue;

				cont.lastPacket	= state.unPacketNum;
				cont.lastUpdate	= now;
				cont.id			= _GetControllerID( idx );

				for (usize i = 0; i < CountOf(state.rAxis); ++i)
				{
					auto&	axis	= cont.axis[i];
					float2	v		= float2{ state.rAxis[i].x, state.rAxis[i].y };
					float2	old		= axis.value;
					float2	del		= v - old;

					axis.value = v;

					if ( not axis.pressed )
						continue;

					_input.OnAxisStateChanged( cont.id, uint(i), v, del, 0.f );
				}
			}
		}

		_UpdateHMDMatrixPose();

		_input.Update( timeSinceStart );

		return true;
	}

/*
=================================================
	_ProcessControllerEvents
=================================================
*/
	void  OpenVRDevice::_ProcessControllerEvents (INOUT Controller &cont, const VREvent_t &ev)
	{
		switch( ev.eventType )
		{
			case EVREventType_VREvent_ButtonPress :
			{
				const uint	key		= ev.data.controller.button;
				bool &		curr	= cont.keys[ key ];
				auto		state	= curr ? EGestureState::Update : EGestureState::Begin;
				curr = true;

				_input.OnButton( cont.id, key, state );
				break;
			}

			case EVREventType_VREvent_ButtonUnpress :
			{
				const uint	key	 = ev.data.controller.button;
				cont.keys[ key ] = false;

				_input.OnButton( cont.id, key, EGestureState::End );
				break;
			}

			case EVREventType_VREvent_ButtonTouch :
			case EVREventType_VREvent_ButtonUntouch :
			{
				uint	idx = ev.data.controller.button - EVRButtonId_k_EButton_Axis0;
				if ( idx < cont.axis.size() ) {
					cont.axis[idx].pressed = (ev.eventType == EVREventType_VREvent_ButtonTouch);
				}
				break;
			}

			case EVREventType_VREvent_MouseMove :
				AE_LOGI( "MouseMove: " + ToString(ev.data.mouse.button) + ", dev: " + ToString(ev.trackedDeviceIndex) );
				break;

			case EVREventType_VREvent_MouseButtonDown :
				AE_LOGI( "MouseButtonDown: " + ToString(ev.data.mouse.button) + ", dev: " + ToString(ev.trackedDeviceIndex) );
				break;

			case EVREventType_VREvent_MouseButtonUp :
				AE_LOGI( "MouseButtonUp: " + ToString(ev.data.mouse.button) + ", dev: " + ToString(ev.trackedDeviceIndex) );
				break;

			case EVREventType_VREvent_TouchPadMove :
				AE_LOGI( "TouchPadMove: " + ToString(ev.data.mouse.button) + ", dev: " + ToString(ev.trackedDeviceIndex) );
				break;
		}
	}

/*
=================================================
	_UpdateHMDMatrixPose
=================================================
*/
	void  OpenVRDevice::_UpdateHMDMatrixPose ()
	{
		// TODO: game pose?
		CHECK( _vrCompositor->WaitGetPoses( OUT _trackedDevicePose, uint(CountOf(_trackedDevicePose)), null, 0 ) == EVRCompositorError_VRCompositorError_None );

		auto&	hmd_pose = _trackedDevicePose[ k_unTrackedDeviceIndex_Hmd ];

		if ( hmd_pose.bPoseIsValid )
		{
			auto&	mat		= hmd_pose.mDeviceToAbsoluteTracking;
			auto&	vel		= hmd_pose.vVelocity;
			auto&	avel	= hmd_pose.vAngularVelocity;

			_input.SetPose(
				HMD_ID,
				OpenVRMatToMat3( mat ).Inverse(),					// rotation
				float3{ mat.m[0][3], mat.m[1][3], mat.m[2][3] },	// position
				float3{ vel.v[0], vel.v[1], vel.v[2] },				// velocity
				float3{ avel.v[0], avel.v[1], avel.v[2] });			// angular velocity
		}

		for (auto [idx, cont] : _controllers)
		{
			auto&	cont_pose	= _trackedDevicePose[ idx ];

			if ( not cont_pose.bPoseIsValid )
				continue;

			auto&	mat		= cont_pose.mDeviceToAbsoluteTracking;
			auto&	vel		= cont_pose.vVelocity;
			auto&	avel	= cont_pose.vAngularVelocity;

			_input.SetPose(
				_GetControllerID( idx ),
				OpenVRMatToMat3( mat ).Inverse(),					// rotation
				float3{ mat.m[0][3], mat.m[1][3], mat.m[2][3] },	// position
				float3{ vel.v[0], vel.v[1], vel.v[2] },				// velocity
				float3{ avel.v[0], avel.v[1], avel.v[2] });			// angular velocity
		}
	}

/*
=================================================
	_ProcessHmdEvents
=================================================
*/
	void  OpenVRDevice::_ProcessHmdEvents (const VREvent_t &ev)
	{
		switch( ev.eventType )
		{
			case EVREventType_VREvent_TrackedDeviceActivated :
			case EVREventType_VREvent_TrackedDeviceUserInteractionStarted :
				if ( _hmdState < EState::Started )
				{
					_hmdState = EState::Started;

					if_likely( _listener )
						_listener->OnStateChanged( *this, _hmdState );
				}
				break;

			case EVREventType_VREvent_TrackedDeviceUserInteractionEnded :
				// to background
				if ( _hmdState == EState::InForeground or _hmdState == EState::Focused )
				{
					_hmdState = EState::InBackground;

					if_likely( _listener )
						_listener->OnStateChanged( *this, _hmdState );
				}
				// stop
				{
					_hmdState = EState::Stopped;

					if_likely( _listener )
						_listener->OnStateChanged( *this, _hmdState );
				}
				break;

			case EVREventType_VREvent_ButtonPress :
				if ( ev.data.controller.button == EVRButtonId_k_EButton_ProximitySensor )
				{
					_hmdState = EState::Focused;

					if_likely( _listener )
						_listener->OnStateChanged( *this, _hmdState );
				}
				break;

			case EVREventType_VREvent_ButtonUnpress :
				if ( ev.data.controller.button == EVRButtonId_k_EButton_ProximitySensor )
				{
					_hmdState = EState::InBackground;

					if_likely( _listener )
						_listener->OnStateChanged( *this, _hmdState );
				}
				break;

			case EVREventType_VREvent_Quit :
				_hmdState = EState::Destroyed;

				if_likely( _listener )
					_listener->OnStateChanged( *this, _hmdState );
				break;

			case EVREventType_VREvent_LensDistortionChanged :
				_SetupCamera();
				AE_LOGI( "LensDistortionChanged" );
				break;

			case EVREventType_VREvent_PropertyChanged :
				//AE_LOGI( "PropertyChanged" );
				break;
		}
	}

/*
=================================================
	_InitControllers
=================================================
*/
	void  OpenVRDevice::_InitControllers ()
	{
		const auto	now = TimePoint_t::clock::now();

		_controllers.clear();
		for (TrackedDeviceIndex_t i = 0; i < k_unMaxTrackedDeviceCount; ++i)
		{
			ETrackedDeviceClass	dev_class = _vrSystem->GetTrackedDeviceClass( i );

			if ( dev_class == ETrackedDeviceClass_TrackedDeviceClass_Controller or
				 dev_class == ETrackedDeviceClass_TrackedDeviceClass_GenericTracker )
			{
				Controller	cont;
				cont.id			= _GetControllerID( i );
				cont.lastPacket	= ~0u;
				cont.lastUpdate	= now;
				cont.keys.fill( false );

				_controllers.insert_or_assign( i, cont );
			}
		}
	}

/*
=================================================
	_GetControllerID
=================================================
*/
	ControllerID  OpenVRDevice::_GetControllerID (uint tdi) const
	{
		ETrackedControllerRole	role = _vrSystem->GetControllerRoleForTrackedDeviceIndex( tdi );
		switch_enum( role )
		{
			case ETrackedControllerRole_TrackedControllerRole_LeftHand :	return LeftHand_ID;
			case ETrackedControllerRole_TrackedControllerRole_RightHand :	return RightHand_ID;
			case ETrackedControllerRole_TrackedControllerRole_Invalid :
			case ETrackedControllerRole_TrackedControllerRole_OptOut :
			case ETrackedControllerRole_TrackedControllerRole_Treadmill :
			case ETrackedControllerRole_TrackedControllerRole_Max :			break;
		}
		switch_end
		return ControllerID::Invalid;
	}

/*
=================================================
	GetRequiredVkPhysicalDevice
=================================================
*/
#ifdef AE_ENABLE_VULKAN
	VkPhysicalDevice  OpenVRDevice::GetRequiredVkPhysicalDevice (VkInstance inst) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( _vrSystem );

		ulong	hmd_physical_device = 0;
		_vrSystem->GetOutputDevice( OUT &hmd_physical_device, ETextureType_TextureType_Vulkan, inst );

		return BitCast<VkPhysicalDevice>( hmd_physical_device );
	}
#endif

/*
=================================================
	CreateRenderSurface
=================================================
*/
	bool  OpenVRDevice::CreateRenderSurface (const VRImageDesc &desc) __NE___
	{
		switch ( desc.format )
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
				RETURN_ERR( "unsupported image format for OpenVR" );
		}

		CHECK_ERR( AllBits( desc.usage, EImageUsage::TransferSrc | EImageUsage::Sampled ));

		return _surface.Create( desc );
	}


} // AE::App

#endif // AE_ENABLE_OPENVR
