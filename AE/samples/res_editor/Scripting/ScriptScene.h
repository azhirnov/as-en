// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Scripting/ScriptBaseRenderPass.h"
#include "Scripting/ScriptGeomSource.h"
#include "Scripting/ScriptRayTracingPass.h"
#include "Passes/Scene.h"

namespace AE::ResEditor
{

	//
	// Scene
	//
	class ScriptScene final : public EnableScriptRC
	{
	// types
	private:
		struct GeometryInstance
		{
			ScriptGeomSourcePtr		geom;
			float4x4				transform;
		};
		using GeomInstances_t = Array< GeometryInstance >;


	// variables
	public:
		GeomInstances_t				_geomInstances;
	private:
		ScriptBaseControllerPtr		_controller;
		uint						_passCount			= 0;
		bool						_hasRayTracingPass	= false;
		String						_dbgName;

		RC<SceneData>				_scene;


	// methods
	public:
		ScriptScene () {}
		~ScriptScene ();

		void  Name (const String &name)														__Th___;

		// default controller, can be overridden by pass
		void  InputController (const ScriptBaseControllerPtr &)								__Th___;

		void  InputGeometry1 (const ScriptGeomSourcePtr	&,
							  const packed_float3		&pos,
							  const packed_float3		&rotation,
							  float						scale)								__Th___;
		void  InputGeometry2 (const ScriptGeomSourcePtr	&,
							  const packed_float3		&pos)								__Th___;
		void  InputGeometry3 (const ScriptGeomSourcePtr	&)									__Th___;
		void  InputGeometry4 (const ScriptGeomSourcePtr	&,
							  const packed_float4x4		&mat)								__Th___;

		ScriptSceneGraphicsPass*	AddGraphicsPass (const String &name)					__Th___;
		ScriptSceneRayTracingPass*	AddRayTracingPass (const String &name)					__Th___;
		ScriptSceneRayQueryPass*	AddRayQueryPass (const String &name)					__Th___;

		ND_ RC<SceneData>			 ToScene ()												__Th___;
		ND_ ScriptBaseControllerPtr  GetController ()										__Th___	{ return _controller; }

		static void  Bind (const ScriptEnginePtr &se)										__Th___;
	};


} // AE::ResEditor
