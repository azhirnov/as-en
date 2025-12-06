// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Scripting/ScriptExe.h"

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
	Add*
=================================================
*/
	void  ScriptCollection::Add11 (const String &key, const ScriptArray<ScriptImagePtr> &value) __Th___ {
		_AddArr< ScriptImagePtr >( key, value );
	}

	void  ScriptCollection::Add12 (const String &key, const ScriptArray<ScriptBufferPtr> &value) __Th___ {
		_AddArr< ScriptBufferPtr >( key, value );
	}

	template <typename T>
	void  ScriptCollection::_AddArr (const String &key, const ScriptArray<T> &value) __Th___
	{
		CHECK_THROW_MSG( not key.empty() );

		auto [it, inserted] = _arrMap.emplace( key, ObjArray_t{} );

		CHECK_THROW_MSG( inserted,
			"Variable '"s << key << "' is already exists" );

		it->second.reserve( value.size() );

		for (auto& obj : value) {
			it->second.push_back( obj );
		}
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
	Get*
=================================================
*/
	void  ScriptCollection::GetImageArray (const String &key, OUT ScriptArray<ScriptImagePtr> &result) C_Th___ {
		_GetArr< ScriptImagePtr >( key, OUT result );
	}

	void  ScriptCollection::GetBufferArray (const String &key, OUT ScriptArray<ScriptBufferPtr> &result) C_Th___ {
		_GetArr< ScriptBufferPtr >( key, OUT result );
	}

	template <typename T>
	void  ScriptCollection::_GetArr (const String &key, OUT ScriptArray<T> &result) C_Th___
	{
		using P = AE::Scripting::AngelScriptHelper::RemoveSharedPtr< T >;

		result.clear();

		auto	it = _arrMap.find( key );
		CHECK_THROW_MSG( it != _arrMap.end(),
			"Variable '"s << key << "' is not exists in collection" );

		if ( it->second.empty() )
			return;

		P*	temp = DynCast<P>( it->second[0].Get() );
		CHECK_THROW_MSG( temp != null,
			"Variable '"s << key << "' is not a '" << TypeNameOf<P>() << "' type" );

		result.reserve( it->second.size() );

		for (auto& obj : it->second) {
			result.push_back( obj );
		}
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
		AS_METHOD( binder, ScriptCollection::Add1,			"Add",			{"key", "value"} );
		AS_METHOD( binder, ScriptCollection::Add2,			"Add",			{"key", "value"} );
		AS_METHOD( binder, ScriptCollection::Add3,			"Add",			{"key", "value"} );
		AS_METHOD( binder, ScriptCollection::Add4,			"Add",			{"key", "value"} );
		AS_METHOD( binder, ScriptCollection::Add5,			"Add",			{"key", "value"} );
		AS_METHOD( binder, ScriptCollection::Add6,			"Add",			{"key", "value"} );
		AS_METHOD( binder, ScriptCollection::Add7,			"Add",			{"key", "value"} );
		AS_METHOD( binder, ScriptCollection::Add8,			"Add",			{"key", "value"} );
		AS_METHOD( binder, ScriptCollection::Add9,			"Add",			{"key", "value"} );
		AS_METHOD( binder, ScriptCollection::Add10,			"Add",			{"key", "value"} );
		AS_METHOD( binder, ScriptCollection::Add11,			"Add",			{"key", "value"} );
		AS_METHOD( binder, ScriptCollection::Add12,			"Add",			{"key", "value"} );

		binder.Comment( "Returns dynamic values." );
		AS_METHOD( binder, ScriptCollection::GetDynDim,		"DynDim",		{"key"} );
		AS_METHOD( binder, ScriptCollection::GetDynI4,		"DynInt4",		{"key"} );
		AS_METHOD( binder, ScriptCollection::GetDynF4,		"DynFloat4",	{"key"} );

		binder.Comment( "Returns resources." );
		AS_METHOD( binder, ScriptCollection::GetImage,		"Image",		{"key"} );
		AS_METHOD( binder, ScriptCollection::GetVideoImage,	"VideoImage",	{"key"} );
		AS_METHOD( binder, ScriptCollection::GetBuffer,		"Buffer",		{"key"} );
		AS_METHOD( binder, ScriptCollection::GetGeomSource,	"Geometry",		{"key"} );
		AS_METHOD( binder, ScriptCollection::GetController,	"Controller",	{"key"} );
		AS_METHOD( binder, ScriptCollection::GetRTGeometry,	"RTGeometry",	{"key"} );
		AS_METHOD( binder, ScriptCollection::GetRTScene,	"RTScene",		{"key"} );
		AS_METHOD( binder, ScriptCollection::GetImageArray,	"ImageArray",	{"key", "result"} );
		AS_METHOD( binder, ScriptCollection::GetBufferArray,"BufferArray",	{"key", "result"} );
	}


} // AE::ResEditor
