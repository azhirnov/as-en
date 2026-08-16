// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "Scripting/ScriptCommon.h"

namespace AE::ResEditor
{

	//
	// Script Collection
	//
	class ScriptCollection final : public EnableScriptRC
	{
	// types
	private:
		using ObjArray_t = Array< ScriptRC<EnableScriptRC> >;


	// variables
	private:
		HashMap< String, ScriptRC<EnableScriptRC> >		_map;
		HashMap< String, ObjArray_t >					_arrMap;


	// methods
	public:
		ScriptCollection () {}

		void  Add1  (const String &key, const ScriptDynamicDimPtr &value)				__Th___;
		void  Add2  (const String &key, const ScriptDynamicInt4Ptr &value)				__Th___;
		void  Add3  (const String &key, const ScriptDynamicFloat4Ptr &value)			__Th___;
		void  Add4  (const String &key, const ScriptImagePtr &value)					__Th___;
		void  Add5  (const String &key, const ScriptVideoImagePtr &value)				__Th___;
		void  Add6  (const String &key, const ScriptBufferPtr &value)					__Th___;
		void  Add7  (const String &key, const ScriptGeomSourcePtr &value)				__Th___;
		void  Add8  (const String &key, const ScriptBaseControllerPtr &value)			__Th___;
		void  Add9  (const String &key, const ScriptRTGeometryPtr &value)				__Th___;
		void  Add10 (const String &key, const ScriptRTScenePtr &value)					__Th___;

		void  Add11 (const String &key, const ScriptArray<ScriptImagePtr> &)			__Th___;
		void  Add12 (const String &key, const ScriptArray<ScriptBufferPtr> &)			__Th___;

		ScriptDynamicDim*		GetDynDim (const String &key)							C_Th___;
		ScriptDynamicInt4*		GetDynI4 (const String &key)							C_Th___;
		ScriptDynamicFloat4*	GetDynF4 (const String &key)							C_Th___;
		ScriptImage*			GetImage (const String &key)							C_Th___;
		ScriptVideoImage*		GetVideoImage (const String &key)						C_Th___;
		ScriptBuffer*			GetBuffer (const String &key)							C_Th___;
		ScriptGeomSource*		GetGeomSource (const String &key)						C_Th___;
		ScriptBaseController*	GetController (const String &key)						C_Th___;
		ScriptRTGeometry*		GetRTGeometry (const String &key)						C_Th___;
		ScriptRTScene*			GetRTScene (const String &key)							C_Th___;

		void  GetImageArray (const String &key, OUT ScriptArray<ScriptImagePtr> &)		C_Th___;
		void  GetBufferArray (const String &key, OUT ScriptArray<ScriptBufferPtr> &)	C_Th___;

		static void  Bind (const ScriptEnginePtr &se)									__Th___;


	private:
		template <typename T>
		void  _Add (const String &key, const T &value)									__Th___;

		template <typename T>
		void  _AddArr (const String &key, const ScriptArray<T> &value)					__Th___;

		template <typename T>
		T*    _Get (const String &key)													C_Th___;

		template <typename T>
		void  _GetArr (const String &key, OUT ScriptArray<T> &)							C_Th___;
	};


} // AE::ResEditor
