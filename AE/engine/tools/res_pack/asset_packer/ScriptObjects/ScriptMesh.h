// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ObjectStorage.h"

namespace AE::AssetPacker
{

	//
	// Script Mesh
	//

	class ScriptMesh final : public EnableScriptRC
	{
	// types
	private:


	// variables
	private:


	// methods
	public:
		ScriptMesh ();
		~ScriptMesh ();

		static void  Bind (const ScriptEnginePtr &se) __Th___;
	};

	using ScriptMeshPtr = ScriptRC< ScriptMesh >;


} // AE::AssetPacker
