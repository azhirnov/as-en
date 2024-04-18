
#ifndef AE_ENABLE_BROTLI
#	error AE_ENABLE_BROTLI required
#endif

#include <cstddef>
#include "brotli/decode.h"
#include "brotli/encode.h"


#ifdef ANDROID
# include "TestHelper.h"

extern int AE_Brotli_Test ()
#else
int main ()
#endif
{
	{
		auto*	inst = BrotliDecoderCreateInstance( nullptr, nullptr, nullptr );
		if ( ! inst )
			return 1;

		BrotliDecoderDestroyInstance( inst );
	}
	{
		auto*	inst = BrotliEncoderCreateInstance( nullptr, nullptr, nullptr );
		if ( ! inst )
			return 2;

		BrotliEncoderDestroyInstance( inst );
	}
	return 0;
}
