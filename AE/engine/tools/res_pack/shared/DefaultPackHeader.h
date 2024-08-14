// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Utils/Version.h"

namespace AE::Base
{

	struct DefaultPackHeader
	{
		uint	name;
		uint	ver;
		uint	license;

		constexpr DefaultPackHeader ()									__NE___	{}
		constexpr DefaultPackHeader (Version2 ver, uint name, uint lic)	__NE___ : name{name}, ver{ver.ToUInt()}, license{lic} {}

		ND_ constexpr Version2	Version ()								C_NE___	{ return Version2::FromUInt( ver ); }
	};

	StaticAssert( sizeof(DefaultPackHeader) == 12 );

} // AE::Base
