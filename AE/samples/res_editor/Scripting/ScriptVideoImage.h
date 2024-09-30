// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptDynamicVars.h"
#include "res_editor/Resources/VideoImage.h"
#include "res_editor/Resources/VideoImage2.h"

namespace AE::ResEditor
{

	//
	// Video Image
	//

	class ScriptVideoImage final : public EnableScriptRC
	{
	// types
	public:
		using VideImage_t		= Union< NullUnion, RC<VideoImage>, RC<VideoImage2> >;
		using VideoStreamInfo	= Video::IVideoDecoder::VideoStreamInfo;


	// variables
	private:
		EResourceUsage				_resUsage		= Default;
		EPixelFormat				_format			= Default;
		uint						_imageType		= 0;		// EImageType
		uint2						_dim;
		VFS::FileName				_videoFile;
		String						_dbgName;

		SamplerDesc					_sampDesc;
		SamplerYcbcrConversionDesc	_ycbcrDesc;
		String						_sampName;
		String						_ycbcrSampName;

		ScriptDynamicDimPtr			_outDynSize;	// image dimension will change this value

		VideImage_t					_resource;

		Promise<VideoStreamInfo>	_videoInfo;


	// methods
	public:
		ScriptVideoImage () {}
		ScriptVideoImage (EPixelFormat format, const String &filename)	__Th___;
		~ScriptVideoImage ();

		void  Name (const String &name)									__Th___;
		void  AddUsage (EResourceUsage usage)							__Th___;

		void  SetSampler (const String &name)							__Th___;
		void  Ycbcr_SetFormat (EPixelFormat value)						__Th___;
		void  Ycbcr_SetModel (ESamplerYcbcrModelConversion value)		__Th___;
		void  Ycbcr_SetRange (ESamplerYcbcrRange value)					__Th___;
		void  Ycbcr_SetComponents (const String &value)					__Th___;
		void  Ycbcr_SetXChromaOffset (ESamplerChromaLocation value)		__Th___;
		void  Ycbcr_SetYChromaOffset (ESamplerChromaLocation value)		__Th___;
		void  Ycbcr_SetChromaFilter (EFilter value)						__Th___;
		void  Ycbcr_ForceExplicitReconstruction (bool value)			__Th___;

		ND_ ScriptDynamicDim*	Dimension ()							__Th___;
		ND_ uint				ImageType ()							C_NE___	{ return _imageType; }
		ND_ bool				HasYcbcrSampler ()						C_NE___	{ return _ycbcrDesc != Default; }
		ND_ auto const&			GetYcbcrDesc ()							C_NE___	{ return _ycbcrDesc; }
		ND_ String const&		GetSamplerName ()						C_NE___	{ return _ycbcrSampName; }

		static void  Bind (const ScriptEnginePtr &se)					__Th___;

		ND_ VideImage_t	ToResource (PipelinePackID packId)				__Th___;
			void		Validate (String sampName)						__Th___;

	private:
			void		_CreateYcbcrSampler (String)					__Th___;
	};


} // AE::ResEditor
