// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ObjectStorage.h"

namespace AE::AssetPacker
{

    //
    // Script Model
    //

    class ScriptModel final : public EnableScriptRC
    {
    // types
    private:


    // variables
    private:


    // methods
    public:
        ScriptModel ();
        ~ScriptModel ();

        static void  Bind (const ScriptEnginePtr &se) __Th___;
    };

    using ScriptModelPtr = ScriptRC< ScriptModel >;


} // AE::AssetPacker
