// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/ScriptEngine.inl.h"

namespace AE::Scripting
{
namespace
{
	template <int I>
	struct ScriptRandom_Binomial
	{
		Math::Random::_Binomial<Vec<int,I>>		_bin;

		PackedVec<int,I>	Gen ()	{ return _bin(); }
	};

	template <>
	struct ScriptRandom_Binomial<1>
	{
		Math::Random::_Binomial<int>			_bin;

		int					Gen ()	{ return _bin(); }
	};


	template <int I>
	struct ScriptRandom_Normal
	{
		Math::Random::_Normal<Vec<float,I>>		_bin;

		PackedVec<float,I>	Gen ()	{ return _bin(); }
	};

	template <>
	struct ScriptRandom_Normal<1>
	{
		Math::Random::_Normal<float>			_bin;

		float				Gen ()	{ return _bin(); }
	};


	struct ScriptRandom
	{
		Math::Random	_rnd;

		float			Uniform1f (float min, float max)								{ return _rnd.Uniform( min, max ); }
		packed_float2	Uniform2f (const packed_float2 &min, const packed_float2 &max)	{ return _rnd.Uniform( float2{min}, float2{max} ); }
		packed_float3	Uniform3f (const packed_float3 &min, const packed_float3 &max)	{ return _rnd.Uniform( float3{min}, float3{max} ); }
		packed_float4	Uniform4f (const packed_float4 &min, const packed_float4 &max)	{ return _rnd.Uniform( float4{min}, float4{max} ); }

		int				Uniform1i (int min, int max)									{ return _rnd.Uniform( min, max ); }
		packed_int2		Uniform2i (const packed_int2 &min, const packed_int2 &max)		{ return _rnd.Uniform( int2{min}, int2{max} ); }
		packed_int3		Uniform3i (const packed_int3 &min, const packed_int3 &max)		{ return _rnd.Uniform( int3{min}, int3{max} ); }
		packed_int4		Uniform4i (const packed_int4 &min, const packed_int4 &max)		{ return _rnd.Uniform( int4{min}, int4{max} ); }

		uint			Uniform1u (uint min, uint max)									{ return _rnd.Uniform( min, max ); }
		packed_uint2	Uniform2u (const packed_uint2 &min, const packed_uint2 &max)	{ return _rnd.Uniform( uint2{min}, uint2{max} ); }
		packed_uint3	Uniform3u (const packed_uint3 &min, const packed_uint3 &max)	{ return _rnd.Uniform( uint3{min}, uint3{max} ); }
		packed_uint4	Uniform4u (const packed_uint4 &min, const packed_uint4 &max)	{ return _rnd.Uniform( uint4{min}, uint4{max} ); }

		RGBA32f			UniformColor ()													{ return _rnd.UniformColor(); }

		bool			Bernoulli1 (float p)											{ return _rnd.Bernoulli( double(p) ); }
		packed_bool2	Bernoulli2 (float p)											{ return _rnd.Bernoulli2( double(p) ); }
		packed_bool3	Bernoulli3 (float p)											{ return _rnd.Bernoulli3( double(p) ); }
		packed_bool4	Bernoulli4 (float p)											{ return _rnd.Bernoulli4( double(p) ); }

		ScriptRandom_Binomial<1>	Binomial1 (int trials, float probability)			{ return ScriptRandom_Binomial<1>{_rnd.Binomial<int >( trials, double(probability) )}; }
		ScriptRandom_Binomial<2>	Binomial2 (int trials, float probability)			{ return ScriptRandom_Binomial<2>{_rnd.Binomial<int2>( trials, double(probability) )}; }
		ScriptRandom_Binomial<3>	Binomial3 (int trials, float probability)			{ return ScriptRandom_Binomial<3>{_rnd.Binomial<int3>( trials, double(probability) )}; }
		ScriptRandom_Binomial<4>	Binomial4 (int trials, float probability)			{ return ScriptRandom_Binomial<4>{_rnd.Binomial<int4>( trials, double(probability) )}; }

		ScriptRandom_Normal<1>		Normal1 (float mean, float sigma)					{ return ScriptRandom_Normal<1>{_rnd.Normal<float >( mean, sigma )}; }
		ScriptRandom_Normal<2>		Normal2 (float mean, float sigma)					{ return ScriptRandom_Normal<2>{_rnd.Normal<float2>( mean, sigma )}; }
		ScriptRandom_Normal<3>		Normal3 (float mean, float sigma)					{ return ScriptRandom_Normal<3>{_rnd.Normal<float3>( mean, sigma )}; }
		ScriptRandom_Normal<4>		Normal4 (float mean, float sigma)					{ return ScriptRandom_Normal<4>{_rnd.Normal<float4>( mean, sigma )}; }
	};

} // namespace
} // AE::Scripting

AE_DECL_SCRIPT_OBJ( AE::Scripting::ScriptRandom,				"Random" );
AE_DECL_SCRIPT_OBJ( AE::Scripting::ScriptRandom_Binomial<1>,	"Random_Binomial1" );
AE_DECL_SCRIPT_OBJ( AE::Scripting::ScriptRandom_Binomial<2>,	"Random_Binomial2" );
AE_DECL_SCRIPT_OBJ( AE::Scripting::ScriptRandom_Binomial<3>,	"Random_Binomial3" );
AE_DECL_SCRIPT_OBJ( AE::Scripting::ScriptRandom_Binomial<4>,	"Random_Binomial4" );
AE_DECL_SCRIPT_OBJ( AE::Scripting::ScriptRandom_Normal<1>,		"Random_Normal1" );
AE_DECL_SCRIPT_OBJ( AE::Scripting::ScriptRandom_Normal<2>,		"Random_Normal2" );
AE_DECL_SCRIPT_OBJ( AE::Scripting::ScriptRandom_Normal<3>,		"Random_Normal3" );
AE_DECL_SCRIPT_OBJ( AE::Scripting::ScriptRandom_Normal<4>,		"Random_Normal4" );

namespace AE::Scripting
{
namespace
{
/*
=================================================
	Bind_Random_Binomial
=================================================
*/
	static void  Bind_Random_Binomial (const ScriptEnginePtr &se)
	{
		using namespace AngelScript;
		const auto	flags = asEObjTypeFlags::asOBJ_APP_CLASS |
							asEObjTypeFlags::asOBJ_APP_CLASS_DESTRUCTOR |
							asEObjTypeFlags::asOBJ_APP_CLASS_COPY_CONSTRUCTOR;
		{
			ClassBinder<ScriptRandom_Binomial<1>>	binder{ se };
			binder.CreateClassValue( flags );
			binder.Operators().Call( &ScriptRandom_Binomial<1>::Gen );
		}{
			ClassBinder<ScriptRandom_Binomial<2>>	binder{ se };
			binder.CreateClassValue( flags );
			binder.Operators().Call( &ScriptRandom_Binomial<2>::Gen );
		}{
			ClassBinder<ScriptRandom_Binomial<3>>	binder{ se };
			binder.CreateClassValue( flags );
			binder.Operators().Call( &ScriptRandom_Binomial<3>::Gen );
		}{
			ClassBinder<ScriptRandom_Binomial<4>>	binder{ se };
			binder.CreateClassValue( flags );
			binder.Operators().Call( &ScriptRandom_Binomial<4>::Gen );
		}
	}

/*
=================================================
	Bind_Random_Normal
=================================================
*/
	static void  Bind_Random_Normal (const ScriptEnginePtr &se)
	{
		using namespace AngelScript;
		const auto	flags = asEObjTypeFlags::asOBJ_APP_CLASS |
							asEObjTypeFlags::asOBJ_APP_CLASS_DESTRUCTOR |
							asEObjTypeFlags::asOBJ_APP_CLASS_COPY_CONSTRUCTOR;
		{
			ClassBinder<ScriptRandom_Normal<1>>		binder{ se };
			binder.CreateClassValue( flags );
			binder.Operators().Call( &ScriptRandom_Normal<1>::Gen );
		}{
			ClassBinder<ScriptRandom_Normal<2>>		binder{ se };
			binder.CreateClassValue( flags );
			binder.Operators().Call( &ScriptRandom_Normal<2>::Gen );
		}{
			ClassBinder<ScriptRandom_Normal<3>>		binder{ se };
			binder.CreateClassValue( flags );
			binder.Operators().Call( &ScriptRandom_Normal<3>::Gen );
		}{
			ClassBinder<ScriptRandom_Normal<4>>		binder{ se };
			binder.CreateClassValue( flags );
			binder.Operators().Call( &ScriptRandom_Normal<4>::Gen );
		}
	}

/*
=================================================
	Bind_Random
=================================================
*/
	static void  Bind_Random (const ScriptEnginePtr &se)
	{
		ClassBinder<ScriptRandom>	binder{ se };
		binder.CreateClassValue();

		binder.AddMethod( &ScriptRandom::Uniform1f,		"Uniform",		{"min", "max"} );
		binder.AddMethod( &ScriptRandom::Uniform2f,		"Uniform",		{"min", "max"} );
		binder.AddMethod( &ScriptRandom::Uniform3f,		"Uniform",		{"min", "max"} );
		binder.AddMethod( &ScriptRandom::Uniform4f,		"Uniform",		{"min", "max"} );

		binder.AddMethod( &ScriptRandom::Uniform1i,		"Uniform",		{"min", "max"} );
		binder.AddMethod( &ScriptRandom::Uniform2i,		"Uniform",		{"min", "max"} );
		binder.AddMethod( &ScriptRandom::Uniform3i,		"Uniform",		{"min", "max"} );
		binder.AddMethod( &ScriptRandom::Uniform4i,		"Uniform",		{"min", "max"} );

		binder.AddMethod( &ScriptRandom::Uniform1u,		"Uniform",		{"min", "max"} );
		binder.AddMethod( &ScriptRandom::Uniform2u,		"Uniform",		{"min", "max"} );
		binder.AddMethod( &ScriptRandom::Uniform3u,		"Uniform",		{"min", "max"} );
		binder.AddMethod( &ScriptRandom::Uniform4u,		"Uniform",		{"min", "max"} );

		binder.AddMethod( &ScriptRandom::UniformColor,	"UniformColor",	{} );

		binder.AddMethod( &ScriptRandom::Bernoulli1,	"Bernoulli",	{"p"} );
		binder.AddMethod( &ScriptRandom::Bernoulli2,	"Bernoulli2",	{"p"} );
		binder.AddMethod( &ScriptRandom::Bernoulli3,	"Bernoulli3",	{"p"} );
		binder.AddMethod( &ScriptRandom::Bernoulli4,	"Bernoulli4",	{"p"} );

		binder.AddMethod( &ScriptRandom::Binomial1,		"Binomial",		{"trials", "probability"} );
		binder.AddMethod( &ScriptRandom::Binomial2,		"Binomial2",	{"trials", "probability"} );
		binder.AddMethod( &ScriptRandom::Binomial3,		"Binomial3",	{"trials", "probability"} );
		binder.AddMethod( &ScriptRandom::Binomial4,		"Binomial4",	{"trials", "probability"} );

		binder.AddMethod( &ScriptRandom::Normal1,		"Normal",		{"mean", "sigma"} );
		binder.AddMethod( &ScriptRandom::Normal2,		"Normal2",		{"mean", "sigma"} );
		binder.AddMethod( &ScriptRandom::Normal3,		"Normal3",		{"mean", "sigma"} );
		binder.AddMethod( &ScriptRandom::Normal4,		"Normal4",		{"mean", "sigma"} );
	}

} // namespace


/*
=================================================
	BindRandom
=================================================
*/
	void  CoreBindings::BindRandom (const ScriptEnginePtr &se) __Th___
	{
		CHECK_THROW( se and se->IsInitialized() );

		Bind_Random_Binomial( se );
		Bind_Random_Normal( se );
		Bind_Random( se );
	}

} // AE::Scripting
