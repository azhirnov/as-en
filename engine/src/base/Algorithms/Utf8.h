// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_UTF8PROC

# include "base/Algorithms/Cast.h"
# include "utf8proc.h"

namespace AE::Base
{

/*
=================================================
	Utf8Decode
=================================================
*/
	forceinline char32_t  Utf8Decode (const char *str, usize length, INOUT usize &pos)
	{
		ASSERT( pos < length );

		utf8proc_int32_t	ref;
		utf8proc_ssize_t	res = utf8proc_iterate( Cast<utf8proc_uint8_t>(str + pos), length - pos, OUT &ref );
		
		ASSERT( res >= 0 );

		pos = res > 0 ? pos + res : pos;
		return ref;
	}

	forceinline char32_t  Utf8Decode (StringView str, INOUT usize &pos)
	{
		return Utf8Decode( str.data(), str.length(), INOUT pos );
	}


}	// AE::Base

#endif	// AE_ENABLE_UTF8PROC
