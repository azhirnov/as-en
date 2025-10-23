// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Scripting/ScriptCommon.h"

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

				binder.Comment( "ShaderFunctionProfiling - record time of user function calls, then sort results and save to file.\n" );
				binder.AddValue( "Enable_ShaderFnProf",	EFlags::Enable_ShaderFnProf );

				//binder.AddValue( "Enable_ShaderTmProf",	EFlags::Enable_ShaderTmProf );	// not supported yet

				binder.Comment( "Enable all debug features." );
				binder.AddValue( "Enable_AllShaderDbg",	EFlags::Enable_AllShaderDbg );
				
				binder.Comment( "Compile shader using Slang compiler." );
				binder.AddValue( "UseSLang",	EFlags::UseSLang );

				StaticAssert( uint(EFlags::All) == 0xF );
			}
		}

		using T = typename B::Class_t;
		AS_IMPL_CAST_T( classBinder, ScriptBasePass_ToBase<T> );

		classBinder.Comment( "Set debug label and color. It is used in graphics profiler." );
		AS_METHOD_T( classBinder, ScriptBasePass::SetDebugLabel1,	"SetDebugLabel",	{"label"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SetDebugLabel2,	"SetDebugLabel",	{"label", "color"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SetDebugLabel3,	"SetDebugLabel",	{"label", "color"} );

		AS_METHOD_T( classBinder, ScriptBasePass::AddFlag,			"AddFlag",			{} );
		
		classBinder.Comment( "Measure GPU time for this pass. If pass executed multiple times during 'Repeat()' then average time will be returned." );
		AS_METHOD_T( classBinder, ScriptBasePass::MeasureTime,		"MeasureTime",		{"timeInSeconds"} );

		classBinder.Comment( "Add slider to UI. Data passed to all shaders in the current pass." );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderI0,			"SliderI",			{"name"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderI1,			"Slider",			{"name", "min", "max"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderI2,			"Slider",			{"name", "min", "max"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderI3,			"Slider",			{"name", "min", "max"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderI4,			"Slider",			{"name", "min", "max"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderI1a,		"Slider",			{"name", "min", "max", "initial"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderI2a,		"Slider",			{"name", "min", "max", "initial"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderI3a,		"Slider",			{"name", "min", "max", "initial"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderI4a,		"Slider",			{"name", "min", "max", "initial"} );

		AS_METHOD_T( classBinder, ScriptBasePass::SliderF0,			"SliderF",			{"name"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderF1,			"Slider",			{"name", "min", "max"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderF2,			"Slider",			{"name", "min", "max"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderF3,			"Slider",			{"name", "min", "max"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderF4,			"Slider",			{"name", "min", "max"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderF1a,		"Slider",			{"name", "min", "max", "initial"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderF2a,		"Slider",			{"name", "min", "max", "initial"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderF3a,		"Slider",			{"name", "min", "max", "initial"} );
		AS_METHOD_T( classBinder, ScriptBasePass::SliderF4a,		"Slider",			{"name", "min", "max", "initial"} );

		AS_METHOD_T( classBinder, ScriptBasePass::ColorSelector1,	"ColorSelector",	{"name"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ColorSelector2,	"ColorSelector",	{"name", "initial"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ColorSelector3,	"ColorSelector",	{"name", "initial"} );

		classBinder.Comment( "Value of 'dynamicValue' will be passed to all shaders in the current pass.\n"
							 "Value of 'dynamicValue' is constant for whole frame and also can be used in draw call or another pass." );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantF1,		"Constant",			{"name", "dynamicValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantF2,		"Constant",			{"name", "dynamicValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantF3,		"Constant",			{"name", "dynamicValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantF4,		"Constant",			{"name", "dynamicValue"} );

		AS_METHOD_T( classBinder, ScriptBasePass::ConstantI1,		"Constant",			{"name", "dynamicValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantI2,		"Constant",			{"name", "dynamicValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantI3,		"Constant",			{"name", "dynamicValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantI4,		"Constant",			{"name", "dynamicValue"} );

		AS_METHOD_T( classBinder, ScriptBasePass::ConstantU1,		"Constant",			{"name", "dynamicValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantU2,		"Constant",			{"name", "dynamicValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantU3,		"Constant",			{"name", "dynamicValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantU4,		"Constant",			{"name", "dynamicValue"} );

		AS_METHOD_T( classBinder, ScriptBasePass::ConstantDD,		"Constant",			{"name", "dynamicValue"} );

		AS_METHOD_T( classBinder, ScriptBasePass::ConstantCF1,		"Constant",			{"name", "constValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantCF2,		"Constant",			{"name", "constValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantCF3,		"Constant",			{"name", "constValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantCF4,		"Constant",			{"name", "constValue"} );

		AS_METHOD_T( classBinder, ScriptBasePass::ConstantCI1,		"Constant",			{"name", "constValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantCI2,		"Constant",			{"name", "constValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantCI3,		"Constant",			{"name", "constValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantCI4,		"Constant",			{"name", "constValue"} );

		AS_METHOD_T( classBinder, ScriptBasePass::ConstantCU1,		"Constant",			{"name", "constValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantCU2,		"Constant",			{"name", "constValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantCU3,		"Constant",			{"name", "constValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::ConstantCU4,		"Constant",			{"name", "constValue"} );

		classBinder.Comment( "Returns dynamic dimension of the pass.\n"
							 "It is auto-detected when used render targets with dynamic dimension or dynamic size for compute dispatches." );
		AS_METHOD_T( classBinder, ScriptBasePass::_Dimension,		"Dimension",		{} );

		AS_METHOD_T( classBinder, ScriptBasePass::EnableIfEqual,	"EnableIfEqual",	{"dynamic", "refValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::EnableIfLess,		"EnableIfLess",		{"dynamic", "refValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::EnableIfGreater,	"EnableIfGreater",	{"dynamic", "refValue"} );
		AS_METHOD_T( classBinder, ScriptBasePass::EnableIfAnyBit,	"EnableIfAnyBit",	{"dynamic", "refValue"} );

		classBinder.Comment( "Repeat pass multiple times.\nCan be used for performance tests." );
		AS_METHOD_T( classBinder, ScriptBasePass::SetRepeatCount,	"Repeat",			{} );

		if ( withArgs )
		{
			classBinder.Comment( "Add resource to all shaders in the current pass.\n"
								 "In  - resource is used for read access.\n"
								 "Out - resource is used for write access.\n" );
			AS_METHOD_T( classBinder, ScriptBasePass::ArgSceneIn,		"ArgIn",			{"uniformName", "resource"} );

			AS_METHOD_T( classBinder, ScriptBasePass::ArgBufferIn,		"ArgIn",			{"uniformName", "resource"} );
			AS_METHOD_T( classBinder, ScriptBasePass::ArgBufferOut,		"ArgOut",			{"uniformName", "resource"} );
			AS_METHOD_T( classBinder, ScriptBasePass::ArgBufferInOut,	"ArgInOut",			{"uniformName", "resource"} );

			AS_METHOD_T( classBinder, ScriptBasePass::ArgImageIn,		"ArgIn",			{"uniformName", "resource"} );
			AS_METHOD_T( classBinder, ScriptBasePass::ArgImageOut,		"ArgOut",			{"uniformName", "resource"} );
			AS_METHOD_T( classBinder, ScriptBasePass::ArgImageInOut,	"ArgInOut",			{"uniformName", "resource"} );

			AS_METHOD_T( classBinder, ScriptBasePass::ArgTextureIn,		"ArgTex",			{"uniformName", "resource"} );
			AS_METHOD_T( classBinder, ScriptBasePass::ArgTextureIn2,	"ArgIn",			{"uniformName", "resource", "samplerName"} );
			AS_METHOD_T( classBinder, ScriptBasePass::ArgTextureArrIn,	"ArgTex",			{"uniformName", "resources"} );
			AS_METHOD_T( classBinder, ScriptBasePass::ArgTextureArrIn2,	"ArgIn",			{"uniformName", "resources", "samplerName"} );

			AS_METHOD_T( classBinder, ScriptBasePass::ArgVideoIn,		"ArgIn",			{"uniformName", "resource", "samplerName"} );
			AS_METHOD_T( classBinder, ScriptBasePass::ArgController,	"Set",				{"camera"} );

			AS_METHOD_T( classBinder, ScriptBasePass::ArgImageArrIn,	"ArgIn",			{"uniformName", "resources"} );
			AS_METHOD_T( classBinder, ScriptBasePass::ArgImageArrOut,	"ArgOut",			{"uniformName", "resources"} );
			AS_METHOD_T( classBinder, ScriptBasePass::ArgImageArrInOut,	"ArgInOut",			{"uniformName", "resources"} );
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
		}

		// with load/store
		{
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &, EAttachmentLoadOp, EAttachmentStoreOp)					>( &ScriptBaseRenderPass::_Output, "OutputLS", {"image", "loadOp", "storeOp"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, EAttachmentLoadOp, EAttachmentStoreOp) >( &ScriptBaseRenderPass::_Output, "OutputLS", {"name", "image", "loadOp", "storeOp"} );
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
			classBinder.template AddGenericMethod< void (const String &, const String&, const ScriptImagePtr &)						>( &ScriptBaseRenderPass::_InOut, "InOut", {"inName", "outName", "image"} );
			classBinder.template AddGenericMethod< void (const String &, const String&, const ScriptImagePtr &, const RGBA32f &)	>( &ScriptBaseRenderPass::_InOut, "InOut", {"inName", "outName", "image", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const String&, const ScriptImagePtr &, const RGBA32i &)	>( &ScriptBaseRenderPass::_InOut, "InOut", {"inName", "outName", "image", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const String&, const ScriptImagePtr &, const RGBA32u &)	>( &ScriptBaseRenderPass::_InOut, "InOut", {"inName", "outName", "image", "clearColor"} );
			classBinder.template AddGenericMethod< void (const String &, const String&, const ScriptImagePtr &, const DepthStencil &)>( &ScriptBaseRenderPass::_InOut, "InOut", {"inName", "outName", "image", "clearDS"} );
			classBinder.template AddGenericMethod< void (const String &, const ScriptImagePtr &, const DepthStencil &)				>( &ScriptBaseRenderPass::_InOut, "InOut", {"inName", "image", "clearDS"} );
			
			classBinder.Comment( "Used instead of 'ArgIn' to define image as input attachment. Supports color and depth formats." );
			AS_METHOD_T( classBinder, ScriptBaseRenderPass::_Input, "Input", {"inName", "image", "attachmentName"} );
		}

		// shading rate
		{
			classBinder.Comment( "Add fragment shading rate attachment." );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &)	>( &ScriptBaseRenderPass::_FragmentShadingRate,	"FragmentShadingRate", {"image"} );
			
			classBinder.Comment( "Add fragment density map attachment." );
			classBinder.template AddGenericMethod< void (const ScriptImagePtr &)	>( &ScriptBaseRenderPass::_FragmentDensityMap,	"FragmentDensityMap", {"image"} );
		}

		// depth
		AS_METHOD_T( classBinder, ScriptBaseRenderPass::_SetDepthRange, "DepthRange", {"min", "max"} );

		// viewports
		{
			classBinder.Comment( "Add viewport. 'rect' defined in unorm coords." );
			AS_METHOD_T( classBinder, ScriptBaseRenderPass::_AddViewport0, "AddViewport", {"rect", "minDepth", "maxDepth", "scissor", "wScale"} );
			AS_METHOD_T( classBinder, ScriptBaseRenderPass::_AddViewport1, "AddViewport", {"rect", "minDepth", "maxDepth"} );
			AS_METHOD_T( classBinder, ScriptBaseRenderPass::_AddViewport2, "AddViewport", {"rect"} );
			AS_METHOD_T( classBinder, ScriptBaseRenderPass::_AddViewport3, "AddViewport", {"left", "top", "right", "bottom"} );
			AS_METHOD_T( classBinder, ScriptBaseRenderPass::_AddViewport4, "AddViewport", {"rect", "minDepth", "maxDepth", "scissor"} );
		}
	}


} // AE::ResEditor
