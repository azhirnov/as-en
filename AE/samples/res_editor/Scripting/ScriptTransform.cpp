// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Scripting/ScriptTransform.h"
#include "Scripting/ScriptExe.h"

namespace AE::ResEditor
{

/*
=================================================
	Position*
=================================================
*/
	ScriptTransform*  ScriptTransform::Position1 (const packed_float3 &value)
	{
		_pos = value;
		return this;
	}

	ScriptTransform*  ScriptTransform::Position2 (float x, float y, float z)
	{
		_pos = packed_float3{ x, y, z };
		return this;
	}

/*
=================================================
	Rotation
=================================================
*/
	ScriptTransform*  ScriptTransform::Rotation1 (const packed_float3 &value)
	{
		_angles = value;
		return this;
	}

	ScriptTransform*  ScriptTransform::Rotation2 (float x, float y, float z)
	{
		_angles = packed_float3{ x, y, z };
		return this;
	}

	ScriptTransform*  ScriptTransform::RotationQuat (const packed_float4 &)
	{
		// TODO
		return this;
	}

/*
=================================================
	RotateX / RotateY / RotateZ
=================================================
*/
	ScriptTransform*  ScriptTransform::RotateX (float value)
	{
		_angles.x = value;
		return this;
	}

	ScriptTransform*  ScriptTransform::RotateY (float value)
	{
		_angles.y = value;
		return this;
	}

	ScriptTransform*  ScriptTransform::RotateZ (float value)
	{
		_angles.z = value;
		return this;
	}

/*
=================================================
	Scale*
=================================================
*/
	ScriptTransform*  ScriptTransform::Scale1 (const packed_float3 &value)
	{
		_scale = value;
		return this;
	}

	ScriptTransform*  ScriptTransform::Scale2 (float x, float y, float z)
	{
		_scale = packed_float3{ x, y, z };
		return this;
	}

	ScriptTransform*  ScriptTransform::Scale3 (float value)
	{
		_scale = packed_float3{value};
		return this;
	}

/*
=================================================
	ToMatrix*
=================================================
*/
	float3x3  ScriptTransform::ToRotationMatrix () C_NE___
	{
		float3x3	mat = float3x3::RotateX( Rad{_angles.x} );
		mat = mat * float3x3::RotateY( Rad{_angles.y} );
		mat = mat * float3x3::RotateZ( Rad{_angles.z} );
		return mat;
	}

	float4x3  ScriptTransform::ToMatrix4x3 () C_NE___
	{
		float3x3	mat = ToRotationMatrix();

		mat *= float3x3::Scale( _scale );

		return float4x3{mat}.SetTranslation( _pos );
	}

	float4x4  ScriptTransform::ToMatrix4x4 () C_NE___
	{
		return float4x4{ToMatrix4x3()};
	}

	Transformation  ScriptTransform::ToTransform () C_NE___
	{
		float	scale = Max( _scale.x, _scale.y, _scale.z );
		CHECK( All(Equal( _scale, packed_float3{scale} )));

		return Transformation{ _pos, Quat::Rotate2( RadianVec<float,3>{ _angles }), scale };
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptTransform::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptTransform>		binder{ se };
		binder.CreateClassValue();

		AS_METHOD( binder, ScriptTransform::Position1,		"Position",		{} );
		AS_METHOD( binder, ScriptTransform::Position2,		"Position",		{"x", "y", "z"} );

		AS_METHOD( binder, ScriptTransform::Rotation1,		"Rotation",		{} );
		AS_METHOD( binder, ScriptTransform::Rotation2,		"Rotation",		{"x", "y", "z"} );

	//	AS_METHOD( binder, ScriptTransform::RotationQuat,	"RotationQuat",	{} );	// TODO
		AS_METHOD( binder, ScriptTransform::RotateX,		"RotateX",		{} );
		AS_METHOD( binder, ScriptTransform::RotateY,		"RotateY",		{} );
		AS_METHOD( binder, ScriptTransform::RotateZ,		"RotateZ",		{} );

		AS_METHOD( binder, ScriptTransform::Scale1,			"Scale",		{} );
		AS_METHOD( binder, ScriptTransform::Scale2,			"Scale",		{"x", "y", "z"} );
		AS_METHOD( binder, ScriptTransform::Scale3,			"Scale",		{"scalar"} );
	}

} // AE::ResEditor
