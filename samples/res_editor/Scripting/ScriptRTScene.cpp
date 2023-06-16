// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Resources/RTScene.h"

#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/EnumBinder.h"

#include "res_editor/Scripting/PassCommon.inl.h"

namespace AE::ResEditor
{
namespace
{
	static void  RTInstanceCustomIndex_Ctor (OUT void* mem, uint value) {
		PlacementNew<RTInstanceCustomIndex>( OUT mem, value );
	}
	
	static void  RTInstanceMask_Ctor (OUT void* mem, uint value) {
		PlacementNew<RTInstanceMask>( OUT mem, value );
	}
	
	static void  RTInstanceSBTOffset_Ctor (OUT void* mem, uint value) {
		PlacementNew<RTInstanceSBTOffset>( OUT mem, value );
	}

} // namespace
//-----------------------------------------------------------------------------


	
/*
=================================================
	RTInstanceCustomIndex::Bind
=================================================
*/
	void  RTInstanceCustomIndex::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<RTInstanceCustomIndex>	binder{ se };
		binder.CreateClassValue();
		binder.AddConstructor( &RTInstanceCustomIndex_Ctor );
	}
//-----------------------------------------------------------------------------

	
/*
=================================================
	RTInstanceMask::Bind
=================================================
*/
	void  RTInstanceMask::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<RTInstanceMask>		binder{ se };
		binder.CreateClassValue();
		binder.AddConstructor( &RTInstanceMask_Ctor );
	}
//-----------------------------------------------------------------------------
	

/*
=================================================
	RTInstanceSBTOffset::Bind
=================================================
*/
	void  RTInstanceSBTOffset::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<RTInstanceSBTOffset>	binder{ se };
		binder.CreateClassValue();
		binder.AddConstructor( &RTInstanceSBTOffset_Ctor );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ScriptRTGeometry::ScriptRTGeometry () __Th___ :
		_dbgName{ "RTGeometry" }
	{}
	
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
	
/*
=================================================
	_MutableResource
=================================================
*/
	void  ScriptRTGeometry::_MutableResource () C_Th___
	{
		CHECK_THROW_MSG( not _resource,
			"resource is already created, can not change content" );
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
	Bind
=================================================
*/
	void  ScriptRTGeometry::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptRTGeometry>	binder{ se };
		binder.CreateRef();
		binder.AddMethod( &ScriptRTGeometry::Name,					"Name"					);
		binder.AddMethod( &ScriptRTGeometry::AddTriangles1,			"AddTriangles"			);
		binder.AddMethod( &ScriptRTGeometry::AddTriangles2,			"AddTriangles"			);
		binder.AddMethod( &ScriptRTGeometry::AddIndexedTriangles1,	"AddIndexedTriangles"	);
		binder.AddMethod( &ScriptRTGeometry::AddIndexedTriangles2,	"AddIndexedTriangles"	);
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

		CHECK_THROW_MSG( not _triangleMeshes.empty() );
		_Validate();

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

		Renderer&	renderer	= ScriptExe::ScriptResourceApi::GetRenderer(); // throw
		auto		result		= MakeRC<RTGeometry>( RVRef(tri_meshes), renderer, _dbgName );

		_resource = result;
		return _resource;
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
			return;	// can't get buffer layout

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
							tri_mesh.maxVertex = field.arraySize;
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
							tri_mesh.maxPrimitives = (field.rows * field.arraySize) / 3;
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
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ScriptRTScene::ScriptRTScene () __Th___ :
		_dbgName{ "RTScene" }
	{}
	
/*
=================================================
	Name
=================================================
*/
	void  ScriptRTScene::Name (const String &name) __Th___
	{
		_dbgName = name;
	}
	
/*
=================================================
	_MutableResource
=================================================
*
	void  ScriptRTScene::_MutableResource () C_Th___
	{
		CHECK_THROW_MSG( not _resource,
			"resource is already created, can not change content" );
	}
	
/*
=================================================
	_CheckBuffer
=================================================
*
	void  ScriptRTScene::_CheckBuffer (const ScriptBufferPtr &buf) __Th___
	{
		CHECK_THROW_MSG( buf );
		CHECK_THROW_MSG( buf->HasLayout() );
		CHECK_THROW_MSG( not buf->IsDynamicSize() );

		buf->AddUsage( EResourceUsage::ASBuild );
		buf->AddUsage( EResourceUsage::ComputeRead );
	}
	
/*
=================================================
	_AddInstance
=================================================
*/
	void  ScriptRTScene::_AddInstance (Scripting::ScriptArgList args) __Th___
	{
		args.GetObject< ScriptRTScene >()->_AddInstance2( args );
	}
	
	void  ScriptRTScene::_AddInstance2 (Scripting::ScriptArgList args) __Th___
	{
		auto&	dst	= _instances.emplace_back();
		uint	idx	= 0;

		if ( args.IsArg< ScriptRTGeometryPtr const& >(idx) )
		{
			dst.geometry = args.Arg< ScriptRTGeometryPtr const& >(idx++);
			CHECK_THROW_MSG( dst.geometry );
		}
		
		if ( args.IsArg< packed_float3 const& >(idx) )
		{
			auto&	pos = args.Arg< packed_float3 const& >(idx++);
			dst.transform.get<0,3>() = pos.x;
			dst.transform.get<1,3>() = pos.y;
			dst.transform.get<2,3>() = pos.z;
		}
		
		if ( args.IsArg< RTInstanceCustomIndex const& >(idx) )
			dst.instanceCustomIndex = args.Arg< RTInstanceCustomIndex const& >(idx++).value;
		
		if ( args.IsArg< RTInstanceMask const& >(idx) )
			dst.mask = args.Arg< RTInstanceMask const& >(idx++).value;
		
		if ( args.IsArg< RTInstanceSBTOffset const& >(idx) )
			dst.mask = args.Arg< RTInstanceSBTOffset const& >(idx++).value;
		
		CHECK_THROW_MSG( idx == args.ArgCount() );
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptRTScene::Bind (const ScriptEnginePtr &se) __Th___
	{
		RTInstanceCustomIndex::Bind( se );
		RTInstanceMask::Bind( se );
		RTInstanceSBTOffset::Bind( se );

		Scripting::ClassBinder<ScriptRTScene>	binder{ se };
		binder.CreateRef();
		binder.AddMethod( &ScriptRTScene::Name,		"Name"	);

		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &) >( &ScriptRTScene::_AddInstance, "AddInstance" );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &) >( &ScriptRTScene::_AddInstance, "AddInstance" );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceCustomIndex &) >( &ScriptRTScene::_AddInstance, "AddInstance" );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceMask &) >( &ScriptRTScene::_AddInstance, "AddInstance" );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance" );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceCustomIndex &, const RTInstanceMask &) >( &ScriptRTScene::_AddInstance, "AddInstance" );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance" );
		binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance" );
	}
	
/*
=================================================
	ToResource
=================================================
*/
	RC<RTScene>  ScriptRTScene::ToResource () __Th___
	{
		if ( _resource )
			return _resource;
		
		CHECK_THROW_MSG( not _instances.empty() );

		RTScene::Instances_t	instances;
		instances.reserve( _instances.size() );

		for (auto& src : _instances)
		{
			auto&	dst = instances.emplace_back();
			dst.geometry			= src.geometry->ToResource();
			dst.transform			= src.transform;
			dst.instanceCustomIndex	= src.instanceCustomIndex;
			dst.mask				= src.mask;
			dst.instanceSBTOffset	= src.instanceSBTOffset;
			dst.flags				= src.flags;
			CHECK_THROW( dst.geometry );
		}

		Renderer&	renderer	= ScriptExe::ScriptResourceApi::GetRenderer(); // throw
		auto		result		= MakeRC<RTScene>( RVRef(instances), renderer, _dbgName );

		_resource = result;
		return _resource;
	}


} // AE::ResEditor
