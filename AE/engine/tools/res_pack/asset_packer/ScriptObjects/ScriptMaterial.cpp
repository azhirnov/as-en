// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ScriptMaterial.h"

#include "scripting/Impl/ClassBinder.h"

AE_DECL_SCRIPT_OBJ_RC(	AE::AssetPacker::ScriptMaterial,	"Material" );


namespace AE::AssetPacker
{

/*
=================================================
	constructor
=================================================
*/
	ScriptMaterial::ScriptMaterial ()
	{}

/*
=================================================
	destructor
=================================================
*/
	ScriptMaterial::~ScriptMaterial ()
	{}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptMaterial::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptMaterial>	binder{ se };
		binder.CreateRef();

	}


} // AE::AssetPacker
