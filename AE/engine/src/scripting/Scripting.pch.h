// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

// AngelScript + Addons //
#ifdef AE_ENABLE_ANGELSCRIPT
# include "base/Defines/StdInclude.h"

# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wunused-parameter"
# endif

# include "angelscript.h"
# include "scriptarray.h"
# include "scriptstdstring.h"

# if defined(AE_COMPILER_CLANG) or defined(AE_COMPILER_CLANG_CL)
#	pragma clang diagnostic pop
# endif

#endif

#include "pch/Base.h"
