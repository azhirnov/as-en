// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Core/EditorUI.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	ScriptBasePass::Slider::Slider ()
	{}

	ScriptBasePass::Slider::Slider (const Slider &other) :
		name{ other.name },
		index{ other.index },
		type{ other.type },
		count{ other.count }
	{
		MemCopy( OUT intRange, other.intRange, Sizeof(intRange) );
	}

	ScriptBasePass::Slider::Slider (Slider &&other) :
		name{ RVRef(other.name) },
		index{ other.index },
		type{ other.type },
		count{ other.count }
	{
		MemCopy( OUT intRange, other.intRange, Sizeof(intRange) );
	}
//-----------------------------------------------------------------------------


/*
=================================================
	constructor
=================================================
*/
	ScriptBasePass::ScriptBasePass () __Th___ :
		_args{ [this](ScriptPassArgs::Argument &arg) { _OnAddArg( arg ); }}
	{}

/*
=================================================
	AddFlag
=================================================
*/
	void  ScriptBasePass::AddFlag (EFlags value) __Th___
	{
		_baseFlags |= value;
	}

/*
=================================================
	_Slider
=================================================
*/
	template <typename T>
	void  ScriptBasePass::_Slider (const String &name, const T &min, const T &max, T val, ESlider type) __Th___
	{
		CHECK_THROW_MSG( _uniqueSliderNames.insert( name ).second,
			"Slider/Constant '"s << name << "' is already exists" );

		uint	idx = _sliderCounter[ uint(type) ]++;
		CHECK_THROW_MSG( idx < UIInteraction::MaxSlidersPerType,
			"Slider count "s << ToString(idx+1) << " must be less than " << ToString(UIInteraction::MaxSlidersPerType) );

		auto&	dst = _sliders.emplace_back();

		dst.name	= name;
		dst.index	= idx;
		dst.count	= sizeof(T) / sizeof(int);
		dst.type	= type;

		val = Clamp( val, min, max );

		MemCopy( OUT &dst.intRange[0], &min, Sizeof(min) );
		MemCopy( OUT &dst.intRange[1], &max, Sizeof(max) );
		MemCopy( OUT &dst.intRange[2], &val, Sizeof(val) );
	}

/*
=================================================
	SliderI
=================================================
*/
	void  ScriptBasePass::SliderI0 (const String &name) __Th___
	{
		int	min = 0, max = 1024;
		return _Slider( name, min, max, min, ESlider::Int );
	}

	void  ScriptBasePass::SliderI1 (const String &name, int min, int max) __Th___
	{
		return _Slider( name, min, max, min, ESlider::Int );
	}

	void  ScriptBasePass::SliderI2 (const String &name, const packed_int2 &min, const packed_int2 &max) __Th___
	{
		return _Slider( name, min, max, min, ESlider::Int );
	}

	void  ScriptBasePass::SliderI3 (const String &name, const packed_int3 &min, const packed_int3 &max) __Th___
	{
		return _Slider( name, min, max, min, ESlider::Int );
	}

	void  ScriptBasePass::SliderI4 (const String &name, const packed_int4 &min, const packed_int4 &max) __Th___
	{
		return _Slider( name, min, max, min, ESlider::Int );
	}

	void  ScriptBasePass::SliderI1a (const String &name, int min, int max, int val) __Th___
	{
		return _Slider( name, min, max, val, ESlider::Int );
	}

	void  ScriptBasePass::SliderI2a (const String &name, const packed_int2 &min, const packed_int2 &max, const packed_int2 &val) __Th___
	{
		return _Slider( name, min, max, val, ESlider::Int );
	}

	void  ScriptBasePass::SliderI3a (const String &name, const packed_int3 &min, const packed_int3 &max, const packed_int3 &val) __Th___
	{
		return _Slider( name, min, max, val, ESlider::Int );
	}

	void  ScriptBasePass::SliderI4a (const String &name, const packed_int4 &min, const packed_int4 &max, const packed_int4 &val) __Th___
	{
		return _Slider( name, min, max, val, ESlider::Int );
	}

/*
=================================================
	SliderF
=================================================
*/
	void  ScriptBasePass::SliderF0 (const String &name) __Th___
	{
		float min = 0.f, max = 1.f;
		return _Slider( name, min, max, min, ESlider::Float );
	}

	void  ScriptBasePass::SliderF1 (const String &name, float min, float max) __Th___
	{
		return _Slider( name, min, max, min, ESlider::Float );
	}

	void  ScriptBasePass::SliderF2 (const String &name, const packed_float2 &min, const packed_float2 &max) __Th___
	{
		return _Slider( name, min, max, min, ESlider::Float );
	}

	void  ScriptBasePass::SliderF3 (const String &name, const packed_float3 &min, const packed_float3 &max) __Th___
	{
		return _Slider( name, min, max, min, ESlider::Float );
	}

	void  ScriptBasePass::SliderF4 (const String &name, const packed_float4 &min, const packed_float4 &max) __Th___
	{
		return _Slider( name, min, max, min, ESlider::Float );
	}

	void  ScriptBasePass::SliderF1a (const String &name, float min, float max, float val) __Th___
	{
		return _Slider( name, min, max, val, ESlider::Float );
	}

	void  ScriptBasePass::SliderF2a (const String &name, const packed_float2 &min, const packed_float2 &max, const packed_float2 &val) __Th___
	{
		return _Slider( name, min, max, val, ESlider::Float );
	}

	void  ScriptBasePass::SliderF3a (const String &name, const packed_float3 &min, const packed_float3 &max, const packed_float3 &val) __Th___
	{
		return _Slider( name, min, max, val, ESlider::Float );
	}

	void  ScriptBasePass::SliderF4a (const String &name, const packed_float4 &min, const packed_float4 &max, const packed_float4 &val) __Th___
	{
		return _Slider( name, min, max, val, ESlider::Float );
	}

/*
=================================================
	ColorSelector
=================================================
*/
	void  ScriptBasePass::ColorSelector1 (const String &name) __Th___
	{
		RGBA32f	min{0.f}, max{1.f};
		return _Slider( name, min, max, max, ESlider::Color );
	}

	void  ScriptBasePass::ColorSelector2 (const String &name, const RGBA32f &val) __Th___
	{
		RGBA32f	min{0.f}, max{1.f};
		return _Slider( name, min, max, val, ESlider::Color );
	}

	void  ScriptBasePass::ColorSelector3 (const String &name, const RGBA8u &val) __Th___
	{
		RGBA32f	min{0.f}, max{1.f};
		return _Slider( name, min, max, RGBA32f{val}, ESlider::Color );
	}

/*
=================================================
	ConstantF*
=================================================
*/
	void  ScriptBasePass::ConstantF1 (const String &name, const ScriptDynamicFloatPtr  &dynVal) __Th___	{ return _Constant( name, dynVal, ESlider::Float, 1 ); }
	void  ScriptBasePass::ConstantF2 (const String &name, const ScriptDynamicFloat2Ptr &dynVal) __Th___	{ return _Constant( name, dynVal, ESlider::Float, 2 ); }
	void  ScriptBasePass::ConstantF3 (const String &name, const ScriptDynamicFloat3Ptr &dynVal) __Th___	{ return _Constant( name, dynVal, ESlider::Float, 3 ); }
	void  ScriptBasePass::ConstantF4 (const String &name, const ScriptDynamicFloat4Ptr &dynVal) __Th___	{ return _Constant( name, dynVal, ESlider::Float, 4 ); }

/*
=================================================
	ConstantI*
=================================================
*/
	void  ScriptBasePass::ConstantI1 (const String &name, const ScriptDynamicIntPtr  &dynVal)	__Th___	{ return _Constant( name, dynVal, ESlider::Int, 1 ); }
	void  ScriptBasePass::ConstantI2 (const String &name, const ScriptDynamicInt2Ptr &dynVal)	__Th___	{ return _Constant( name, dynVal, ESlider::Int, 2 ); }
	void  ScriptBasePass::ConstantI3 (const String &name, const ScriptDynamicInt3Ptr &dynVal)	__Th___	{ return _Constant( name, dynVal, ESlider::Int, 3 ); }
	void  ScriptBasePass::ConstantI4 (const String &name, const ScriptDynamicInt4Ptr &dynVal)	__Th___	{ return _Constant( name, dynVal, ESlider::Int, 4 ); }

/*
=================================================
	ConstantU*
=================================================
*/
	void  ScriptBasePass::ConstantU1 (const String &name, const ScriptDynamicUIntPtr  &dynVal)	__Th___	{ return _Constant( name, dynVal, ESlider::Int, 1 ); }
	void  ScriptBasePass::ConstantU2 (const String &name, const ScriptDynamicUInt2Ptr &dynVal)	__Th___	{ return _Constant( name, dynVal, ESlider::Int, 2 ); }
	void  ScriptBasePass::ConstantU3 (const String &name, const ScriptDynamicUInt3Ptr &dynVal)	__Th___	{ return _Constant( name, dynVal, ESlider::Int, 3 ); }
	void  ScriptBasePass::ConstantU4 (const String &name, const ScriptDynamicUInt4Ptr &dynVal)	__Th___	{ return _Constant( name, dynVal, ESlider::Int, 4 ); }

	void  ScriptBasePass::ConstantDD (const String &name, const ScriptDynamicDimPtr &dynVal)	__Th___	{ return _Constant( name, dynVal, ESlider::Int, 3 ); }

/*
=================================================
	ConstantCF*
=================================================
*/
	void  ScriptBasePass::ConstantCF1 (const String &name, float value)					__Th___	{ _Constant2( name, MakeRCTh<DynamicFloat >(value), ESlider::Float, 1 ); }
	void  ScriptBasePass::ConstantCF2 (const String &name, const packed_float2 &value)	__Th___	{ _Constant2( name, MakeRCTh<DynamicFloat2>(value), ESlider::Float, 2 ); }
	void  ScriptBasePass::ConstantCF3 (const String &name, const packed_float3 &value)	__Th___	{ _Constant2( name, MakeRCTh<DynamicFloat3>(value), ESlider::Float, 3 ); }
	void  ScriptBasePass::ConstantCF4 (const String &name, const packed_float4 &value)	__Th___	{ _Constant2( name, MakeRCTh<DynamicFloat4>(value), ESlider::Float, 4 ); }

/*
=================================================
	ConstantCI*
=================================================
*/
	void  ScriptBasePass::ConstantCI1 (const String &name, int value)					__Th___	{ _Constant2( name, MakeRCTh<DynamicInt >(value), ESlider::Int, 1 ); }
	void  ScriptBasePass::ConstantCI2 (const String &name, const packed_int2 &value)	__Th___	{ _Constant2( name, MakeRCTh<DynamicInt2>(value), ESlider::Int, 2 ); }
	void  ScriptBasePass::ConstantCI3 (const String &name, const packed_int3 &value)	__Th___	{ _Constant2( name, MakeRCTh<DynamicInt3>(value), ESlider::Int, 3 ); }
	void  ScriptBasePass::ConstantCI4 (const String &name, const packed_int4 &value)	__Th___	{ _Constant2( name, MakeRCTh<DynamicInt4>(value), ESlider::Int, 4 ); }

/*
=================================================
	ConstantCU*
=================================================
*/
	void  ScriptBasePass::ConstantCU1 (const String &name, uint value)					__Th___	{ _Constant2( name, MakeRCTh<DynamicUInt >(value), ESlider::Int, 1 ); }
	void  ScriptBasePass::ConstantCU2 (const String &name, const packed_uint2 &value)	__Th___	{ _Constant2( name, MakeRCTh<DynamicUInt2>(value), ESlider::Int, 2 ); }
	void  ScriptBasePass::ConstantCU3 (const String &name, const packed_uint3 &value)	__Th___	{ _Constant2( name, MakeRCTh<DynamicUInt3>(value), ESlider::Int, 3 ); }
	void  ScriptBasePass::ConstantCU4 (const String &name, const packed_uint4 &value)	__Th___	{ _Constant2( name, MakeRCTh<DynamicUInt4>(value), ESlider::Int, 4 ); }

/*
=================================================
	_Constant
=================================================
*/
	template <typename T>
	void  ScriptBasePass::_Constant (const String &name, const T &dynVal, ESlider type, ubyte count) __Th___
	{
		CHECK_THROW_MSG( dynVal );
		return _Constant2( name, dynVal->Get(), type, count );
	}

	template <typename T>
	void  ScriptBasePass::_Constant2 (const String &name, const T &dynVal, const ESlider type, const ubyte count) __Th___
	{
		CHECK_THROW_MSG( dynVal );
		CHECK_THROW_MSG( _uniqueSliderNames.insert( name ).second,
			"Constant/Slider '"s << name << "' is already exists" );

		uint	idx = _constantCounter[ uint(type) ]++;
		CHECK_THROW_MSG( idx < UIInteraction::MaxSlidersPerType,
			"Constants count "s << ToString(idx) << " must be less than " << ToString(UIInteraction::MaxSlidersPerType) );

		auto&	dst	= _constants.emplace_back();
		dst.name	= name;
		dst.index	= idx;
		dst.type	= type;
		dst.count	= count;
		dst.rc		= dynVal;
	}

/*
=================================================
	SetDebugLabel
=================================================
*/
	void  ScriptBasePass::SetDebugLabel1 (const String &name) __Th___
	{
		_dbgName	= name;
	}

	void  ScriptBasePass::SetDebugLabel2 (const String &name, const RGBA8u &color) __Th___
	{
		_dbgName	= name;
		_dbgColor	= color;
	}

/*
=================================================
	_AddSlidersToUIInteraction
=================================================
*/
	void  ScriptBasePass::_AddSlidersToUIInteraction (IPass* pass) const
	{
		if ( _sliders.empty() )
			return;

		UIInteraction::PerPassSlidersInfo	info;
		SliderCounter_t						slider_idx {};

		for (const auto& slider : _sliders)
		{
			const uint	idx = slider_idx[ uint(slider.type) ]++;
			switch_enum( slider.type )
			{
				case ESlider::Int :
					info.intRange [idx][0]							= slider.intRange[0];
					info.intRange [idx][1]							= slider.intRange[1];
					info.intRange [idx][2]							= slider.intRange[2];
					info.intVecSize [idx]							= slider.count;
					info.names [idx] [UIInteraction::IntSliderIdx]	= slider.name;
					break;

				case ESlider::Float :
					info.floatRange [idx][0]						= slider.floatRange[0];
					info.floatRange [idx][1]						= slider.floatRange[1];
					info.floatRange [idx][2]						= slider.floatRange[2];
					info.floatVecSize [idx]							= slider.count;
					info.names [idx][UIInteraction::FloatSliderIdx]	= slider.name;
					break;

				case ESlider::Color :
					info.colors [idx]									= RGBA32f{slider.floatRange[2]};
					info.names [idx] [UIInteraction::ColorSelectorIdx]	= slider.name;
					break;

				case ESlider::_Count :	break;
			}
			switch_end
		}

		info.passName = this->_dbgName;
		UIInteraction::Instance().AddSliders( pass, RVRef(info) );
	}

/*
=================================================
	_CopyConstants
=================================================
*/
	void  ScriptBasePass::_CopyConstants (OUT IPass::Constants &dst) const
	{
		for (auto& c : _constants)
		{
			switch_enum( c.type )
			{
				case ESlider::Float :
					dst.f[ c.index ] = c.rc;
					break;

				case ESlider::Int :
					dst.i[ c.index ] = c.rc;
					break;

				case ESlider::Color :
				case ESlider::_Count :  break;
			}
			switch_end
		}
	}

/*
=================================================
	_Dimension
=================================================
*/
	ScriptDynamicDim*  ScriptBasePass::_Dimension () __Th___
	{
		CHECK_THROW_MSG( _HasCustomDynamicDimension() );
		return ScriptDynamicDimPtr{_dynamicDim}.Detach();
	}

/*
=================================================
	_SetDynamicDimension
=================================================
*/
	void  ScriptBasePass::_SetDynamicDimension (const ScriptDynamicDimPtr &dynDim) __Th___
	{
		CHECK_THROW_MSG( dynDim and dynDim->Get() );

		if ( _HasCustomDynamicDimension() )
		{
			if ( _dynamicDim == dynDim or _dynamicDim->Get() == dynDim->Get() )
				return;
		}

		CHECK_THROW_MSG( not _HasCustomDynamicDimension(),
			"Previous dynamic dimension is already used" );

		_dynamicDim = dynDim;
	}

/*
=================================================
	_HasCustomDynamicDimension
=================================================
*/
	bool  ScriptBasePass::_HasCustomDynamicDimension () C_Th___
	{
		return bool{_dynamicDim};
	}

/*
=================================================
	_SetConstDimension
=================================================
*/
	void  ScriptBasePass::_SetConstDimension (const uint3 &) __Th___
	{
		// TODO
	}

/*
=================================================
	ArgController
=================================================
*/
	void  ScriptBasePass::ArgController (const ScriptBaseControllerPtr &controller) __Th___
	{
		CHECK_THROW_MSG( controller );
		CHECK_THROW_MSG( not _controller, "controller is already exists" );

		_controller = controller;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptBasePass::Bind (const ScriptEnginePtr &se) __Th___
	{
		using namespace Scripting;

		ClassBinder<ScriptBasePass>		binder{ se };
		binder.CreateRef( 0, False{"no ctor"} );
	}

/*
=================================================
	_AddDefines
=================================================
*/
	void  ScriptBasePass::_AddDefines (StringView defines, INOUT String &header) __Th___
	{
		if ( not defines.empty() )
		{
			Array<StringView>	def_tokens;
			Parser::Tokenize( defines, ';', OUT def_tokens );

			for (auto def : def_tokens) {
				header << "#define " << def << '\n';
			}
		}
	}

/*
=================================================
	_AddSliders
=================================================
*/
	void  ScriptBasePass::_AddSliders (INOUT String &header) C_Th___
	{
		// add sliders
		{
			const uint	max_sliders = UIInteraction::MaxSlidersPerType;
			for (usize i = 0; i < _sliderCounter.size(); ++i) {
				CHECK_THROW_MSG( _sliderCounter[i] <= max_sliders );
			}

			for (auto& slider : _sliders)
			{
				header << "#define " << slider.name << " un_PerPass.";
				switch_enum( slider.type )
				{
					case ESlider::Int :		header << "intSliders[";	break;
					case ESlider::Float :	header << "floatSliders[";	break;
					case ESlider::Color :	header << "colors[";		break;
					case ESlider::_Count :
					default :				CHECK_THROW_MSG( false, "unknown slider type" );
				}
				switch_end

				header << ToString( slider.index ) << "]";
				switch ( slider.count )
				{
					case 1 :	header << ".x";		break;
					case 2 :	header << ".xy";	break;
					case 3 :	header << ".xyz";	break;
					case 4 :	header << ".xyzw";	break;
					default :	CHECK_THROW_MSG( false, "unknown slider value size" );
				}
				header << "\n";
			}

			// add min/max for integer
			const auto	AddValue = [&header] (int4 val, const ubyte count)
			{{
				switch ( count )
				{
					case 1:	header << "(" << ToString( val.x ) << ")";  break;
					case 2:	header << "int2(" << ToString( val.x ) << ", " << ToString( val.y ) << ")"; break;
					case 3:	header << "int3(" << ToString( val.x ) << ", " << ToString( val.y ) << ToString( val.z )<< ")";  break;
					case 4:	header << "int4(" << ToString( val.x ) << ", " << ToString( val.y ) << ", " << ToString( val.z ) << ", " << ToString( val.w ) << ")";  break;
				}
			}};

			for (auto& slider : _sliders)
			{
				if ( slider.type == ESlider::Int )
				{
					header << "#define " << slider.name << "_min ";
					AddValue( slider.intRange[0], slider.count );

					header << "\n#define " << slider.name << "_max ";
					AddValue( slider.intRange[1], slider.count );

					header << '\n';
				}
			}
		}

		// add constants
		{
			const uint	max_constants = UIInteraction::MaxSlidersPerType;
			for (usize i = 0; i < _constantCounter.size(); ++i) {
				CHECK_THROW_MSG( _constantCounter[i] <= max_constants );
			}

			for (auto& c : _constants)
			{
				header << "#define " << c.name << " un_PerPass.";
				switch_enum( c.type )
				{
					case ESlider::Int :		header << "intConst[";		break;
					case ESlider::Float :	header << "floatConst[";	break;
					case ESlider::Color :
					case ESlider::_Count :
					default :				CHECK_THROW_MSG( false, "unknown constant type" );
				}
				switch_end

				header << ToString( c.index ) << "]";
				switch ( c.count )
				{
					case 1 :	header << ".x";		break;
					case 2 :	header << ".xy";	break;
					case 3 :	header << ".xyz";	break;
					case 4 :	header << ".xyzw";	break;
					default :	CHECK_THROW_MSG( false, "unknown constant value size" );
				}
				header << "\n";
			}
		}
	}

/*
=================================================
	_AddSlidersAsMacros
=================================================
*/
	void  ScriptBasePass::_AddSlidersAsMacros (OUT String &macros) C_Th___
	{
		String	temp;
		_AddSliders( OUT temp );

		const StringView	str {temp};
		const StringView	def {"#define "};

		// change format from '#define name  value' to 'name = value;'
		for (usize pos = 0;;)
		{
			pos = str.find( def, pos );
			if ( pos == StringView::npos )
				break;

			const usize	begin	= pos + def.length();

			pos = str.find( '\n', begin );
			if ( pos == StringView::npos )
				break;

			usize	p0 = str.find( " ", begin );

			macros << str.substr( begin, p0 - begin ); // name
			macros << " = ";
			macros << str.substr( p0+1, pos - p0-1 );  // value
			macros << '\n';
		}
	}

/*
=================================================
	SetRepeatCount
=================================================
*/
	void  ScriptBasePass::SetRepeatCount (const ScriptDynamicUIntPtr &value) __Th___
	{
		CHECK_THROW_MSG( value );
		CHECK_THROW_MSG( not _repeatCount, "Repeat count is already set" );

		_repeatCount = value;
	}

/*
=================================================
	EnableIf*
=================================================
*/
	void  ScriptBasePass::EnableIfEqual (const ScriptDynamicUIntPtr &dyn, uint ref) __Th___
	{
		CHECK_THROW_MSG( dyn );
		CHECK_THROW_MSG( not _enablePass.dynamic, "EnableIf is already used" );

		_enablePass.dynamic	= dyn;
		_enablePass.ref		= ref;
		_enablePass.op		= ECompare::Equal;
	}

	void  ScriptBasePass::EnableIfLess (const ScriptDynamicUIntPtr &dyn, uint ref) __Th___
	{
		CHECK_THROW_MSG( dyn );
		CHECK_THROW_MSG( not _enablePass.dynamic, "EnableIf is already used" );

		_enablePass.dynamic	= dyn;
		_enablePass.ref		= ref;
		_enablePass.op		= ECompare::Less;
	}

	void  ScriptBasePass::EnableIfGreater (const ScriptDynamicUIntPtr &dyn, uint ref) __Th___
	{
		CHECK_THROW_MSG( dyn );
		CHECK_THROW_MSG( not _enablePass.dynamic, "EnableIf is already used" );

		_enablePass.dynamic	= dyn;
		_enablePass.ref		= ref;
		_enablePass.op		= ECompare::Greater;
	}

	void  ScriptBasePass::EnableIfAnyBit (const ScriptDynamicUIntPtr &dyn, uint ref) __Th___
	{
		CHECK_THROW_MSG( dyn );
		CHECK_THROW_MSG( not _enablePass.dynamic, "EnableIf is already used" );

		_enablePass.dynamic	= dyn;
		_enablePass.ref		= ref;
		_enablePass.op		= ECompare::AnyBit;
	}

/*
=================================================
	_Init
=================================================
*/
	void  ScriptBasePass::_Init (IPass &dst, const ScriptBaseControllerPtr &defaultController) C_Th___
	{
		_CopyConstants( OUT dst._shConst );
		_AddSlidersToUIInteraction( &dst );

		dst._dbgName	= this->_dbgName;
		dst._dbgColor	= this->_dbgColor;

		ScriptBaseControllerPtr	controller = this->_controller ? this->_controller : defaultController;

		if ( controller )
		{
			controller->SetDimensionIfNotSet( _dynamicDim );
			dst._controller	= controller->ToController();  // throw
			CHECK_THROW( dst._controller );
		}

		if ( this->_enablePass.dynamic )
		{
			dst._enablePass.dynamic	= this->_enablePass.dynamic->Get();
			dst._enablePass.ref		= this->_enablePass.ref;
			dst._enablePass.op		= this->_enablePass.op;
		}

		if ( this->_repeatCount )
			dst._repeatCount = this->_repeatCount->Get();

		AE_LOGI( "Compiled: "s << this->_dbgName );
	}

/*
=================================================
	_CreateUBuffer
=================================================
*/
	Strong<BufferID>  ScriptBasePass::_CreateUBuffer (Bytes size, StringView dbgName, EResourceState defaultState) C_Th___
	{
		Renderer&	renderer	= ScriptExe::ScriptPassApi::GetRenderer();  // throw
		auto&		rstate		= GraphicsScheduler().GetRenderGraph();

		auto id = rstate.CreateBuffer( BufferDesc{ size, EBufferUsage::Uniform | EBufferUsage::TransferDst },
										dbgName, renderer.ChooseAllocator( False{"static"}, size ));
		CHECK_THROW( id );

		rstate.AddResource( id, EResourceState::Invalidate, defaultState );
		return id;
	}


} // AE::ResEditor
