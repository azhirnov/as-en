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
	TBindRect
=================================================
*/
	template <typename T>
	struct TBindRect
	{
	private:
		using Rect_t	= Rectangle<T>;
		using Vec2_t	= PackedVec<T,2>;

		static void  Rect_Ctor1 (void* mem, const Rect_t &value)
		{
			PlacementNew< Rect_t >( OUT mem, value );
		}

		static void  Rect_Ctor2 (void* mem, const Vec2_t &value)
		{
			PlacementNew< Rect_t >( OUT mem, value );
		}

		static void  Rect_Ctor3 (void* mem, const Vec2_t &leftTop, const Vec2_t &rightBottom)
		{
			PlacementNew< Rect_t >( OUT mem, leftTop, rightBottom );
		}

		static void  Rect_Ctor4 (void* mem, T sizeX, T sizeY)
		{
			PlacementNew< Rect_t >( OUT mem, Vec2_t{ sizeX, sizeY });
		}

		static void  Rect_Ctor5 (void* mem, T left, T top, T right, T bottom)
		{
			PlacementNew< Rect_t >( OUT mem, left, top, right, bottom );
		}

	public:
		static void  Bind (const ScriptEnginePtr &se)
		{
			ClassBinder<Rect_t>		binder{ se };

			binder.CreateClassValue();

			binder.AddProperty( &Rect_t::left,		"left"		);
			binder.AddProperty( &Rect_t::top,		"top"		);
			binder.AddProperty( &Rect_t::right,		"right"		);
			binder.AddProperty( &Rect_t::bottom,	"bottom"	);

			binder.AddConstructor( &Rect_Ctor1,	{"value"} );
			binder.AddConstructor( &Rect_Ctor2,	{"value"} );
			binder.AddConstructor( &Rect_Ctor3,	{"leftTop", "rightBottom"} );
			binder.AddConstructor( &Rect_Ctor4,	{"sizeX", "sizeY"} );
			binder.AddConstructor( &Rect_Ctor5,	{"left", "top", "right", "bottom"} );
		}
	};

/*
=================================================
	TRectFunc
=================================================
*/
	template <typename T>
	struct TRectFunc
	{
	private:
		using Rect_t	= Rectangle<T>;
		using Vec2_t	= PackedVec<T,2>;
		using V			= Vec<T,2>;

		static Rect_t&	Add_av (Rect_t &lhs, const Vec2_t &rhs)			{ return lhs += V{rhs}; }
		static Rect_t&	Add_as (Rect_t &lhs, T rhs)						{ return lhs += rhs; }
		static Rect_t	Add_v  (const Rect_t &lhs, const Vec2_t &rhs)	{ return lhs + V{rhs}; }
		static Rect_t	Add_s  (const Rect_t &lhs, T rhs)				{ return lhs + rhs; }

		static Rect_t&	Sub_av (Rect_t &lhs, const Vec2_t &rhs)			{ return lhs -= V{rhs}; }
		static Rect_t&	Sub_as (Rect_t &lhs, T rhs)						{ return lhs -= rhs; }
		static Rect_t	Sub_v  (const Rect_t &lhs, const Vec2_t &rhs)	{ return lhs - V{rhs}; }
		static Rect_t	Sub_s  (const Rect_t &lhs, T rhs)				{ return lhs - rhs; }

		static Rect_t&	Mul_av (Rect_t &lhs, const Vec2_t &rhs)			{ return lhs *= V{rhs}; }
		static Rect_t&	Mul_as (Rect_t &lhs, T rhs)						{ return lhs *= rhs; }
		static Rect_t	Mul_v  (const Rect_t &lhs, const Vec2_t &rhs)	{ return lhs * V{rhs}; }
		static Rect_t	Mul_s  (const Rect_t &lhs, T rhs)				{ return lhs * rhs; }

		static Rect_t&	Div_av (Rect_t &lhs, const Vec2_t &rhs)			{ return lhs /= V{rhs}; }
		static Rect_t&	Div_as (Rect_t &lhs, T rhs)						{ return lhs /= rhs; }
		static Rect_t	Div_v  (const Rect_t &lhs, const Vec2_t &rhs)	{ return lhs / V{rhs}; }
		static Rect_t	Div_s  (const Rect_t &lhs, T rhs)				{ return lhs / rhs; }

	protected:
		static void  _Bind (ClassBinder<Rect_t> &binder)
		{
			binder.Operators()
				.BinaryAssign(	EBinaryOperator::Add, &Add_av )
				.BinaryAssign(	EBinaryOperator::Add, &Add_as )
				.Binary(		EBinaryOperator::Add, &Add_v )
				.Binary(		EBinaryOperator::Add, &Add_s )

				.BinaryAssign(	EBinaryOperator::Sub, &Sub_av )
				.BinaryAssign(	EBinaryOperator::Sub, &Sub_as )
				.Binary(		EBinaryOperator::Sub, &Sub_v )
				.Binary(		EBinaryOperator::Sub, &Sub_s )

				.BinaryAssign(	EBinaryOperator::Mul, &Mul_av )
				.BinaryAssign(	EBinaryOperator::Mul, &Mul_as )
				.Binary(		EBinaryOperator::Mul, &Mul_v )
				.Binary(		EBinaryOperator::Mul, &Mul_s )

				.BinaryAssign(	EBinaryOperator::Div, &Div_av )
				.BinaryAssign(	EBinaryOperator::Div, &Div_as )
				.Binary(		EBinaryOperator::Div, &Div_v )
				.Binary(		EBinaryOperator::Div, &Div_s );
		}
	};

/*
=================================================
	TRectIntFunc
=================================================
*/
	template <typename T>
	struct TRectIntFunc : TRectFunc<T>
	{
	private:
		using Rect_t	= Rectangle<T>;
		using Vec2_t	= PackedVec<T,2>;


	public:
		static void  Bind (const ScriptEnginePtr &se)
		{
			ClassBinder<Rect_t>	binder{ se };
			TRectFunc<T>::_Bind( binder );
		}
	};

/*
=================================================
	TRectFloatFunc
=================================================
*/
	template <typename T>
	struct TRectFloatFunc : TRectFunc<T>
	{
	private:
		using Rect_t	= Rectangle<T>;
		using Vec2_t	= PackedVec<T,2>;

	public:
		static void  Bind (const ScriptEnginePtr &se)
		{
			ClassBinder<Rect_t>	binder{ se };
			TRectFunc<T>::_Bind( binder );
		}
	};

} // namespace


/*
=================================================
	BindRect
=================================================
*/
	void  CoreBindings::BindRect (const ScriptEnginePtr &se) __Th___
	{
		CHECK_THROW( se and se->IsInitialized() );

		// declare
		{
			TBindRect<int>::Bind( se );
			TBindRect<uint>::Bind( se );
			TBindRect<float>::Bind( se );
		}

		// bind
		{
			TRectIntFunc<int>::Bind( se );
			TRectIntFunc<uint>::Bind( se );

			TRectFloatFunc<float>::Bind( se );
		}
	}

} // AE::Scripting
