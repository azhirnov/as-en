// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Math
{

	//
	// Matrix
	//
	
	template <typename T>
	struct Matrix< T, Columns, Rows > final
	{
		STATIC_ASSERT( IsScalar<T> );
		STATIC_ASSERT( IsFloatPoint<T> );

	// types
	public:
		using Value_t		= T;
		using Self			= Matrix< T, Columns, Rows >;
		using _GLM_Mat_t	= glm::mat< glm::length_t(Columns), glm::length_t(Rows), T, GLMQuialifier >;
		using Col_t			= typename _GLM_Mat_t::col_type;	// [Rows]
		using Row_t			= typename _GLM_Mat_t::row_type;	// [Columns]
		using Dim_t			= _hidden_::_MatrixDim;


	// variables
	public:
		_GLM_Mat_t		_value;


	// methods
	public:
		GLM_CONSTEXPR Matrix () {}
		GLM_CONSTEXPR explicit Matrix (const _GLM_Mat_t &mat) : _value{mat} {}
		
		GLM_CONSTEXPR Matrix (const Self &other) : _value{other._value} {}
		Matrix (Self &&other) : _value{other._value} {}

		template <uint Columns2, uint Rows2>
		GLM_CONSTEXPR explicit Matrix (const Matrix<T, Columns2, Rows2> &other) : _value{other._value} {}

		template <typename B>
		GLM_CONSTEXPR explicit Matrix (const Matrix<B, Columns, Rows> &other) : _value{other._value} {}

	#if Columns == 2
		GLM_CONSTEXPR Matrix (const Col_t &col0,
							  const Col_t &col1) :
			_value{ col0, col1 } {}
	#endif
		
	#if Columns == 3
		GLM_CONSTEXPR Matrix (const Col_t &col0,
							  const Col_t &col1,
							  const Col_t &col2) :
			_value{ col0, col1, col2 } {}

		explicit Matrix (const Quat<T> &q) : _value{glm::mat3_cast(q._value)} {}
	#endif
		
	#if Columns == 4
		GLM_CONSTEXPR Matrix (const Col_t &col0,
							  const Col_t &col1,
							  const Col_t &col2,
							  const Col_t &col3) :
			_value{ col0, col1, col2, col3 } {}

		explicit Matrix (const Quat<T> &q) : _value{glm::mat4_cast(q._value)} {}
	#endif
		
			Self&	Inverse ()								{ _value = glm::inverse( _value );  return *this; }
		ND_ Self	Inversed ()						const	{ return Self{ glm::inverse( _value )}; }

		ND_ Self	operator + ()					const	{ return *this; }
		ND_ Self	operator - ()					const	{ return Self{ -_value }; }

			Self&	operator = (const Self &rhs)			{ _value = rhs._value;  return *this; }

			Self&	operator += (T rhs)				const	{ _value += rhs;  return *this; }
			Self&	operator -= (T rhs)				const	{ _value -= rhs;  return *this; }
			Self&	operator *= (T rhs)				const	{ _value *= rhs;  return *this; }
			Self&	operator /= (T rhs)				const	{ _value /= rhs;  return *this; }

		ND_ Self	operator +  (T rhs)				const	{ return Self{ _value + rhs }; }
		ND_ Self	operator -  (T rhs)				const	{ return Self{ _value - rhs }; }
		ND_ Self	operator *  (T rhs)				const	{ return Self{ _value * rhs }; }
		ND_ Self	operator /  (T rhs)				const	{ return Self{ _value / rhs }; }

		ND_ Col_t	operator *  (const Row_t &vec)	const	{ return _value * vec; }


		ND_ friend Row_t				operator * (const Col_t &lhs, const Self &rhs)	{ return lhs * rhs._value; }

		ND_ friend Self					operator * (T lhs, const Self &rhs)		{ return Self{ lhs * rhs._value }; }
		ND_ friend Self					operator / (T lhs, const Self &rhs)		{ return Self{ lhs / rhs._value }; }
		
		ND_ GLM_CONSTEXPR Col_t const&	operator [] (usize i) const				{ return _value[ glm::length_t(i) ]; }
		ND_ GLM_CONSTEXPR Col_t&		operator [] (usize i)					{ return _value[ glm::length_t(i) ]; }

		ND_ const T						operator () (usize i) const				{ return _value[ glm::length_t(i) / Rows ][ glm::length_t(i) % Rows ]; }
		ND_ T &							operator () (usize i)					{ return _value[ glm::length_t(i) / Rows ][ glm::length_t(i) % Rows ]; }

		ND_ static GLM_CONSTEXPR Self	Identity ()								{ return Self{ _GLM_Mat_t{ T{1} }}; }
		ND_ static GLM_CONSTEXPR Self	Zero ()									{ return Self{ _GLM_Mat_t{ T{0} }}; }
		ND_ Matrix<T,Rows,Columns>		Transpose () const						{ return Matrix<T,Rows,Columns>{ glm::transpose( _value )}; }

		ND_ static constexpr usize		size ()									{ return Columns; }
		ND_ static constexpr Dim_t		Dimension ()							{ return Dim_t{ Columns, Rows }; }
		
	#if Columns == 2 and Rows == 2
		ND_ static Self  Rotate (RadiansTempl<T> angle);
	#endif

	#if Columns == 3 and Rows == 3
		ND_ static Self  ToCubeFace (ubyte face);
		ND_ static Self  FromDirection (const Vec<T,3> &dir, const Vec<T,3> &up);
	#endif
		
	#if Columns == 4 and Rows == 4
		ND_ static Self  Ortho (const Rectangle<T> &viewport, const Vec<T,2> &range)			{ return Self{ glm::ortho( viewport.left, viewport.right, viewport.bottom, viewport.top, range[0], range[1] )}; }
		ND_ static Self  InfinitePerspective (RadiansTempl<T> fovY, T aspect, T zNear)			{ return Self{ glm::infinitePerspective( T(fovY), aspect, zNear )}; }
		ND_ static Self  Perspective (RadiansTempl<T> fovY, T aspect, const Vec<T,2> &range)	{ return Self{ glm::perspective( T(fovY), aspect, range[0], range[1] )}; }
		ND_ static Self  Perspective (RadiansTempl<T> fovY, const Vec<T,2> &viewport, const Vec<T,2> &range)	{ return Self{ glm::perspectiveFov( T(fovY), viewport.x, viewport.y, range[0], range[1] )}; }
		ND_ static Self  Frustum (const Rectangle<T> &viewport, const Vec<T,2> &range)			{ return Self{ glm::frustum( viewport.left, viewport.right, viewport.bottom, viewport.top, range[0], range[1] )}; }
		ND_ static Self  InfiniteFrustum (const Rectangle<T> &viewport, T zNear);
		ND_ static Self  Translate (const Vec<T,3> &translation)								{ return Self{ glm::translate( Self::Identity()._value, translation )}; }
		ND_ static Self  Scale (const Vec<T,3> &scale)											{ return Self{ glm::scale( Self::Identity()._value, scale )}; }
		ND_ static Self  Scale (const T scale)													{ return Scale( Vec<T,3>{ scale }); }
	#endif
		
	#if Columns >= 3 and Rows >= 3
		ND_ static Self  RotateX (RadiansTempl<T> angle);
		ND_ static Self  RotateY (RadiansTempl<T> angle);
		ND_ static Self  RotateZ (RadiansTempl<T> angle);
	#endif

	private:
	  #if Rows == 3
		ND_ static Col_t  _CreateCol0 (T x, T y, T z)		{ return Col_t{ x, y, z }; }
		ND_ static Col_t  _CreateCol1 (T x, T y, T z)		{ return Col_t{ x, y, z }; }
	  #elif Rows == 4
		ND_ static Col_t  _CreateCol0 (T x, T y, T z)		{ return Col_t{ x, y, z, T(0) }; }
		ND_ static Col_t  _CreateCol1 (T x, T y, T z)		{ return Col_t{ x, y, z, T(1) }; }
	  #endif
	};

	
	
#if Columns == 2 and Rows == 2
/*
=================================================
	Rotate
=================================================
*/
	template <typename T>
	Matrix<T, Columns, Rows>  Matrix<T, Columns, Rows>::Rotate (RadiansTempl<T> angle)
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
	template <typename T>
	Matrix<T, Columns, Rows>  Matrix<T, Columns, Rows>::ToCubeFace (ubyte face)
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
	template <typename T>
	Matrix<T, Columns, Rows>  Matrix<T, Columns, Rows>::FromDirection (const Vec<T,3> &dir, const Vec<T,3> &up)
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
	template <typename T>
	Matrix<T, Columns, Rows>  Matrix<T, Columns, Rows>::RotateX (RadiansTempl<T> angle)
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
	
	template <typename T>
	Matrix<T, Columns, Rows>  Matrix<T, Columns, Rows>::RotateY (RadiansTempl<T> angle)
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
	
	template <typename T>
	Matrix<T, Columns, Rows>  Matrix<T, Columns, Rows>::RotateZ (RadiansTempl<T> angle)
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
	template <typename T>
	Matrix<T, Columns, Rows>  Matrix<T, Columns, Rows>::InfiniteFrustum (const Rectangle<T> &viewport, T zNear)
	{
		Matrix<T, 4, 4> proj;
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
