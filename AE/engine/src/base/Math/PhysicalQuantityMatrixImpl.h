// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

namespace AE::Base
{

	//
	// Physical Quantity Matrix
	//

	template <typename Quantity, glm::qualifier Q>
	struct PhysicalQuantityMatrix< Quantity, Columns, Rows, Q > final
	{
	// types
	public:
		using Value_t		= typename Quantity::Value_t;
		using Scale_t		= typename Quantity::Scale_t;
		using Quantity_t	= Quantity;
		using Dimension_t	= typename Quantity::Dimension_t;

		StaticAssert( IsPhysicalDimension< Dimension_t >);
		StaticAssert( IsPhysicalQuantity< Quantity_t >);
		StaticAssert( IsFloatPoint< Value_t >);

		using Self			= PhysicalQuantityMatrix< Quantity, Columns, Rows, Q >;
		using VQuat_t		= TQuat< Value_t, Q >;
		using VMatrix_t		= TMatrix< Value_t,  Columns, Rows, Q >;
		using VCol_t		= typename VMatrix_t::Col_t;			// [Rows]
		using VRow_t		= typename VMatrix_t::Row_t;			// [Columns]
		using QMatrix_t		= TMatrix< Quantity, Columns, Rows, Q >;
		using QCol_t		= PhysicalQuantity_FromVec< typename VMatrix_t::Col_t, Dimension_t, Scale_t, Q >;	// PhysicalQuantityVec< Rows >
		using QRow_t		= PhysicalQuantity_FromVec< typename VMatrix_t::Row_t, Dimension_t, Scale_t, Q >;	// PhysicalQuantityVec< Columns >

		StaticAssert( sizeof(QMatrix_t) == sizeof(VMatrix_t) );
		StaticAssert( sizeof(VCol_t) == sizeof(QCol_t) );
		StaticAssert( sizeof(VRow_t) == sizeof(QRow_t) );

		using Inversed_t	= PhysicalQuantityMatrix< typename Quantity::Inversed_t, Columns, Rows, Q >;


	// variables
	public:
		VMatrix_t		_mat;


	// methods
	public:
		PhysicalQuantityMatrix ()												__NE___ {}
		PhysicalQuantityMatrix (const Self &other)								__NE___ = default;
		PhysicalQuantityMatrix (Self &&other)									__NE___ = default;

		template <glm::qualifier Q2>
		explicit PhysicalQuantityMatrix (const TMatrix<Value_t,Columns,Rows,Q2> &other) __NE___ : _mat{ other } {}

		template <glm::qualifier Q2>
		explicit PhysicalQuantityMatrix (const TMatrix<Quantity,Columns,Rows,Q2> &other)__NE___ : _mat{ RefCast<VMatrix_t>(other) } {}

		template <typename S, glm::qualifier Q2>
		PhysicalQuantityMatrix (const TPhysicalQuantityMatrix<Columns, Rows, Value_t, Dimension_t, S, Q2> &other) __NE___ : _mat{other._mat} {}

	#if Columns == 2
		PhysicalQuantityMatrix (const VCol_t &col0,
								const VCol_t &col1)								__NE___ : _mat{ col0, col1 } {}

		PhysicalQuantityMatrix (const QCol_t &col0,
								const QCol_t &col1)								__NE___ : _mat{ col0.GetNonScaled(), col1.GetNonScaled() } {}

		ND_ static Self  FromScalar (Value_t value)								__NE___	{ return Self{ VCol_t{value}, VCol_t{value} }; }
		ND_ static Self  FromScalar (Quantity_t value)							__NE___	{ return Self{ QCol_t{value}, QCol_t{value} }; }
	#endif

	#if Columns == 3
		PhysicalQuantityMatrix (const VCol_t &col0,
								const VCol_t &col1,
								const VCol_t &col2)								__NE___ : _mat{ col0, col1, col2 } {}

		PhysicalQuantityMatrix (const QCol_t &col0,
								const QCol_t &col1,
								const QCol_t &col2)								__NE___	: _mat{ col0.GetNonScaled(), col1.GetNonScaled(), col2.GetNonScaled() } {}

		explicit PhysicalQuantityMatrix (const VQuat_t &q)						__NE___	: _mat{ q } {}

		ND_ static Self  FromScalar (Value_t value)								__NE___	{ return Self{ VCol_t{value}, VCol_t{value}, VCol_t{value} }; }
		ND_ static Self  FromScalar (Quantity_t value)							__NE___	{ return Self{ QCol_t{value}, QCol_t{value}, QCol_t{value} }; }
	#endif

	#if Columns == 4
		PhysicalQuantityMatrix (const VCol_t &col0,
								const VCol_t &col1,
								const VCol_t &col2,
								const VCol_t &col3)								__NE___ : _mat{ col0, col1, col2, col3 } {}

		PhysicalQuantityMatrix (const QCol_t &col0,
								const QCol_t &col1,
								const QCol_t &col2,
								const QCol_t &col3)								__NE___ : _mat{ col0.GetNonScaled(), col1.GetNonScaled(), col2.GetNonScaled(), col3.GetNonScaled() } {}

		explicit PhysicalQuantityMatrix (const VQuat_t &q)						__NE___	: _mat{ q } {}

		ND_ static Self  FromScalar (Value_t value)								__NE___	{ return Self{ VCol_t{value}, VCol_t{value}, VCol_t{value}, VCol_t{value} }; }
		ND_ static Self  FromScalar (Quantity_t value)							__NE___	{ return Self{ QCol_t{value}, QCol_t{value}, QCol_t{value}, QCol_t{value} }; }
	#endif

		ND_ VMatrix_t const&	GetNonScaled ()									C_NE___	{ return _mat; }
		ND_ VMatrix_t &			GetNonScaledRef ()								__NE___	{ return _mat; }
		ND_ VMatrix_t			GetScaled ()									C_NE___	{ return GetNonScaled() * VMatrix_t{Scale_t::Value}; }

		ND_ QMatrix_t const&	AsQMatrix ()									C_NE___	{ return RefCast<QMatrix_t>(_mat); }
		ND_ QMatrix_t &			AsQMatrix ()									__NE___	{ return RefCast<QMatrix_t>(_mat); }

		ND_ Inversed_t			Inversed ()										C_NE___	{ return Inversed_t{ _mat.Inversed() }; }

		ND_ Self				operator + ()									C_NE___	{ return *this; }
		ND_ Self				operator - ()									C_NE___	{ return Self{ -_mat }; }

		ND_ Self				operator +  (Value_t rhs)						C_NE___	{ return Self{ _mat + rhs }; }
		ND_ Self				operator -  (Value_t rhs)						C_NE___	{ return Self{ _mat - rhs }; }
		ND_ Self				operator *  (Value_t rhs)						C_NE___	{ return Self{ _mat * rhs }; }
		ND_ Self				operator /  (Value_t rhs)						C_NE___	{ return Self{ _mat / rhs }; }

		ND_ QCol_t				operator *  (const QRow_t &vec)					C_NE___	{ return QCol_t{ _mat * vec.GetNonScaled() }; }
		ND_ friend QRow_t		operator *  (const QCol_t &lhs, const Self &rhs)__NE___	{ return QRow_t{ lhs.GetNonScaled() * rhs._mat }; }

			Self&				operator = (const Self &rhs)					__NE___ = default;
			Self&				operator = (Self &&rhs)							__NE___ = default;

		ND_ bool				operator == (const Self &rhs)					C_NE___	{ return _mat == rhs._mat; }

		ND_ bool				IsIdentity ()									C_NE___	{ return _mat.IsIdentity(); }
		ND_ Quantity_t			Determinant ()									C_NE___	{ return Quantity_t{ _mat.Determinant() }; }

		// return column
		ND_ QCol_t const&		operator [] (usize c)							C_NE___	{ return RefCast<QCol_t>(_mat[c]); }
		ND_ QCol_t &			operator [] (usize c)							__NE___	{ return RefCast<QCol_t>(_mat[c]); }

		template <uint C>		ND_ QCol_t const&		get ()					C_NE___	{ return RefCast<QCol_t>(_mat.template get<C>()); }
		template <uint C>		ND_ QCol_t &			get ()					__NE___	{ return RefCast<QCol_t>(_mat.template get<C>()); }

		// return scalar
		ND_ Quantity_t const&	operator () (usize c, usize r)					C_NE___	{ return RefCast<Quantity_t>(_mat(c,r)); }
		ND_ Quantity_t &		operator () (usize c, usize r)					__NE___	{ return RefCast<Quantity_t>(_mat(c,r)); }

		template <uint C, uint R>	ND_ Quantity_t		get ()					C_NE___	{ return RefCast<Quantity_t>( _mat.template get<C,R>() ); }
		template <uint C, uint R>	ND_ Quantity_t &	get ()					__NE___	{ return RefCast<Quantity_t>( _mat.template get<C,R>() ); }

		// access to array
		ND_ Quantity_t const&	operator () (usize i)							C_NE___	{ return RefCast<Quantity_t>(_mat(i)); }
		ND_ Quantity_t &		operator () (usize i)							__NE___	{ return RefCast<Quantity_t>(_mat(i)); }

		ND_ static Self			Identity ()										__NE___	{ return Self{ VMatrix_t::Identity() }; }
		ND_ static Self			Zero ()											__NE___	{ return Self{ VMatrix_t::Zero() }; }

		NdCe__ static usize		size ()											__NE___	{ return VMatrix_t::size(); }
		NdCe__ static usize		ElementCount ()									__NE___	{ return VMatrix_t::ElementCount(); }
		NdCe__ static MatrixDim	Dimension ()									__NE___	{ return VMatrix_t::Dimension(); }
		NdCe__ static bool		IsColumnMajor ()								__NE___	{ return VMatrix_t::IsColumnMajor(); }
	};


} // AE::Base
