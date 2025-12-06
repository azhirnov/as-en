// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_OPENXR
# include "platform/OpenXR/OpenXRLoader.h"

namespace AE::App
{

	//
	// OpenXR Device
	//

	class OpenXRDevice : public OpenXRInstanceFn
	{
	// types
	public:
		using XRVersion		= TVersion3< "XRVersion"_Hash >;

		static constexpr XrDebugUtilsMessageSeverityFlagsEXT	c_DefaultDebugMessageSeverity =	//XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
																								//XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
																								XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
																								XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		static constexpr XrDebugUtilsMessageTypeFlagsEXT		c_DefaultDebugMessageTypes	=	XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
																								XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
																								XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
																								XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;

		#define XRFEATS_STRUCT
		#include "xr_features.h"
		#undef  XRFEATS_STRUCT

	protected:
		using ExtensionName_t	= FixedString<XR_MAX_EXTENSION_NAME_SIZE>;
		using ExtensionSet_t	= FlatHashSet< ExtensionName_t >;


	// variables
	protected:
		XrSession				_xrSession		= Default;
		XrInstance				_xrInstance		= Default;
		XrSystemId				_xrSystemId		= Default;		// physical device

		XrFormFactor			_formFactor		= XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
		XrSystemProperties		_systemProps;

		XRExtensions			_extensions;
		ExtensionSet_t			_extensionNames;

		DRC_ONLY(
			RWDataRaceCheck		_drCheck;)


	// methods
	public:
		ND_ XrInstance			GetXrInstance ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _xrInstance; }
		ND_ XrSystemId			GetXrSystemId ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _xrSystemId; }
		ND_ XrSession			GetXrSession ()									C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _xrSession; }

		ND_ XRExtensions const&	GetExtensions ()								C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _extensions; }

		ND_ bool				IsInitialized ()								C_NE___	{ return GetXrSession() != Default; }
		ND_ bool				HasExtension (StringView name)					C_NE___;


		// debug //
		void  BeginDebugGroup (NtStringView name)								C_NE___;
		void  EndDebugGroup ()													C_NE___;
		void  InsertLabel (NtStringView label)									C_NE___;
		void  SetObjectName (ulong id, NtStringView name, XrObjectType type)	C_NE___;

	protected:
		OpenXRDevice ()															__NE___;
		~OpenXRDevice ()														__NE___;
	};



	//
	// OpenXR Device Initializer
	//

	class OpenXRDeviceInitializer final : public OpenXRDevice
	{
	// types
	public:
		struct InstanceCreateInfo
		{
			U8StringView				appName				= u8"" AE_ENGINE_NAME "-XRApp";
			ArrayView<const char*>		layers;
			ArrayView<const char*>		extensions;
			XRVersion					version				{1, 1, XR_VERSION_PATCH(XR_CURRENT_API_VERSION) };
			uint						appVer				= 0;
			StringView					xrLib;

			InstanceCreateInfo () __NE___ {}
		};

		struct ObjectDbgInfo
		{
			StringView		type;
			StringView		name;
			ulong			handle;
		};

		struct DebugReport
		{
			ArrayView<ObjectDbgInfo>	objects;
			StringView					message;
			bool						isError		= false;
		};
		using DebugReport_t = Function< void (const DebugReport &) >;

	private:
		struct DbgReportData
		{
			XrDebugUtilsMessengerEXT	debugUtilsMessenger		= Default;
			DebugReport_t				callback;

			bool						breakOnValidationError	= true;
			Array<ObjectDbgInfo>		tempObjectDbgInfos;
			String						tempString;
		};


	// variables
	private:
	  #ifndef AE_CFG_RELEASE
		Synchronized< SharedMutex,
			DbgReportData >			_dbgReport;
	  #endif

		bool						_enableInfoLog		= false;


	// methods
	public:
		explicit OpenXRDeviceInitializer (Bool enableInfoLog = False{})										__NE___;
		~OpenXRDeviceInitializer ()																			__NE___;

		ND_ bool  CreateInstance (const InstanceCreateInfo &)												__NE___;
			void  DestroyInstance ()																		__NE___;

		ND_ bool  CreateDebugCallback (XrDebugUtilsMessageSeverityFlagsEXT	severity,
									   XrDebugUtilsMessageTypeFlagsEXT		type,
									   DebugReport_t						callback = Default)				__NE___;
			void  DestroyDebugCallback ()																	__NE___;

		ND_ bool  CreateSession ()																			__NE___;
			void  DestroySession ()																			__NE___;

	private:
		#define XRFEATS_FN_DECL
		#include "xr_features.h"
		#undef  XRFEATS_FN_DECL

		bool  _PlatformSpecificSetup (const InstanceCreateInfo &info)										__NE___;
		void  _OnCreateInstance (ArrayView<const char*> layers, ArrayView<const char*> ext)					__NE___;

		void  _ValidateLayers (INOUT Array<const char*> &layers, Bool silent)								C_Th___;
		void  _ValidateExtensions (Array<const char*> layers, INOUT Array<const char*> &ext, Bool silent)	C_Th___;

		void  _LogInstance (ArrayView<const char*> layers)													C_NE___;
		void  _LogSession ()																				C_NE___;

		void  _LogViewConfigurationViews ()																	C_NE___;
		void  _LogSwapchainFormats ()																		C_NE___;

		static XrBool32  _DebugMessageCallback (XrDebugUtilsMessageSeverityFlagsEXT, XrDebugUtilsMessageTypeFlagsEXT,
												const XrDebugUtilsMessengerCallbackDataEXT*, void*)			__NE___;
	};


} // AE::App

#endif // AE_ENABLE_OPENXR
