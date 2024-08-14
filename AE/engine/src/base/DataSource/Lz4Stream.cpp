// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_LZ4
# include "base/Defines/StdInclude.h"
# include "lz4.h"
# include "lz4hc.h"
# include "base/DataSource/Lz4Stream.h"

# if LZ4_VERSION_MAJOR != 1 or LZ4_VERSION_MINOR != 10
#	error required lz4 version 1.10.0
# endif

namespace AE::Base
{

/*
=================================================
	Compress
=================================================
*/
	bool  Lz4Utils::Compress (OUT void* dstData, INOUT Bytes &dstSize,
							  const void* srcData, Bytes srcSize,
							  const Lz4WStream::Config &cfg) __NE___
	{
		CHECK_ERR( srcSize <= MaxValue<int>() );
		CHECK_ERR( dstSize <= MaxValue<int>() );

		if ( cfg.hc )
		{
			const int lvl		= int(Lerp( float(LZ4HC_CLEVEL_MIN), float(LZ4HC_CLEVEL_MAX), cfg.hcLevel ) + 0.5f);
			const int comp_size	= LZ4_compress_HC( Cast<char>(srcData), OUT Cast<char>(dstData), int(srcSize), int(dstSize), lvl );
			CHECK_ERR( comp_size > 0 );

			dstSize = Bytes{uint(comp_size)};
			return true;
		}
		else
		{
			const int comp_size = LZ4_compress_default( Cast<char>(srcData), OUT Cast<char>(dstData), int(srcSize), int(dstSize) );
			CHECK_ERR( comp_size > 0 );

			dstSize = Bytes{uint(comp_size)};
			return true;
		}
	}

/*
=================================================
	Decompress
=================================================
*/
	bool  Lz4Utils::Decompress (OUT void* dstData, INOUT Bytes &dstSize,
								const void* srcData, Bytes srcSize) __NE___
	{
		CHECK_ERR( srcSize <= MaxValue<int>() );
		CHECK_ERR( dstSize <= MaxValue<int>() );

		int decomp_size = LZ4_decompress_safe( Cast<char>(srcData), OUT Cast<char>(dstData), int(srcSize), int(dstSize) );
		CHECK_ERR( decomp_size > 0 );

		dstSize = Bytes{uint(decomp_size)};
		return true;
	}


} // AE::Base

#endif // AE_ENABLE_LZ4
