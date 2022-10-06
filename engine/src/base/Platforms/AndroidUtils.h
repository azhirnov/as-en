// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_PLATFORM_ANDROID
# include "base/Containers/NtStringView.h"
# include "base/Platforms/UnixUtils.h"

namespace AE::Base
{

	//
	// Android Utils
	//

	struct AndroidUtils final : UnixUtils
	{
	};

}	// AE::Base

#endif // AE_PLATFORM_ANDROID
