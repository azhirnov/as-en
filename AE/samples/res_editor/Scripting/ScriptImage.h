// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptDynamicVars.h"
#include "res_editor/Resources/Image.h"

namespace AE::ResEditor
{

	//
	// Image
	//

	class ScriptImage final : public EnableScriptRC
	{
	// types
	public:
		enum class EImageType : uint
		{
			Unknown,
			ConstDataFromFile,
			Storage,
			RenderTarget,
		};

		using ELoadOpFlags = Image::ELoadOpFlags;

	private:
		using LoadOps_t = Array< Image::LoadOp >;


	// variables
	private:
		ImageDesc				_desc;
		ImageViewDesc			_viewDesc;
		const bool				_descDefined	= false;

		EResourceUsage			_resUsage		= Default;
		uint					_imageType		= 0;		// PipelineCompiler::EImageType
		LoadOps_t				_loadOps;
		String					_dbgName;

		ScriptDynamicDimPtr		_inDynSize;		// image dimension depends on it
		ScriptDynamicDimPtr		_outDynSize;	// image dimension will change this value

		RC<Image>				_resource;
		ScriptImagePtr			_base;


	// methods
	public:
		ScriptImage () = delete;
		ScriptImage (uint imageType, const String &filename)						__Th___;
		ScriptImage (EPixelFormat format, const ScriptDynamicDimPtr &ds)			__Th___;
		ScriptImage (EPixelFormat format, const packed_uint3 &dim)					__Th___;
		ScriptImage (EPixelFormat format, const packed_uint3 &dim,
					 const ImageLayer &layers, const MipmapLevel &mipmaps)			__Th___;
		ScriptImage (EPixelFormat format, const ScriptDynamicDimPtr &ds,
					 const ImageLayer &layers, const MipmapLevel &mipmaps)			__Th___;
		~ScriptImage ();

		void  Name (const String &name)												__Th___;

		void  AddUsage (EResourceUsage usage)										__Th___;
		void  SetSwizzle (const String &value)										__Th___;
		void  SetAspectMask (EImageAspect value)									__Th___;

		void  LoadLayer1 (const String &filename, uint layer)						__Th___;
		void  LoadLayer2 (const String &filename, uint layer, ELoadOpFlags flags)	__Th___;
		void  LoadLayer3 (const String &filename, uint layer, uint flags)			__Th___;

		ND_ packed_uint2		Dimension2 ()										C_Th___	{ return packed_uint2(Dimension3()); }
		ND_ packed_uint3		Dimension2_Layers ()								C_Th___	{ return packed_uint3{Dimension2(), ArrayLayers()}; }
		ND_ packed_uint3		Dimension3 ()										C_Th___;

		ND_ bool				IsMutableDimension ()								C_Th___;
		ND_ ScriptDynamicDim*	Dimension ()										C_Th___;
		ND_ ScriptDynamicDimPtr	DimensionRC ()										C_Th___;
		ND_ RC<DynamicDim>		DynamicDimension ()									C_Th___;

		ND_ uint				ArrayLayers ()										C_Th___;
		ND_ uint				MipmapCount ()										C_Th___;

		ND_ ImageDesc			Description ()										C_NE___;
		ND_ ImageViewDesc		ViewDescription ()									C_NE___	{ return _viewDesc; }
		ND_ uint				ImageType ()										C_NE___	{ return _imageType; }

		ND_ bool				IsColor ()											C_NE___	{ return not IsDepthOrStencil(); }
		ND_ bool				HasDepth ()											C_NE___;
		ND_ bool				HasStencil ()										C_NE___;
		ND_ bool				IsDepthOrStencil ()									C_NE___;
		ND_ bool				IsDepthAndStencil ()								C_NE___	{ return HasDepth() and HasStencil(); }

			ScriptImage*		CreateView1 (EImage				viewType,
											 const MipmapLevel&	baseMipmap,
											 uint				mipmapCount,
											 const ImageLayer&	baseLayer,
											 uint				layerCount)			__Th___;

			ScriptImage*		CreateView2 (EImage				viewType)			__Th___;

			ScriptImage*		CreateView3 (EImage				viewType,
											 const MipmapLevel&	baseMipmap,
											 uint				mipmapCount)		__Th___;

			ScriptImage*		CreateView4 (EImage				viewType,
											 const ImageLayer&	baseLayer,
											 uint				layerCount)			__Th___;

			ScriptImage*		CreateView5 ()										__Th___	{ return CreateView2( Default ); }


		static void  Bind (const ScriptEnginePtr &se)								__Th___;

		ND_ RC<Image>  ToResource ()												__Th___;


	private:
		ScriptImage (int) {}

		void  _Load (const String &filename, MipmapLevel mipmap,
					 ImageLayer layer, ELoadOpFlags flags)							__Th___;

		friend class ScriptVideoImage;
		static void  _ValidateResourceUsage (EResourceUsage usage)					__Th___;

		ND_ auto  _GetImageType ()													C_Th___;

		ND_ bool  _IsFloatFormat ()													C_Th___;	// float / half / unorm / snorm
		ND_ bool  _IsIntFormat ()													C_Th___;
		ND_ bool  _IsUIntFormat ()													C_Th___;

		ND_ bool  _Is1D ()															C_Th___;
		ND_ bool  _Is2D ()															C_Th___;
		ND_ bool  _Is3D ()															C_Th___;
		ND_ bool  _IsCube ()														C_Th___;
		ND_ bool  _Is1DArray ()														C_Th___;
		ND_ bool  _Is2DArray ()														C_Th___;
		ND_ bool  _IsCubeArray ()													C_Th___;
	};


} // AE::ResEditor
