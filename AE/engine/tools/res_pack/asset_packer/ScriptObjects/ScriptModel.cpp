// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ScriptModel.h"

#include "scripting/Impl/ClassBinder.h"

AE_DECL_SCRIPT_OBJ_RC(  AE::AssetPacker::ScriptModel,   "Model" );


namespace AE::AssetPacker
{

/*
=================================================
    constructor
=================================================
*/
    ScriptModel::ScriptModel ()
    {}

/*
=================================================
    destructor
=================================================
*/
    ScriptModel::~ScriptModel ()
    {}

/*
=================================================
    Bind
=================================================
*/
    void  ScriptModel::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptModel> binder{ se };
        binder.CreateRef();

    }


} // AE::AssetPacker
