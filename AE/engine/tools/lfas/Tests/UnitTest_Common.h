// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "CPP_VM/VirtualMachine.h"
#include "CPP_VM/Atomic.h"
#include "CPP_VM/Mutex.h"
#include "CPP_VM/Storage.h"

#include "shared/UnitTest_Shared.h"
#include "shared/LfDebugInstanceCounter.h"

#ifdef AE_ENABLE_DATA_RACE_CHECK
# if AE_ENABLE_DATA_RACE_CHECK
#   error AE_ENABLE_DATA_RACE_CHECK must be 0
# endif
#endif

#define DRC_EXLOCK( ... )
#define DRC_SHAREDLOCK( ... )
#define DRC_ONLY( ... )

using namespace LFAS;
using namespace LFAS::CPP;

#include "base/Math/BitMath.h"
#include "base/Math/Math.h"
#include "base/Memory/UntypedAllocator.h"
#include "base/Memory/IAllocator.h"
