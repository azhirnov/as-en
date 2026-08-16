// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef AE_ENABLE_STB
# include "pch/Base.h"
# include "stb_rect_pack.h"

namespace AE::AtlasTools
{
	using namespace AE::Base;


	//
	// Rect Packer STB
	//

	class RectPackerSTB
	{
	// variables
	private:
		Array< stbrp_rect >		_rects;
		ulong					_maxArea		= 0;
		uint2					_maxSize		{0};
		uint2					_targetSize		{0};
		uint					_targetWidth	= 0;


	// methods
	public:
		RectPackerSTB () {}

			void	Add (const uint2 &size);
			void	Add (const uint2 &size, uint id);

		// Used to build small atlases with the same width and then merge them to one large atlas.
			void	SetWidth (uint w);

		ND_ bool	Pack ();
			void	Reset ();

		ND_ uint2	TargetSize ()	const	{ return _targetSize; }
		ND_ float	PackingRate ()	const;

		ND_ ArrayView<stbrp_rect>  GetResult () const { return _rects; }

	private:
		ND_ static uint2  _BestDimensionForArea (ulong area, uint targetW, const uint2 minSize);
			static void   _FindMaxY (ArrayView<stbrp_rect>, OUT uint &);
	};


/*
=================================================
	Add
=================================================
*/
	inline void  RectPackerSTB::Add (const uint2 &size)
	{
		return Add( size, uint(_rects.size()) );
	}

	inline void  RectPackerSTB::Add (const uint2 &size, uint id)
	{
		ASSERT( size.x <= STBRP__MAXVAL );
		ASSERT( size.y <= STBRP__MAXVAL );

		auto&	dst	= _rects.emplace_back();
		dst.id	= id;
		dst.x	= 0;
		dst.y	= 0;
		dst.w	= int(size.x);
		dst.h	= int(size.y);

		_maxArea	+= Area( size );
		_maxSize	= Max( _maxSize, size );
	}

/*
=================================================
	PackingRate
=================================================
*/
	inline float  RectPackerSTB::PackingRate () const
	{
		return float(_maxArea) / float(_targetSize.x * _targetSize.y);
	}

/*
=================================================
	PackingRate
=================================================
*/
	inline void  RectPackerSTB::SetWidth (uint w)
	{
		ASSERT( All( _targetSize == Zero ));

		_targetWidth = w;
	}


} // AE::AtlasTools

#endif // AE_ENABLE_STB
