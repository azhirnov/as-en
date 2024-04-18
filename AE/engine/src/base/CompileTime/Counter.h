// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_FAMETA_COUNTER
# include "external/shared/fameta-counter/counter.hpp"
# include "base/Common.h"

namespace AE::Base
{

	template <typename UniqueType>
	using CT_Counter = fameta::counter< UniqueType >;


} // AE::Base

#endif // AE_ENABLE_FAMETA_COUNTER
