// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{
	
	//
	// Non-instancable base class
	//

	class Noninstancable
	{
	protected:
		Noninstancable () = delete;

		//~Noninstancable () = delete;

		Noninstancable (const Noninstancable &) = delete;

		Noninstancable (Noninstancable &&) = delete;

		Noninstancable& operator = (const Noninstancable &) = delete;

		Noninstancable& operator = (Noninstancable &&) = delete;
	};

}	// AE::Base
