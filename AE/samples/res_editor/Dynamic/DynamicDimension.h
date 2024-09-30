// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Dynamic/DynamicScalar.h"
#include "res_editor/Dynamic/DynamicVec.h"

namespace AE::ResEditor
{

	//
	// Dynamic Dimension
	//

	class DynamicDim final : public EnableRC<DynamicDim>
	{
	// types
	public:
		using ERounding		= EDynamicVarRounding;
		using GetValueFn_t	= uint3 (*) (EnableRCBase*);


	// variables
	private:
		mutable RWSpinLock		_guard;
		uint3					_dimension;							// unused if '_base' is not null
		int3					_scale			{1,1,1};
		ERounding				_rounding		= ERounding::Round;
		const EImageDim			_numDimensions;
		const RC<>				_base;
		const GetValueFn_t		_getValue		= null;


	// methods
	public:
		explicit DynamicDim (const uint		dim)					__NE___	: DynamicDim{ uint3{ dim, 0u, 0u }, EImageDim_1D } {}
		explicit DynamicDim (const uint2	&dim)					__NE___	: DynamicDim{ uint3{ dim, 0u }, EImageDim_2D } {}
		explicit DynamicDim (const uint3	&dim,
							 EImageDim		imgDim = EImageDim_2D)	__NE___;
		explicit DynamicDim (RC<DynamicDim> base)					__NE___;
		explicit DynamicDim (RC<>			base,
							 GetValueFn_t	getValue,
							 EImageDim		imgDim = EImageDim_2D)	__NE___;

			void		Resize (const uint &dim)					__NE___	{ return Resize( uint3{ dim, 1u, 1u }); }
			void		Resize (const uint2 &dim)					__NE___	{ return Resize( uint3{ dim, 1u }); }
			void		Resize (const uint3 &dim)					__NE___;

			void		SetScale (int3 scale)						__NE___	{ return SetScale( scale, ERounding::Round ); }
			void		SetScale (int3, ERounding)					__NE___;

		ND_ bool		IsChanged (INOUT uint3 &dim)				C_NE___;
		ND_ bool		IsChanged_NonZero (INOUT uint3 &dim)		C_NE___;
		ND_ bool		IsChanged_NonZero (INOUT ImageDim_t &)		C_NE___;
		ND_ bool		IsChanged (INOUT float &aspect)				C_NE___;

		ND_ float		Aspect ()									C_NE___;
		ND_ uint		Dimension1 ()								C_NE___	{ ASSERT( _numDimensions == EImageDim_1D );  return Dimension3().x; }
		ND_ uint		Dimension1_NonZero ()						C_NE___	{ ASSERT( _numDimensions == EImageDim_1D );  return Dimension3_NonZero().x; }
		ND_ uint2		Dimension2 ()								C_NE___	{ ASSERT( _numDimensions == EImageDim_2D );  return uint2(Dimension3()); }
		ND_ uint2		Dimension2_NonZero ()						C_NE___	{ ASSERT( _numDimensions == EImageDim_2D );  return uint2(Dimension3_NonZero()); }
		ND_ uint3		Dimension3 ()								C_NE___;
		ND_ uint3		Dimension3_NonZero ()						C_NE___	{ return Max( Dimension3(), 1u ); }
		ND_ int3		Scale ()									C_NE___	{ SHAREDLOCK( _guard );  return _scale; }
		ND_ EImageDim	NumDimensions ()							C_NE___	{ return _numDimensions; }
		ND_ uint3		BaseDimension ()							C_NE___;

		ND_ uint		Area ()										C_NE___;
		ND_ uint		Volume ()									C_NE___;

		ND_ uint		Remap (uint src)							C_NE___	{ return Remap(uint3{ src, 0u, 0u }).x; }
		ND_ uint2		Remap (uint2 src)							C_NE___	{ return uint2{Remap(uint3{ src, 0u })}; }
		ND_ uint3		Remap (uint3 src)							C_NE___;

		ND_ uint		Remap (float src)							C_NE___	{ return Remap(float3{ src, 0.f, 0.f }).x; }
		ND_ uint2		Remap (float2 src)							C_NE___	{ return uint2{Remap(float3{ src, 0.f })}; }
		ND_ uint3		Remap (float3 src)							C_NE___;

		ND_ RC<DynamicUInt>		GetDynamicX ()						__NE___;
		ND_ RC<DynamicUInt>		GetDynamicY ()						__NE___;
		ND_ RC<DynamicUInt2>	GetDynamicXY ()						__NE___;
		ND_ RC<DynamicUInt>		GetDynamicArea ()					__NE___;
		ND_ RC<DynamicUInt>		GetDynamicVolume ()					__NE___;

	private:
		ND_ uint3		_BaseDim ()									C_NE___;

		ND_ static uint  _ApplyScale (uint, int, ERounding)			__NE___;
		ND_ static uint  _ApplyScale (float, int, ERounding)		__NE___;

		ND_ static uint		_GetX (EnableRCBase*)					__NE___;
		ND_ static uint		_GetY (EnableRCBase*)					__NE___;
		ND_ static uint2	_GetXY (EnableRCBase*)					__NE___;
		ND_ static uint		_GetArea (EnableRCBase*)				__NE___;
		ND_ static uint		_GetVolume (EnableRCBase*)				__NE___;

		ND_ static uint3	_GetDim (EnableRCBase*)					__NE___;
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

	inline DynamicDim::DynamicDim (RC<> base, GetValueFn_t getValue, EImageDim imgDim) __NE___ :
		_numDimensions{ imgDim },
		_base{ RVRef(base) },
		_getValue{ getValue }
	{}

	inline DynamicDim::DynamicDim (RC<DynamicDim> base) __NE___ :
		_numDimensions{ base ? base->NumDimensions() : EImageDim_2D },
		_base{ RVRef(base) },
		_getValue{ _base ? &_GetDim : null }
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

	inline uint3  DynamicDim::Remap (float3 src) C_NE___
	{
		uint3	dst;
		dst.x = _ApplyScale( src.x, _scale.x, _rounding );
		dst.y = _ApplyScale( src.y, _scale.y, _rounding );
		dst.z = _ApplyScale( src.z, _scale.z, _rounding );
		return dst;
	}

/*
=================================================
	_ApplyScale
=================================================
*/
	inline uint  DynamicDim::_ApplyScale (uint dim, int scale, ERounding rounding) __NE___
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

	inline uint  DynamicDim::_ApplyScale (float src, int scale, ERounding rounding) __NE___
	{
		float	fscale = float(scale);

		if ( scale >= 0 )
			src = src * fscale + 0.5f;
		else
		{
					fscale	= -fscale;
			float	bias	= 0.f;
			switch_enum( rounding )
			{
				case ERounding::Floor :		bias = 0.f;				break;
				case ERounding::Round :		bias = fscale*0.5f;		break;
				case ERounding::Ceil :		bias = fscale-1.f;		break;
			}
			switch_end

			src = ((src + bias) / fscale);
		}
		return uint(src);
	}

/*
=================================================
	Dimension3
=================================================
*/
	inline uint3  DynamicDim::Dimension3 () C_NE___
	{
		SHAREDLOCK( _guard );

		uint3			dim		 = _BaseDim();
		const bool3		was_zero = (dim == uint3{0});
		
		switch ( _numDimensions )
		{
			case EImageDim::_1D :
				dim.x = _ApplyScale( dim.x, _scale.x, _rounding );	break;

			case EImageDim::_2D :
				dim.x = _ApplyScale( dim.x, _scale.x, _rounding );
				dim.y = _ApplyScale( dim.y, _scale.y, _rounding );	break;

			case EImageDim::_3D :
				dim.x = _ApplyScale( dim.x, _scale.x, _rounding );
				dim.y = _ApplyScale( dim.y, _scale.y, _rounding );
				dim.z = _ApplyScale( dim.z, _scale.z, _rounding );	break;
		}

		dim *= uint3{not was_zero};
		return dim;
	}

/*
=================================================
	BaseDimension
=================================================
*/
	inline uint3  DynamicDim::BaseDimension () C_NE___
	{
		SHAREDLOCK( _guard );
		return _BaseDim();
	}

	inline uint3  DynamicDim::_BaseDim () C_NE___
	{
		return _getValue ? _getValue( _base.get() ) : _dimension;
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

	inline bool  DynamicDim::IsChanged_NonZero (INOUT ImageDim_t &oldDim) C_NE___
	{
		uint3	old_dim {oldDim};
		bool	res		= IsChanged_NonZero( INOUT old_dim );
		oldDim = ImageDim_t{old_dim};
		return res;
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

/*
=================================================
	Area
=================================================
*/
	inline uint  DynamicDim::Area () C_NE___
	{
		uint3	dim = Dimension3_NonZero();
		switch ( _numDimensions ) {
			case EImageDim::_1D :	return dim.x;
			case EImageDim::_2D :
			case EImageDim::_3D :	return dim.x * dim.y;
		}
		return 0;
	}

/*
=================================================
	Volume
=================================================
*/
	inline uint  DynamicDim::Volume () C_NE___
	{
		uint3	dim = Dimension3_NonZero();
		switch ( _numDimensions ) {
			case EImageDim::_1D :	return dim.x;
			case EImageDim::_2D :	return dim.x * dim.y;
			case EImageDim::_3D :	return dim.x * dim.y * dim.z;
		}
		return 0;
	}

/*
=================================================
	GetDynamicArea
=================================================
*/
	inline uint  DynamicDim::_GetArea (EnableRCBase* base) __NE___
	{
		return Cast<DynamicDim>(base)->Area();
	}

	inline RC<DynamicUInt>  DynamicDim::GetDynamicArea () __NE___
	{
		ASSERT( _numDimensions >= EImageDim_2D );
		return MakeRC<DynamicUInt>( RC<>{GetRC()}, &_GetArea );
	}

/*
=================================================
	GetDynamicVolume
=================================================
*/
	inline uint  DynamicDim::_GetVolume (EnableRCBase* base) __NE___
	{
		return Cast<DynamicDim>(base)->Volume();
	}

	inline RC<DynamicUInt>  DynamicDim::GetDynamicVolume () __NE___
	{
		ASSERT( _numDimensions == EImageDim_3D );
		return MakeRC<DynamicUInt>( RC<>{GetRC()}, &_GetVolume );
	}

/*
=================================================
	GetDynamicX
=================================================
*/
	inline uint  DynamicDim::_GetX (EnableRCBase* base) __NE___
	{
		return Cast<DynamicDim>(base)->Dimension2().x;
	}

	inline RC<DynamicUInt>  DynamicDim::GetDynamicX () __NE___
	{
		ASSERT( _numDimensions >= EImageDim_1D );
		return MakeRC<DynamicUInt>( RC<>{GetRC()}, &_GetX );
	}

/*
=================================================
	GetDynamicY
=================================================
*/
	inline uint  DynamicDim::_GetY (EnableRCBase* base) __NE___
	{
		return Cast<DynamicDim>(base)->Dimension2().y;
	}

	inline RC<DynamicUInt>  DynamicDim::GetDynamicY () __NE___
	{
		ASSERT( _numDimensions >= EImageDim_2D );
		return MakeRC<DynamicUInt>( RC<>{GetRC()}, &_GetY );
	}

/*
=================================================
	GetDynamicXY
=================================================
*/
	inline uint2  DynamicDim::_GetXY (EnableRCBase* base) __NE___
	{
		return Cast<DynamicDim>(base)->Dimension2();
	}

	inline RC<DynamicUInt2>  DynamicDim::GetDynamicXY () __NE___
	{
		ASSERT( _numDimensions >= EImageDim_2D );
		return MakeRC<DynamicUInt2>( RC<>{GetRC()}, &_GetXY );
	}

/*
=================================================
	_GetDim
=================================================
*/
	inline uint3  DynamicDim::_GetDim (EnableRCBase* base) __NE___
	{
		NonNull( base );
		return Cast<DynamicDim>(base)->Dimension3();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ToDim2
=================================================
*/
	template <>
	inline uint3  TDynamicScalar<uint>::_GetDim2 (EnableRCBase* base) __NE___
	{
		return uint3{ uint2{Cast<TDynamicScalar<uint>>( base )->Get()}, 1u };
	}

	template <>
	inline RC<DynamicDim>  TDynamicScalar<uint>::ToDim2 () __NE___
	{
		return MakeRC<DynamicDim>( GetRC(), &_GetDim2, EImageDim_2D );
	}

/*
=================================================
	ToDim3
=================================================
*/
	template <>
	inline uint3  TDynamicScalar<uint>::_GetDim3 (EnableRCBase* base) __NE___
	{
		return uint3{ Cast<TDynamicScalar<uint>>( base )->Get() };
	}

	template <>
	inline RC<DynamicDim>  TDynamicScalar<uint>::ToDim3 () __NE___
	{
		return MakeRC<DynamicDim>( GetRC(), &_GetDim3, EImageDim_3D );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	ToDim
=================================================
*/
	template <typename T, int I>
	uint3  TDynamicVec<T,I>::_GetDim (EnableRCBase* base) __NE___
	{
		auto	src = Cast<TDynamicVec<T,I>>( base )->Get();
		if constexpr( I == 2 )
			return uint3{ src, 1u };
		if constexpr( I == 3 )
			return src;
	}

	template <typename T, int I>
	RC<DynamicDim>  TDynamicVec<T,I>::ToDim () __NE___
	{
		return MakeRC<DynamicDim>( this->GetRC(), &_GetDim, (I == 2 ? EImageDim_2D : EImageDim_3D) );
	}


} // AE::ResEditor
