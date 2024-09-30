// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"
#include "res_editor/Scripting/ScriptController.h"
#include "res_editor/Scripting/ScriptPassArgs.h"
#include "res_editor/Passes/IPass.h"

namespace AE::ResEditor
{

	//
	// Base Pass
	//

	class ScriptBasePass : public EnableScriptRC
	{
	// types
	public:
		struct CppStructsFromShaders
		{
			HashSet< String >	uniqueTypes;
			String				cpp;
		};

		enum class ESlider : ubyte
		{
			Int,
			Float,
			Color,
			_Count
		};

		enum class EFlags : ubyte
		{
			Unknown					= 0,

			Enable_ShaderTrace		= 1 << 0,
			Enable_ShaderFnProf		= 1 << 1,
			Enable_ShaderTmProf		= 1 << 2,

			_Last,
			All						= ((_Last - 1) << 1) - 1,

			Enable_AllShaderDbg		= Enable_ShaderTrace | Enable_ShaderFnProf | Enable_ShaderTmProf,
		};

		struct Slider
		{
			String				name;
			uint				index		= UMax;
			ESlider				type		= ESlider::_Count;
			ubyte				count		= 0;
			union {
				packed_int4		intRange	[3]	= {};	// min, max, initial
				packed_float4	floatRange	[3];		// min, max, initial
			};

			Slider ();
			Slider (const Slider &);
			Slider (Slider &&);
		};


	protected:
		using Sliders_t				= Array< Slider >;
		using UniqueSliderNames_t	= FlatHashSet< String >;
		using SliderCounter_t		= StaticArray< uint, uint(ESlider::_Count) >;

		using Controllers_t			= Array< RC<IController> >;
		using ControllersView_t		= ArrayView< RC<IController> >;

		using ECompare				= IPass::ECompare;

		struct Constant
		{
			String		name;
			uint		index		= UMax;
			ESlider		type		= ESlider::_Count;
			ubyte		count		= 0;
			RC<>		rc;			// Dynamic + Int|UInt|Float|Dim + 1|2|3|4
		};
		using Constants_t	= Array< Constant >;


	// variables
	protected:
		EFlags					_baseFlags	= Default;

		String					_defines;

		String					_dbgName;
		RGBA8u					_dbgColor	= HtmlColor::Red;

		ScriptDynamicDimPtr		_dynamicDim;
		ScriptDynamicUIntPtr	_repeatCount;

		ScriptBaseControllerPtr	_controller;
		ScriptPassArgs			_args;

		struct {
			ScriptDynamicUIntPtr	dynamic;
			uint					ref			= 0;
			ECompare				op			= Default;
		}						_enablePass;

	private:
		Sliders_t				_sliders;
		SliderCounter_t			_sliderCounter		{};

		Constants_t				_constants;
		SliderCounter_t			_constantCounter	{};

		UniqueSliderNames_t		_uniqueSliderNames;


	// interface
	public:
		// Returns non-null pass or throw exception.
		ND_ virtual RC<IPass>	ToPass ()																					C_Th___ = 0;


	// methods
	public:
		void  SetDebugLabel1 (const String &name)																			__Th___;
		void  SetDebugLabel2 (const String &name, const RGBA8u &color)														__Th___;

		void  AddFlag (EFlags value)																						__Th___;


		void  SliderI0 (const String &name)																					__Th___;
		void  SliderI1 (const String &name, int min, int max)																__Th___;
		void  SliderI2 (const String &name, const packed_int2 &min, const packed_int2 &max)									__Th___;
		void  SliderI3 (const String &name, const packed_int3 &min, const packed_int3 &max)									__Th___;
		void  SliderI4 (const String &name, const packed_int4 &min, const packed_int4 &max)									__Th___;

		void  SliderI1a (const String &name, int min, int max, int val)														__Th___;
		void  SliderI2a (const String &name, const packed_int2 &min, const packed_int2 &max, const packed_int2 &val)		__Th___;
		void  SliderI3a (const String &name, const packed_int3 &min, const packed_int3 &max, const packed_int3 &val)		__Th___;
		void  SliderI4a (const String &name, const packed_int4 &min, const packed_int4 &max, const packed_int4 &val)		__Th___;

		void  SliderF0 (const String &name)																					__Th___;
		void  SliderF1 (const String &name, float min, float max)															__Th___;
		void  SliderF2 (const String &name, const packed_float2 &min, const packed_float2 &max)								__Th___;
		void  SliderF3 (const String &name, const packed_float3 &min, const packed_float3 &max)								__Th___;
		void  SliderF4 (const String &name, const packed_float4 &min, const packed_float4 &max)								__Th___;

		void  SliderF1a (const String &name, float min, float max, float val)												__Th___;
		void  SliderF2a (const String &name, const packed_float2 &min, const packed_float2 &max, const packed_float2 &val)	__Th___;
		void  SliderF3a (const String &name, const packed_float3 &min, const packed_float3 &max, const packed_float3 &val)	__Th___;
		void  SliderF4a (const String &name, const packed_float4 &min, const packed_float4 &max, const packed_float4 &val)	__Th___;

		void  ColorSelector1 (const String &name)																			__Th___;
		void  ColorSelector2 (const String &name, const RGBA32f &val)														__Th___;
		void  ColorSelector3 (const String &name, const RGBA8u &val)														__Th___;


		void  ConstantF1 (const String &name, const ScriptDynamicFloatPtr  &value)						__Th___;
		void  ConstantF2 (const String &name, const ScriptDynamicFloat2Ptr &value)						__Th___;
		void  ConstantF3 (const String &name, const ScriptDynamicFloat3Ptr &value)						__Th___;
		void  ConstantF4 (const String &name, const ScriptDynamicFloat4Ptr &value)						__Th___;

		void  ConstantI1 (const String &name, const ScriptDynamicIntPtr  &value)						__Th___;
		void  ConstantI2 (const String &name, const ScriptDynamicInt2Ptr &value)						__Th___;
		void  ConstantI3 (const String &name, const ScriptDynamicInt3Ptr &value)						__Th___;
		void  ConstantI4 (const String &name, const ScriptDynamicInt4Ptr &value)						__Th___;

		void  ConstantU1 (const String &name, const ScriptDynamicUIntPtr  &value)						__Th___;
		void  ConstantU2 (const String &name, const ScriptDynamicUInt2Ptr &value)						__Th___;
		void  ConstantU3 (const String &name, const ScriptDynamicUInt3Ptr &value)						__Th___;
		void  ConstantU4 (const String &name, const ScriptDynamicUInt4Ptr &value)						__Th___;

		void  ConstantCF1 (const String &name, float value)												__Th___;
		void  ConstantCF2 (const String &name, const packed_float2 &value)								__Th___;
		void  ConstantCF3 (const String &name, const packed_float3 &value)								__Th___;
		void  ConstantCF4 (const String &name, const packed_float4 &value)								__Th___;

		void  ConstantCI1 (const String &name, int value)												__Th___;
		void  ConstantCI2 (const String &name, const packed_int2 &value)								__Th___;
		void  ConstantCI3 (const String &name, const packed_int3 &value)								__Th___;
		void  ConstantCI4 (const String &name, const packed_int4 &value)								__Th___;

		void  ConstantCU1 (const String &name, uint value)												__Th___;
		void  ConstantCU2 (const String &name, const packed_uint2 &value)								__Th___;
		void  ConstantCU3 (const String &name, const packed_uint3 &value)								__Th___;
		void  ConstantCU4 (const String &name, const packed_uint4 &value)								__Th___;

		void  ConstantDD (const String &name, const ScriptDynamicDimPtr &value)							__Th___;


		void  SetRepeatCount (const ScriptDynamicUIntPtr &)												__Th___;

		void  EnableIfEqual (const ScriptDynamicUIntPtr &dyn, uint ref)									__Th___;
		void  EnableIfLess (const ScriptDynamicUIntPtr &dyn, uint ref)									__Th___;
		void  EnableIfGreater (const ScriptDynamicUIntPtr &dyn, uint ref)								__Th___;
		void  EnableIfAnyBit (const ScriptDynamicUIntPtr &dyn, uint ref)								__Th___;

		void  ArgSceneIn (const String &name, const ScriptRTScenePtr &scene)							__Th___	{ _args.ArgSceneIn( name, scene ); }

		void  ArgBufferIn (const String &name, const ScriptBufferPtr &buf)								__Th___	{ _args.ArgBufferIn( name, buf ); }
		void  ArgBufferOut (const String &name, const ScriptBufferPtr &buf)								__Th___	{ _args.ArgBufferOut( name, buf ); }
		void  ArgBufferInOut (const String &name, const ScriptBufferPtr &buf)							__Th___	{ _args.ArgBufferInOut( name, buf ); }

		void  ArgImageIn (const String &name, const ScriptImagePtr &img)								__Th___	{ _args.ArgImageIn( name, img ); }
		void  ArgImageOut (const String &name, const ScriptImagePtr &img)								__Th___	{ _args.ArgImageOut( name, img ); }
		void  ArgImageInOut (const String &name, const ScriptImagePtr &img)								__Th___	{ _args.ArgImageInOut( name, img ); }

		void  ArgImageArrIn (const String &name, const ScriptArray<ScriptImagePtr> &arr)				__Th___	{ _args.ArgImageArrIn( name, Array<ScriptImagePtr>{arr} ); }
		void  ArgImageArrOut (const String &name, const ScriptArray<ScriptImagePtr> &arr)				__Th___	{ _args.ArgImageArrOut( name, Array<ScriptImagePtr>{arr} ); }
		void  ArgImageArrInOut (const String &name, const ScriptArray<ScriptImagePtr> &arr)				__Th___	{ _args.ArgImageArrInOut( name, Array<ScriptImagePtr>{arr} ); }

		void  ArgTextureIn (const String &name, const ScriptImagePtr &tex)								__Th___	{ _args.ArgTextureIn( name, tex ); }
		void  ArgTextureIn2 (const String &name, const ScriptImagePtr &tex, const String &samplerName)	__Th___	{ _args.ArgTextureIn2( name, tex, samplerName ); }
		void  ArgTextureArrIn (const String &name, const ScriptArray<ScriptImagePtr> &arr)				__Th___	{ _args.ArgTextureArrIn( name, Array<ScriptImagePtr>{arr} ); }
		void  ArgTextureArrIn2 (const String &name, const ScriptArray<ScriptImagePtr> &arr, const String &s)__Th___	{ _args.ArgTextureArrIn2( name, Array<ScriptImagePtr>{arr}, s ); }

		void  ArgVideoIn (const String &name, const ScriptVideoImagePtr &tex, const String &samplerName)__Th___	{ _args.ArgVideoIn( name, tex, samplerName ); }
		void  ArgController (const ScriptBaseControllerPtr &)											__Th___;

		static void  Bind (const ScriptEnginePtr &se)													__Th___;


	private:
		template <typename T>
		void  _Slider (const String &name, const T &min, const T &max, T val, ESlider type)				__Th___;

		template <typename T>
		void  _Constant (const String &name, const T &dynVal, ESlider type, ubyte count)				__Th___;

		template <typename T>
		void  _Constant2 (const String &name, const T &dynVal, ESlider type, ubyte count)				__Th___;

		void  _AddSlidersToUIInteraction (IPass* pass)													const;
		void  _CopyConstants (OUT IPass::Constants &)													const;


	protected:
		ScriptBasePass ()																				__Th___;

		void  _Init (IPass &dst, const ScriptBaseControllerPtr &defaultController)						C_Th___;

		ND_ ScriptDynamicDim*	_Dimension ()															__Th___;
		void  _SetDynamicDimension (const ScriptDynamicDimPtr &)										__Th___;
		void  _SetConstDimension (const uint3 &dim)														__Th___;

		ND_ bool  _HasCustomDynamicDimension ()															C_Th___;

		template <typename B>
		static void  _BindBase (B &binder, Bool withArgs)												__Th___;

		virtual void  _OnAddArg (INOUT ScriptPassArgs::Argument &arg)									C_Th___ = 0;

		static void  _AddDefines (StringView defines, INOUT String &header)								__Th___;

		void  _AddSliders (INOUT String &header)														C_Th___;
		void  _AddSlidersAsMacros (OUT String &macros)													C_Th___;

		ND_ Strong<BufferID>  _CreateUBuffer (Bytes size, StringView dbgName, EResourceState defaultState)	C_Th___;
	};

	AE_BIT_OPERATORS( ScriptBasePass::EFlags );


} // AE::ResEditor
