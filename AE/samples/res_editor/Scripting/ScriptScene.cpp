// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Scripting/ScriptExe.h"
#include "Core/EditorUI.h"

#include "Scripting/ScriptBasePass.cpp.h"
#include "_data/cpp/types.h"

#include "Scripting/PipelineCompiler.inl.h"

namespace AE::ResEditor
{
	using DebugModeBits = EnumSet<IPass::EDebugMode>;

/*
=================================================
	destructor
=================================================
*/
	ScriptScene::~ScriptScene ()
	{
		if ( not _scene )
			AE_LOGW( "Unused scene '"s << _dbgName << "'" );
	}

/*
=================================================
	Name
=================================================
*/
	void  ScriptScene::Name (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _scene,
			"Resource is already created, can not change debug name" );

		_dbgName = name.substr( 0, ResNameMaxLen );
	}

/*
=================================================
	InputController
=================================================
*/
	void  ScriptScene::InputController (const ScriptBaseControllerPtr &value) __Th___
	{
		CHECK_THROW_MSG( value );

		_controller = value;
	}

/*
=================================================
	InputGeometry*
=================================================
*/
	void  ScriptScene::InputGeometry1 (const ScriptGeomSourcePtr &geom, const packed_float3 &pos, const packed_float3 &rotation, float scale) __Th___
	{
		CHECK_THROW_MSG( geom );

		auto&	dst		= _geomInstances.emplace_back();
		dst.geom		= geom;
		dst.transform	=	float4x4::RotateX( Rad{rotation.x} )	*
							float4x4::RotateY( Rad{rotation.y} )	*
							float4x4::RotateZ( Rad{rotation.z} )	*
							float4x4::Translate( pos )				*
							float4x4::Scale( scale );
	}

	void  ScriptScene::InputGeometry2 (const ScriptGeomSourcePtr &geom, const packed_float3 &pos) __Th___
	{
		CHECK_THROW_MSG( geom );

		auto&	dst		= _geomInstances.emplace_back();
		dst.geom		= geom;
		dst.transform	= float4x4::Translate( pos );
	}

	void  ScriptScene::InputGeometry3 (const ScriptGeomSourcePtr &geom) __Th___
	{
		CHECK_THROW_MSG( geom );

		auto&	dst		= _geomInstances.emplace_back();
		dst.geom		= geom;
		dst.transform	= float4x4::Identity();
	}

	void  ScriptScene::InputGeometry4 (const ScriptGeomSourcePtr &geom, const packed_float4x4 &mat) __Th___
	{
		CHECK_THROW_MSG( geom );

		auto&	dst		= _geomInstances.emplace_back();
		dst.geom		= geom;
		dst.transform	= float4x4{mat};
	}

/*
=================================================
	AddGraphicsPass
=================================================
*/
	ScriptSceneGraphicsPass*  ScriptScene::AddGraphicsPass (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _geomInstances.empty() );

		++_passCount;
		return ScriptSceneGraphicsPassPtr{ new ScriptSceneGraphicsPass{ ScriptScenePtr{this}, name }}.Detach();
	}

/*
=================================================
	AddRayTracingPass
=================================================
*/
	ScriptSceneRayTracingPass*  ScriptScene::AddRayTracingPass (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _geomInstances.empty() );

		++_passCount;
		_hasRayTracingPass = true;
		return ScriptSceneRayTracingPassPtr{ new ScriptSceneRayTracingPass{ ScriptScenePtr{this}, name }}.Detach();
	}

/*
=================================================
	AddRayQueryPass
=================================================
*/
	ScriptSceneRayQueryPass*  ScriptScene::AddRayQueryPass (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _geomInstances.empty() );

		++_passCount;
		_hasRayTracingPass = true;
		return ScriptSceneRayQueryPassPtr{ new ScriptSceneRayQueryPass{ ScriptScenePtr{this}, name }}.Detach();
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptScene::Bind (const ScriptEnginePtr &se) __Th___
	{
		using namespace Scripting;

		ClassBinder<ScriptScene>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Set resource name. It is used for debugging." );
		AS_METHOD( binder, ScriptScene::Name,				"Name",					{} );

		binder.Comment( "Attach geometry to scene." );
		AS_METHOD( binder, ScriptScene::InputGeometry1,		"Add",					{"geometry", "position", "rotationInRads", "scale"} );
		AS_METHOD( binder, ScriptScene::InputGeometry2,		"Add",					{"geometry", "position"} );
		AS_METHOD( binder, ScriptScene::InputGeometry3,		"Add",					{"geometry"} );
		AS_METHOD( binder, ScriptScene::InputGeometry4,		"Add",					{"geometry", "transform"} );

		binder.Comment( "Set camera to scene." );
		AS_METHOD( binder, ScriptScene::InputController,	"Set",					{"controller"} );

		binder.Comment( "Add graphics pass. It will link geometries with pipelines and draw it." );
		AS_METHOD( binder, ScriptScene::AddGraphicsPass,	"AddGraphicsPass",		{"name"} );
		AS_METHOD( binder, ScriptScene::AddRayTracingPass,	"AddRayTracingPass",	{"name"} );
		AS_METHOD( binder, ScriptScene::AddRayQueryPass,	"AddRayQueryPass",		{"name"} );
	}

/*
=================================================
	ToScene
=================================================
*/
	RC<SceneData>  ScriptScene::ToScene () __Th___
	{
		if ( _scene )
			return _scene;

		CHECK_THROW_MSG( not _geomInstances.empty() );
		CHECK_THROW_MSG( _passCount > 0 );

		if ( _hasRayTracingPass )
		{
			for (auto& inst : _geomInstances) {
				for (auto i : IndicesOnly<ScriptGeomSource::EGeometryType>()) {
					Unused( inst.geom->GetRTGeometry( ScriptGeomSource::EGeometryType(i) ));  // throw on error, return null if not supported
				}
			}
		}

		_scene = MakeRC<SceneData>();

		for (auto& src : _geomInstances)
		{
			auto	geom	= src.geom->ToGeomSource();  // throw
			CHECK_THROW( geom );

			auto&	dst		= _scene->_geomInstances.emplace_back();
			dst.transform	= src.transform;
			dst.geometry	= geom;
		}

		return _scene;
	}


} // AE::ResEditor
