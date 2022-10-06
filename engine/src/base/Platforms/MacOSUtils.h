// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_MACOS
# include "base/Platforms/UnixUtils.h"

namespace AE::Base
{

	//
	// MacOS Utils
	//

	struct MacOSUtils final : UnixUtils
	{
	};

}	// AE::Base

#endif // AE_PLATFORM_MACOS
