// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_pack/asset_packer/ScriptObjects/ObjectStorage.h"

namespace AE::AssetPacker
{

	//
	// Script Material
	//

	class ScriptMaterial final : public EnableScriptRC
	{
	// types
	private:


	// variables
	private:


	// methods
	public:
		ScriptMaterial ();
		~ScriptMaterial ();

		static void  Bind (const ScriptEnginePtr &se) __Th___;
	};


} // AE::AssetPacker
