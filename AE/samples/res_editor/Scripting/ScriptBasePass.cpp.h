// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"

namespace AE::ResEditor
{
namespace
{
/*
=================================================
	ScriptBasePass_ToBase
=================================================
*/
	template <typename T>
	static ScriptBasePass*  ScriptBasePass_ToBase (T* ptr)
	{
		StaticAssert( IsBaseOf< ScriptBasePass, T >);

		ScriptBasePassPtr  result{ ptr };
		return result.Detach();
	}

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	_BindBase
=================================================
*/
	template <typename B>
	void  ScriptBasePass::_BindBase (B &classBinder, Bool withArgs) __Th___
	{
		// enums
		{
			Scripting::EnumBinder<EFlags>	binder{ classBinder.GetEngine() };
			if ( not binder.IsRegistered() )
			{
				binder.Create();
				binder.AddValue( "None",				EFlags::Unknown );

				binder.Comment( "ShaderTrace - record all variables, function result, etc and save it to file.\n"
								"It is very useful to debug shaders. In UI select 'Debugging' menu, select pass,"
								"'Trace' and shader stage then click 'G' key to record trace for pixel under cursor.\n"
								"Reference to the last recorded trace will be added to console and IDE log, click on it to open file." );
				binder.AddValue( "Enable_ShaderTrace",	EFlags::Enable_ShaderTrace );

				binder.Comment( "ShaderFunctionProfiling - record time of user function calls, sort it and save to file.\n" );
				binder.AddValue( "Enable_ShaderFnProf",	EFlags::Enable_ShaderFnProf );

				//binder.AddValue( "Enable_ShaderTmProf",	EFlags::Enable_ShaderTmProf );	// not supported yet

				binder.Comment( "Enable all debug features." );
				binder.AddValue( "Enable_AllShaderDbg",	EFlags::Enable_AllShaderDbg );
				StaticAssert( uint(EFlags::All) == 7 );
			}
		}

		using T = typename B::Class_t;
		classBinder.Operators().ImplCast( &ScriptBasePass_ToBase<T> );

		classBinder.Comment( "Set debug label and color. It is used in graphics profiler." );
		classBinder.AddMethod( &ScriptBasePass::SetDebugLabel1,		"SetDebugLabel",	{"label"} );
		classBinder.AddMethod( &ScriptBasePass::SetDebugLabel2,		"SetDebugLabel",	{"label", "color"} );

		classBinder.AddMethod( &ScriptBasePass::AddFlag,			"AddFlag",			{} );

		classBinder.Comment( "Add slider to UI. Data passed to all shaders in the current pass." );
		classBinder.AddMethod( &ScriptBasePass::SliderI0,			"SliderI",			{"name"} );
		classBinder.AddMethod( &ScriptBasePass::SliderI1,			"Slider",			{"name", "min", "max"} );
		classBinder.AddMethod( &ScriptBasePass::SliderI2,			"Slider",			{"name", "min", "max"} );
		classBinder.AddMethod( &ScriptBasePass::SliderI3,			"Slider",			{"name", "min", "max"} );
		classBinder.AddMethod( &ScriptBasePass::SliderI4,			"Slider",			{"name", "min", "max"} );
		classBinder.AddMethod( &ScriptBasePass::SliderI1a,			"Slider",			{"name", "min", "max", "initial"} );
		classBinder.AddMethod( &ScriptBasePass::SliderI2a,			"Slider",			{"name", "min", "max", "initial"} );
		classBinder.AddMethod( &ScriptBasePass::SliderI3a,			"Slider",			{"name", "min", "max", "initial"} );
		classBinder.AddMethod( &ScriptBasePass::SliderI4a,			"Slider",			{"name", "min", "max", "initial"} );

		classBinder.AddMethod( &ScriptBasePass::SliderF0,			"SliderF",			{"name"} );
		classBinder.AddMethod( &ScriptBasePass::SliderF1,			"Slider",			{"name", "min", "max"} );
		classBinder.AddMethod( &ScriptBasePass::SliderF2,			"Slider",			{"name", "min", "max"} );
		classBinder.AddMethod( &ScriptBasePass::SliderF3,			"Slider",			{"name", "min", "max"} );
		classBinder.AddMethod( &ScriptBasePass::SliderF4,			"Slider",			{"name", "min", "max"} );
		classBinder.AddMethod( &ScriptBasePass::SliderF1a,			"Slider",			{"name", "min", "max", "initial"} );
		classBinder.AddMethod( &ScriptBasePass::SliderF2a,			"Slider",			{"name", "min", "max", "initial"} );
		classBinder.AddMethod( &ScriptBasePass::SliderF3a,			"Slider",			{"name", "min", "max", "initial"} );
		classBinder.AddMethod( &ScriptBasePass::SliderF4a,			"Slider",			{"name", "min", "max", "initial"} );

		classBinder.AddMethod( &ScriptBasePass::ColorSelector1,		"ColorSelector",	{"name"} );
		classBinder.AddMethod( &ScriptBasePass::ColorSelector2,		"ColorSelector",	{"name", "initial"} );
		classBinder.AddMethod( &ScriptBasePass::ColorSelector3,		"ColorSelector",	{"name", "initial"} );

		classBinder.Comment( "Value of 'dynamicValue' will be passed to all shaders in the current pass.\n"
							 "Value of 'dynamicValue' is constant for whole frame and also can be used in draw call or another pass." );
		classBinder.AddMethod( &ScriptBasePass::ConstantF1,			"Constant",			{"name", "dynamicValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantF2,			"Constant",			{"name", "dynamicValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantF3,			"Constant",			{"name", "dynamicValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantF4,			"Constant",			{"name", "dynamicValue"} );

		classBinder.AddMethod( &ScriptBasePass::ConstantI1,			"Constant",			{"name", "dynamicValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantI2,			"Constant",			{"name", "dynamicValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantI3,			"Constant",			{"name", "dynamicValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantI4,			"Constant",			{"name", "dynamicValue"} );

		classBinder.AddMethod( &ScriptBasePass::ConstantU1,			"Constant",			{"name", "dynamicValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantU2,			"Constant",			{"name", "dynamicValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantU3,			"Constant",			{"name", "dynamicValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantU4,			"Constant",			{"name", "dynamicValue"} );

		classBinder.AddMethod( &ScriptBasePass::ConstantDD,			"Constant",			{"name", "dynamicValue"} );

		classBinder.AddMethod( &ScriptBasePass::ConstantCF1,		"Constant",			{"name", "constValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantCF2,		"Constant",			{"name", "constValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantCF3,		"Constant",			{"name", "constValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantCF4,		"Constant",			{"name", "constValue"} );

		classBinder.AddMethod( &ScriptBasePass::ConstantCI1,		"Constant",			{"name", "constValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantCI2,		"Constant",			{"name", "constValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantCI3,		"Constant",			{"name", "constValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantCI4,		"Constant",			{"name", "constValue"} );

		classBinder.AddMethod( &ScriptBasePass::ConstantCU1,		"Constant",			{"name", "constValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantCU2,		"Constant",			{"name", "constValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantCU3,		"Constant",			{"name", "constValue"} );
		classBinder.AddMethod( &ScriptBasePass::ConstantCU4,		"Constant",			{"name", "constValue"} );

		classBinder.Comment( "Returns dynamic dimension of the pass.\n"
							 "It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches." );
		classBinder.AddMethod( &ScriptBasePass::_Dimension,			"Dimension",		{} );

		classBinder.AddMethod( &ScriptBasePass::EnableIfEqual,		"EnableIfEqual",	{"dynamic", "refValue"} );
		classBinder.AddMethod( &ScriptBasePass::EnableIfLess,		"EnableIfLess",		{"dynamic", "refValue"} );
		classBinder.AddMethod( &ScriptBasePass::EnableIfGreater,	"EnableIfGreater",	{"dynamic", "refValue"} );
		classBinder.AddMethod( &ScriptBasePass::EnableIfAnyBit,		"EnableIfAnyBit",	{"dynamic", "refValue"} );

		classBinder.Comment( "Repeat pass multiple times.\nCan be used for performance tests." );
		classBinder.AddMethod( &ScriptBasePass::SetRepeatCount,		"Repeat",			{} );

		if ( withArgs )
		{
			classBinder.Comment( "Add resource to all shaders in the current pass.\n"
								 "In  - resource is used for read access.\n"
								 "Out - resource is used for write access.\n" );
			classBinder.AddMethod( &ScriptBasePass::ArgSceneIn,			"ArgIn",			{"uniformName", "resource"} );

			classBinder.AddMethod( &ScriptBasePass::ArgBufferIn,		"ArgIn",			{"uniformName", "resource"} );
			classBinder.AddMethod( &ScriptBasePass::ArgBufferOut,		"ArgOut",			{"uniformName", "resource"} );
			classBinder.AddMethod( &ScriptBasePass::ArgBufferInOut,		"ArgInOut",			{"uniformName", "resource"} );

			classBinder.AddMethod( &ScriptBasePass::ArgImageIn,			"ArgIn",			{"uniformName", "resource"} );
			classBinder.AddMethod( &ScriptBasePass::ArgImageOut,		"ArgOut",			{"uniformName", "resource"} );
			classBinder.AddMethod( &ScriptBasePass::ArgImageInOut,		"ArgInOut",			{"uniformName", "resource"} );

			classBinder.AddMethod( &ScriptBasePass::ArgTextureIn,		"ArgTex",			{"uniformName", "resource"} );
			classBinder.AddMethod( &ScriptBasePass::ArgTextureIn2,		"ArgIn",			{"uniformName", "resource", "samplerName"} );
			classBinder.AddMethod( &ScriptBasePass::ArgTextureArrIn,	"ArgTex",			{"uniformName", "resources"} );
			classBinder.AddMethod( &ScriptBasePass::ArgTextureArrIn2,	"ArgIn",			{"uniformName", "resources", "samplerName"} );

			classBinder.AddMethod( &ScriptBasePass::ArgVideoIn,			"ArgIn",			{"uniformName", "resource", "samplerName"} );
			classBinder.AddMethod( &ScriptBasePass::ArgController,		"Set",				{"camera"} );

			classBinder.AddMethod( &ScriptBasePass::ArgImageArrIn,		"ArgIn",			{"uniformName", "resources"} );
			classBinder.AddMethod( &ScriptBasePass::ArgImageArrOut,		"ArgOut",			{"uniformName", "resources"} );
			classBinder.AddMethod( &ScriptBasePass::ArgImageArrInOut,	"ArgInOut",			{"uniformName", "resources"} );
		}
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_BindBaseRenderPass
=================================================
*/
	template <typename B>
	void  ScriptBaseRenderPass::_BindBaseRenderPass (B &classBinder, Bool withBlending, Bool withRWAtt) __Th___
	{
		using C = typename B::Class_t;

		// without name
		{
			classBinder.Comment( "Add color/depth render target.\n"
								 "Implicitly name will be 'out_Color' + index." );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &)												>( &ScriptBaseRenderPass::_Output, "Output", {"image"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &)							>( &ScriptBaseRenderPass::_Output, "Output", {"image", "mipmap"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &)							>( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &)		>( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "mipmap"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint)						>( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "layerCount"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &)	>( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "layerCount", "mipmap"} );

			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const RGBA32f &)												 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, const RGBA32f &)							 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "mipmap", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const RGBA32f &)							 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32f &)		 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "mipmap", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint, const RGBA32f &)						 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "layerCount", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "layerCount", "mipmap", "clearColor"} );

			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const RGBA32u &)												 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, const RGBA32u &)							 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "mipmap", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const RGBA32u &)							 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32u &)		 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "mipmap", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint, const RGBA32u &)						 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "layerCount", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "layerCount", "mipmap", "clearColor"} );

			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const RGBA32i &)												 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, const RGBA32i &)							 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "mipmap", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const RGBA32i &)							 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32i &)		 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "mipmap", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint, const RGBA32i &)						 >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "layerCount", "clearColor"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "layerCount", "mipmap", "clearColor"} );

			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const DepthStencil &)												  >( &ScriptBaseRenderPass::_Output, "Output", {"image", "clearDepthStencil"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, const DepthStencil &)							  >( &ScriptBaseRenderPass::_Output, "Output", {"image", "mipmap", "clearDepthStencil"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const DepthStencil &)							  >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "clearDepthStencil"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const DepthStencil &)		  >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "mipmap", "clearDepthStencil"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint, const DepthStencil &)					  >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "layerCount", "clearDepthStencil"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output", {"image", "baseLayer", "layerCount", "mipmap", "clearDepthStencil"} );
		}

		// with name
		{
			classBinder.Comment( "Add color/depth render target with explicit name." );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &)												>( &ScriptBaseRenderPass::_Output, "Output", {"name", "image"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &)							>( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "mipmap"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &)							>( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &)		>( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "mipmap"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint)						>( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "layerCount"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &)	>( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "layerCount", "mipmap"} );

			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const RGBA32f &)												 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, const RGBA32f &)							 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "mipmap", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const RGBA32f &)							 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32f &)		 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "mipmap", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint, const RGBA32f &)						 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "layerCount", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32f &) >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "layerCount", "mipmap", "clearColor"} );

			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const RGBA32u &)												 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, const RGBA32u &)							 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "mipmap", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const RGBA32u &)							 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32u &)		 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "mipmap", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint, const RGBA32u &)						 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "layerCount", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32u &) >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "layerCount", "mipmap", "clearColor"} );

			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const RGBA32i &)												 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, const RGBA32i &)							 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "mipmap", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const RGBA32i &)							 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const RGBA32i &)		 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "mipmap", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint, const RGBA32i &)						 >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "layerCount", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &, const RGBA32i &) >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "layerCount", "mipmap", "clearColor"} );

			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const DepthStencil &)												  >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "clearDepthStencil"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, const DepthStencil &)							  >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "mipmap", "clearDepthStencil"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const DepthStencil &)							  >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "clearDepthStencil"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, const DepthStencil &)		  >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "mipmap", "clearDepthStencil"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint, const DepthStencil &)					  >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "layerCount", "clearDepthStencil"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &, const DepthStencil &) >( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "baseLayer", "layerCount", "mipmap", "clearDepthStencil"} );

			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, EAttachmentLoadOp, EAttachmentStoreOp)		>( &ScriptBaseRenderPass::_Output, "Output", {"name", "image", "loadOp", "storeOp"} );
		}

		// with blend
		if ( withBlending )
		{
			classBinder.Comment( "Add color render target with blend operation.\n"
								 "Implicitly name will be 'out_Color' + index." );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, EBlendFactor, EBlendFactor, EBlendOp)												  >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"image", "src", "dst", "op"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp)							  >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"image", "mipmap", "src", "dst", "op"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp)							  >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"image", "baseLayer", "src", "dst", "op"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp)		  >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"image", "baseLayer", "mipmap", "src", "dst", "op"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint, EBlendFactor, EBlendFactor, EBlendOp)					  >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"image", "baseLayer", "layerCount", "src", "dst", "op"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"image", "baseLayer", "layerCount", "mipmap", "src", "dst", "op"} );

			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp)												>( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"image", "srcRGB", "dstRGB", "opRGB", "srcA", "dstA", "opA"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp)							>( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"image", "mipmap", "srcRGB", "dstRGB", "opRGB", "srcA", "dstA", "opA"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp)							>( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"image", "baseLayer", "srcRGB", "dstRGB", "opRGB", "srcA", "dstA", "opA"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp)		>( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"image", "baseLayer", "mipmap", "srcRGB", "dstRGB", "opRGB", "srcA", "dstA", "opA"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp)						>( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"image", "baseLayer", "layerCount", "srcRGB", "dstRGB", "opRGB", "srcA", "dstA", "opA"} );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"image", "baseLayer", "layerCount", "mipmap", "srcRGB", "dstRGB", "opRGB", "srcA", "dstA", "opA"} );

			classBinder.Comment( "Add color render target with blend operation and with explicit name." );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, EBlendFactor, EBlendFactor, EBlendOp)												  >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"name", "image", "src", "dst", "op"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp)							  >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"name", "image", "mipmap", "src", "dst", "op"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp)							  >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"name", "image", "baseLayer", "src", "dst", "op"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp)		  >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"name", "image", "baseLayer", "mipmap", "src", "dst", "op"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint, EBlendFactor, EBlendFactor, EBlendOp)					  >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"name", "image", "baseLayer", "layerCount", "src", "dst", "op"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"name", "image", "baseLayer", "layerCount", "mipmap", "src", "dst", "op"} );

			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp)												>( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"name", "image", "srcRGB", "dstRGB", "opRGB", "srcA", "dstA", "opA"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp)							>( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"name", "image", "mipmap", "srcRGB", "dstRGB", "opRGB", "srcA", "dstA", "opA"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp)							>( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"name", "image", "baseLayer", "srcRGB", "dstRGB", "opRGB", "srcA", "dstA", "opA"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp)		>( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"name", "image", "baseLayer", "mipmap", "srcRGB", "dstRGB", "opRGB", "srcA", "dstA", "opA"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp)						>( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"name", "image", "baseLayer", "layerCount", "srcRGB", "dstRGB", "opRGB", "srcA", "dstA", "opA"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const ImageLayer &, uint, const MipmapLevel &, EBlendFactor, EBlendFactor, EBlendOp, EBlendFactor, EBlendFactor, EBlendOp) >( &ScriptBaseRenderPass::_OutputBlend, "OutputBlend", {"name", "image", "baseLayer", "layerCount", "mipmap", "srcRGB", "dstRGB", "opRGB", "srcA", "dstA", "opA"} );
		}

		// read/write input attachment
		if ( withRWAtt )
		{
			classBinder.Comment( "Used instead of 'Output()' to define image as input attachment & color attachment (read/write input attachment)." );
			classBinder.AddMethod( &ScriptBaseRenderPass::_InOut, "InOut", {"inName", "outName", "image"} );
		}

		// depth
		classBinder.AddMethod( &ScriptBaseRenderPass::_SetDepthRange, "DepthRange", {"min", "max"} );

		// viewports
		classBinder.AddMethod( &ScriptBaseRenderPass::_AddViewport0, "AddViewport", {"rect", "minDepth", "maxDepth", "scissor", "wScale"} );
		classBinder.AddMethod( &ScriptBaseRenderPass::_AddViewport1, "AddViewport", {"rect", "minDepth", "maxDepth"} );
		classBinder.AddMethod( &ScriptBaseRenderPass::_AddViewport2, "AddViewport", {"rect"} );
		classBinder.AddMethod( &ScriptBaseRenderPass::_AddViewport3, "AddViewport", {"left", "top", "right", "bottom"} );
		classBinder.AddMethod( &ScriptBaseRenderPass::_AddViewport4, "AddViewport", {"rect", "minDepth", "maxDepth", "scissor"} );
	}


} // AE::ResEditor
