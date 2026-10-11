// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Common.h"


int main (const int argc, char const* argv[])
{
	Unused( argc, argv );
	StaticLogger::LoggerScope log{};

	#if 0
	{
		FileRDataSource		src {Path{R"()"}};
		FileWDataSource		dst {Path{R"()"}};

		CHECK_FATAL( ConvertAEImage( src, dst ));
	}
	#endif

	#if 0
	{
	#endif

	return 0;
}

#include "graphics_rhi/Private/ImageMemView.cpp"
#include "graphics_rhi/Private/BufferMemView.cpp"
