// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "../shared/UnitTest_Shared.h"

#include "ScriptObjects/DescriptorSetLayout.h"
#include "ScriptObjects/ObjectStorage.h"
using namespace AE::PipelineCompiler;


template <typename T>
ND_ String  ToGLSL (T &ptr)
{
    String  hdr     = "\n";
    String  fields  = "Buffer {\n";

    CHECK_ERR( ptr->ToGLSL( true, INOUT hdr, INOUT fields ));
    return hdr << fields << "}\n";
}

template <typename T>
ND_ String  ToMSL (T &ptr)
{
    String  hdr = "\n";
    CHECK_ERR( ptr->ToMSL( INOUT hdr ));
    return hdr;
}

template <typename T>
ND_ String  ToCPP (T &ptr)
{
    String  src = "\n";
    CHECK_ERR( ptr->ToCPP( INOUT src ));
    return src;
}
