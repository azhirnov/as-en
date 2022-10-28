// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/Quat.h"
#include "base/Math/Matrix.h"

namespace AE::Math
{

	//
	// Transformation
	//

	template <typename T>
	struct Transformation
	{
	// types
	public:
		using Value_t	= T;
		using Vec3_t	= Vec< T, 3 >;
		using Quat_t	= Quat< T >;
		using Mat4_t	= Matrix< T, 4, 4 >;
		using Self		= Transformation< T >;


	// variables
	public:
		Quat_t		orientation		{Quat_t::Identity()};
		Vec3_t		position		{ T{0} };
		Value_t		scale			{ T{1} };


	// methods
	public:
		constexpr Transformation () = default;
		constexpr Transformation (const Self &) = default;
		constexpr Transformation (Self &&) = default;

		constexpr Transformation (const Vec3_t &pos, const Quat_t &orient, const T &scale = T{1}) :
			orientation{orient}, position{pos}, scale{scale} {}

		explicit Transformation (const Mat4_t &mat);
		
			constexpr Self&	operator =  (const Self &) = default;
			constexpr Self&	operator =  (Self &&) = default;

			Self &	operator += (const Self &rhs);
		ND_ Self	operator +  (const Self &rhs)	const	{ return Self{*this} += rhs; }
		
			Self &	operator -= (const Self &rhs)			{ return Self{*this} += rhs.Inversed(); }
		ND_ Self	operator -  (const Self &rhs)	const	{ return Self{*this} -= rhs; }

		ND_ bool	operator == (const Self &rhs)	const;
		ND_ bool	operator != (const Self &rhs)	const	{ return not (*this == rhs); }

			Self &	Move (const Vec3_t &delta);
			Self &	Rotate (const Quat_t &delta);
			Self &	Scale (float scale);

			Self &	Inverse ();
		ND_ Self	Inversed ()	const						{ return Self{*this}.Inverse(); }

		ND_ Mat4_t	ToMatrix () const;
		ND_ Mat4_t	ToRotationMatrix () const;

		ND_ bool	IsIdentity () const;


		// local space to global
		ND_ Vec3_t	ToGlobalVector (const Vec3_t &local)	const;
		ND_ Vec3_t	ToGlobalPosition (const Vec3_t &local)	const	{ return ToGlobalVector( local ) + position; }

		// global space to local
		ND_ Vec3_t	ToLocalVector (const Vec3_t &global)	const;
		ND_ Vec3_t	ToLocalPosition (const Vec3_t &global)	const	{ return ToLocalVector( global - position ); }
	};

	
	using Transform = Transformation<float>;

	
/*
=================================================
	constructor (mat4x4)
=================================================
*/
	template <typename T>
	inline Transformation<T>::Transformation (const Mat4_t &mat)
	{
		Vec3_t		scale3;
		Vec3_t		skew;
		Vec<T,4>	perspective;
		Unused( glm::decompose( mat._value, OUT scale3, OUT orientation._value, OUT position, OUT skew, OUT perspective ));

		ASSERT( Equals( scale3.x, scale3.y ) and Equals( scale3.x, scale3.z ));
		scale = scale3.x;
	}
	
/*
=================================================
	operator +=
=================================================
*/
	template <typename T>
	inline Transformation<T>&  Transformation<T>::operator += (const Self &rhs)
	{
		position	+= orientation * (rhs.position * scale);
		orientation	*= rhs.orientation;
		scale		*= rhs.scale;
		return *this;
	}
	
/*
=================================================
	operator ==
=================================================
*/
	template <typename T>
	inline bool Transformation<T>::operator == (const Self &rhs) const
	{
		return	All( orientation == rhs.orientation )	&
				All( position	 == rhs.position )		&
				(scale			 == rhs.scale);
	}
	
/*
=================================================
	Equals
=================================================
*/
	template <typename T>
	ND_ inline bool  Equals (const Transformation<T> &lhs, const Transformation<T> &rhs, const T &err = Epsilon<T>())
	{
		return	All( Math::Equals( lhs.orientation, rhs.orientation, err ))	&
				All( Math::Equals( lhs.position, rhs.position, err ))		&
				Math::Equals( lhs.scale, rhs.scale, err );
	}

/*
=================================================
	Move
=================================================
*/
	template <typename T>
	inline Transformation<T>&  Transformation<T>::Move (const Vec3_t &delta)
	{
		position += orientation * (delta * scale);
		return *this;
	}
	
/*
=================================================
	Rotate
=================================================
*/
	template <typename T>
	inline Transformation<T>&  Transformation<T>::Rotate (const Quat_t &delta)
	{
		orientation *= delta;
		return *this;
	}
	
/*
=================================================
	Scale
=================================================
*/
	template <typename T>
	inline Transformation<T>&  Transformation<T>::Scale (float value)
	{
		this->scale *= value;
		return *this;
	}
	
/*
=================================================
	Inverse
=================================================
*/
	template <typename T>
	inline Transformation<T>&  Transformation<T>::Inverse ()
	{
		ASSERT( Math::IsNotZero( scale ));

		orientation.Inverse();
		scale		= T{1} / scale;
		position	= orientation * (-position * scale);
		return *this;
	}
	
/*
=================================================
	ToGlobalVector
=================================================
*/
	template <typename T>
	inline typename Transformation<T>::Vec3_t
		Transformation<T>::ToGlobalVector (const Vec3_t &local) const
	{
		return orientation * (local * scale);
	}
	
/*
=================================================
	ToLocalVector
=================================================
*/
	template <typename T>
	inline typename Transformation<T>::Vec3_t
		Transformation<T>::ToLocalVector (const Vec3_t &global) const
	{
		return (orientation.Inversed() * global) / scale;
	}
	
/*
=================================================
	ToMatrix
=================================================
*/
	template <typename T>
	inline typename Transformation<T>::Mat4_t  Transformation<T>::ToMatrix () const
	{
		Mat4_t	result{ orientation };
		
		result[3] = Vec<T,4>{ position, T{1} };

		return result * Mat4_t::Scale( scale );
	}
	
/*
=================================================
	ToRotationMatrix
=================================================
*/
	template <typename T>
	inline typename Transformation<T>::Mat4_t  Transformation<T>::ToRotationMatrix () const
	{
		return Mat4_t{ orientation } * Mat4_t::Scale( scale );
	}
	
/*
=================================================
	IsIdentity
=================================================
*/
	template <typename T>
	bool  Transformation<T>::IsIdentity () const
	{
		return Equals( *this, Self{} );
	}


} // AE::Math


namespace AE::Base
{
	template <typename T>	struct TMemCopyAvailable< Transformation<T> >		{ static constexpr bool  value = IsMemCopyAvailable<T>; };
	template <typename T>	struct TZeroMemAvailable< Transformation<T> >		{ static constexpr bool  value = IsZeroMemAvailable<T>; };
	template <typename T>	struct TTrivialySerializable< Transformation<T> >	{ static constexpr bool  value = IsTrivialySerializable<T>; };
	
} // AE::Base
