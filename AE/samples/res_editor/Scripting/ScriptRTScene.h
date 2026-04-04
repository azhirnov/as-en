// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Scripting/ScriptCommon.h"
#include "Resources/RTScene.h"

namespace AE::ResEditor
{

	//
	// Ray Tracing Instance Custom Index
	//
	struct RTInstanceCustomIndex
	{
	// variables
		uint		value	= 0;

	// methods
		RTInstanceCustomIndex ()						__NE___	{}
		explicit RTInstanceCustomIndex (uint v)			__NE___	: value{v} {}

		static void  Bind (const ScriptEnginePtr &se)	__Th___;
	};


	//
	// Ray Tracing Instance Mask
	//
	struct RTInstanceMask
	{
	// variables
		uint		value	= 0xFF;

	// methods
		RTInstanceMask ()								__NE___	{}
		explicit RTInstanceMask (uint v)				__NE___	: value{v} {}

		static void  Bind (const ScriptEnginePtr &se)	__Th___;
	};


	//
	// Ray Tracing Instance SBT Offset
	//
	struct RTInstanceSBTOffset
	{
	// variables
		uint		value	= 0;

	// methods
		RTInstanceSBTOffset ()							__NE___	{}
		explicit RTInstanceSBTOffset (uint v)			__NE___	: value{v} {}

		static void  Bind (const ScriptEnginePtr &se)	__Th___;
	};



	//
	// Ray Tracing Scene
	//

	class ScriptRTScene final : public EnableScriptRC
	{
	// types
	private:
		struct Instance
		{
			ScriptRTGeometryPtr		geometry;
			float4x3				transform			= float4x3::Identity();
			uint					instanceCustomIndex	= UMax;
			uint					mask				= UMax;
			uint					instanceSBTOffset	= UMax;
			ERTInstanceOpt			flags				= Default;
		};


	// variables
	private:
		Array<Instance>			_instances;
		ScriptBufferPtr			_instanceBuffer;
		ScriptBufferPtr			_indirectBuffer;
		String					_dbgName;

		uint					_maxRayTypes		= 0;
		bool					_immutableInstances	= false;
		bool					_allowUpdate		= false;

		RC<RTScene>				_resource;


	// methods
	public:
		ScriptRTScene ()																	__Th___;
		~ScriptRTScene ();

		void  Name (const String &name)														__Th___;
		void  EnableHistory ()																__Th___;
		void  MaxRayTypes (uint value)														__Th___;
		void  AllowUpdate ()																__Th___;

		ND_ bool			HasIndirectBuffer ()											const	{ return bool{_indirectBuffer}; }
		ND_ ScriptBufferPtr	GetInstanceBuffer ()											__Th___;
		ND_ ScriptBufferPtr	GetIndirectBuffer ()											__Th___;

		ND_ uint			GetInstanceCount ()												__Th___;
		ND_ uint			GetMaxRayTypes ()												C_NE___	{ return _maxRayTypes; }

		ND_ StringView		GetName ()														C_NE___	{ return _dbgName; }

		ND_ bool			WithHistory ()													C_Th___;
		ND_ bool			HasMicromaps ()													C_NE___;

		void  AddInstance (const ScriptRTGeometryPtr &geom, const float4x3 &transform,
						   const RTInstanceCustomIndex &, const RTInstanceMask &,
						   const RTInstanceSBTOffset &, ERTInstanceOpt)						__Th___;

		static void  Bind (const ScriptEnginePtr &se)										__Th___;

		// Returns non-null resource or throw exception.
		ND_ RC<RTScene>  ToResource ()														__Th___;


	private:
				void  _AddInstance2 (Scripting::ScriptArgList args)							__Th___;
		static	void  _AddInstance (Scripting::ScriptArgList args)							__Th___;

		ND_ ScriptBuffer*	_GetInstanceBuffer ()											__Th___;

		ND_ ScriptBuffer*	_GetIndirectBuffer ()											__Th___;

			void  _MakeInstancesImmutable ()												__Th___;
			void  _MutableResource ()														C_Th___;
	};


} // AE::ResEditor
