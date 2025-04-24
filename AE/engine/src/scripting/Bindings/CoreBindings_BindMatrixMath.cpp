// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/ScriptEngine.inl.h"

namespace AE::Scripting
{
namespace
{
	template <typename T>
	struct MatCtor;

/*
=================================================
	MatCtor (mat2x*)
=================================================
*/
	template <typename T, int Rows>
	struct MatCtor2
	{
	private:
		using Mat_t	= PackedMatrix<T,2,Rows>;
		using Col_t	= typename Mat_t::Col_t;

		static void  _Ctor1 (void* mem)
		{
			PlacementNew< Mat_t >( OUT mem, Mat_t::Identity() );
		}

		static void  _Ctor2 (void* mem, const Mat_t &value)
		{
			PlacementNew< Mat_t >( OUT mem, value );
		}

		static void  _Ctor3 (void* mem, const Col_t &col0, const Col_t &col1)
		{
			PlacementNew< Mat_t >( OUT mem, col0, col1 );
		}


	public:
		static void  Bind (ClassBinder<Mat_t> &binder)
		{
		//	binder.AddConstructor( &_Ctor1 );
			binder.AddConstructor( &_Ctor2, {"value"} );
			binder.AddConstructor( &_Ctor3,	{"col0", "col1"} );

			Mat_t	temp;
			binder.AddProperty( temp, temp.template get<0>(), "col0" );
			binder.AddProperty( temp, temp.template get<1>(), "col1" );
		}
	};

	template <>	struct MatCtor< packed_float2x2 > : MatCtor2< float, 2 > {};
	template <>	struct MatCtor< packed_float2x3 > : MatCtor2< float, 3 > {};
	template <>	struct MatCtor< packed_float2x4 > : MatCtor2< float, 4 > {};

/*
=================================================
	MatCtor3 (mat3x*)
=================================================
*/
	template <typename T, int Rows>
	struct MatCtor3
	{
	private:
		using Mat_t	= PackedMatrix<T,3,Rows>;
		using Col_t	= typename Mat_t::Col_t;

		static void  _Ctor1 (void* mem)
		{
			PlacementNew< Mat_t >( OUT mem, Mat_t::Identity() );
		}

		static void  _Ctor2 (void* mem, const Mat_t &value)
		{
			PlacementNew< Mat_t >( OUT mem, value );
		}

		static void  _Ctor3 (void* mem, const Col_t &col0, const Col_t &col1, const Col_t &col2)
		{
			PlacementNew< Mat_t >( OUT mem, col0, col1, col2 );
		}


	public:
		static void  Bind (ClassBinder<Mat_t> &binder)
		{
		//	binder.AddConstructor( &_Ctor1 );
			binder.AddConstructor( &_Ctor2,	{"value"} );
			binder.AddConstructor( &_Ctor3,	{"col0", "col1", "col2"} );

			Mat_t	temp;
			binder.AddProperty( temp, temp.template get<0>(), "col0" );
			binder.AddProperty( temp, temp.template get<1>(), "col1" );
			binder.AddProperty( temp, temp.template get<2>(), "col2" );
		}
	};

	template <>	struct MatCtor< packed_float3x2 > : MatCtor3< float, 2 > {};
	template <>	struct MatCtor< packed_float3x3 > : MatCtor3< float, 3 > {};
	template <>	struct MatCtor< packed_float3x4 > : MatCtor3< float, 4 > {};

/*
=================================================
	MatCtor4 (mat4x*)
=================================================
*/
	template <typename T, int Rows>
	struct MatCtor4
	{
	private:
		using Mat_t	= PackedMatrix<T,4,Rows>;
		using Col_t	= typename Mat_t::Col_t;

		static void  _Ctor1 (void* mem)
		{
			PlacementNew< Mat_t >( OUT mem, Mat_t::Identity() );
		}

		static void  _Ctor2 (void* mem, const Mat_t &value)
		{
			PlacementNew< Mat_t >( OUT mem, value );
		}

		static void  _Ctor3 (void* mem, const Col_t &col0, const Col_t &col1, const Col_t &col2, const Col_t &col3)
		{
			PlacementNew< Mat_t >( OUT mem, col0, col1, col2, col3 );
		}


	public:
		static void  Bind (ClassBinder<Mat_t> &binder)
		{
		//	binder.AddConstructor( &_Ctor1 );
			binder.AddConstructor( &_Ctor2,	{"value"} );
			binder.AddConstructor( &_Ctor3,	{"col0", "col1", "col2", "col3"} );

			Mat_t	temp;
			binder.AddProperty( temp, temp.template get<0>(), "col0" );
			binder.AddProperty( temp, temp.template get<1>(), "col1" );
			binder.AddProperty( temp, temp.template get<2>(), "col2" );
			binder.AddProperty( temp, temp.template get<3>(), "col3" );
		}
	};

	template <>	struct MatCtor< packed_float4x2 > : MatCtor4< float, 2 > {};
	template <>	struct MatCtor< packed_float4x3 > : MatCtor4< float, 3 > {};
	template <>	struct MatCtor< packed_float4x4 > : MatCtor4< float, 4 > {};

/*
=================================================
	MatFunc
=================================================
*/
	template <typename T>
	struct MatFunc
	{
		using Mat_t		= T;
		using Col_t		= typename T::Col_t;
		using Row_t		= typename T::Row_t;
		using Scalar_t	= typename T::Value_t;
		using Rect_t	= typename T::Rect_t;
		using Vec2_t	= typename T::Vec2_t;
		using Vec3_t	= typename T::Vec3_t;


		static Mat_t	Add_m_s (const Mat_t& lhs, Scalar_t rhs)					{ return lhs + rhs; }
		static Mat_t	Add_s_m (Scalar_t lhs, const Mat_t& rhs)					{ return rhs + lhs; }

		static Mat_t	Sub_m_s (const Mat_t& lhs, Scalar_t rhs)					{ return lhs - rhs; }

		static Mat_t	Mul_m_s (const Mat_t& lhs, Scalar_t rhs)					{ return lhs * rhs; }
		static Mat_t	Mul_s_m (Scalar_t lhs, const Mat_t& rhs)					{ return lhs * rhs; }
		static Col_t	Mul_m_v (const Mat_t& lhs, const Row_t &rhs)				{ return lhs * rhs; }
		static Row_t	Mul_v_m (const Col_t& lhs, const Mat_t &rhs)				{ return lhs * rhs; }

		static Mat_t	Div_m_s (const Mat_t& lhs, Scalar_t rhs)					{ return lhs / rhs; }

		static Mat_t	Rotate  (const Mat_t&, Scalar_t angle, const Vec3_t &axis)	{ return Mat_t::Rotate( Rad{angle}, axis ); }
		static Mat_t	RotateX (const Mat_t&, Scalar_t angle)						{ return Mat_t::RotateX( Rad{angle} ); }
		static Mat_t	RotateY (const Mat_t&, Scalar_t angle)						{ return Mat_t::RotateY( Rad{angle} ); }
		static Mat_t	RotateZ (const Mat_t&, Scalar_t angle)						{ return Mat_t::RotateZ( Rad{angle} ); }

		static Mat_t	Ortho (const Mat_t&, const Rect_t &viewport, const Vec2_t &range)						{ return Mat_t::Ortho( viewport, range ); }
		static Mat_t	InfinitePerspective (const Mat_t&, Scalar_t fovY, Scalar_t aspectRatio, Scalar_t zNear)	{ return Mat_t::InfinitePerspective( Rad{fovY}, aspectRatio, zNear ); }
		static Mat_t	Perspective1 (const Mat_t&, Scalar_t fovY, Scalar_t aspectRatio, const Vec2_t &range)	{ return Mat_t::Perspective( Rad{fovY}, aspectRatio, range ); }
		static Mat_t	Perspective2 (const Mat_t&, Scalar_t fovY, const Vec2_t &viewport, const Vec2_t &range)	{return Mat_t::Perspective( Rad{fovY}, viewport, range ); }
		static Mat_t	Frustum (const Mat_t&, const Rect_t &viewport, const Vec2_t &range)						{ return Mat_t::Frustum( viewport, range ); }
		static Mat_t	InfiniteFrustum (const Mat_t&, const Rect_t &viewport, Scalar_t zNear)					{ return Mat_t::InfiniteFrustum( viewport, zNear ); }
		static Mat_t	Translate (const Mat_t& m, const Vec3_t &translation)									{ return m.Translate( translation ); }
		static Mat_t	Scale1 (const Mat_t&, const Vec3_t &scale)												{ return Mat_t::Scale( scale ); }
		static Mat_t	Scale2 (const Mat_t&, const Scalar_t scale)												{ return Mat_t::Scale( scale ); }

		static Mat_t	LookAt (const Mat_t &, const Vec3_t &eye, const Vec3_t &center, const Vec3_t &up)		{ return Mat_t::LookAt( eye, center, up ); }

		static Vec3_t	Project (const Mat_t &self, const Vec3_t &pos, const Rect_t &viewport)					{ return self.Project( pos, viewport ); }
		static Vec3_t	UnProject (const Mat_t &self, const Vec3_t &pos, const Rect_t &viewport)				{ return self.UnProject( pos, viewport ); }

		template <uint C1, uint R2>
		static auto		Mul_m_m (const Mat_t& lhs,
								 const TMatrix<Scalar_t, C1, R2, GLMPackedQualifier> &rhs)	{ return lhs * rhs; }
	};

/*
=================================================
	BindFloatMat
=================================================
*/
	template <typename T>
	static void  BindFloatMat (ClassBinder<T> &binder, const ScriptEnginePtr &)
	{
		using F = MatFunc<T>;

		constexpr uint	R = T::Dimension().columns;

	//	AS_OP_BIN_ASSIGN(	binder, EBinaryOperator::Add, F::Add_am_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Add, F::Add_m_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Add, F::Add_s_m );

	//	AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Sub, F::Sub_am_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Sub, F::Sub_m_s );

	//	AS_OP_BIN_ASSIGN_T(	binder, EBinaryOperator::Mul, F::Mul_am_s );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Mul, F::Mul_m_s );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Mul, F::Mul_s_m );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Mul, F::Mul_m_v );
		AS_OP_BINARY_RH_T(	binder, EBinaryOperator::Mul, F::Mul_v_m );

		AS_OP_BINARY_T(		binder, EBinaryOperator::Mul, (F::template Mul_m_m<2,R>) );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Mul, (F::template Mul_m_m<3,R>) );
		AS_OP_BINARY_T(		binder, EBinaryOperator::Mul, (F::template Mul_m_m<4,R>) );

		AS_OP_BINARY_T(		binder, EBinaryOperator::Div, F::Div_m_s );

		if constexpr( IsSame< T, packed_float3x3 > or
					  IsSame< T, packed_float3x4 > or
					  IsSame< T, packed_float4x3 > or
					  IsSame< T, packed_float4x4 >)
		{
			AS_METHOD_T( binder, F::RotateX,			"RotateX",			{"angle"} );
			AS_METHOD_T( binder, F::RotateY,			"RotateY",			{"angle"} );
			AS_METHOD_T( binder, F::RotateZ,			"RotateZ",			{"angle"} );
		}

		if constexpr( IsSame< T, packed_float4x4 >)
		{
			AS_METHOD_T( binder, F::Rotate,				"Rotate",			{"angle", "axis"} );
			AS_METHOD_T( binder, F::Ortho,				"Ortho",			{"viewport", "range"} );
			AS_METHOD_T( binder, F::InfinitePerspective,"InfinitePerspective", {"fovY", "aspectRatio", "zNear"} );
			AS_METHOD_T( binder, F::Perspective1,		"Perspective",		{"fovY", "aspectRatio", "range"} );
			AS_METHOD_T( binder, F::Perspective2,		"Perspective",		{"fovY", "viewport", "range"} );
			AS_METHOD_T( binder, F::Frustum,			"Frustum",			{"viewport", "range"} );
			AS_METHOD_T( binder, F::InfiniteFrustum,	"InfiniteFrustum",	{"viewport", "zNear"} );
			AS_METHOD_T( binder, F::Translate,			"Translate",		{"translation"} );
			AS_METHOD_T( binder, F::Scale1,				"Scale",			{"scale"} );
			AS_METHOD_T( binder, F::Scale2,				"Scale",			{"scale"} );
			AS_METHOD_T( binder, F::Project,			"Project",			{"pos", "viewport"} );
			AS_METHOD_T( binder, F::UnProject,			"UnProject",		{"pos", "viewport"} );
			AS_METHOD_T( binder, F::LookAt,				"LookAt",			{"eye", "center", "up"} );
		}
	}

/*
=================================================
	DefineMat_Func
=================================================
*/
	struct DefineMat_Func
	{
		ScriptEnginePtr	_se;

		explicit DefineMat_Func (const ScriptEnginePtr &se) : _se{se}
		{}

		template <typename T, usize Index>
		void  operator () ()
		{
			ClassBinder<T>	binder{ _se };

			binder.CreateClassValue();
		}
	};

/*
=================================================
	BindMat_Func
=================================================
*/
	struct BindMat_Func
	{
		ScriptEnginePtr	_se;

		explicit BindMat_Func (const ScriptEnginePtr &se) : _se{se}
		{}

		template <typename T, usize Index>
		void  operator () ()
		{
			using Value_t = typename T::Value_t;

			ClassBinder<T>	binder{ _se };

			MatCtor<T>::Bind( binder );

			BindFloatMat( binder, _se );
		}
	};

} // namespace


/*
=================================================
	BindMatrixMath
=================================================
*/
	void  CoreBindings::BindMatrixMath (const ScriptEnginePtr &se) __Th___
	{
		CHECK_THROW( se and se->IsInitialized() );

		// declare
		{
			DefineMat_Func	func{ se };

			MatTypes::VisitTh( func );
		}

		// bind
		{
			BindMat_Func	func{ se };

			MatTypes::VisitTh( func );
		}
	}


} // AE::Scripting
