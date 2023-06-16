// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"

namespace AE::ResEditor
{

	//
	// Script Collection
	//
	class ScriptCollection final : public EnableScriptRC
	{
	// variables
	private:
		HashMap< String, ScriptRC<EnableScriptRC> >		_map;


	// methods
	public:
		ScriptCollection () {}
		
		void  Add1 (const String &key, const ScriptDynamicDimPtr &value)		__Th___;
		void  Add2 (const String &key, const ScriptDynamicInt4Ptr &value)		__Th___;
		void  Add3 (const String &key, const ScriptDynamicFloat4Ptr &value)		__Th___;
		void  Add4 (const String &key, const ScriptImagePtr &value)				__Th___;
		void  Add5 (const String &key, const ScriptVideoImagePtr &value)		__Th___;
		void  Add6 (const String &key, const ScriptBufferPtr &value)			__Th___;
		void  Add7 (const String &key, const ScriptGeomSourcePtr &value)		__Th___;
		void  Add8 (const String &key, const ScriptBaseControllerPtr &value)	__Th___;

		ScriptDynamicDim*		GetDynDim (const String &key)					C_Th___;
		ScriptDynamicInt4*		GetDynI4 (const String &key)					C_Th___;
		ScriptDynamicFloat4*	GetDynF4 (const String &key)					C_Th___;
		ScriptImage*			GetImage (const String &key)					C_Th___;
		ScriptVideoImage*		GetVideoImage (const String &key)				C_Th___;
		ScriptBuffer*			GetBuffer (const String &key)					C_Th___;
		ScriptGeomSource*		GetGeomSource (const String &key)				C_Th___;
		ScriptBaseController*	GetController (const String &key)				C_Th___;

		static void  Bind (const ScriptEnginePtr &se)							__Th___;


	private:
		template <typename T>
		void  _Add (const String &key, const T &value)							__Th___;

		template <typename T>
		T*    _Get (const String &key)											C_Th___;
	};
	

} // AE::ResEditor
