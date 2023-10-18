// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Resources/RTScene.h"
#include "res_editor/Scripting/PipelineCompiler.inl.h"

namespace AE::ResEditor
{
namespace
{
    static void  RTInstanceCustomIndex_Ctor (OUT void* mem, uint value) {
        CHECK_THROW_MSG( value <= (1<<24), "Only first 24 bits are used" );
        PlacementNew<RTInstanceCustomIndex>( OUT mem, value );
    }

    static void  RTInstanceMask_Ctor (OUT void* mem, uint value) {
        CHECK_THROW_MSG( value < (1<<8), "Only first 8 bits are used" );
        PlacementNew<RTInstanceMask>( OUT mem, value );
    }

    static void  RTInstanceSBTOffset_Ctor (OUT void* mem, uint value) {
        CHECK_THROW_MSG( value <= (1<<24), "Only first 24 bits are used" );
        PlacementNew<RTInstanceSBTOffset>( OUT mem, value );
    }

    static void  RTInstanceTransform_Ctor1 (OUT void* mem, const packed_float3 &pos, const packed_float3 &angles) {
        PlacementNew<RTInstanceTransform>( OUT mem, pos, angles );
    }

    static void  RTInstanceTransform_Ctor2 (OUT void* mem, const packed_float3 &pos, const packed_float3 &angles, float scale) {
        PlacementNew<RTInstanceTransform>( OUT mem, pos, angles, scale );
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
        Scripting::ClassBinder<RTInstanceCustomIndex>   binder{ se };
        binder.CreateClassValue();
        binder.Comment( "Pass custom index to the shader, used first 24 bits.\n"
                        "Use 'gl.rayQuery.GetIntersectionInstanceCustomIndex()' or 'gl.InstanceCustomIndex' to get it in shader." );
        binder.AddConstructor( &RTInstanceCustomIndex_Ctor, {} );
    }
//-----------------------------------------------------------------------------


/*
=================================================
    RTInstanceMask::Bind
=================================================
*/
    void  RTInstanceMask::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<RTInstanceMask>      binder{ se };
        binder.CreateClassValue();
        binder.Comment( "Set instance cull mask, used only first 8 bits.\n"
                        "In trace ray call if (cullMask_argument & instance_cullMask) != 0 then instance is visible." );
        binder.AddConstructor( &RTInstanceMask_Ctor, {} );
    }
//-----------------------------------------------------------------------------


/*
=================================================
    RTInstanceSBTOffset::Bind
=================================================
*/
    void  RTInstanceSBTOffset::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<RTInstanceSBTOffset> binder{ se };
        binder.CreateClassValue();
        binder.Comment( "Set shader binding table offset, used first 24 bits.\n"
                        "By default SBTOffset is calculated as 'instanceIndex * MaxRayTypes()'." );
        binder.AddConstructor( &RTInstanceSBTOffset_Ctor, {} );
    }
//-----------------------------------------------------------------------------


/*
=================================================
    RTInstanceTransform::Bind
=================================================
*/
    void  RTInstanceTransform::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<RTInstanceTransform> binder{ se };
        binder.CreateClassValue();
        binder.AddConstructor( &RTInstanceTransform_Ctor1, {"position", "angleInRadians"} );
        binder.AddConstructor( &RTInstanceTransform_Ctor2, {"position", "angleInRadians", "scale"} );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ScriptRTGeometry::ScriptRTGeometry () __Th___ :
        _dbgName{ "RTGeometry" }
    {
        auto&   fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
        CHECK_THROW_MSG( fs.accelerationStructure() == EFeature::RequireTrue, "RTGeometry is not supported" );
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
            AE_LOG_SE( "Unused RTGeometry '"s << _dbgName << "'" );
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
        uint        with    = 0;
        uint        without = 0;
        const auto  Test    = [&with, &without] (bool b)
        {{
            if ( b )    ++with;
            else        ++without;
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

        auto&   dst = _triangleMeshes.emplace_back();
        dst.vbuffer = vbuf;
    }

    void  ScriptRTGeometry::AddTriangles2 (const ScriptBufferPtr &vbuf, uint maxVertex, uint maxPrimitives) __Th___
    {
        _MutableResource();
        _CheckBuffer( vbuf );
        CHECK_THROW_MSG( maxVertex > 0 );
        CHECK_THROW_MSG( maxPrimitives > 0 );

        auto&   dst = _triangleMeshes.emplace_back();
        dst.vbuffer         = vbuf;
        dst.maxVertex       = maxVertex;
        dst.maxPrimitives   = maxPrimitives;
    }

    void  ScriptRTGeometry::AddTriangles3 (const ScriptBufferPtr &vbuf, const String &vbField) __Th___
    {
        _MutableResource();
        _CheckBuffer( vbuf );
        CHECK_THROW_MSG( not vbuf->IsDynamicSize(), "'maxVertex' and 'maxPrimitives' must be defined for dynamic vertex buffer" );
        CHECK_THROW_MSG( not vbField.empty() );

        auto&   dst = _triangleMeshes.emplace_back();
        dst.vbuffer         = vbuf;
        dst.vbufferField    = vbField;
    }

    void  ScriptRTGeometry::AddTriangles4 (const ScriptBufferPtr &vbuf, const String &vbField, uint maxVertex, uint maxPrimitives) __Th___
    {
        _MutableResource();
        _CheckBuffer( vbuf );
        CHECK_THROW_MSG( maxVertex > 0 );
        CHECK_THROW_MSG( maxPrimitives > 0 );
        CHECK_THROW_MSG( not vbField.empty() );

        auto&   dst = _triangleMeshes.emplace_back();
        dst.vbuffer         = vbuf;
        dst.vbufferField    = vbField;
        dst.maxVertex       = maxVertex;
        dst.maxPrimitives   = maxPrimitives;
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

        auto&   dst = _triangleMeshes.emplace_back();
        dst.vbuffer = vbuf;
        dst.ibuffer = ibuf;
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

        auto&   dst = _triangleMeshes.emplace_back();
        dst.vbuffer         = vbuf;
        dst.ibuffer         = ibuf;
        dst.maxVertex       = maxVertex;
        dst.maxPrimitives   = maxPrimitives;
        dst.indexType       = indexType;
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

        auto&   dst = _triangleMeshes.emplace_back();
        dst.vbuffer         = vbuf;
        dst.vbufferField    = vbField;
        dst.ibuffer         = ibuf;
        dst.ibufferField    = ibField;
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

        auto&   dst = _triangleMeshes.emplace_back();
        dst.vbuffer         = vbuf;
        dst.vbufferField    = vbField;
        dst.ibuffer         = ibuf;
        dst.ibufferField    = ibField;
        dst.maxVertex       = maxVertex;
        dst.maxPrimitives   = maxPrimitives;
        dst.indexType       = Default;
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
            _indirectBuffer->SetArrayLayout1( "ASBuildIndirectCommand", uint(_triangleMeshes.size()) );

            if ( auto& fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
                 fs.accelerationStructureIndirectBuild != EFeature::RequireTrue )
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
        Scripting::ClassBinder<ScriptRTGeometry>    binder{ se };
        binder.CreateRef();

        binder.Comment( "Set resource name. It is used for debugging." );
        binder.AddMethod( &ScriptRTGeometry::Name,                  "Name",                 {} );

        binder.Comment( "Add triangle mesh.\n"
                        "Supported formats:\n"
                        "   float2/float3  position []/[x];\n"
                        "   Vertex{ float2/float3  pos; ... }  verts []/[x];\n"
                        "   with static or dynamic array." );
        binder.AddMethod( &ScriptRTGeometry::AddTriangles1,         "AddTriangles",         {"vertexBuffer"} );
        binder.AddMethod( &ScriptRTGeometry::AddTriangles2,         "AddTriangles",         {"vertexBuffer", "maxVertex", "maxPrimitives"} );
        binder.AddMethod( &ScriptRTGeometry::AddTriangles3,         "AddTriangles",         {"vertexBuffer", "vbField"} );
        binder.AddMethod( &ScriptRTGeometry::AddTriangles4,         "AddTriangles",         {"vertexBuffer", "vbField", "maxVertex", "maxPrimitives"} );

        binder.Comment( "Add indexed triangle mesh.\n"
                        "Supported formats:\n"
                        "   ushort/uint  indices []/[x];\n"
                        "   with static or dynamic array." );
        binder.AddMethod( &ScriptRTGeometry::AddIndexedTriangles1,  "AddIndexedTriangles",  {"vertexBuffer", "indexBuffer"} );
        binder.AddMethod( &ScriptRTGeometry::AddIndexedTriangles2,  "AddIndexedTriangles",  {"vertexBuffer", "maxVertex", "maxPrimitives", "indexBuffer", "indexType"} );
        binder.AddMethod( &ScriptRTGeometry::AddIndexedTriangles3,  "AddIndexedTriangles",  {"vertexBuffer", "vbField", "indexBuffer", "ibField"} );
        binder.AddMethod( &ScriptRTGeometry::AddIndexedTriangles4,  "AddIndexedTriangles",  {"vertexBuffer", "vbField", "maxVertex", "maxPrimitives", "indexBuffer", "ibField"} );

        binder.Comment( "Returns indirect buffer, only this buffer must be used for indirect build." );
        binder.AddMethod( &ScriptRTGeometry::_GetIndirectBuffer,    "IndirectBuffer",       {} );

        binder.Comment( "Returns number of meshes." );
        binder.AddMethod( &ScriptRTGeometry::_GetGeometryCount,     "GeometryCount",        {} );
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

        Renderer&   renderer = ScriptExe::ScriptResourceApi::GetRenderer(); // throw

        if ( _dummy )
        {
            _resource = MakeRC<RTGeometry>( renderer, _dbgName );
            return _resource;
        }

        CHECK_THROW_MSG( not _triangleMeshes.empty() );
        _Validate();

        RC<Buffer>  ind_buf;
        if ( _indirectBuffer )
        {
            ind_buf = _indirectBuffer->ToResource();
            CHECK_THROW( ind_buf );
        }

        RTGeometry::TriangleMeshes_t    tri_meshes;
        tri_meshes.reserve( _triangleMeshes.size() );

        for (auto& src : _triangleMeshes)
        {
            auto&   dst         = tri_meshes.emplace_back();
            dst                 = src;
            dst.vbuffer         = src.vbuffer->ToResource();                        CHECK_THROW( dst.vbuffer );
            dst.ibuffer         = src.ibuffer ? src.ibuffer->ToResource() : null;   CHECK_THROW( bool{dst.ibuffer} == bool{src.ibuffer} );
            dst.vertexStride    = src.vertexStride;
            dst.vertexDataOffset= src.vertexDataOffset;
            dst.indexDataOffset = src.indexDataOffset;
        }

        _resource = MakeRC<RTGeometry>( RVRef(tri_meshes), RVRef(ind_buf), renderer, _dbgName, Bool{_allowUpdate} );
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

        auto    storage = ObjectStorage::Instance();
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
                const auto  SetVertexInfo = [&tri_mesh] (const auto &field)
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

                    tri_mesh.vertexStride       = AlignUp( EVertexType_SizeOf( tri_mesh.vertexFormat ), field.align );
                    tri_mesh.vertexDataOffset   = field.offset;
                }};

                tri_mesh.vbuffer->AddLayoutReflection();

                auto    it = storage->structTypes.find( tri_mesh.vbuffer->GetTypeName() );
                CHECK_THROW_MSG( it != storage->structTypes.end(),
                    "ShaderStructType '"s << tri_mesh.vbuffer->GetTypeName() << "' is not defined" );

                auto    fields = it->second->Fields();

                if ( not tri_mesh.vbufferField.empty() )
                {
                    bool    found = false;
                    for (auto& field : fields)
                    {
                        if ( field.name == tri_mesh.vbufferField )
                        {
                            found = true;
                            SetVertexInfo( field );
                            tri_mesh.maxVertex = field.arraySize;
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
                    //      float3  positions [];
                    //  or
                    //      Vertex  vertices [];

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
                    //      ...
                    //      float3  positions [x];
                    //  or
                    //      Vertex  vertices [x];
                    //      ...

                    for (auto& field : fields)
                    {
                        const bool  is_pos  = AnyEqual( field.type, EValueType::Float32, EValueType::Float16 ) and
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
                const auto  SetIndexInfo = [&tri_mesh] (const auto &field)
                {{
                    CHECK_THROW_MSG( field.IsScalar() or field.IsVec() );
                    CHECK_THROW_MSG( AnyEqual( field.rows, 1, 2, 4 ));  // 3 is packed as 4

                    switch ( field.type ) {
                        case EValueType::UInt16 :   tri_mesh.indexType = EIndex::UShort;    break;
                        case EValueType::UInt32 :   tri_mesh.indexType = EIndex::UInt;      break;
                        default :                   CHECK_THROW_MSG( false, "require 'ushort' or 'uint' index type" );
                    }
                    tri_mesh.indexDataOffset = field.offset;
                }};

                tri_mesh.ibuffer->AddLayoutReflection();

                auto    it = storage->structTypes.find( tri_mesh.ibuffer->GetTypeName() );
                CHECK_THROW_MSG( it != storage->structTypes.end() )

                auto    fields = it->second->Fields();

                if ( not tri_mesh.ibufferField.empty() )
                {
                    bool    found = false;
                    for (auto& field : fields)
                    {
                        if ( field.name == tri_mesh.ibufferField )
                        {
                            found = true;
                            SetIndexInfo( field );
                            tri_mesh.maxPrimitives = (field.rows * field.arraySize) / 3;
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
                    //      uint    indices [];

                    CHECK_THROW_MSG( fields.back().IsDynamicArray() );

                    CHECK_THROW_MSG( tri_mesh.maxPrimitives > 0, "for dynamic array specify 'maxPrimitives' in script" );
                    SetIndexInfo( fields.back() );
                }
                else
                {
                    // pattern:
                    //      ...
                    //      uint    indices [x];
                    //      ...

                    for (auto& field : fields)
                    {
                        const bool  is_idx  = AnyEqual( field.type, EValueType::UInt16, EValueType::UInt32 ) and
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
        _instanceBuffer{ new ScriptBuffer{} },
        _dbgName{ "RTScene" }
    {
        auto&   fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
        CHECK_THROW_MSG( fs.accelerationStructure() == EFeature::RequireTrue, "RTScene is not supported" );

        _instanceBuffer->Name( "RTScene-Instances" );
        _instanceBuffer->AddUsage( EResourceUsage::ASBuild );
    }

/*
=================================================
    destructor
=================================================
*/
    ScriptRTScene::~ScriptRTScene ()
    {
        if ( not _resource )
            AE_LOG_SE( "Unused RTScene '"s << _dbgName << "'" );
    }

/*
=================================================
    Name
=================================================
*/
    void  ScriptRTScene::Name (const String &name) __Th___
    {
        CHECK_THROW_MSG( not _resource,
            "resource is already created, can not change content" );

        _dbgName = name;
    }

/*
=================================================
    EnableHistory
=================================================
*/
    void  ScriptRTScene::EnableHistory () __Th___
    {
        CHECK_THROW_MSG( not _resource,
            "resource is already created, can not change content" );

        GetInstanceBuffer()->EnableHistory();
        GetIndirectBuffer()->EnableHistory();
    }

/*
=================================================
    AllowUpdate
=================================================
*/
    void  ScriptRTScene::AllowUpdate () __Th___
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
    bool  ScriptRTScene::WithHistory () C_Th___
    {
        uint        with    = 0;
        uint        without = 0;
        const auto  Test    = [&with, &without] (bool b)
        {{
            if ( b )    ++with;
            else        ++without;
        }};

        Test( _instanceBuffer->WithHistory() );
        Test( _indirectBuffer->WithHistory() );

        if ( with > 0 )
        {
            CHECK_THROW_MSG( without == 0 );
            return true;
        }
        return false;
    }

/*
=================================================
    _GetInstanceBuffer
=================================================
*/
    ScriptBufferPtr  ScriptRTScene::GetInstanceBuffer () __Th___
    {
        CHECK_THROW_MSG( not _resource,
            "resource is already created, can not change buffer usage" );

        _MakeInstancesImmutable();
        return _instanceBuffer;
    }

    ScriptBuffer*  ScriptRTScene::_GetInstanceBuffer () __Th___
    {
        return ScriptBufferPtr{GetInstanceBuffer()}.Detach();
    }

/*
=================================================
    GetInstanceCount
=================================================
*/
    uint  ScriptRTScene::GetInstanceCount () __Th___
    {
        _MakeInstancesImmutable();
        return uint(_instances.size());
    }

/*
=================================================
    _MakeInstancesImmutable
=================================================
*/
    void  ScriptRTScene::_MakeInstancesImmutable ()
    {
        CHECK_THROW_MSG( not _resource,
            "resource is already created, can not change content" );

        if ( not _immutableInstances )
        {
            _immutableInstances = true;
            _instanceBuffer->SetArrayLayout1( "AccelStructInstance", uint(_instances.size()) );
        }
    }

/*
=================================================
    _GetIndirectBuffer
=================================================
*/
    ScriptBufferPtr  ScriptRTScene::GetIndirectBuffer () __Th___
    {
        CHECK_THROW_MSG( not _resource,
            "resource is already created, can not change content" );

        if ( not _indirectBuffer )
        {
            _indirectBuffer.Set( new ScriptBuffer{} );
            _indirectBuffer->Name( "RTScene-Indirect" );
            _indirectBuffer->AddUsage( EResourceUsage::IndirectBuffer );
            _indirectBuffer->SetLayout1( "ASBuildIndirectCommand" );

            if ( auto& fs = ScriptExe::ScriptResourceApi::GetFeatureSet();
                 fs.accelerationStructureIndirectBuild != EFeature::RequireTrue )
            {
                EnableHistory();
            }

            _MakeInstancesImmutable();
        }

        return _indirectBuffer;
    }

    ScriptBuffer*  ScriptRTScene::_GetIndirectBuffer () __Th___
    {
        return ScriptBufferPtr{GetIndirectBuffer()}.Detach();
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
        CHECK_THROW_MSG( not _resource,
            "resource is already created, can not change content" );
        CHECK_THROW_MSG( not _immutableInstances,
            "can not add instance when 'InstanceCount()' was used" );

        auto&   dst = _instances.emplace_back();
        uint    idx = 0;

        if ( args.IsArg< ScriptRTGeometryPtr const& >(idx) )
        {
            dst.geometry = args.Arg< ScriptRTGeometryPtr const& >(idx++);
            CHECK_THROW_MSG( dst.geometry );
        }

        if ( args.IsArg< RTInstanceTransform const& >(idx) )
        {
            auto&       tr = args.Arg< RTInstanceTransform const& >(idx++);

            float3x3    mat = float3x3::RotateX( Rad{tr.angles.x} );
            mat = mat * float3x3::RotateY( Rad{tr.angles.y} );
            mat = mat * float3x3::RotateZ( Rad{tr.angles.z} );

            mat *= float3x3::Scaled( tr.scale );

            dst.transform = float4x3{mat}.SetTranslation( tr.pos );
        }
        else
        if ( args.IsArg< packed_float3 const& >(idx) )
        {
            auto&   pos = args.Arg< packed_float3 const& >(idx++);

            dst.transform = float4x3::Identity().SetTranslation( pos );
        }

        if ( args.IsArg< RTInstanceCustomIndex const& >(idx) )
            dst.instanceCustomIndex = args.Arg< RTInstanceCustomIndex const& >(idx++).value;
        else
            dst.instanceCustomIndex = uint(_instances.size()-1);

        if ( args.IsArg< RTInstanceMask const& >(idx) )
            dst.mask = args.Arg< RTInstanceMask const& >(idx++).value;
        else
            dst.mask = UMax;

        if ( args.IsArg< RTInstanceSBTOffset const& >(idx) )
            dst.instanceSBTOffset = args.Arg< RTInstanceSBTOffset const& >(idx++).value;
        else
            dst.instanceSBTOffset = uint(_instances.size()-1) * _maxRayTypes;

        if ( args.IsArg< ERTInstanceOpt >(idx) )
            dst.flags = args.Arg< ERTInstanceOpt >(idx++);

        CHECK_THROW_MSG( idx == args.ArgCount() );
    }

/*
=================================================
    AddInstance
=================================================
*/
    void  ScriptRTScene::AddInstance (const ScriptRTGeometryPtr &geom, const float4x3 &transform,
                                      const RTInstanceCustomIndex &customIdx, const RTInstanceMask &mask,
                                      const RTInstanceSBTOffset &sbtOffset, ERTInstanceOpt opt) __Th___
    {
        auto&   dst = _instances.emplace_back();
        dst.geometry            = geom;
        dst.transform           = transform;
        dst.instanceCustomIndex = customIdx.value;
        dst.mask                = mask.value;
        dst.instanceSBTOffset   = sbtOffset.value;
        dst.flags               = opt;
    }

/*
=================================================
    MaxRayTypes
=================================================
*/
    void  ScriptRTScene::MaxRayTypes (uint value) __Th___
    {
        CHECK_THROW_MSG( _instances.empty(),
            "MaxRayTypes() must be used before any AddInstance() call" );

        _maxRayTypes = value;
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
        RTInstanceTransform::Bind( se );

        Scripting::ClassBinder<ScriptRTScene>   binder{ se };
        binder.CreateRef();

        binder.Comment( "Set resource name. It is used for debugging." );
        binder.AddMethod( &ScriptRTScene::Name,     "Name",     {} );

        binder.Comment( "Add instance to the scene." );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceMask &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, const RTInstanceMask &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );

        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceMask &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceCustomIndex &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceCustomIndex &, const RTInstanceMask &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const packed_float3 &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );

        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &, const RTInstanceMask &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &, const RTInstanceCustomIndex &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &, const RTInstanceCustomIndex &, const RTInstanceMask &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );

        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceMask &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceSBTOffset &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, const RTInstanceMask &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );

        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &, const RTInstanceMask &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &, const RTInstanceSBTOffset &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &, const RTInstanceCustomIndex &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &, const RTInstanceCustomIndex &, const RTInstanceMask &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &, const RTInstanceCustomIndex &, const RTInstanceSBTOffset &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );
        binder.AddGenericMethod< void (const ScriptRTGeometryPtr &, const RTInstanceTransform &, const RTInstanceCustomIndex &, const RTInstanceMask &, const RTInstanceSBTOffset &, ERTInstanceOpt) >( &ScriptRTScene::_AddInstance, "AddInstance", {} );

        binder.Comment( "Returns instance buffer, can be used to update instances in compute shader." );
        binder.AddMethod( &ScriptRTScene::_GetInstanceBuffer,   "InstanceBuffer",   {} );

        binder.Comment( "Returns number of instances." );
        binder.AddMethod( &ScriptRTScene::GetInstanceCount,     "InstanceCount",    {} );

        binder.Comment( "Returns indirect buffer, only this buffer must be used for indirect build." );
        binder.AddMethod( &ScriptRTScene::_GetIndirectBuffer,   "IndirectBuffer",   {} );

        binder.Comment( "Set number of ray types. It is used to calculate SBTOffset for instances." );
        binder.AddMethod( &ScriptRTScene::MaxRayTypes,          "MaxRayTypes",      {} );
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
        CHECK_THROW_MSG( _instanceBuffer );

        _MakeInstancesImmutable();

        auto    inst_buf = _instanceBuffer->ToResource();
        CHECK_THROW( inst_buf );

        RC<Buffer>  ind_buf;
        if ( _indirectBuffer )
        {
            ind_buf = _indirectBuffer->ToResource();
            CHECK_THROW( ind_buf );
        }

        RTScene::Instances_t    instances;
        instances.reserve( _instances.size() );

        for (auto& src : _instances)
        {
            auto&   dst = instances.emplace_back();
            dst.geometry            = src.geometry->ToResource();
            dst.transform           = src.transform;
            dst.instanceCustomIndex = src.instanceCustomIndex;
            dst.mask                = src.mask;
            dst.instanceSBTOffset   = src.instanceSBTOffset;
            dst.flags               = src.flags;
            CHECK_THROW( dst.geometry );
        }

        Renderer&   renderer    = ScriptExe::ScriptResourceApi::GetRenderer(); // throw
        auto        result      = MakeRC<RTScene>( RVRef(instances), inst_buf, ind_buf, renderer, _dbgName, Bool{_allowUpdate} );

        _resource = result;
        return _resource;
    }


} // AE::ResEditor
