// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_LFAS_ENABLED
# error AE_LFAS_ENABLED is required
#endif

#include "base/Math/Byte.h"
#include "base/Utils/Helpers.h"
#include "base/Memory/MemUtils.h"
#include "base/Algorithms/StringUtils.h"
#include "base/Containers/ArrayView.h"
#include "base/Math/Random.h"
#include "base/Platforms/ThreadUtils.h"

namespace LFAS
{
    using AE::uint;
    using AE::ulong;
    using AE::usize;
    using AE::Base::Bytes;
    using AE::Base::operator "" _b;
    using AE::Base::String;
    using AE::Base::StringView;
    using AE::Base::Array;
    using AE::Base::ArrayView;
    using AE::Base::StaticArray;
    using AE::Base::HashSet;
    using AE::Base::HashMap;
    using AE::Base::UMax;
    using AE::Base::Default;
    using AE::Base::Noncopyable;
    using AE::Base::SharedPtr;
    using AE::Base::Function;
    using AE::Base::secondsf;
    using AE::Base::IsInteger;
    using AE::Base::IsClass;
    using AE::Base::Conditional;
    using AE::Base::DeferredTemplate;
    using AE::Base::ValueToType;
    using AE::Base::SizeOf;
    using AE::Base::AddressOf;
    using AE::Base::OffsetOf;
    using AE::Base::RVRef;

    using AE::Math::Random;
    using AE::Math::Max;
    using AE::Base::FwdArg;

} // LFAS
