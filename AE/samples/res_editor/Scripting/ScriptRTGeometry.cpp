// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Scripting/ScriptExe.h"
#include "Scripting/PipelineCompiler.inl.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	ScriptRTGeometry::ScriptRTGeometry () __Th___ :
		_dbgName{ "RTGeometry" }
	{
		auto&	fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
		CHECK_THROW_MSG( fs.accelerationStructure() == FeatureSet::EFeature::RequireTrue, "RTGeometry is not supported" );
	}

	ScriptRTGeometry::ScriptRTGeometry (Bool isDummy) __Th___ :
		ScriptRTGeometry{}
	{
		_dummy = isDummy;
	}

/*
=================================================
	destructor
=================================================
*/
	ScriptRTGeometry::~ScriptRTGeometry ()
	{
		if ( not _resource )
			AE_LOGW( "Unused RTGeometry '"s << _dbgName << "'" );
	}

/*
=================================================
	Name
=================================================
*/
	void  ScriptRTGeometry::Name (const String &name) __Th___
	{
		_MutableResource();

		_dbgName = name;
	}

/*
=================================================
	EnableHistory
=================================================
*/
	void  ScriptRTGeometry::EnableHistory () __Th___
	{
		if ( WithHistory() )
			return;

		_MutableResource();
		GetIndirectBuffer()->EnableHistory();

		for (auto& tri_mesh : _triangleMeshes)
		{
			tri_mesh.vbuffer->EnableHistory();
			if ( tri_mesh.ibuffer )
				tri_mesh.ibuffer->EnableHistory();
		}

		// TODO: micromap
	}

/*
=================================================
	AllowUpdate
=================================================
*/
	void  ScriptRTGeometry::AllowUpdate () __Th___
	{
		CHECK_THROW_MSG( not _resource,
			"resource is already created, can not change content" );

		_allowUpdate = true;
	}

/*
=================================================
	WithHistory
=================================================
*/
	bool  ScriptRTGeometry::WithHistory () C_Th___
	{
		uint		with	= 0;
		uint		without	= 0;
		const auto	Test	= [&with, &without] (bool b)
		{{
			if ( b )	++with;
			else		++without;
		}};

		if ( _indirectBuffer )
			Test( _indirectBuffer->WithHistory() );

		for (auto& tri_mesh : _triangleMeshes)
		{
			Test( tri_mesh.vbuffer->WithHistory() );
			if ( tri_mesh.ibuffer )
				Test( tri_mesh.ibuffer->WithHistory() );
		}

		if ( with > 0 )
		{
			CHECK_THROW_MSG( without == 0 );
			return true;
		}
		return false;
	}

/*
=================================================
	AddTriangles*
=================================================
*/
	void  ScriptRTGeometry::AddTriangles1 (const ScriptBufferPtr &vbuf) __Th___
	{
		_MutableResource();
		_CheckBuffer( vbuf );
		CHECK_THROW_MSG( not vbuf->IsDynamicSize(), "'maxVertex' and 'maxPrimitives' must be defined for dynamic vertex buffer" );

		auto&	dst = _triangleMeshes.emplace_back();
		dst.vbuffer	= vbuf;
	}

	void  ScriptRTGeometry::AddTriangles2 (const ScriptBufferPtr &vbuf, uint maxVertex, uint maxPrimitives) __Th___
	{
		_MutableResource();
		_CheckBuffer( vbuf );
		CHECK_THROW_MSG( maxVertex > 0 );
		CHECK_THROW_MSG( maxPrimitives > 0 );

		auto&	dst = _triangleMeshes.emplace_back();
		dst.vbuffer			= vbuf;
		dst.maxVertex		= maxVertex;
		dst.maxPrimitives	= maxPrimitives;
	}

	void  ScriptRTGeometry::AddTriangles3 (const ScriptBufferPtr &vbuf, const String &vbField) __Th___
	{
		_MutableResource();
		_CheckBuffer( vbuf );
		CHECK_THROW_MSG( not vbuf->IsDynamicSize(), "'maxVertex' and 'maxPrimitives' must be defined for dynamic vertex buffer" );
		CHECK_THROW_MSG( not vbField.empty() );

		auto&	dst = _triangleMeshes.emplace_back();
		dst.vbuffer			= vbuf;
		dst.vbufferField	= vbField;
	}

	void  ScriptRTGeometry::AddTriangles4 (const ScriptBufferPtr &vbuf, const String &vbField, uint maxVertex, uint maxPrimitives) __Th___
	{
		_MutableResource();
		_CheckBuffer( vbuf );
		CHECK_THROW_MSG( maxVertex > 0 );
		CHECK_THROW_MSG( maxPrimitives > 0 );
		CHECK_THROW_MSG( not vbField.empty() );

		auto&	dst = _triangleMeshes.emplace_back();
		dst.vbuffer			= vbuf;
		dst.vbufferField	= vbField;
		dst.maxVertex		= maxVertex;
		dst.maxPrimitives	= maxPrimitives;
	}

/*
=================================================
	AddIndexedTriangles*
=================================================
*/
	void  ScriptRTGeometry::AddIndexedTriangles1 (const ScriptBufferPtr &vbuf, const ScriptBufferPtr &ibuf) __Th___
	{
		_MutableResource();
		_CheckBuffer( vbuf );
		_CheckBuffer( ibuf );
		CHECK_THROW_MSG( not vbuf->IsDynamicSize(), "'maxVertex' must be defined for dynamic vertex buffer" );
		CHECK_THROW_MSG( not ibuf->IsDynamicSize(), "'maxPrimitives' must be defined for dynamic index buffer" );

		auto&	dst = _triangleMeshes.emplace_back();
		dst.vbuffer	= vbuf;
		dst.ibuffer	= ibuf;
	}

	void  ScriptRTGeometry::AddIndexedTriangles2 (const ScriptBufferPtr &vbuf, uint maxVertex, uint maxPrimitives,
												  const ScriptBufferPtr &ibuf, EIndex indexType) __Th___
	{
		_MutableResource();
		_CheckBuffer( vbuf );
		_CheckBuffer( ibuf );
		CHECK_THROW_MSG( maxVertex > 0 );
		CHECK_THROW_MSG( maxPrimitives > 0 );
		CHECK_THROW_MSG( indexType != Default );

		auto&	dst = _triangleMeshes.emplace_back();
		dst.vbuffer			= vbuf;
		dst.ibuffer			= ibuf;
		dst.maxVertex		= maxVertex;
		dst.maxPrimitives	= maxPrimitives;
		dst.indexType		= indexType;
	}

	void  ScriptRTGeometry::AddIndexedTriangles3 (const ScriptBufferPtr &vbuf, const String &vbField,
												  const ScriptBufferPtr &ibuf, const String &ibField) __Th___
	{
		_MutableResource();
		_CheckBuffer( vbuf );
		_CheckBuffer( ibuf );
		CHECK_THROW_MSG( not vbuf->IsDynamicSize(), "'maxVertex' must be defined for dynamic vertex buffer" );
		CHECK_THROW_MSG( not ibuf->IsDynamicSize(), "'maxPrimitives' must be defined for dynamic index buffer" );
		CHECK_THROW_MSG( not vbField.empty() );
		CHECK_THROW_MSG( not ibField.empty() );

		auto&	dst = _triangleMeshes.emplace_back();
		dst.vbuffer			= vbuf;
		dst.vbufferField	= vbField;
		dst.ibuffer			= ibuf;
		dst.ibufferField	= ibField;
	}

	void  ScriptRTGeometry::AddIndexedTriangles4 (const ScriptBufferPtr &vbuf, const String &vbField, uint maxVertex, uint maxPrimitives,
												  const ScriptBufferPtr &ibuf, const String &ibField) __Th___
	{
		_MutableResource();
		_CheckBuffer( vbuf );
		_CheckBuffer( ibuf );
		CHECK_THROW_MSG( maxVertex > 0 );
		CHECK_THROW_MSG( maxPrimitives > 0 );
		CHECK_THROW_MSG( not vbField.empty() );
		CHECK_THROW_MSG( not ibField.empty() );

		auto&	dst = _triangleMeshes.emplace_back();
		dst.vbuffer			= vbuf;
		dst.vbufferField	= vbField;
		dst.ibuffer			= ibuf;
		dst.ibufferField	= ibField;
		dst.maxVertex		= maxVertex;
		dst.maxPrimitives	= maxPrimitives;
		dst.indexType		= Default;
	}

	void  ScriptRTGeometry::AddIndexedTriangles5 (const ScriptMeshPtr &mesh) __Th___
	{
		CHECK_THROW_MSG( mesh );
		_MutableResource();

		mesh->AddUsage( EResourceUsage::ShaderAddress );

		ScriptBufferPtr		buf;	buf.Attach( mesh->ToBuffer() );

		AddIndexedTriangles4( buf, "position", mesh->VertexCount(), mesh->PrimitiveCount(), buf, "indices" );
	}

/*
=================================================
	AddMicromap
=================================================
*/
	void  ScriptRTGeometry::AddMicromap1 (const ScriptRTMicromapPtr &mm) __Th___
	{
		AddMicromap2( mm, 0 );
	}

	void  ScriptRTGeometry::AddMicromap2 (const ScriptRTMicromapPtr &mm, uint baseTriangle) __Th___
	{
		_MutableResource();
		CHECK_THROW_MSG( mm );

		CHECK_THROW_MSG( not _triangleMeshes.empty(), "Can add micromap: triangle geometry is empty." );
		CHECK_THROW_MSG( _triangleMeshes.back().micromapIndex == UMax, "Can add micromap: micromap already used in previous triangle geometry." );

		_triangleMeshes.back().micromapIndex = uint(_micromaps.size());

		auto&	dst = _micromaps.emplace_back();
		dst.baseTriangle	= baseTriangle;
		dst.micromap		= mm;
	}

/*
=================================================
	_MutableResource
=================================================
*/
	void  ScriptRTGeometry::_MutableResource () C_Th___
	{
		CHECK_THROW_MSG( not _resource,
			"resource is already created, can not change content" );
		CHECK_THROW_MSG( not _immutableGeom,
			"RTGeometry is immutable" );
	}

/*
=================================================
	_CheckBuffer
=================================================
*/
	void  ScriptRTGeometry::_CheckBuffer (const ScriptBufferPtr &buf) __Th___
	{
		CHECK_THROW_MSG( buf );
		CHECK_THROW_MSG( buf->HasLayout() );
		CHECK_THROW_MSG( not buf->IsDynamicSize() );

		buf->AddUsage( EResourceUsage::ASBuild );
		buf->AddUsage( EResourceUsage::ComputeRead );
	}

/*
=================================================
	MakeImmutable
=================================================
*/
	void  ScriptRTGeometry::MakeImmutable () __Th___
	{
		_immutableGeom = true;
	}

/*
=================================================
	_GetIndirectBuffer
=================================================
*/
	ScriptBufferPtr  ScriptRTGeometry::GetIndirectBuffer () __Th___
	{
		if ( not _indirectBuffer )
		{
			_MutableResource();

			_indirectBuffer.Set( new ScriptBuffer{} );
			_indirectBuffer->Name( "RTGeometry-Indirect" );
			_indirectBuffer->AddUsage( EResourceUsage::IndirectBuffer );
			_indirectBuffer->AddUsage( EResourceUsage::ShaderAddress );
			_indirectBuffer->SetArrayLayout1( "ASBuildIndirectCommand", uint(_triangleMeshes.size()) );

			if ( auto& fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
				 fs.accelerationStructureIndirectBuild != FeatureSet::EFeature::RequireTrue )
			{
				EnableHistory();
			}

			MakeImmutable();
		}

		return _indirectBuffer;
	}

	ScriptBuffer*  ScriptRTGeometry::_GetIndirectBuffer () __Th___
	{
		return ScriptBufferPtr{GetIndirectBuffer()}.Detach();
	}

/*
=================================================
	_GetGeometryCount
=================================================
*/
	uint  ScriptRTGeometry::_GetGeometryCount () __Th___
	{
		MakeImmutable();
		return uint(_triangleMeshes.size());
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptRTGeometry::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptRTGeometry>	binder{ se };
		binder.CreateRef();

		binder.Comment( "Set resource name. It is used for debugging." );
		AS_METHOD( binder, ScriptRTGeometry::Name,					"Name",					{} );

		binder.Comment( "Add triangle mesh.\n"
						"Supported formats:\n"
						"	float2/float3  position []/[x];\n"
						"	Vertex{ float2/float3  pos; ... }  verts []/[x];\n"
						"	with static or dynamic array." );
		AS_METHOD( binder, ScriptRTGeometry::AddTriangles1,			"AddTriangles",			{"vertexBuffer"} );
		AS_METHOD( binder, ScriptRTGeometry::AddTriangles2,			"AddTriangles",			{"vertexBuffer", "maxVertex", "maxPrimitives"} );
		AS_METHOD( binder, ScriptRTGeometry::AddTriangles3,			"AddTriangles",			{"vertexBuffer", "vbField"} );
		AS_METHOD( binder, ScriptRTGeometry::AddTriangles4,			"AddTriangles",			{"vertexBuffer", "vbField", "maxVertex", "maxPrimitives"} );

		binder.Comment( "Add indexed triangle mesh.\n"
						"Supported formats:\n"
						"	ushort/uint  indices []/[x];\n"
						"	with static or dynamic array." );
		AS_METHOD( binder, ScriptRTGeometry::AddIndexedTriangles1,	"AddIndexedTriangles",	{"vertexBuffer", "indexBuffer"} );
		AS_METHOD( binder, ScriptRTGeometry::AddIndexedTriangles2,	"AddIndexedTriangles",	{"vertexBuffer", "maxVertex", "maxPrimitives", "indexBuffer", "indexType"} );
		AS_METHOD( binder, ScriptRTGeometry::AddIndexedTriangles3,	"AddIndexedTriangles",	{"vertexBuffer", "vbField", "indexBuffer", "ibField"} );
		AS_METHOD( binder, ScriptRTGeometry::AddIndexedTriangles4,	"AddIndexedTriangles",	{"vertexBuffer", "vbField", "maxVertex", "maxPrimitives", "indexBuffer", "ibField"} );
		AS_METHOD( binder, ScriptRTGeometry::AddIndexedTriangles5,	"AddIndexedTriangles",	{"mesh"} );

		binder.Comment( "Add micromap for previously added triangle geometry." );
		AS_METHOD( binder, ScriptRTGeometry::AddMicromap1,			"AddMicromap",			{} );
		AS_METHOD( binder, ScriptRTGeometry::AddMicromap2,			"AddMicromap",			{"micromap", "baseTriangle"} );

		binder.Comment( "Returns indirect buffer, only this buffer must be used for indirect build."
						"Layout: 'ASBuildIndirectCommand [mesh_count]'" );
		AS_METHOD( binder, ScriptRTGeometry::_GetIndirectBuffer,	"IndirectBuffer",		{} );

		binder.Comment( "Returns number of meshes." );
		AS_METHOD( binder, ScriptRTGeometry::_GetGeometryCount,		"GeometryCount",		{} );
	}

/*
=================================================
	ToResource
=================================================
*/
	RC<RTGeometry>  ScriptRTGeometry::ToResource () __Th___
	{
		if ( _resource )
			return _resource;

		Renderer&	renderer = ScriptExe::ScriptResourceApi::GetRenderer(); // throw

		if ( _dummy )
		{
			_resource = RTGeometry::Create( renderer, _dbgName );  // throw
			return _resource;
		}

		CHECK_THROW_MSG( not _triangleMeshes.empty() );
		_Validate();

		RC<Buffer>	ind_buf;
		if ( _indirectBuffer )
		{
			ind_buf = _indirectBuffer->ToResource();
			CHECK_THROW( ind_buf );
		}

		RTGeometry::TriangleMeshes_t	tri_meshes;
		tri_meshes.reserve( _triangleMeshes.size() );

		for (auto& src : _triangleMeshes)
		{
			auto&	dst			= tri_meshes.emplace_back();
			dst					= src;
			dst.vbuffer			= src.vbuffer->ToResource();						CHECK_THROW( dst.vbuffer );
			dst.ibuffer			= src.ibuffer ? src.ibuffer->ToResource() : null;	CHECK_THROW( bool{dst.ibuffer} == bool{src.ibuffer} );
			dst.vertexStride	= src.vertexStride;
			dst.vertexDataOffset= src.vertexDataOffset;
			dst.indexDataOffset	= src.indexDataOffset;
		}

		RTGeometry::Micromaps_t		micromaps;
		micromaps.reserve( _micromaps.size() );

		for (auto& src : _micromaps)
		{
			auto&	dst = micromaps.emplace_back();
			dst				= src;
			dst.micromap	= src.micromap->ToResource();
		}

		_resource = RTGeometry::Create( RVRef(tri_meshes), RVRef(micromaps), RVRef(ind_buf), renderer, _dbgName, Bool{_allowUpdate} );  // throw
		return _resource;
	}

/*
=================================================
	_Validate2
=================================================
*/
	void  ScriptRTGeometry::_Validate2 () __Th___
	{
		for (auto& tri_mesh : _triangleMeshes)
		{
			CHECK_THROW_MSG( not tri_mesh.vbuffer->HasLayout() );

			CHECK_THROW_MSG( tri_mesh.maxVertex > 0 );
			CHECK_THROW_MSG( tri_mesh.vertexFormat != Default );
			CHECK_THROW_MSG( tri_mesh.maxPrimitives > 0 );

			if ( tri_mesh.ibuffer )
			{
				CHECK_THROW_MSG( tri_mesh.indexType != Default );
				CHECK_THROW_MSG( not tri_mesh.ibuffer->HasLayout() );
			}
		}
	}

/*
=================================================
	_Validate
=================================================
*/
	void  ScriptRTGeometry::_Validate () __Th___
	{
		using namespace PipelineCompiler;

		auto	storage = ObjectStorage::Instance();
		if ( not storage )
		{
			// can't get buffer layout
			_Validate2();
			return;
		}

		for (auto& tri_mesh : _triangleMeshes)
		{
			// vertex buffer
			{
				const auto	SetVertexInfo = [&tri_mesh] (const auto &field)
				{{
					CHECK_THROW_MSG( AnyEqual( field.type, EValueType::Float32, EValueType::Float16 ));
					CHECK_THROW_MSG( field.IsVec() );

					switch ( field.rows )
					{
						case 2 :
							tri_mesh.vertexFormat = (field.type == EValueType::Float32 ? EVertexType::Float2 : EVertexType::Half2);
							break;

						case 3 :
						case 4 :
							tri_mesh.vertexFormat = (field.type == EValueType::Float32 ? EVertexType::Float3 : EVertexType::Half3);
							break;

						default :
							CHECK_THROW_MSG( false, "require 'float2' or 'float3' position type" );
					}

					tri_mesh.vertexStride		= AlignUp( EVertexType_SizeOf( tri_mesh.vertexFormat ), field.align );
					tri_mesh.vertexDataOffset	= field.offset;
				}};

				tri_mesh.vbuffer->AddLayoutReflection();

				auto	it = storage->structTypes.find( tri_mesh.vbuffer->GetTypeName() );
				CHECK_THROW_MSG( it != storage->structTypes.end(),
					"ShaderStructType '"s << tri_mesh.vbuffer->GetTypeName() << "' is not defined" );

				auto	fields = it->second->Fields();

				if ( not tri_mesh.vbufferField.empty() )
				{
					bool	found = false;
					for (auto& field : fields)
					{
						if ( field.name == tri_mesh.vbufferField )
						{
							found = true;
							SetVertexInfo( field );
							tri_mesh.maxVertex = uint(field.arraySize);
							CHECK_THROW_MSG( not field.IsDynamicArray(), "for dynamic array specify 'maxVertex' in script" );
							break;
						}
					}
					CHECK_THROW_MSG( found,
						"can't find field '"s << tri_mesh.vbufferField << "' in struct '" << tri_mesh.vbuffer->GetTypeName() << "'" );
				}
				else
				if ( it->second->HasDynamicArray() )
				{
					// pattern:
					//		float3	positions [];
					//	or
					//		Vertex	vertices [];

					CHECK_THROW_MSG( fields.back().IsDynamicArray() );
					CHECK_THROW_MSG( tri_mesh.maxVertex > 0, "for dynamic array specify 'maxVertex' in script" );

					if ( fields.back().stType )
					{
						SetVertexInfo( fields.back().stType->Fields().back() );
						tri_mesh.vertexDataOffset += fields.back().offset;
					}
					else{
						SetVertexInfo( fields.back() );
					}
				}
				else
				{
					// pattern:
					//		...
					//		float3	positions [x];
					//	or
					//		Vertex	vertices [x];
					//		...

					for (auto& field : fields)
					{
						const bool	is_pos	= AnyEqual( field.type, EValueType::Float32, EValueType::Float16 ) and
											  AnyEqual( field.rows, 1, 2, 3 );

						if ( field.IsStaticArray() and (field.stType or is_pos) )
						{
							if ( field.stType ){
								SetVertexInfo( field );
							}else{
								SetVertexInfo( field );
							}
							tri_mesh.maxVertex = uint(field.arraySize);
							break;
						}
					}
				}
			}
			CHECK_THROW_MSG( tri_mesh.maxVertex > 0 );
			CHECK_THROW_MSG( tri_mesh.vertexFormat != Default );

			// index buffer
			if ( tri_mesh.ibuffer )
			{
				const auto	SetIndexInfo = [&tri_mesh] (const auto &field)
				{{
					CHECK_THROW_MSG( field.IsScalar() or field.IsVec() );
					CHECK_THROW_MSG( AnyEqual( field.rows, 1, 2, 4 ));	// 3 is packed as 4

					switch ( field.type ) {
						case EValueType::UInt16 :	tri_mesh.indexType = EIndex::UShort;	break;
						case EValueType::UInt32 :	tri_mesh.indexType = EIndex::UInt;		break;
						default :					CHECK_THROW_MSG( false, "require 'ushort' or 'uint' index type" );
					}
					tri_mesh.indexDataOffset = field.offset;
				}};

				tri_mesh.ibuffer->AddLayoutReflection();

				auto	it = storage->structTypes.find( tri_mesh.ibuffer->GetTypeName() );
				CHECK_THROW_MSG( it != storage->structTypes.end() )

				auto	fields = it->second->Fields();

				if ( not tri_mesh.ibufferField.empty() )
				{
					bool	found = false;
					for (auto& field : fields)
					{
						if ( field.name == tri_mesh.ibufferField )
						{
							found = true;
							SetIndexInfo( field );
							tri_mesh.maxPrimitives = uint((field.rows * field.arraySize) / 3);
							CHECK_THROW_MSG( not field.IsDynamicArray(), "for dynamic array specify 'maxPrimitives' in script" );
							break;
						}
					}
					CHECK_THROW_MSG( found,
						"can't find field '"s << tri_mesh.ibufferField << "' in struct '" << tri_mesh.vbuffer->GetTypeName() << "'" );
				}
				else
				if ( it->second->HasDynamicArray() )
				{
					// pattern:
					//		uint	indices [];

					CHECK_THROW_MSG( fields.back().IsDynamicArray() );

					CHECK_THROW_MSG( tri_mesh.maxPrimitives > 0, "for dynamic array specify 'maxPrimitives' in script" );
					SetIndexInfo( fields.back() );
				}
				else
				{
					// pattern:
					//		...
					//		uint	indices [x];
					//		...

					for (auto& field : fields)
					{
						const bool	is_idx	= AnyEqual( field.type, EValueType::UInt16, EValueType::UInt32 ) and
											  AnyEqual( field.rows, 1, 2, 4 );

						if ( field.IsStaticArray() and is_idx )
						{
							SetIndexInfo( field );
							tri_mesh.maxPrimitives = uint((field.rows * field.arraySize) / 3);
							break;
						}
					}
				}
			}
			else
			{
				tri_mesh.maxPrimitives = tri_mesh.maxVertex / 3;
			}

			CHECK_THROW_MSG( tri_mesh.maxPrimitives > 0 );
		}
	}

} // AE::ResEditor
