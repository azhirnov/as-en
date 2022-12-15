// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::Math
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

		STATIC_ASSERT( IsPhysicalDimension< Dimension_t >);
		STATIC_ASSERT( IsPhysicalQuantity< Quantity_t >);

		using Self			= PhysicalQuantityMatrix< Quantity, Columns, Rows, Q >;
		using VQuat_t		= Quat< Value_t >;
		using VMatrix_t		= TMatrix< Value_t,  Columns, Rows, Q >;
		using VCol_t		= typename VMatrix_t::Col_t;			// [Rows]
		using VRow_t		= typename VMatrix_t::Row_t;			// [Columns]
		using QMatrix_t		= TMatrix< Quantity, Columns, Rows, Q >;
		using QCol_t		= PhysicalQuantity_FromVec< typename VMatrix_t::Col_t, Dimension_t, Scale_t >;	// PhysicalQuantityVec< Rows >
		using QRow_t		= PhysicalQuantity_FromVec< typename VMatrix_t::Row_t, Dimension_t, Scale_t >;	// PhysicalQuantityVec< Columns >
		
		STATIC_ASSERT( sizeof(QMatrix_t) == sizeof(VMatrix_t) );
		STATIC_ASSERT( sizeof(VCol_t) == sizeof(QCol_t) );
		STATIC_ASSERT( sizeof(VRow_t) == sizeof(QRow_t) );

		using Inversed_t	= PhysicalQuantityMatrix< typename Quantity::Inversed_t, Columns, Rows, Q >;
		using _Dim_t		= typename VMatrix_t::Dim_t;


	// variables
	public:
		VMatrix_t		_value;


	// methods
	public:
		PhysicalQuantityMatrix ()												__NE___ {}
		PhysicalQuantityMatrix (const Self &other)								__NE___ = default;
		PhysicalQuantityMatrix (Self && other)									__NE___ = default;
		
		explicit PhysicalQuantityMatrix (const VMatrix_t &other)				__NE___ : _value{other} {}
		explicit PhysicalQuantityMatrix (const QMatrix_t &other)				__NE___ : _value{ reinterpret_cast< VMatrix_t const &>(other) } {}
		
		template <typename S>
		PhysicalQuantityMatrix (const TPhysicalQuantityMatrix<Columns, Rows, Value_t, Dimension_t, S, Q> &other) __NE___ : _value{other._value} {}
		
	#if Columns == 2
		PhysicalQuantityMatrix (const VCol_t &col0,
								const VCol_t &col1)								__NE___ : _value{ col0, col1 } {}
		
		PhysicalQuantityMatrix (const QCol_t &col0,
								const QCol_t &col1)								__NE___ : _value{ col0.GetNonScaled(), col1.GetNonScaled() } {}
		
		ND_ static Self  FromScalar (Value_t value)								__NE___	{ return Self{ VCol_t{value}, VCol_t{value} }; }
		ND_ static Self  FromScalar (Quantity_t value)							__NE___	{ return Self{ QCol_t{value}, QCol_t{value} }; }
	#endif
		
	#if Columns == 3
		PhysicalQuantityMatrix (const VCol_t &col0,
								const VCol_t &col1,
								const VCol_t &col2)								__NE___ : _value{ col0, col1, col2 } {}
			
		PhysicalQuantityMatrix (const QCol_t &col0,
								const QCol_t &col1,
								const QCol_t &col2)								__NE___	: _value{ col0.GetNonScaled(), col1.GetNonScaled(), col2.GetNonScaled() } {}

		explicit PhysicalQuantityMatrix (const VQuat_t &q)						__NE___	: _value{q} {}
		
		ND_ static Self  FromScalar (Value_t value)								__NE___	{ return Self{ VCol_t{value}, VCol_t{value}, VCol_t{value} }; }
		ND_ static Self  FromScalar (Quantity_t value)							__NE___	{ return Self{ QCol_t{value}, QCol_t{value}, QCol_t{value} }; }
	#endif
		
	#if Columns == 4
		PhysicalQuantityMatrix (const VCol_t &col0,
								const VCol_t &col1,
								const VCol_t &col2,
								const VCol_t &col3)								__NE___ : _value{ col0, col1, col2, col3 } {}
			
		PhysicalQuantityMatrix (const QCol_t &col0,
								const QCol_t &col1,
								const QCol_t &col2,
								const QCol_t &col3)								__NE___ : _value{ col0.GetNonScaled(), col1.GetNonScaled(), col2.GetNonScaled(), col3.GetNonScaled() } {}

		explicit PhysicalQuantityMatrix (const VQuat_t &q)						__NE___	: _value{q} {}
		
		ND_ static Self  FromScalar (Value_t value)								__NE___	{ return Self{ VCol_t{value}, VCol_t{value}, VCol_t{value}, VCol_t{value} }; }
		ND_ static Self  FromScalar (Quantity_t value)							__NE___	{ return Self{ QCol_t{value}, QCol_t{value}, QCol_t{value}, QCol_t{value} }; }
	#endif

		ND_ VMatrix_t const&	GetNonScaled ()									C_NE___	{ return _value; }
		ND_ VMatrix_t &			GetNonScaledRef ()								__NE___	{ return _value; }
		ND_ VMatrix_t			GetScaled ()									C_NE___	{ return GetNonScaled() * VMatrix_t{Scale_t::Value}; }
		
		ND_ QMatrix_t const&	AsQMatrix ()									C_NE___	{ return reinterpret_cast< QMatrix_t const &>(_value); }
		ND_ QMatrix_t	&		AsQMatrix ()									__NE___	{ return reinterpret_cast< QMatrix_t	   &>(_value); }
		
		ND_ Inversed_t			Inversed ()										C_NE___	{ return Inversed_t{ _value.Inversed() }; }

		ND_ Self				operator + ()									C_NE___	{ return *this; }
		ND_ Self				operator - ()									C_NE___	{ return Self{ -_value }; }
		
			Self&				operator = (const Self &rhs)					__NE___ = default;
			Self&				operator = (Self && rhs)						__NE___ = default;
		
		// return column
		ND_ QCol_t const&		operator [] (usize c)							C_NE___	{ return reinterpret_cast< QCol_t const &>(_value[c]); }
		ND_ QCol_t&				operator [] (usize c)							__NE___	{ return reinterpret_cast< QCol_t       &>(_value[c]); }
		
		// return scalar
		ND_ Quantity_t const&	operator () (usize c, usize r)					C_NE___	{ return reinterpret_cast< Quantity_t const &>(_value(c,r)); }
		ND_ Quantity_t &		operator () (usize c, usize r)					__NE___	{ return reinterpret_cast< Quantity_t       &>(_value(c,r)); }

		// access to array
		ND_ Quantity_t const&	operator () (usize i)							C_NE___	{ return reinterpret_cast< Quantity_t const &>(_value(i)); }
		ND_ Quantity_t &		operator () (usize i)							__NE___	{ return reinterpret_cast< Quantity_t       &>(_value(i)); }
		
		ND_ static Self			Identity ()										__NE___	{ return Self{ VMatrix_t::Identity() }; }
		ND_ static Self			Zero ()											__NE___	{ return Self{ VMatrix_t::Zero() }; }
		
		ND_ static constexpr usize		size ()									__NE___	{ return VMatrix_t::size(); }
		ND_ static constexpr usize		ElementCount ()							__NE___	{ return VMatrix_t::ElementCount(); }
		ND_ static constexpr _Dim_t		Dimension ()							__NE___	{ return VMatrix_t::Dimension(); }
	};


} // AE::Math
