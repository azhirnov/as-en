// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ScriptMesh.h"

#include "scripting/Impl/ClassBinder.h"

AE_DECL_SCRIPT_OBJ_RC(  AE::AssetPacker::ScriptMesh,    "Mesh" );


namespace AE::AssetPacker
{

/*
=================================================
    constructor
=================================================
*/
    ScriptMesh::ScriptMesh ()
    {}

/*
=================================================
    destructor
=================================================
*/
    ScriptMesh::~ScriptMesh ()
    {}

/*
=================================================
    Bind
=================================================
*/
    void  ScriptMesh::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptMesh>  binder{ se };
        binder.CreateRef();

    }


} // AE::AssetPacker
