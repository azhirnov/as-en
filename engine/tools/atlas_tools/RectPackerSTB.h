// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_STB
# include "base/Math/Rectangle.h"
# include "base/Math/Vec.h"
# include "base/Containers/ArrayView.h"

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
		ulong					_maxArea	= 0;
		uint2					_maxSize	{0};
		uint2					_targetSize	{0};


	// methods
	public:
		RectPackerSTB () {}
		
			void  Add (const uint2 &size);
			void  Add (const uint2 &size, uint id);

		ND_ bool  Pack ();
			void  Reset ();

		ND_ uint2  TargetSize ()	const	{ return _targetSize; }
		ND_ float  PackingRate ()	const;

		ND_ ArrayView<stbrp_rect>  GetResult () const { return _rects; }
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


} // AE::AtlasTools

#endif // AE_ENABLE_STB
