// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Defines/StdInclude.h"

#include <filesystem>
namespace _ae_fs_ = std::filesystem;

#include "base/Defines/Undef.h"
#include "base/Algorithms/Cast.h"

namespace AE::Base
{
	using Path = _ae_fs_::path;

} // AE::Base
