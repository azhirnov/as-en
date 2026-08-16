// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "res_pack/asset_packer/ScriptObjects/ObjectStorage.h"

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


} // AE::AssetPacker
