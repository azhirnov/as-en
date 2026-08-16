// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "Scripting/ScriptCommon.h"
#include "Resources/RTScene.h"

namespace AE::ResEditor
{

	//
	// Transformation
	//
	struct ScriptTransform
	{
	// variables
	private:
		packed_float3	_pos	{0.f};
		packed_float3	_angles	{0.f};
		packed_float3	_scale	{1.f};


	// methods
	public:
		ScriptTransform ()										__NE___	{}

		ScriptTransform*	Position1 (const packed_float3 &);
		ScriptTransform*	Position2 (float x, float y, float z);

		ScriptTransform*	Rotation1 (const packed_float3 &);
		ScriptTransform*	Rotation2 (float x, float y, float z);

		ScriptTransform*	RotationQuat (const packed_float4 &);

		ScriptTransform*	RotateX (float);
		ScriptTransform*	RotateY (float);
		ScriptTransform*	RotateZ (float);

		ScriptTransform*	Scale1 (const packed_float3 &);
		ScriptTransform*	Scale2 (float x, float y, float z);
		ScriptTransform*	Scale3 (float);

		float3x3			ToRotationMatrix ()					C_NE___;
		float4x3			ToMatrix4x3 ()						C_NE___;
		float4x4			ToMatrix4x4 ()						C_NE___;
		Transformation		ToTransform ()						C_NE___;

		static void  Bind (const ScriptEnginePtr &se)			__Th___;
	};

} // AE::ResEditor
