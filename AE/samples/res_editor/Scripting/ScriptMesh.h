// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "Scripting/ScriptCommon.h"


namespace AE::ResEditor
{

	//
	// Mesh
	//

	class ScriptMesh final : public EnableScriptRC
	{
	// types
	public:
		enum class EAttribute : uint
		{
			//							default name in buffer
			Unknown					= 0,
			Position				= 1 << 0,		//	position
			Texcoord2D				= 1 << 1,		//	uv
			Texcoord3D				= 1 << 2,		//	uvw
			Normal					= 1 << 3,		//	normal
			Tangent					= 1 << 4,		//	tangent
			BiTangent				= 1 << 5,		//	bitangent
			TBN_Quat				= 1 << 6,		//	tbnQuat
			TriangleIndices			= 1 << 7,		//	triangles
			TriangleIndicesPack32	= 1 << 8,		//	trianglesPack32		- RGB10_A2U
			Color					= 1 << 9,		//	color
			FloatParams				= 1 << 10,		//	fparams
			IntParams				= 1 << 11,		//	iparams
			SubMeshID				= 1 << 12,		//	subMeshId
			_Last,

			TB						= Tangent | BiTangent,
			TBN						= Tangent | BiTangent | Normal,
			_BITOPS_				= 0
		};

		// attributes will be generated in 'ToBuffer()'
		static constexpr EAttribute	_GenMask =	EAttribute::TBN_Quat | EAttribute::TriangleIndices |
												EAttribute::TriangleIndicesPack32 | EAttribute::SubMeshID;


	// variables
	private:
		EAttribute				_attribs		= Default;

		Array<packed_float3>	_position;
		Array<packed_float2>	_uv;
		Array<packed_float3>	_uvw;
		Array<packed_float3>	_normal;
		Array<packed_float3>	_tangent;
		Array<packed_float3>	_bitangent;
		Array<packed_float4>	_tbnQuat;
		Array<packed_float4>	_color;
		Array<packed_float4>	_fParams;
		Array<packed_int4>		_iParams;
		Array<uint>				_subMeshId;
		Array<uint>				_indices;

		StaticArray<uint, 12>	_offsets		= {};
		uint					_subMeshCount	= 0;
		bool					_subMesh		= false;
		bool					_dontValidate	= false;

		EResourceUsage			_bufferUsage	= Default;
		String					_bufferName;
		ScriptBufferPtr			_buffer;


	// methods
	public:
		ScriptMesh ()																				__Th___;
		//ScriptMesh (const ScriptMesh &)															__Th___ = default;
		~ScriptMesh ();

		void  Name (const String &name)																__Th___;

		void  SetAttributes1 (EAttribute value)														__Th___;
		void  SetAttributes2 (uint value)															__Th___;

		void  AddUsage (EResourceUsage usage)														__Th___;

		void  Add (const ScriptMeshPtr &)															__Th___;

		void  AddVertices1 (const ScriptArray<packed_float3> &position)								__Th___;

		void  AddVertices2 (const ScriptArray<packed_float3> &position,
							const ScriptArray<packed_float2> &uv)									__Th___;

		void  AddVertices3 (const ScriptArray<packed_float3> &position,
							const ScriptArray<packed_float2> &uv,
							const ScriptArray<packed_float3> &normal)								__Th___;

		void  AddVertices4 (const ScriptArray<packed_float3> &position,
							const ScriptArray<packed_float2> &uv,
							const ScriptArray<packed_float3> &normal,
							const ScriptArray<packed_float3> &tangent,
							const ScriptArray<packed_float3> &bitangent)							__Th___;

		void  AddVertices5 (const ScriptArray<packed_float3> &position,
							const ScriptArray<RGBA32f>		 &color)								__Th___;

		void  AddVertices6 (const ScriptArray<packed_float3> &position,
							const ScriptArray<packed_float2> &uv,
							const ScriptArray<RGBA32f>		 &color)								__Th___;

		void  AddIndices1 (const ScriptArray<uint> &indices)										__Th___;
		void  AddIndices2 (const ScriptArray<uint> &indices)										__Th___;

		void  ScaleUV1 (float uScale, float vScale)													__Th___;
		void  ScaleUV2 (const packed_float2 &uvScale)												__Th___;

		void  AddCube1 ()																			__Th___;
		void  AddCube2 (const ScriptTransform &tr)													__Th___;

		void  AddSphere1 (uint lod)																	__Th___;
		void  AddSphere2 (uint lod, const ScriptTransform &tr)										__Th___;

		// XY space, snorm
		void  AddGrid1 (uint lod)																	__Th___;
		void  AddGrid2 (uint lod, const ScriptTransform &tr)										__Th___;

		// XZ space, snorm
		void  AddGridXZ1 (uint lod)																	__Th___;
		void  AddGridXZ2 (uint lod, const ScriptTransform &tr)										__Th___;

		void  AddTube1 (uint segmentCount)															__Th___;
		void  AddTube2 (uint segmentCount, const ScriptTransform &tr)								__Th___;

		void  AddInnerTube1 (uint segmentCount)														__Th___;
		void  AddInnerTube2 (uint segmentCount, const ScriptTransform &tr)							__Th___;

		void  AddCylinder1 (uint segmentCount)														__Th___;
		void  AddCylinder2 (uint segmentCount, const ScriptTransform &tr)							__Th___;

		void  AddCylinderEnds1 (uint segmentCount)													__Th___;
		void  AddCylinderEnds2 (uint segmentCount, const ScriptTransform &tr)						__Th___;

		void  AddCapsule1 (uint segmentCount)														__Th___;
		void  AddCapsule2 (uint segmentCount, const ScriptTransform &tr)							__Th___;

		void  AddCone1 (uint segmentCount, float radius, float height)								__Th___;
		void  AddCone2 (uint segmentCount, float radius, float height, const ScriptTransform &tr)	__Th___;
		void  AddCone3 (uint segmentCount)															__Th___;
		void  AddCone4 (uint segmentCount, const ScriptTransform &tr)								__Th___;

		void  AddSphericalCube1 (uint lod)															__Th___;
		void  AddSphericalCube2 (uint lod, const ScriptTransform &tr)								__Th___;

		uint  IndexCount ()																			C_NE___	{ return uint(_indices.size()); }
		uint  PrimitiveCount ()																		C_NE___	{ return uint(_indices.size() / 3); }
		uint  VertexCount ()																		C_NE___	{ return uint(_position.size()); }

		ulong	DeviceAddress (EAttribute attrib)													__Th___;

		ScriptBuffer*	ToBuffer ()																	__Th___;

		static void  Bind (const ScriptEnginePtr &se)												__Th___;

	private:
		void  _AddCylinder (int shape, uint segmentCount, const ScriptTransform &tr)				__Th___;

		void  _Add (const ScriptMesh &);
		void  _AddTemp (INOUT ScriptMesh &, const ScriptTransform &tr);

		void  _Resize (usize attrCount);
		void  _Transform (const ScriptTransform &tr);
		void  _CalcTBN ();
		void  _CalcTBNQuat ();

		void  _ValidateSize ()																		C_Th___;
		void  _ValidateTBN ()																		C_Th___;
		void  _ValidateIndices ()																	C_Th___;

		void  _IsMutable ();
	};


} // AE::ResEditor
