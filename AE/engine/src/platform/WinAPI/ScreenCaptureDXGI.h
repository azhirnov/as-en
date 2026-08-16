// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	refs
	https://github.com/microsoft/Windows-classic-samples/tree/main/Samples/DXGIDesktopDuplication
	https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/desktop-dup-api?redirectedfrom=MSDN
*/

#pragma once

#ifdef AE_PLATFORM_WINDOWS
# include "platform/Public/ScreenCapture.h"
# include "platform/Private/ApplicationBase.h"

namespace AE::App
{

	//
	// Screen Capture DXGI
	//

	class ScreenCaptureDXGI : public IScreenCapture
	{
	// variables
	protected:
		Library						_dx11Lib;
		Library						_dxgiLib;

		StdThread					_dxThread;
		Atomic<bool>				_looping;
		Atomic<bool>				_complete;

		void*						_dxDevice				= null;		// ID3D11Device
		void*						_dxContext				= null;		// ID3D11DeviceContext

	  #ifdef AE_DEBUG
		void*						_dxDebug				= null;		// IDXGIDebug1
		void*						_dxDebugQueue			= null;		// IDXGIInfoQueue
	  #endif

		void*						_acquiredDesktopImage	= null;		// ID3D11Texture2D
		void*						_desktopDuplication		= null;		// IDXGIOutputDuplication

		Config						_config;

		ApplicationBase const&		_app;

		bool						_inHostMemory			= false;
		Monitor::EOrientation		_rotation				= Default;
		uint2						_displayDim;
		EPixelFormat				_surfaceFormat			= Default;
		float						_refreshRate			= 0.f;			// Hz

		HighResClock::time_point	_lastPresentTime;
		HighResClock::time_point	_lastMouseUpdateTime;

		Array<uint>					_tempBuffer;


	// methods
	public:
		ScreenCaptureDXGI (const ApplicationBase &app)		__NE___ : _app{app} {}
		~ScreenCaptureDXGI ()								__NE_OV;

		Description  GetDescription ()						C_NE_OV;

	protected:
		ND_ bool  _InitDX11 ()								__NE___;
		ND_ bool  _OpenDesktopInThread ()					__NE___;
		ND_ bool  _InitDuplication ()						__NE___;
			void  _ReleaseDuplication ()					__NE___;
			void  _Destroy ()								__NE___;

		ND_ bool  _GetFrame (OUT bool		&outTimeout,
							 OUT bool		&outRecreate,
							 OUT FrameInfo	&frameInfo,
							 milliseconds	timeout)		__NE___;
			void  _ReleaseFrame ()							__NE___;

	// interface
	protected:
		ND_ virtual bool  _FindAdapter (void* factory, OUT void** adapter) __NE___ = 0;
	};
//-----------------------------------------------------------------------------



	//
	// Screen Capture DXGI with Host access
	//
	class ScreenCaptureDXGI_HostAccess final : public ScreenCaptureDXGI
	{
	// types
	private:
		static constexpr uint		QueueSize = 4;
		using StagingImages_t		= StaticArray< void*, QueueSize >;		// ID3D11Texture2D[]
		using FrameInfos_t			= StaticArray< FrameInfo, QueueSize >;
		using Queue_t				= FixedArray< ubyte, QueueSize >;
		using UnusedImagesBits_t	= Bitfield<uint>;

		struct SyncAccessData
		{
			UnusedImagesBits_t	unusedImages;
			Queue_t				captureQueue;
			ubyte				index			= UMax;
			ErrorCode			error			= ErrorCode::OK;
			void*				mappedPtr		= null;
		};
		using SyncAccess = Threading::Synchronized< Threading::RWSpinLock, SyncAccessData >;


	// variables
	private:
		StagingImages_t		_dxStagingImages	= {};
		// TODO: mipmaps
		FrameInfos_t		_frameInfos;

		SyncAccess			_syncAccess;
		Bytes				_rowPitch;

		SyncReadImageFn_t	_syncRead;


	// methods
	public:
		ScreenCaptureDXGI_HostAccess (const ApplicationBase &app)				__NE___ : ScreenCaptureDXGI{app} {}
		~ScreenCaptureDXGI_HostAccess ()										__NE_OV;

		bool		Start (const Config &)										__NE___;
		void		Finish ()													__NE_OV;
		AsyncTask	FinishAsync ()												__NE_OV;

		ErrorCode	AcquireImage (OUT ImageID &, OUT ImageViewID &, OUT FrameInfo &,
								  Graphics::CommandBatch&, milliseconds)		__NE_OV  { return ErrorCode::Error_NotSupported; }

		ErrorCode	ReadHostImage (const ReadImageFn_t &)						__NE_OV;

		bool		SetReadImageCallback (SyncReadImageFn_t fn)					__NE_OV;

		EState		GetState ()													C_NE_OV;

	private:
		void  _ThreadFn ()														__NE___;
		void  _DestroyStagingImages ()											__NE___;
		void  _MapNextImage ()													__NE___;
		bool  _CopyToStaging (const FrameInfo &)								__NE___;
		bool  _FindAdapter (void* factory, OUT void** adapter)					__NE_OV;
		bool  _RecreateDuplication ()											__NE___;

		static auto  _FinishTask (RC<ScreenCaptureDXGI_HostAccess>)				__NE___ -> InlineCoro<ETaskQueue::Background>;
	};
//-----------------------------------------------------------------------------



# ifdef AE_ENABLE_VULKAN

	//
	// Screen Capture DXGI Vulkan interop
	//
	class ScreenCaptureDXGI_Vulkan final : public ScreenCaptureDXGI, public Graphics::VulkanInstanceFn
	{
	// types
	private:
		using ImageID		= Graphics::ImageID;
		using ImageViewID	= Graphics::ImageViewID;

		struct SharedImage
		{
			void*					dxHandle	= null;
			VkDeviceMemory			vkMemory	= Default;
			Strong<ImageID>			image;
			Strong<ImageViewID>		view;

			UntypedStorage<72,8>	keyedMutexAcqRel;	// VkWin32KeyedMutexAcquireReleaseInfoKHR
		};


	// variables
	private:
		PFN_vkVoidFunction		_fnGetMemoryWin32HandleProperties	= null;


	// methods
	public:
		ScreenCaptureDXGI_Vulkan (const ApplicationBase &app)			__NE___ : ScreenCaptureDXGI{app} {}
		~ScreenCaptureDXGI_Vulkan ()									__NE_OV;

		bool		Start (const Config &)								__NE___;
		void		Finish ()											__NE_OV;
		AsyncTask	FinishAsync ()										__NE_OV;

		ErrorCode	AcquireImage (OUT ImageID				&imageId,
								  OUT ImageViewID			&viewId,
								  OUT FrameInfo				&info,
								  Graphics::CommandBatch	&cmdBatch,
								  milliseconds				timeout)	__NE_OV;

		ErrorCode	ReadHostImage (const ReadImageFn_t &)				__NE_OV	{ return ErrorCode::Error_NotSupported; }

		bool		SetReadImageCallback (SyncReadImageFn_t)			__NE_OV	{ return false; }

		EState		GetState ()											C_NE_OV;

	private:
		ND_ bool  _CreateVulkanImage (void* acquiredDesktopImage,
									  OUT SharedImage &)				__NE___;
			void  _DestroyVulkanImage (SharedImage &)					__NE___;
		ND_ bool  _FindAdapter (void* factory, OUT void** adapter)		__NE_OV;
		ND_ bool  _CheckVulkanCompatibility ()							__NE___;

		static auto  _FinishTask (RC<ScreenCaptureDXGI_Vulkan>)			__NE___ -> InlineCoro<ETaskQueue::Background>;
	};

# endif // AE_ENABLE_VULKAN

} // AE::App

#endif // AE_PLATFORM_WINDOWS
