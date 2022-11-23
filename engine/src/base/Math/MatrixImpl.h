// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Math
{

	//
	// Matrix
	//
	
	template <typename T, glm::qualifier Q>
	struct TMatrix< T, Columns, Rows, Q > final
	{
	// types
	public:
		using Value_t		= T;
		using Self			= TMatrix< T, Columns, Rows, Q >;
		using _GLM_Mat_t	= glm::mat< glm::length_t(Columns), glm::length_t(Rows), T, Q >;
		using Col_t			= typename _GLM_Mat_t::col_type;	// [Rows]
		using Row_t			= typename _GLM_Mat_t::row_type;	// [Columns]
		using Dim_t			= _hidden_::_MatrixDim;


	// variables
	public:
		_GLM_Mat_t		_value;


	// methods
	public:
		TMatrix ()																		__NE___ = default;
		explicit TMatrix (const _GLM_Mat_t &mat)										__NE___ : _value{mat} {}
		
		TMatrix (const Self &other)														__NE___ = default;
		TMatrix (Self &&other)															__NE___ = default;

		template <uint Columns2, uint Rows2, glm::qualifier Q2>
		explicit TMatrix (const TMatrix<T, Columns2, Rows2, Q2> &other)					__NE___ : _value{other._value} {}

		template <typename B, glm::qualifier Q2>
		explicit TMatrix (const TMatrix<B, Columns, Rows, Q2> &other)					__NE___ : _value{other._value} {}

	#if Columns == 2
		TMatrix (const Col_t &col0,
				 const Col_t &col1)														__NE___ :
			_value{ col0, col1 } {}

		ND_ static Self  FromScalar (Value_t value)										__NE___	{ return Self{ Col_t{value}, Col_t{value} }; }
	#endif
		
	#if Columns == 3
		TMatrix (const Col_t &col0,
				 const Col_t &col1,
				 const Col_t &col2)														__NE___ :
			_value{ col0, col1, col2 } {}

		explicit TMatrix (const Quat<T> &q)												__NE___	: _value{glm::mat3_cast(q._value)} {}
		
		ND_ static Self  FromScalar (Value_t value)										__NE___	{ return Self{ Col_t{value}, Col_t{value}, Col_t{value} }; }
	#endif
		
	#if Columns == 4
		TMatrix (const Col_t &col0,
				 const Col_t &col1,
				 const Col_t &col2,
				 const Col_t &col3)														__NE___ :
			_value{ col0, col1, col2, col3 } {}

		explicit TMatrix (const Quat<T> &q)												__NE___	: _value{glm::mat4_cast(q._value)} {}
		
		ND_ static Self  FromScalar (Value_t value)										__NE___	{ return Self{ Col_t{value}, Col_t{value}, Col_t{value}, Col_t{value} }; }
	#endif
		
			Self&	Inverse ()															__NE___	{ _value = glm::inverse( _value );  return *this; }
		ND_ Self	Inversed ()															C_NE___	{ return Self{ glm::inverse( _value )}; }

		ND_ Self	operator + ()														C_NE___	{ return *this; }
		ND_ Self	operator - ()														C_NE___	{ return Self{ -_value }; }

			Self&	operator = (const Self &rhs)										__NE___ = default;
			Self&	operator = (Self && rhs)											__NE___ = default;

			Self&	operator += (T rhs)													C_NE___	{ _value += rhs;  return *this; }
			Self&	operator -= (T rhs)													C_NE___	{ _value -= rhs;  return *this; }
			Self&	operator *= (T rhs)													C_NE___	{ _value *= rhs;  return *this; }
			Self&	operator /= (T rhs)													C_NE___	{ _value /= rhs;  return *this; }

		ND_ Self	operator +  (T rhs)													C_NE___	{ return Self{ _value + rhs }; }
		ND_ Self	operator -  (T rhs)													C_NE___	{ return Self{ _value - rhs }; }
		ND_ Self	operator *  (T rhs)													C_NE___	{ return Self{ _value * rhs }; }
		ND_ Self	operator /  (T rhs)													C_NE___	{ return Self{ _value / rhs }; }

		ND_ Col_t	operator *  (const Row_t &vec)										C_NE___	{ return _value * vec; }


		ND_ friend Row_t				operator * (const Col_t &lhs, const Self &rhs)	__NE___	{ return lhs * rhs._value; }

		ND_ friend Self					operator * (T lhs, const Self &rhs)				__NE___	{ return Self{ lhs * rhs._value }; }
		ND_ friend Self					operator / (T lhs, const Self &rhs)				__NE___	{ return Self{ lhs / rhs._value }; }
		
		// return column
		ND_ Col_t const&				operator [] (usize c)							C_NE___	{ ASSERT( c < Columns );  return _value[ glm::length_t(c) ]; }
		ND_ Col_t&						operator [] (usize c)							__NE___	{ ASSERT( c < Columns );  return _value[ glm::length_t(c) ]; }

		// return scalar
		ND_ const T						operator () (usize c, usize r)					C_NE___	{ ASSERT( c < Columns and r < Rows );  return _value[c][r]; }
		ND_ T &							operator () (usize c, usize r)					__NE___	{ ASSERT( c < Columns and r < Rows );  return _value[c][r]; }

		// access to array
		ND_ const T						operator () (usize i)							C_NE___	{ ASSERT( i < ElementCount() );  return _value[ glm::length_t(i) / Rows ][ glm::length_t(i) % Rows ]; }
		ND_ T &							operator () (usize i)							__NE___	{ ASSERT( i < ElementCount() );  return _value[ glm::length_t(i) / Rows ][ glm::length_t(i) % Rows ]; }

		ND_ static Self					Identity ()										__NE___	{ return Self{ _GLM_Mat_t{ T{1} }}; }
		ND_ static Self					Zero ()											__NE___	{ return Self{ _GLM_Mat_t{ T{0} }}; }
		ND_ TMatrix<T,Rows,Columns,Q>	Transpose ()									C_NE___	{ return TMatrix<T,Rows,Columns,Q>{ glm::transpose( _value )}; }

		ND_ static constexpr usize		size ()											__NE___	{ return Columns; }
		ND_ static constexpr usize		ElementCount ()									__NE___	{ return Columns*Rows; }
		ND_ static constexpr Dim_t		Dimension ()									__NE___	{ return Dim_t{ Columns, Rows }; }
		
	#if Columns == 2 and Rows == 2
		ND_ static Self  Rotate (RadiansTempl<T> angle)									__NE___;
	#endif

	#if Columns == 3 and Rows == 3
		ND_ static Self  ToCubeFace (ubyte face)										__NE___;
		ND_ static Self  FromDirection (const Vec<T,3> &dir, const Vec<T,3> &up)		__NE___;
	#endif
		
	#if Columns == 4 and Rows == 4
		ND_ static Self  Ortho (const Rectangle<T> &viewport, const Vec<T,2> &range)						__NE___	{ return Self{ glm::ortho( viewport.left, viewport.right, viewport.bottom, viewport.top, range[0], range[1] )}; }
		ND_ static Self  InfinitePerspective (RadiansTempl<T> fovY, T aspect, T zNear)						__NE___	{ return Self{ glm::infinitePerspective( T(fovY), aspect, zNear )}; }
		ND_ static Self  Perspective (RadiansTempl<T> fovY, T aspect, const Vec<T,2> &range)				__NE___	{ return Self{ glm::perspective( T(fovY), aspect, range[0], range[1] )}; }
		ND_ static Self  Perspective (RadiansTempl<T> fovY, const Vec<T,2> &viewport, const Vec<T,2> &range)__NE___	{ return Self{ glm::perspectiveFov( T(fovY), viewport.x, viewport.y, range[0], range[1] )}; }
		ND_ static Self  Frustum (const Rectangle<T> &viewport, const Vec<T,2> &range)						__NE___	{ return Self{ glm::frustum( viewport.left, viewport.right, viewport.bottom, viewport.top, range[0], range[1] )}; }
		ND_ static Self  InfiniteFrustum (const Rectangle<T> &viewport, T zNear)							__NE___;
		ND_ static Self  Translate (const Vec<T,3> &translation)											__NE___	{ return Self{ glm::translate( Self::Identity()._value, translation )}; }
		ND_ static Self  Scale (const Vec<T,3> &scale)														__NE___	{ return Self{ glm::scale( Self::Identity()._value, scale )}; }
		ND_ static Self  Scale (const T scale)																__NE___	{ return Scale( Vec<T,3>{ scale }); }
	#endif
		
	#if Columns >= 3 and Rows >= 3
		ND_ static Self  RotateX (RadiansTempl<T> angle)								__NE___;
		ND_ static Self  RotateY (RadiansTempl<T> angle)								__NE___;
		ND_ static Self  RotateZ (RadiansTempl<T> angle)								__NE___;
	#endif

	private:
	  #if Rows == 3
		ND_ static Col_t  _CreateCol0 (T x, T y, T z)									__NE___	{ return Col_t{ x, y, z }; }
		ND_ static Col_t  _CreateCol1 (T x, T y, T z)									__NE___	{ return Col_t{ x, y, z }; }
	  #elif Rows == 4
		ND_ static Col_t  _CreateCol0 (T x, T y, T z)									__NE___	{ return Col_t{ x, y, z, T(0) }; }
		ND_ static Col_t  _CreateCol1 (T x, T y, T z)									__NE___	{ return Col_t{ x, y, z, T(1) }; }
	  #endif
	};

	
	
#if Columns == 2 and Rows == 2
/*
=================================================
	Rotate
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::Rotate (RadiansTempl<T> angle) __NE___
	{
		const T	s = Sin( angle );
		const T	c = Cos( angle );
		return Self{ Col_t{ c, s }, Col_t{ -s, c }};
	}
#endif

#if Columns == 3 and Rows == 3
/*
=================================================
	ToCubeFace
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::ToCubeFace (ubyte face) __NE___
	{
		ASSERT( face < 6 );

		const int	idx		 = face < 6 ? face : 5;		// pos: 0, 2, 4; neg: 1, 3, 5
		const int	norm	 = idx >> 1;				// norm: 0, 1, 2
		const bool	negative = idx & 1;
		Self		m		 = Zero();

		m( norm==0 ) = T{1};
		m( 6-norm  ) = T{1};
		m( norm+6  ) = negative ? T{-1} : T{1};
		
		return m.Transpose();
	}
	
/*
=================================================
	FromDirection
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::FromDirection (const Vec<T,3> &dir, const Vec<T,3> &up) __NE___
	{
		Vec<T,3>	hor = Normalize( Cross( up,  dir ));
		Vec<T,3>	ver = Normalize( Cross( dir, hor ));
		return Self{ hor, ver, dir };
	}
#endif

#if Columns >= 3 and Rows >= 3
/*
=================================================
	Rotate*
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::RotateX (RadiansTempl<T> angle) __NE___
	{
		const T	s = Sin( angle );
		const T	c = Cos( angle );

		return Self{
				_CreateCol0( T(1),  T(0),  T(0) ),
				_CreateCol0( T(0),   c,     s   ),
				_CreateCol0( T(0),  -s,     c   )
			#if Columns == 4
			  ,	_CreateCol1( T(0),  T(0),  T(0) )
			#endif
			};
	}
	
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::RotateY (RadiansTempl<T> angle) __NE___
	{
		const T	s = Sin( angle );
		const T	c = Cos( angle );

		return Self{
				_CreateCol0(  c,    T(0),  -s   ),
				_CreateCol0( T(0),  T(1),  T(0) ),
				_CreateCol0(  s,    T(0),   c   )
			#if Columns == 4
			  ,	_CreateCol1( T(0),  T(0),  T(0) )
			#endif
			};
	}
	
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::RotateZ (RadiansTempl<T> angle) __NE___
	{
		const T	s = Sin( angle );
		const T	c = Cos( angle );

		return Self{
				_CreateCol0(  c,    T(0),   s   ),
				_CreateCol0( -s,     c,    T(0) ),
				_CreateCol0( T(0),  T(0),  T(1) )
			#if Columns == 4
			  ,	_CreateCol1( T(0),  T(0),  T(0) )
			#endif
			};
	}
#endif
	
#if Columns == 4 and Rows == 4
/*
=================================================
	InfiniteFrustum
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::InfiniteFrustum (const Rectangle<T> &viewport, T zNear) __NE___
	{
		Self	proj;
		proj[0][0] = T(2) / (viewport.right - viewport.left);
		proj[1][1] = T(2) / (viewport.bottom - viewport.top);
		proj[2][0] = (viewport.right + viewport.left) / (viewport.right - viewport.left);
		proj[2][1] = (viewport.top + viewport.bottom) / (viewport.bottom - viewport.top);
		proj[2][2] = -T(1);
		proj[2][3] = -T(1);
		proj[3][2] = -T(2) * zNear;
		return proj;
	}
#endif

} // AE::Math
