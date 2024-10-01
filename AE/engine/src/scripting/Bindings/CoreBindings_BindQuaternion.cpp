// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/ScriptEngine.inl.h"

namespace AE::Scripting
{
namespace
{

/*
=================================================
	QuatCtor
=================================================
*/
	template <typename T>
	struct QuatCtor
	{
	private:
		using Quat_t	= TQuat< T, GLMPackedQualifier >;
		using Vec4_t	= PackedVec< T, 4 >;


		static void  _Ctor3 (void* mem, T w, T x, T y, T z)
		{
			PlacementNew< Quat_t >( OUT mem, w, x, y, z );
		}


		static void  _V4Ctor (void* mem, const Quat_t &q)
		{
			PlacementNew< Vec4_t >( OUT mem, q.x, q.y, q.z, q.w );
		}


	public:
		static void  Bind (ClassBinder<Quat_t> &binder, ClassBinder<Vec4_t> &binder2)
		{
			using Field_t = T (Quat_t::*);

			binder.CreateClassValue();

			binder.AddConstructor( &_Ctor3,	{"w", "x", "y", "z"} );

			binder.AddProperty( BitCast<Field_t>( &Quat_t::x ), "x" );
			binder.AddProperty( BitCast<Field_t>( &Quat_t::y ), "y" );
			binder.AddProperty( BitCast<Field_t>( &Quat_t::z ), "z" );
			binder.AddProperty( BitCast<Field_t>( &Quat_t::w ), "w" );

			binder2.AddConstructor( &_V4Ctor, {"quat"} );
		}
	};

/*
=================================================
	QuatFunc
=================================================
*/
	template <typename Q>
	struct QuatFunc
	{
		using Quat_t	= Q;
		using T			= typename Q::Value_t;
		using Vec3_t	= PackedVec< T, 3 >;
		using Vec4_t	= PackedVec< T, 4 >;
		using Rad_t		= typename Q::Rad_t;
		using Rad3_t	= typename Q::Rad3_t;


		static Quat_t	Add_q_q  (const Quat_t& lhs, const Quat_t &rhs)						{ return lhs + rhs; }
		static Quat_t	Sub_q_q  (const Quat_t& lhs, const Quat_t &rhs)						{ return lhs - rhs; }
		static Quat_t	Mul_q_q  (const Quat_t& lhs, const Quat_t &rhs)						{ return lhs * rhs; }
		static Vec3_t	Mul_q_v3 (const Quat_t& lhs, const Vec3_t &rhs)						{ return lhs * rhs; }
		static Vec4_t	Mul_q_v4 (const Quat_t& lhs, const Vec4_t &rhs)						{ return lhs * rhs; }
		static Quat_t	Mul_q_s  (const Quat_t& lhs, T rhs)									{ return lhs * rhs; }
		static Quat_t	Div_q_s  (const Quat_t& lhs, T rhs)									{ return lhs / rhs; }

		static Quat_t	RotateX  (const Quat_t&, T angle)									{ return Quat_t::RotateX( Rad_t{angle} ); }
		static Quat_t	RotateY  (const Quat_t&, T angle)									{ return Quat_t::RotateY( Rad_t{angle} ); }
		static Quat_t	RotateZ  (const Quat_t&, T angle)									{ return Quat_t::RotateZ( Rad_t{angle} ); }

		static Quat_t	Rotate   (const Quat_t&, T angle, const Vec3_t &axis)				{ return Quat_t::Rotate( Rad_t{angle}, axis ); }
		static Quat_t	Rotate1  (const Quat_t&, const Vec3_t &angles)						{ return Quat_t::Rotate( Rad3_t{angles} ); }
		static Quat_t	Rotate3  (const Quat_t&, T angleX, T angleY, T angleZ)				{ return Quat_t::Rotate( Rad3_t{Vec3_t{ angleX, angleY, angleZ }}); }
		static Quat_t	Rotate2  (const Quat_t&, const Vec3_t &angles)						{ return Quat_t::Rotate2( Rad3_t{angles} ); }

		static Quat_t	LookAt   (const Quat_t&, const Vec3_t &dir, const Vec3_t &up)		{ return Quat_t::LookAt( dir, up ); }
		static Quat_t	From2Normals (const Quat_t&, const Vec3_t &n1, const Vec3_t &n2)	{ return Quat_t::From2Normals( n1, n2 ); }
		static Quat_t	FromAngleAxis (const Quat_t&, T angle, const Vec3_t &axis)			{ return Quat_t::FromAngleAxis( Rad_t{angle}, axis ); }
	};

/*
=================================================
	BindFloatQuat
=================================================
*/
	template <typename T>
	static void  BindFloatQuat (ClassBinder<T> &binder, const ScriptEnginePtr &)
	{
		StaticAssert( IsQuat<T> );

		using F = QuatFunc<T>;

		binder.Operators()
		//	.BinaryAssign(	EBinaryOperator::Add, &F::Add_am_s )
			.Binary(		EBinaryOperator::Add, &F::Add_q_q )

		//	.BinaryAssign(	EBinaryOperator::Sub, &F::Sub_am_s )
			.Binary(		EBinaryOperator::Sub, &F::Sub_q_q )

		//	.BinaryAssign(	EBinaryOperator::Mul, &F::Mul_am_s )
			.Binary(		EBinaryOperator::Mul, &F::Mul_q_q )
			.Binary(		EBinaryOperator::Mul, &F::Mul_q_v3 )
			.Binary(		EBinaryOperator::Mul, &F::Mul_q_v4 )
			.Binary(		EBinaryOperator::Mul, &F::Mul_q_s )

			.Binary(		EBinaryOperator::Div, &F::Div_q_s );

		binder.AddMethodFromGlobal( &F::RotateX,		"RotateX",			{"angle"} );
		binder.AddMethodFromGlobal( &F::RotateY,		"RotateY",			{"angle"} );
		binder.AddMethodFromGlobal( &F::RotateZ,		"RotateZ",			{"angle"} );

		binder.AddMethodFromGlobal( &F::Rotate,			"Rotate",			{"angle", "axis"} );
		binder.AddMethodFromGlobal( &F::Rotate1,		"Rotate",			{"angles"} );
		binder.AddMethodFromGlobal( &F::Rotate2,		"Rotate2",			{"angles"} );
		binder.AddMethodFromGlobal( &F::Rotate3,		"Rotate",			{"angleX", "angleY", "angleZ"} );

		binder.AddMethodFromGlobal( &F::LookAt,			"LookAt",			{"dir", "up"} );
		binder.AddMethodFromGlobal( &F::From2Normals,	"From2Normals",		{"norm1", "norm2"} );
		binder.AddMethodFromGlobal( &F::FromAngleAxis,	"FromAngleAxis",	{"angle", "axis"} );
	}

/*
=================================================
	BindQuat_Func
=================================================
*/
	struct BindQuat_Func
	{
		ScriptEnginePtr	_se;

		explicit BindQuat_Func (const ScriptEnginePtr &se) : _se{se}
		{}

		template <typename T, usize Index>
		void  operator () ()
		{
			using Value_t	= typename T::Value_t;
			using Quat_t	= T;
			using Vec4_t	= PackedVec< Value_t, 4 >;

			ClassBinder<Quat_t>		binder	{ _se };
			ClassBinder<Vec4_t>		binder2	{ _se };

			QuatCtor<Value_t>::Bind( binder, binder2 );

			BindFloatQuat( binder, _se );
		}
	};

} // namespace


/*
=================================================
	BindQuaternion
=================================================
*/
	void  CoreBindings::BindQuaternion (const ScriptEnginePtr &se) __Th___
	{
		CHECK_THROW( se and se->IsInitialized() );

		BindQuat_Func	func{ se };

		QuatTypes::VisitTh( func );
	}


} // AE::Scripting
