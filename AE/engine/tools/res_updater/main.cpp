// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "pch/Base.h"

using namespace AE;
using namespace AE::Base;


extern bool  ConvertAEImage (RDataSource &src, WDataSource &dst);


int main (const int argc, char const* argv[])
{
	Unused( argc, argv );
	StaticLogger::LoggerScope log{};

	{
		FileRDataSource		src {Path{R"()"}};
		FileWDataSource		dst {Path{R"()"}};

		CHECK_FATAL( ConvertAEImage( src, dst ));
	}

	return 0;
}

#include "graphics_rhi/Private/ImageMemView.cpp"
