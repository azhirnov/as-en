// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ObjectStorage.h"

namespace AE::AssetPacker
{
	class ScriptBaseLayout;


	//
	// Script UI Widget
	//

	class ScriptUIWidget final : public EnableScriptRC
	{
	// types
	public:


	// variables
	private:
		ScriptRC<ScriptBaseLayout>		_root;


	// methods
	public:
		ScriptUIWidget ();
		~ScriptUIWidget ();


		void  Initialize (const ScriptRC<ScriptBaseLayout> &)	__Th___;
		void  Store (const String &nameInArchive)				__Th___;

		static void  Bind (const ScriptEnginePtr &se)			__Th___;
	};

	using ScriptUIWidgetPtr = ScriptRC< ScriptUIWidget >;


} // AE::AssetPacker
