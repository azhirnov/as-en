// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Common.h"

namespace AE::Base
{
	//
	// Stack Trace
	//

	class StackTrace
	{
	public:

		ND_ static String  ToString (StringView skipFile, StringView prefix = "    ") __Th___;
	};

} // AE::Base
