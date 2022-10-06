// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/CompileTime/FunctionInfo.h"

#include "base/Math/Bytes.h"
#include "base/Math/BitMath.h"
#include "base/Math/Vec.h"

#include "base/Utils/HandleTmpl.h"
#include "base/Utils/TypeId.h"
#include "base/Utils/Noncopyable.h"
#include "base/Utils/RefCounter.h"

#include "base/Containers/FixedArray.h"
#include "base/Containers/ArrayView.h"

#include "base/CompileTime/Math.h"

#include "base/Memory/UntypedAllocator.h"
#include "base/Algorithms/ArrayUtils.h"

#include "threading/Primitives/DataRaceCheck.h"

#define AE_ECS_VALIDATE_SYSTEM_FN

#ifndef AE_ECS_ENABLE_DEFAULT_MESSAGES
#	define AE_ECS_ENABLE_DEFAULT_MESSAGES	1
#endif

namespace AE::ECS::_hidden_
{
	// tag: HandleTmpl UID
	static constexpr uint	ECSIDs_Start	= 4 << 24;

} // AE::ECS::_hidden_


namespace AE::ECS
{
	using namespace AE::Base;

	namespace Components {}
	namespace SingleComponents {}
	namespace Systems {}
	namespace Events {}
	
#	if AE_ENABLE_DATA_RACE_CHECK
	using Threading::DataRaceCheck;
#	endif

	using Threading::Atomic;

	using EntityID	= HandleTmpl< 16, 16, _hidden_::ECSIDs_Start + 1 >;
	using QueryID	= HandleTmpl< 16, 16, _hidden_::ECSIDs_Start + 2 >;
	

	struct ECS_Config
	{
		static constexpr uint	MaxComponents				= 4 * 64;
		static constexpr uint	MaxComponentsPerArchetype	= 64;
		static constexpr uint	InitialtStorageSize			= 16;
	};

	class Registry;

}	// AE::ECS
