// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Public/Common.h"

namespace AE::App
{
namespace _hidden_
{

	template <typename V>
	struct Pixels
	{
		V	pixels;

		Pixels () {}
		explicit Pixels (const V &val) : pixels{val} {}

		ND_ explicit operator V () const { return pixels; }
	};


	template <typename V>
	struct Dips
	{
		V	dips;
		
		Dips () {}
		explicit Dips (const V &val) : dips{val} {}

		ND_ explicit operator V () const { return dips; }
	};


	template <typename V>
	struct Meters
	{
		V	meters;

		Meters () {}
		explicit Meters (const V &val) : meters{val} {}

		ND_ explicit operator V () const { return meters; }
	};

	
	template <typename V>
	struct SNorm;


	template <typename V>
	struct UNorm
	{
		V	unorm;
		
		UNorm () {}
		explicit UNorm (const V &val) : unorm{val} {}
		explicit UNorm (const SNorm<V> &val);

		ND_ explicit operator V () const { return unorm; }
	};


	template <typename V>
	struct SNorm
	{
		V	snorm;

		SNorm () {}
		explicit SNorm (const V &val) : snorm{val} {}
		explicit SNorm (const UNorm<V> &val);

		ND_ explicit operator V () const { return snorm; }
	};

	
	template <>	inline UNorm<float2>::UNorm (const SNorm<float2> &val) : unorm{ (val.snorm - 1.0f) * 0.5f } {}
	template <>	inline SNorm<float2>::SNorm (const UNorm<float2> &val) : snorm{ (val.unorm * 2.0f) - 1.0f } {}
}


	using Pixels2f		= _hidden_::Pixels< float2 >;
	using Pixels2u		= _hidden_::Pixels< uint2 >;
	using Pixels2i		= _hidden_::Pixels< int2 >;
	using PixelsRectI	= _hidden_::Pixels< RectI >;
	using PixelsRectF	= _hidden_::Pixels< RectF >;

	using Dips2f		= _hidden_::Dips< float2 >;
	using Dips2u		= _hidden_::Dips< uint2 >;
	using Dips2i		= _hidden_::Dips< int2 >;
	using DipsRectF		= _hidden_::Dips< RectF >;
	
	using Meters2f		= _hidden_::Meters< float2 >;
	using Meters2u		= _hidden_::Meters< uint2 >;
	using Meters2i		= _hidden_::Meters< int2 >;

	using UNorm2f		= _hidden_::UNorm< float2 >;
	using SNorm2f		= _hidden_::SNorm< float2 >;


} // AE::App
