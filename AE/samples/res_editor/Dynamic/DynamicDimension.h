// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Common.h"

namespace AE::ResEditor
{

	//
	// Dynamic Dimension
	//

	class DynamicDim final : public EnableRC<DynamicDim>
	{
	// types
	public:
		enum class ERounding : ubyte
		{
			Floor,
			Round,
			Ceil,
		};


	// variables
	private:
		mutable RWSpinLock		_guard;
		uint3					_dimension;							// unused if '_base' is not null
		int3					_scale			{1,1,1};
		ERounding				_rounding		= ERounding::Round;
		const EImageDim			_numDimensions;
		const RC<DynamicDim>	_base;


	// methods
	public:
		explicit DynamicDim (const uint dim)									__NE___	: DynamicDim{ uint3{ dim, 0u, 0u }, EImageDim_1D } {}
		explicit DynamicDim (const uint2 &dim)									__NE___	: DynamicDim{ uint3{ dim, 0u }, EImageDim_2D } {}
		explicit DynamicDim (const uint3 &dim, EImageDim imgDim = EImageDim_2D)	__NE___;
		explicit DynamicDim (RC<DynamicDim> base)								__NE___;

			void		Resize (const uint &dim)				__NE___	{ return Resize( uint3{ dim, 1u, 1u }); }
			void		Resize (const uint2 &dim)				__NE___	{ return Resize( uint3{ dim, 1u }); }
			void		Resize (const uint3 &dim)				__NE___;

			void		SetScale (int3 scale)					__NE___	{ return SetScale( scale, ERounding::Round ); }
			void		SetScale (int3, ERounding)				__NE___;

		ND_ bool		IsChanged (INOUT uint3 &dim)			C_NE___;
		ND_ bool		IsChanged_NonZero (INOUT uint3 &dim)	C_NE___;
		ND_ bool		IsChanged (INOUT float &aspect)			C_NE___;

		ND_ float		Aspect ()								C_NE___;
		ND_ uint		Dimension1 ()							C_NE___	{ ASSERT( _numDimensions == EImageDim_1D );  return Dimension3().x; }
		ND_ uint		Dimension1_NonZero ()					C_NE___	{ ASSERT( _numDimensions == EImageDim_1D );  return Dimension3_NonZero().x; }
		ND_ uint2		Dimension2 ()							C_NE___	{ ASSERT( _numDimensions == EImageDim_2D );  return uint2(Dimension3()); }
		ND_ uint2		Dimension2_NonZero ()					C_NE___	{ ASSERT( _numDimensions == EImageDim_2D );  return uint2(Dimension3_NonZero()); }
		ND_ uint3		Dimension3 ()							C_NE___;
		ND_ uint3		Dimension3_NonZero ()					C_NE___	{ return Max( Dimension3(), 1u ); }
		ND_ int3		Scale ()								C_NE___	{ SHAREDLOCK( _guard );  return _scale; }
		ND_ EImageDim	NumDimensions ()						C_NE___	{ return _numDimensions; }

		ND_ uint		Remap (uint src)						C_NE___	{ return Remap(uint3{ src, 0u, 0u }).x; }
		ND_ uint2		Remap (uint2 src)						C_NE___	{ return uint2{Remap(uint3{ src, 0u })}; }
		ND_ uint3		Remap (uint3 src)						C_NE___;

	private:
		ND_ static uint	_ApplyScale (uint dim, int scale, ERounding);
	};



/*
=================================================
	constructor
=================================================
*/
	inline DynamicDim::DynamicDim (const uint3 &dim, EImageDim imgDim) __NE___ :
		_dimension{ Max( dim, 1u )},
		_numDimensions{ imgDim },
		_base{}
	{
		switch ( _numDimensions ) {
			case EImageDim::_1D :	_dimension = uint3{ _dimension.x, 0u, 0u };				break;
			case EImageDim::_2D :	_dimension = uint3{ _dimension.x, _dimension.y, 0u };	break;
			case EImageDim::_3D :	break;
		}
	}

	inline DynamicDim::DynamicDim (RC<DynamicDim> base) __NE___ :
		_numDimensions{ base ? base->NumDimensions() : EImageDim_2D },
		_base{ RVRef(base) }
	{}

/*
=================================================
	SetScale
=================================================
*/
	inline void  DynamicDim::SetScale (int3 scale, ERounding rnd) __NE___
	{
		ASSERT( All( IsNotZero( scale )));
		EXLOCK( _guard );
		_scale		= scale;
		_rounding	= rnd;
	}

/*
=================================================
	Resize
=================================================
*/
	inline void  DynamicDim::Resize (const uint3 &dim) __NE___
	{
		EXLOCK( _guard );

		ASSERT( not _base );
		ASSERT( Any( dim > uint3{0} ));

		if ( not _base )
		{
			switch ( _numDimensions ) {
				case EImageDim::_1D :	_dimension = uint3{ dim.x, 0u, 0u };	break;
				case EImageDim::_2D :	_dimension = uint3{ dim.x, dim.y, 0u };	break;
				case EImageDim::_3D :	break;
			}
		}
	}

/*
=================================================
	Remap
=================================================
*/
	inline uint3  DynamicDim::Remap (uint3 src) C_NE___
	{
		src.x = _ApplyScale( src.x, _scale.x, _rounding );
		src.y = _ApplyScale( src.y, _scale.y, _rounding );
		src.z = _ApplyScale( src.z, _scale.z, _rounding );
		return src;
	}

/*
=================================================
	Dimension3
=================================================
*/
	inline uint  DynamicDim::_ApplyScale (uint dim, int scale, ERounding rounding)
	{
		if ( scale >= 0 )
			dim = (dim * scale);
		else
		{
			uint	bias = 0;
			switch_enum( rounding )
			{
				case ERounding::Floor :		bias = 0;			break;
				case ERounding::Round :		bias = -scale/2;	break;
				case ERounding::Ceil :		bias = -scale-1;	break;
			}
			switch_end

			dim = ((dim + bias) / -scale);
		}
		return dim;
	}

/*
=================================================
	Dimension3
=================================================
*/
	inline uint3  DynamicDim::Dimension3 () C_NE___
	{
		SHAREDLOCK( _guard );

		uint3			dim		 = _base ? _base->Dimension3() : _dimension;
		const bool3		was_zero = (dim == uint3{0});

		dim.x = _ApplyScale( dim.x, _scale.x, _rounding );
		dim.y = _ApplyScale( dim.y, _scale.y, _rounding );
		dim.z = _ApplyScale( dim.z, _scale.z, _rounding );
		dim  *= uint3{not was_zero};

		return dim;
	}

/*
=================================================
	IsChanged_NonZero
=================================================
*/
	inline bool  DynamicDim::IsChanged_NonZero (INOUT uint3 &oldDim) C_NE___
	{
		const uint3	new_dim = Dimension3_NonZero();

		if ( Any( new_dim != uint3{0} )	and Any( new_dim != oldDim ))
		{
			oldDim = Max( new_dim, 1u );
			return true;
		}
		return false;
	}

/*
=================================================
	IsChanged
=================================================
*/
	inline bool  DynamicDim::IsChanged (INOUT uint3 &oldDim) C_NE___
	{
		const uint3	new_dim = Dimension3();

		if ( Any( new_dim != uint3{0} )	and Any( new_dim != oldDim ))
		{
			oldDim = new_dim;
			return true;
		}
		return false;
	}

/*
=================================================
	IsChanged
=================================================
*/
	inline bool  DynamicDim::IsChanged (INOUT float &oldAspect) C_NE___
	{
		const float	new_aspect = Aspect();

		if ( not Equal( new_aspect, oldAspect ))
		{
			oldAspect = new_aspect;
			return true;
		}
		return false;
	}

/*
=================================================
	Aspect
=================================================
*/
	inline float  DynamicDim::Aspect () C_NE___
	{
		ASSERT( _numDimensions >= EImageDim_2D );
		uint2	dim = Dimension2();
		return float(dim.x) / float(dim.y);
	}


} // AE::ResEditor
