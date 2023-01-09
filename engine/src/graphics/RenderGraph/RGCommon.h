// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/GraphicsImpl.h"

namespace AE::RG
{
namespace _hidden_
{
	using namespace AE::Graphics;
	
	#if AE_ENABLE_DATA_RACE_CHECK
	using AE::Threading::SingleThreadCheck;
	#endif

} // _hidden_

	using namespace AE::Base;

} // AE::RG
