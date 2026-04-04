// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Scripting/ScriptCommon.h"
#include "Resources/RTScene.h"

namespace AE::ResEditor
{

	//
	// Ray Tracing Geometry
	//

	class ScriptRTGeometry final : public EnableScriptRC
	{
	// types
	private:
		struct TriangleMesh : RTGeometryBuild::TrianglesInfo
		{
			ScriptBufferPtr		vbuffer;
			ScriptBufferPtr		ibuffer;

			String				vbufferField;
			String				ibufferField;

			Bytes32u			vertexStride;
			Bytes				vertexDataOffset;
			Bytes				indexDataOffset;
		};
		using TriangleMeshes_t	= Array< TriangleMesh >;

		struct Micromap : RTGeometryBuild::MicromapInfo
		{
			ScriptRTMicromapPtr	micromap;
		//	ScriptBufferPtr		ibuffer;
		};
		using Micromaps_t		= Array< Micromap >;


	// variables
	private:
		TriangleMeshes_t		_triangleMeshes;
		Micromaps_t				_micromaps;

		String					_dbgName;
		ScriptBufferPtr			_indirectBuffer;

		bool					_immutableGeom		= false;
		bool					_dummy				= false;
		bool					_allowUpdate		= false;

		RC<RTGeometry>			_resource;


	// methods
	public:
		ScriptRTGeometry ()																				__Th___;
		explicit ScriptRTGeometry (Bool isDummy)														__Th___;
		~ScriptRTGeometry ();

		void  Name (const String &name)																	__Th___;
		void  EnableHistory ()																			__Th___;
		void  AllowUpdate ()																			__Th___;

		void  AddTriangles1 (const ScriptBufferPtr &vbuf)												__Th___;
		void  AddTriangles2 (const ScriptBufferPtr &vbuf, uint maxVertex, uint maxPrimitives)			__Th___;
		void  AddTriangles3 (const ScriptBufferPtr &vbuf, const String &vbField)						__Th___;
		void  AddTriangles4 (const ScriptBufferPtr &vbuf, const String &vbField,
							 uint maxVertex, uint maxPrimitives)										__Th___;

		void  AddIndexedTriangles1 (const ScriptBufferPtr &vbuf, const ScriptBufferPtr &ibuf)			__Th___;
		void  AddIndexedTriangles2 (const ScriptBufferPtr &vbuf, uint maxVertex, uint maxPrimitives,
								    const ScriptBufferPtr &ibuf, EIndex indexType)						__Th___;
		void  AddIndexedTriangles3 (const ScriptBufferPtr &vbuf, const String &vbField,
									const ScriptBufferPtr &ibuf, const String &ibField)					__Th___;
		void  AddIndexedTriangles4 (const ScriptBufferPtr &vbuf, const String &vbField,
									uint maxVertex, uint maxPrimitives,
									const ScriptBufferPtr &ibuf, const String &ibField)					__Th___;
		void  AddIndexedTriangles5 (const ScriptMeshPtr &mesh)											__Th___;

		void  AddMicromap1 (const ScriptRTMicromapPtr &mm)												__Th___;
		void  AddMicromap2 (const ScriptRTMicromapPtr &mm, uint baseTriangle)							__Th___;

	//	void  AddIndexedMicromap1 (const ScriptRTMicromapPtr &mm, );

		void  MakeImmutable ()																			__Th___;

		ND_ bool			HasIndirectBuffer ()														const	{ return bool{_indirectBuffer}; }
		ND_ ScriptBufferPtr	GetIndirectBuffer ()														__Th___;

		ND_ bool			WithHistory ()																C_Th___;
		ND_ bool			HasMicromaps ()																C_NE___	{ return not _micromaps.empty(); }

		static void  Bind (const ScriptEnginePtr &se)													__Th___;

		// Returns non-null resource or throw exception.
		ND_ RC<RTGeometry>  ToResource ()																__Th___;


	private:
			   void  _Validate ()																		__Th___;
			   void  _Validate2 ()																		__Th___;

			   void  _MutableResource ()																C_Th___;
		static void  _CheckBuffer (const ScriptBufferPtr &buf)											__Th___;

		ND_ ScriptBuffer*	_GetIndirectBuffer ()														__Th___;
		ND_ uint			_GetGeometryCount ()														__Th___;
	};


} // AE::ResEditor
