// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Human language tools
*/

#pragma once

#include "hulang/HuLang.pch.h"

namespace AE::HuLang::_hidden_
{
    // tag: NamedID UID
    static constexpr uint   NamedIDs_Start  = 3 << 24;

} // AE::HuLang::_hidden_


namespace AE::HuLang
{
    using namespace AE::Base;


    using ConsoleCmdName        = NamedID< 64, HuLang::_hidden_::NamedIDs_Start + 1, AE_OPTIMIZE_IDS >;


} // AE::HuLang
