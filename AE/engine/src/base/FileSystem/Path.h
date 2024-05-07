// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#ifdef AE_ENABLE_GFS
#  include "filesystem.hpp"
	namespace _ae_fs_ = ghc::filesystem;
#else
#	include <filesystem>
	namespace _ae_fs_ = std::filesystem;
#endif
#include "base/Defines/Undef.h"
#include "base/Common.h"

namespace AE::Base
{
	using Path = _ae_fs_::path;

} // AE::Base
