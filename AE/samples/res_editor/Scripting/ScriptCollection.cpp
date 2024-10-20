// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"

namespace AE::ResEditor
{
	using namespace AE::Scripting;

/*
=================================================
	Add*
=================================================
*/
	void  ScriptCollection::Add1 (const String &key, const ScriptDynamicDimPtr &value) __Th___ {
		_Add( key, value );
	}

	void  ScriptCollection::Add2 (const String &key, const ScriptDynamicInt4Ptr &value) __Th___ {
		_Add( key, value );
	}

	void  ScriptCollection::Add3 (const String &key, const ScriptDynamicFloat4Ptr &value) __Th___ {
		_Add( key, value );
	}

	void  ScriptCollection::Add4 (const String &key, const ScriptImagePtr &value) __Th___ {
		_Add( key, value );
	}

	void  ScriptCollection::Add5 (const String &key, const ScriptVideoImagePtr &value) __Th___ {
		_Add( key, value );
	}

	void  ScriptCollection::Add6 (const String &key, const ScriptBufferPtr &value) __Th___ {
		_Add( key, value );
	}

	void  ScriptCollection::Add7 (const String &key, const ScriptGeomSourcePtr &value) __Th___ {
		_Add( key, value );
	}

	void  ScriptCollection::Add8 (const String &key, const ScriptBaseControllerPtr &value) __Th___ {
		_Add( key, value );
	}

	void  ScriptCollection::Add9  (const String &key, const ScriptRTGeometryPtr &value) __Th___ {
		_Add( key, value );
	}

	void  ScriptCollection::Add10 (const String &key, const ScriptRTScenePtr &value) __Th___ {
		_Add( key, value );
	}

	template <typename T>
	void  ScriptCollection::_Add (const String &key, const T &value) __Th___
	{
		CHECK_THROW_MSG( not key.empty() );
		CHECK_THROW_MSG( value );

		CHECK_THROW_MSG( _map.emplace( key, value ).second,
			"Variable '"s << key << "' is already exists" );
	}

/*
=================================================
	Get*
=================================================
*/
	ScriptDynamicDim*  ScriptCollection::GetDynDim (const String &key) C_Th___ {
		return _Get<ScriptDynamicDim>( key );
	}

	ScriptDynamicInt4*  ScriptCollection::GetDynI4 (const String &key) C_Th___ {
		return _Get<ScriptDynamicInt4>( key );
	}

	ScriptDynamicFloat4*  ScriptCollection::GetDynF4 (const String &key) C_Th___ {
		return _Get<ScriptDynamicFloat4>( key );
	}

	ScriptImage*  ScriptCollection::GetImage (const String &key) C_Th___ {
		return _Get<ScriptImage>( key );
	}

	ScriptVideoImage*  ScriptCollection::GetVideoImage (const String &key) C_Th___ {
		return _Get<ScriptVideoImage>( key );
	}

	ScriptBuffer*  ScriptCollection::GetBuffer (const String &key) C_Th___ {
		return _Get<ScriptBuffer>( key );
	}

	ScriptGeomSource*  ScriptCollection::GetGeomSource (const String &key) C_Th___ {
		return _Get<ScriptGeomSource>( key );
	}

	ScriptBaseController*  ScriptCollection::GetController (const String &key) C_Th___ {
		return _Get<ScriptBaseController>( key );
	}

	ScriptRTGeometry*  ScriptCollection::GetRTGeometry (const String &key) C_Th___ {
		return _Get<ScriptRTGeometry>( key );
	}

	ScriptRTScene*  ScriptCollection::GetRTScene (const String &key) C_Th___ {
		return _Get<ScriptRTScene>( key );
	}

	template <typename T>
	T*  ScriptCollection::_Get (const String &key) C_Th___
	{
		auto	it = _map.find( key );
		CHECK_THROW_MSG( it != _map.end(),
			"Variable '"s << key << "' is not exists in collection" );

		T*	result = DynCast<T>( it->second.Get() );
		CHECK_THROW_MSG( result != null,
			"Variable '"s << key << "' is not a '" << TypeNameOf<T>() << "' type" );

		return ScriptRC<T>{result}.Detach();
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptCollection::Bind (const ScriptEnginePtr &se) __Th___
	{
		ClassBinder<ScriptCollection>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Add item to collection." );
		binder.AddMethod( &ScriptCollection::Add1,			"Add",			{"key", "value"} );
		binder.AddMethod( &ScriptCollection::Add2,			"Add",			{"key", "value"} );
		binder.AddMethod( &ScriptCollection::Add3,			"Add",			{"key", "value"} );
		binder.AddMethod( &ScriptCollection::Add4,			"Add",			{"key", "value"} );
		binder.AddMethod( &ScriptCollection::Add5,			"Add",			{"key", "value"} );
		binder.AddMethod( &ScriptCollection::Add6,			"Add",			{"key", "value"} );
		binder.AddMethod( &ScriptCollection::Add7,			"Add",			{"key", "value"} );
		binder.AddMethod( &ScriptCollection::Add8,			"Add",			{"key", "value"} );
		binder.AddMethod( &ScriptCollection::Add9,			"Add",			{"key", "value"} );
		binder.AddMethod( &ScriptCollection::Add10,			"Add",			{"key", "value"} );

		binder.Comment( "Returns dynamic values." );
		binder.AddMethod( &ScriptCollection::GetDynDim,		"DynDim",		{"key"} );
		binder.AddMethod( &ScriptCollection::GetDynI4,		"DynInt4",		{"key"} );
		binder.AddMethod( &ScriptCollection::GetDynF4,		"DynFloat4",	{"key"} );

		binder.Comment( "Returns resources." );
		binder.AddMethod( &ScriptCollection::GetImage,		"Image",		{"key"} );
		binder.AddMethod( &ScriptCollection::GetVideoImage,	"VideoImage",	{"key"} );
		binder.AddMethod( &ScriptCollection::GetBuffer,		"Buffer",		{"key"} );
		binder.AddMethod( &ScriptCollection::GetGeomSource,	"Geometry",		{"key"} );
		binder.AddMethod( &ScriptCollection::GetController,	"Controller",	{"key"} );
		binder.AddMethod( &ScriptCollection::GetRTGeometry,	"RTGeometry",	{"key"} );
		binder.AddMethod( &ScriptCollection::GetRTScene,	"RTScene",		{"key"} );
	}


} // AE::ResEditor
