// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Math
{

	//
	// Matrix
	//

	template <typename T, glm::qualifier Q>
	struct TMatrix< T, Columns, Rows, Q > final
	{
		friend struct TQuat< T, Q >;

	// types
	public:
		using Value_t		= T;
		using Self			= TMatrix< T, Columns, Rows, Q >;
		using _GLM_Mat_t	= glm::mat< glm::length_t(Columns), glm::length_t(Rows), T, Q >;
		using Col_t			= typename _GLM_Mat_t::col_type;	// [Rows]
		using Row_t			= typename _GLM_Mat_t::row_type;	// [Columns]
		using Dim_t			= Math::_hidden_::_MatrixDim;
		using Rect_t		= Rectangle<T>;
		using Vec2_t		= TVec< T, 2, Q >;
		using Vec3_t		= TVec< T, 3, Q >;
		using Vec4_t		= TVec< T, 4, Q >;
		using Rad_t			= TRadian<T>;
		using Diag_t		= TVec< T, Min( Columns, Rows ), Q >;


	// variables
	public:
		_GLM_Mat_t		_value;


	// methods
	public:
		TMatrix ()																		__NE___ = default;
		explicit TMatrix (const _GLM_Mat_t &mat)										__NE___ : _value{mat} {}

		TMatrix (const Self &other)														__NE___ = default;
		TMatrix (Self &&other)															__NE___ = default;

		template <typename B, uint Columns2, uint Rows2, glm::qualifier Q2>
		explicit TMatrix (const TMatrix<B, Columns2, Rows2, Q2> &other)					__NE___ : _value{other._value} {}

		explicit TMatrix (const Diag_t &diagonal)										__NE___;

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

		explicit TMatrix (const TQuat<T,Q> &q)											__NE___	: _value{glm::mat3_cast(q._Base())} {}

		ND_ static Self  FromScalar (Value_t value)										__NE___	{ return Self{ Col_t{value}, Col_t{value}, Col_t{value} }; }
	#endif

	#if Columns == 4
		TMatrix (const Col_t &col0,
				 const Col_t &col1,
				 const Col_t &col2,
				 const Col_t &col3)														__NE___ :
			_value{ col0, col1, col2, col3 } {}

		explicit TMatrix (const TQuat<T,Q> &q)											__NE___	: _value{glm::mat4_cast(q._Base())} {}

		ND_ static Self  FromScalar (Value_t value)										__NE___	{ return Self{ Col_t{value}, Col_t{value}, Col_t{value}, Col_t{value} }; }
	#endif

			Self&	Inverse ()															__NE___	{ _value = glm::inverse( _value );  return *this; }
		ND_ Self	Inversed ()															C_NE___	{ return Self{ glm::inverse( _value )}; }

		ND_ Self	operator + ()														C_NE___	{ return *this; }
		ND_ Self	operator - ()														C_NE___	{ return Self{ -_value }; }

			Self&	operator = (const Self &rhs)										__NE___ = default;
			Self&	operator = (Self &&rhs)												__NE___ = default;

		ND_ Self	operator +  (T rhs)													C_NE___	{ return Self{ _value + rhs }; }
		ND_ Self	operator -  (T rhs)													C_NE___	{ return Self{ _value - rhs }; }
		ND_ Self	operator *  (T rhs)													C_NE___	{ return Self{ _value * rhs }; }
		ND_ Self	operator /  (T rhs)													C_NE___	{ return Self{ _value / rhs }; }

		ND_ Col_t	operator *  (const Row_t &vec)										C_NE___	{ return _value * vec; }

		ND_ bool	operator == (const Self &rhs)										C_NE___;

		ND_ bool	IsIdentity ()														C_NE___	{ return *this == Identity(); }

		ND_ T		Determinant ()														C_NE___	{ return glm::determinant( _value ); }


		ND_ friend Row_t			operator * (const Col_t &lhs, const Self &rhs)		__NE___	{ return lhs * rhs._value; }

		ND_ friend Self				operator * (T lhs, const Self &rhs)					__NE___	{ return Self{ lhs * rhs._value }; }
		ND_ friend Self				operator / (T lhs, const Self &rhs)					__NE___	{ return Self{ lhs / rhs._value }; }

		template <uint K>
			Self &					operator *= (const TMatrix<T,K,Columns,Q> &rhs)		__NE___	{ _value *= rhs._value;  return *this; }

		template <uint K>
		ND_ TMatrix<T,K,Rows,Q>		operator *  (const TMatrix<T,K,Columns,Q> &rhs)		C_NE___	{ return TMatrix<T,K,Rows,Q>{ _value * rhs._value }; }

		// return column
		ND_ Col_t const&			operator [] (usize c)								C_NE___	{ ASSERT( c < Columns );  return _value[ glm::length_t(c) ]; }
		ND_ Col_t&					operator [] (usize c)								__NE___	{ ASSERT( c < Columns );  return _value[ glm::length_t(c) ]; }

		template <uint C>			ND_ Col_t const&	get ()							C_NE___	{ StaticAssert( C < Columns );  return _value[C]; }
		template <uint C>			ND_ Col_t &			get ()							__NE___	{ StaticAssert( C < Columns );  return _value[C]; }


		// return scalar
		ND_ const T					operator () (usize c, usize r)						C_NE___	{ ASSERT( c < Columns and r < Rows );  return _value[glm::length_t(c)][glm::length_t(r)]; }
		ND_ T &						operator () (usize c, usize r)						__NE___	{ ASSERT( c < Columns and r < Rows );  return _value[glm::length_t(c)][glm::length_t(r)]; }

		template <uint C, uint R>	ND_ const T	get ()									C_NE___	{ StaticAssert( C < Columns and R < Rows );  return _value[C][R]; }
		template <uint C, uint R>	ND_ T &		get ()									__NE___	{ StaticAssert( C < Columns and R < Rows );  return _value[C][R]; }


		// access to array
		ND_ const T						operator () (usize i)							C_NE___	{ ASSERT( i < ElementCount() );  return _value[ glm::length_t(i) / Rows ][ glm::length_t(i) % Rows ]; }
		ND_ T &							operator () (usize i)							__NE___	{ ASSERT( i < ElementCount() );  return _value[ glm::length_t(i) / Rows ][ glm::length_t(i) % Rows ]; }

		ND_ static Self					Identity ()										__NE___	{ return Self{ _GLM_Mat_t{ T{1} }}; }
		ND_ static Self					Zero ()											__NE___	{ return Self{ _GLM_Mat_t{ T{0} }}; }
		ND_ TMatrix<T,Rows,Columns,Q>	Transpose ()									C_NE___	{ return TMatrix<T,Rows,Columns,Q>{ glm::transpose( _value )}; }

		ND_ static constexpr usize		size ()											__NE___	{ return Columns; }
		ND_ static constexpr usize		ElementCount ()									__NE___	{ return Columns*Rows; }
		ND_ static constexpr Dim_t		Dimension ()									__NE___	{ return Dim_t{ Columns, Rows }; }
		ND_ static constexpr bool		IsColumnMajor ()								__NE___	{ return true; }


	#if Columns == 2 and Rows == 2
		ND_ static Self  Rotate (Rad_t angle)											__NE___;
		ND_ static Self  Scaled (const Vec2_t &scale)									__NE___;
		ND_ static Self  Scaled (const T scale)											__NE___	{ return Scaled( Vec2_t{ scale }); }
	#endif

	#if Columns == 3 and Rows == 3
		ND_ static Self  FromDirection (const Vec3_t &dir, const Vec3_t &up)			__NE___;
		ND_ static Self  Scaled (const Vec3_t &scale)									__NE___;
		ND_ static Self  Scaled (const T scale)											__NE___	{ return Scaled( Vec3_t{ scale }); }
		ND_ static Self  LookAt (const Vec3_t &dir, const Vec3_t &up)					__NE___;
	#endif

	#if Columns == 4 and (Rows == 3 or Rows == 4)
		ND_ static Self	 Translated (const Vec3_t &translation)							__NE___	{ return Identity().SetTranslation( translation ); }
		ND_ Self		 Translate (const Vec3_t &translation)							C_NE___	{ return Self{*this}.AddTranslation( translation ); }
			Self&		 SetTranslation (const Vec3_t &translation)						__NE___;
			Self&		 AddTranslation (const Vec3_t &translation)						__NE___;
	#endif

	#if Columns == 4 and Rows == 4
		ND_ static Self  Ortho (const Rect_t &viewport, const Vec2_t &range)			__NE___	{ return Self{ glm::ortho( viewport.left, viewport.right, viewport.top, viewport.bottom, range[0], range[1] )}; }
		ND_ static Self  InfinitePerspective (Rad_t fovY, T aspect, T zNear)			__NE___;
		ND_ static Self  Perspective (Rad_t fovY, T aspect, const Vec2_t &range)		__NE___	{ return Self{ glm::perspective( T(fovY), aspect, range[0], range[1] )}; }
		ND_ static Self  Perspective (Rad_t fovY, const Vec2_t &viewport, const Vec2_t &range)	__NE___	{ return Self{ glm::perspectiveFov( T(fovY), viewport.x, viewport.y, range[0], range[1] )}; }
		ND_ static Self  Frustum (const Rect_t &viewport, const Vec2_t &range)			__NE___	{ return Self{ glm::frustum( viewport.left, viewport.right, viewport.top, viewport.bottom, range[0], range[1] )}; }
		ND_ static Self  InfiniteFrustum (const Rect_t &viewport, T zNear)				__NE___;

		ND_ static Self  LookAt (const Vec3_t &eye, const Vec3_t &center, const Vec3_t &up)	__NE___	{ return Self{ glm::lookAt( eye, center, up )}; }

		ND_ static Self  Scaled (const Vec3_t &scale)									__NE___;
		ND_ static Self  Scaled (const T scale)											__NE___	{ return Scaled( Vec3_t{ scale }); }

		ND_ Vec3_t		 Project (const Vec3_t &pos, const Rect_t &viewport)			C_NE___;
		ND_ Vec3_t		 UnProject (const Vec3_t &pos, const Rect_t &viewport)			C_NE___;

		ND_ static Self  Rotate  (Rad_t angle, const Vec3_t &axis)						__NE___;

		ND_ static Self  ReverseZTransform ()											__NE___;
	#endif

	#if Columns >= 3 and Rows >= 3
		ND_ static Self  RotateX (Rad_t angle)											__NE___;	// pitch
		ND_ static Self  RotateY (Rad_t angle)											__NE___;	// yaw
		ND_ static Self  RotateZ (Rad_t angle)											__NE___;	// roll

		ND_ Vec3_t		 AxisX ()														C_NE___	{ return Vec3_t{ _value[0][0], _value[1][0], _value[2][0] }; }	// right
		ND_ Vec3_t		 AxisY ()														C_NE___	{ return Vec3_t{ _value[0][1], _value[1][1], _value[2][1] }; }	// up
		ND_ Vec3_t		 AxisZ ()														C_NE___	{ return Vec3_t{ _value[0][2], _value[1][2], _value[2][2] }; }	// forward

		ND_ static Self  ToCubeFace (uint face)											__NE___;
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



/*
=================================================
	constructor
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>::TMatrix (const Diag_t &diagonal) __NE___
	{
		for (glm::length_t c = 0; c < Columns; ++c)
		for (glm::length_t r = 0; r < Rows; ++r)
			_value[c][r] = (c == r ? diagonal[c] : T{0});
	}


#if Columns == 2 and Rows == 2
/*
=================================================
	Rotate
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::Rotate (Rad_t angle) __NE___
	{
		const T	s = Sin( angle );
		const T	c = Cos( angle );
		return Self{ Col_t{ c, s }, Col_t{ -s, c }};
	}

/*
=================================================
	Scaled
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::Scaled (const Vec2_t &scale) __NE___
	{
		return	Self{	Col_t{ scale.x, T(0),   },
						Col_t{ T(0),    scale.y }};
	}
#endif


#if Columns == 3 and Rows == 3
/*
=================================================
	FromDirection
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::FromDirection (const Vec3_t &dir, const Vec3_t &up) __NE___
	{
		Vec3_t	hor = Normalize( Cross( up,  dir ));
		Vec3_t	ver = Normalize( Cross( dir, hor ));
		return Self{ hor, ver, dir };
	}

/*
=================================================
	Scaled
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::Scaled (const Vec3_t &scale) __NE___
	{
		return	Self{	Col_t{ scale.x, T(0),     T(0) },
						Col_t{ T(0),    scale.y,  T(0) },
						Col_t{ T(0),    T(0),     scale.z }};
	}

/*
=================================================
	LookAt
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::LookAt (const Vec3_t &dir, const Vec3_t &up) __NE___
	{
		const Vec3_t	s = Normalize( Cross( up, dir ));
		const Vec3_t	u = Cross( dir, s );

		Self	result;
		result[0][0] = s.x;
		result[1][0] = s.y;
		result[2][0] = s.z;
		result[0][1] = u.x;
		result[1][1] = u.y;
		result[2][1] = u.z;
		result[0][2] = dir.x;
		result[1][2] = dir.y;
		result[2][2] = dir.z;
		return result;
	}
#endif


#if Columns >= 3 and Rows >= 3
/*
=================================================
	Rotate*
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::RotateX (Rad_t angle) __NE___
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
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::RotateY (Rad_t angle) __NE___
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
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::RotateZ (Rad_t angle) __NE___
	{
		const T	s = Sin( angle );
		const T	c = Cos( angle );

		return Self{
				_CreateCol0(  c,     s,    T(0) ),
				_CreateCol0( -s,     c,    T(0) ),
				_CreateCol0( T(0),  T(0),  T(1) )
			#if Columns == 4
			  ,	_CreateCol1( T(0),  T(0),  T(0) )
			#endif
			};
	}

/*
=================================================
	ToCubeFace
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::ToCubeFace (const uint face) __NE___
	{
		switch ( face )
		{
			case 0 :	return RotateY( -90_deg );							// +X
			case 1 :	return RotateY( 90_deg );							// -X
			case 2 :	return RotateX( 90_deg );							// +Y
			case 3 :	return RotateX( -90_deg );							// -Y
			case 4 :	return Identity();									// -Z
			case 5 :	return RotateX( -180_deg ) * RotateZ( -180_deg );	// +Z
		}
		RETURN_ERR( "invalid cubemap face" );
	}
#endif


#if Columns == 4 and Rows == 4
/*
=================================================
	Rotate
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::Rotate (Rad_t angle, const Vec3_t &axis) __NE___
	{
		return Self{ glm::rotate( _GLM_Mat_t{ T{1} }, T{angle}, axis )};
	}

/*
=================================================
	InfinitePerspective
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::InfinitePerspective (Rad_t fovY, T aspect, T zNear) __NE___
	{
		T const	range	= glm::tan( T(fovY) * T(0.5) ) * zNear;
		T const	left	= -range * aspect;
		T const	right	= range * aspect;
		T const	bottom	= -range;
		T const	top		= range;

		Self	result = Zero();
		result[0][0] = (T(2) * zNear) / (right - left);
		result[1][1] = (T(2) * zNear) / (top - bottom);
		result[2][2] = T(1);
		result[2][3] = T(1);
		result[3][2] = - zNear;
		return result;
	}

/*
=================================================
	InfiniteFrustum
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::InfiniteFrustum (const Rect_t &viewport, T zNear) __NE___
	{
		Self	result = Zero();
		result[0][0] = T(2) / (viewport.right - viewport.left);
		result[1][1] = T(2) / (viewport.bottom - viewport.top);
		result[2][0] = - (viewport.right + viewport.left) / (viewport.right - viewport.left);
		result[2][1] = - (viewport.top + viewport.bottom) / (viewport.bottom - viewport.top);
		result[2][2] = T(1);
		result[2][3] = T(1);
		result[3][2] = - zNear;
		return result;
	}

/*
=================================================
	Project
=================================================
*/
	template <typename T, glm::qualifier Q>
	typename TMatrix<T, Columns, Rows, Q>::Vec3_t
		TMatrix<T, Columns, Rows, Q>::Project (const Vec3_t &pos, const Rect_t &viewport) C_NE___
	{
		Vec4_t			temp	= (*this) * Vec4_t( pos, T(1) );
		Vec2_t const	size	= viewport.Size();

		temp  /= temp.w;
		temp   = temp * T(0.5) + T(0.5);
		temp.x = temp.x * size.x + viewport.left;
		temp.y = temp.y * size.y + viewport.bottom;

		return Vec3_t{ temp };
	}

/*
=================================================
	UnProject
=================================================
*/
	template <typename T, glm::qualifier Q>
	typename TMatrix<T, Columns, Rows, Q>::Vec3_t
		TMatrix<T, Columns, Rows, Q>::UnProject (const Vec3_t &pos, const Rect_t &viewport) C_NE___
	{
		Vec4_t			temp	= Vec4_t{ pos, T(1) };
		Vec2_t const	size	= viewport.Size();

		temp.x	= (temp.x - viewport.left) / size.x;
		temp.y	= (temp.y - viewport.bottom) / size.y;
		temp	= temp * T(2) - T(1);

		temp	 = (*this) * temp;
		temp	/= temp.w;

		return Vec3_t{ temp };
	}

/*
=================================================
	ReverseZTransform
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::ReverseZTransform () __NE___
	{
		return	Self{	Col_t{ T(1), T(0),  T( 0), T(0) },
						Col_t{ T(0), T(1),  T( 0), T(0) },
						Col_t{ T(0), T(0),  T(-1), T(0) },
						Col_t{ T(0), T(0),  T( 1), T(1) }};
	}

/*
=================================================
	Scaled
=================================================
*/
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>  TMatrix<T, Columns, Rows, Q>::Scaled (const Vec3_t &scale) __NE___
	{
		return	Self{	Col_t{ scale.x, T(0),     T(0),    T(0) },
						Col_t{ T(0),    scale.y,  T(0),    T(0) },
						Col_t{ T(0),    T(0),     scale.z, T(0) },
						Col_t{ T(0),    T(0),     T(0),    T(1) }};
	}
#endif


/*
=================================================
	operator ==
=================================================
*/
	template <typename T, glm::qualifier Q>
	bool  TMatrix<T, Columns, Rows, Q>::operator == (const Self &rhs) C_NE___
	{
	#if Columns == 2
		return	AllEqual( get<0>(), rhs.get<0>() ) and
				AllEqual( get<1>(), rhs.get<1>() );

	#elif Columns == 3
		return	AllEqual( get<0>(), rhs.get<0>() ) and
				AllEqual( get<1>(), rhs.get<1>() ) and
				AllEqual( get<2>(), rhs.get<2>() );

	#elif Columns == 4
		return	AllEqual( get<0>(), rhs.get<0>() ) and
				AllEqual( get<1>(), rhs.get<1>() ) and
				AllEqual( get<2>(), rhs.get<2>() ) and
				AllEqual( get<3>(), rhs.get<3>() );
	#endif
	}


/*
=================================================
	AddTranslation / SetTranslation
=================================================
*/
#if Columns == 4 and Rows == 3
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>&  TMatrix<T, Columns, Rows, Q>::AddTranslation (const Vec3_t &translation) __NE___
	{
		get<3>() =	get<0>() * translation[0] +
					get<1>() * translation[1] +
					get<2>() * translation[2];
		return *this;
	}

	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>&  TMatrix<T, Columns, Rows, Q>::SetTranslation (const Vec3_t &translation) __NE___
	{
		get<3>() = translation;
		return *this;
	}
#endif

#if Columns == 4 and Rows == 4
	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>&  TMatrix<T, Columns, Rows, Q>::AddTranslation (const Vec3_t &translation) __NE___
	{
		get<3>() =	get<0>() * translation[0] +
					get<1>() * translation[1] +
					get<2>() * translation[2] +
					get<3>();
		return *this;
	}

	template <typename T, glm::qualifier Q>
	TMatrix<T, Columns, Rows, Q>&  TMatrix<T, Columns, Rows, Q>::SetTranslation (const Vec3_t &translation) __NE___
	{
		get<3>() = Vec4_t{ translation, T{1} };
		return *this;
	}
#endif

} // AE::Math
