// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Resources/Buffer.h"
#include "res_editor/Scripting/PipelineCompiler.inl.h"

namespace AE::ResEditor
{
namespace
{
    static ScriptBuffer*  ScriptBuffer_Ctor1 (uint size) {
        return ScriptBufferPtr{ new ScriptBuffer{ Bytes{size} }}.Detach();
    }

    static ScriptBuffer*  ScriptBuffer_Ctor2 (const String &filename) {
        return ScriptBufferPtr{ new ScriptBuffer{ filename }}.Detach();
    }

} // namespace


/*
=================================================
    BufferLayout::Put
=================================================
*/
    uint  ScriptBuffer::BufferLayout::Put (const void* data, const Bytes dataSize, const Bytes align)
    {
        Bytes   offset      = AlignUp( Bytes{_data.size()}, align );
        Bytes   new_size    = AlignUp( offset + dataSize, align );

        _data.resize( usize(new_size) );

        MemCopy( OUT _data.data() + offset, data, dataSize );
        return uint{offset};
    }

/*
=================================================
    BufferLayout::Put
=================================================
*/
    uint  ScriptBuffer::BufferLayout::Put (const Bytes dataSize, const Bytes align)
    {
        Bytes   offset      = AlignUp( Bytes{_data.size()}, align );
        Bytes   new_size    = AlignUp( offset + dataSize, align );

        _data.resize( usize(new_size) );
        return uint{offset};
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ScriptBuffer::ScriptBuffer (Bytes size) __Th___ :
        _type{ EBufferType::MutableData_NonInitialized }
    {
        _desc.size = size;
    }

    ScriptBuffer::ScriptBuffer (const String &filename) __Th___ :
        _filename{ filename },
        _type{ EBufferType::ConstDataFromFile },
        _dbgName{ filename.substr( 0, ResNameMaxLen ) }
    {
        CHECK_THROW_MSG( GetVFS().Exists( _filename ),
            "File '"s << filename << "' is not exists" );
    }

/*
=================================================
    destructor
=================================================
*/
    ScriptBuffer::~ScriptBuffer ()
    {
        if ( not _resource )
            AE_LOG_SE( "Unused buffer '"s << _dbgName << "'" );
    }

/*
=================================================
    GetTypeName
=================================================
*/
    String  ScriptBuffer::GetTypeName () C_NE___
    {
        return  _IsArray() ?
                    _layout.typeName + (_layout.staticSrc.empty() ? "_Array" : "_Array2") :
                    _layout.typeName;
    }

/*
=================================================
    _IsArray
=================================================
*/
    bool  ScriptBuffer::_IsArray () C_NE___
    {
        return  _dynCount or
                (_staticCount > 0 and _staticCount != UMax);
    }

/*
=================================================
    GetDeviceAddress
=================================================
*/
    ulong  ScriptBuffer::GetDeviceAddress () __Th___
    {
        CHECK_THROW_MSG( not IsDynamicSize() );

        AddUsage( EResourceUsage::ShaderAddress );

        auto    buf = ToResource();
        CHECK_THROW( buf );
        CHECK_THROW( not buf->HasHistory() );   // TODO: return dynamic value?

        ulong   addr = buf->GetDeviceAddress( 0 );
        CHECK_THROW( addr != 0 );

        return addr;
    }

/*
=================================================
    _SetType
=================================================
*/
    void  ScriptBuffer::_SetType (EBufferType type) __Th___
    {
        CHECK_THROW_MSG( not _resource,
            "resource is already created, can not change buffer type" );

        if ( _type == type )
            return;

        if ( _type == Default )
        {
            _type = type;
            return;
        }

        if ( _type == EBufferType::ConstDataFromScript and type == EBufferType::MutableDataFromScript )
        {
            _type = type;
            return;
        }

        if ( type == EBufferType::ConstDataFromScript and _type == EBufferType::MutableDataFromScript )
            return; // ignore

        CHECK_THROW_MSG( false, "Can not change buffer layout type" );
    }

/*
=================================================
    AddUsage
=================================================
*/
    void  ScriptBuffer::AddUsage (EResourceUsage usage) __Th___
    {
        if ( not AllBits( _resUsage, usage ))
        {
            CHECK_THROW_MSG( not _resource,
                "resource is already created, can not change usage or content" );
        }

        _resUsage |= usage;

        _ValidateResourceUsage( _resUsage );
    }

/*
=================================================
    _ValidateResourceUsage
=================================================
*/
    void  ScriptBuffer::_ValidateResourceUsage (const EResourceUsage usage) __Th___
    {
        CHECK_THROW_MSG( not AnyBits( usage, EResourceUsage::ColorAttachment ));
        CHECK_THROW_MSG( not AnyBits( usage, EResourceUsage::DepthStencil ));
        CHECK_THROW_MSG( not AnyBits( usage, EResourceUsage::Sampled ));
        CHECK_THROW_MSG( not AnyBits( usage, EResourceUsage::GenMipmaps ));

        if ( AllBits( usage, EResourceUsage::UploadedData ))
        {
            CHECK_THROW_MSG( not AnyBits( usage, EResourceUsage::ComputeWrite ));
            CHECK_THROW_MSG( not AnyBits( usage, EResourceUsage::ShaderAddress ));
        }

        auto&   fs = ScriptExe::ScriptResourceApi::GetFeatureSet();

        if ( AnyBits( usage, EResourceUsage::ASBuild ))
        {
            CHECK_THROW_MSG( fs.accelerationStructure() == EFeature::RequireTrue,
                "AccelerationStructures are not supported" );
        }
        if ( AnyBits( usage, EResourceUsage::ShaderAddress ))
        {
            CHECK_THROW_MSG( fs.bufferDeviceAddress == EFeature::RequireTrue,
                "ShaderAddress is not supported" );
        }
    }

/*
=================================================
    SetSize
=================================================
*/
    void  ScriptBuffer::SetSize (Bytes size, const String &typeName) __Th___
    {
        CHECK_THROW_MSG( not _resource,
            "resource is already created, can not change size" );
        CHECK_THROW_MSG( _layout.Empty(),
            "buffer has const data which will be lost" );

        _desc.size  = size;
        _layout     = BufferLayout{typeName};
    }

/*
=================================================
    SetLayoutName
=================================================
*/
    void  ScriptBuffer::SetLayoutName (const String &typeName) __Th___
    {
        CHECK_THROW_MSG( not _resource,
            "resource is already created, can not change size" );
        CHECK_THROW_MSG( StartsWith( _layout.typeName, "ConstLayout-" ));

        _layout.typeName = typeName;
    }

/*
=================================================
    SetLayout*
=================================================
*/
    void  ScriptBuffer::SetLayout1 (const String &typeName) __Th___
    {
        CHECK_THROW_MSG( _layout.typeName.empty() );
        CHECK_THROW_MSG( _layout.source.empty() );
        CHECK_THROW_MSG( _layout.staticSrc.empty() );
        _SetType( EBufferType::MutableData_NonInitialized );  // throw

        _layout.typeName = typeName;
        _staticCount     = UMax;
    }

    void  ScriptBuffer::SetLayout2 (const String &typeName, const String &source) __Th___
    {
        CHECK_THROW_MSG( _layout.typeName.empty() );
        CHECK_THROW_MSG( _layout.source.empty() );
        CHECK_THROW_MSG( _layout.staticSrc.empty() );
        _SetType( EBufferType::MutableData_NonInitialized );  // throw

        _layout.typeName = typeName;
        _layout.source   = source;
        _staticCount     = UMax;
    }

/*
=================================================
    SetArrayLayout*
=================================================
*/
    void  ScriptBuffer::SetArrayLayout1 (const String &typeName, uint count) __Th___
    {
        CHECK_THROW_MSG( _layout.typeName.empty() );
        CHECK_THROW_MSG( _layout.source.empty() );
        CHECK_THROW_MSG( _layout.staticSrc.empty() );
        CHECK_THROW_MSG( (not _dynCount) and (_staticCount == 0), "array size is already defined" );
        CHECK_THROW_MSG( count > 0 );

        _SetType( EBufferType::MutableData_NonInitialized );  // throw

        _layout.typeName = typeName;
        _staticCount     = count;
    }

    void  ScriptBuffer::SetArrayLayout2 (const String &typeName, const ScriptDynamicUIntPtr &count) __Th___
    {
        CHECK_THROW_MSG( _layout.typeName.empty() );
        CHECK_THROW_MSG( _layout.source.empty() );
        CHECK_THROW_MSG( _layout.staticSrc.empty() );
        CHECK_THROW_MSG( (not _dynCount) and (_staticCount == 0), "array size is already defined" );

        _SetType( EBufferType::MutableData_NonInitialized );  // throw

        _layout.typeName = typeName;
        _dynCount        = count;
    }

    void  ScriptBuffer::SetArrayLayout3 (const String &typeName, const String &source, uint count) __Th___
    {
        CHECK_THROW_MSG( _layout.typeName.empty() );
        CHECK_THROW_MSG( _layout.source.empty() );
        CHECK_THROW_MSG( _layout.staticSrc.empty() );
        CHECK_THROW_MSG( (not _dynCount) and (_staticCount == 0), "array size is already defined" );
        CHECK_THROW_MSG( count > 0 );

        _SetType( EBufferType::MutableData_NonInitialized );  // throw

        _layout.typeName = typeName;
        _layout.source   = source;
        _staticCount     = count;
    }

    void  ScriptBuffer::SetArrayLayout4 (const String &typeName, const String &source, const ScriptDynamicUIntPtr &count) __Th___
    {
        CHECK_THROW_MSG( _layout.typeName.empty() );
        CHECK_THROW_MSG( _layout.source.empty() );
        CHECK_THROW_MSG( _layout.staticSrc.empty() );
        CHECK_THROW_MSG( (not _dynCount) and (_staticCount == 0), "array size is already defined" );

        _SetType( EBufferType::MutableData_NonInitialized );  // throw

        _layout.typeName = typeName;
        _layout.source   = source;
        _dynCount        = count;
    }

/*
=================================================
    SetArrayLayout*
=================================================
*/
    void  ScriptBuffer::SetArrayLayout5 (const String &typeName, const String &source, const String &staticSrc, uint count) __Th___
    {
        CHECK_THROW_MSG( _layout.typeName.empty() );
        CHECK_THROW_MSG( _layout.source.empty() );
        CHECK_THROW_MSG( _layout.staticSrc.empty() );
        CHECK_THROW_MSG( (not _dynCount) and (_staticCount == 0), "array size is already defined" );
        CHECK_THROW_MSG( count > 0 );

        _SetType( EBufferType::MutableData_NonInitialized );  // throw

        _layout.typeName    = typeName;
        _layout.source      = source;
        _layout.staticSrc   = staticSrc;
        _staticCount        = count;
    }

/*
=================================================
    Name
=================================================
*/
    void  ScriptBuffer::Name (const String &name) __Th___
    {
        CHECK_THROW_MSG( not _resource,
            "resource is already created, can not change debug name" );

        _dbgName = name.substr( 0, ResNameMaxLen );
    }

/*
=================================================
    AddReference
=================================================
*/
    void  ScriptBuffer::AddReference (const ScriptBufferPtr &buf) __Th___
    {
        CHECK_THROW_MSG( buf );
        CHECK_THROW_MSG( not _resource,
            "resource is already created, can not add buffer reference" );

        RC<Buffer>  buf2 = buf->ToResource();
        CHECK_THROW_MSG( buf2 );

        _refBuffers.push_back( buf2 );
    }

/*
=================================================
    Float* (vector)
=================================================
*/
    uint  ScriptBuffer::Float1 (const String &name, float x) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  float  " << name << ";\n";
        return _layout.Put( &x, Sizeof(x), 4_b );
    }

    uint  ScriptBuffer::Float2v (const String &name, const packed_float2 &v) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  float2  " << name << ";\n";
        return _layout.Put( &v, Sizeof(v), 8_b );
    }

    uint  ScriptBuffer::Float3v (const String &name, const packed_float3 &v) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  float3  " << name << ";\n";
        return _layout.Put( &v, Sizeof(v), 16_b );
    }

    uint  ScriptBuffer::Float4v (const String &name, const packed_float4 &v) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  float4  " << name << ";\n";
        return _layout.Put( &v, Sizeof(v), 16_b );
    }

/*
=================================================
    Float* (matrix)
=================================================
*/
    uint  ScriptBuffer::Float2x2 (const String &name, const packed_float2x2 &m) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  float2x2  " << name << ";\n";
        return _layout.Put( &m, Sizeof(m), 16_b );
    }

    uint  ScriptBuffer::Float2x3 (const String &name, const packed_float2x3 &m) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  float2x3  " << name << ";\n";
        return _layout.Put( &m, Sizeof(m), 16_b );
    }

    uint  ScriptBuffer::Float2x4 (const String &name, const packed_float2x4 &m) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  float2x4  " << name << ";\n";
        return _layout.Put( &m, Sizeof(m), 16_b );
    }

    uint  ScriptBuffer::Float3x2 (const String &name, const packed_float3x2 &m) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  float3x2  " << name << ";\n";
        return _layout.Put( &m, Sizeof(m), 16_b );
    }

    uint  ScriptBuffer::Float3x3 (const String &name, const packed_float3x3 &m) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  float3x3  " << name << ";\n";
        return _layout.Put( &m, Sizeof(m), 16_b );
    }

    uint  ScriptBuffer::Float3x4 (const String &name, const packed_float3x4 &m) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  float3x4  " << name << ";\n";
        return _layout.Put( &m, Sizeof(m), 16_b );
    }

    uint  ScriptBuffer::Float4x2 (const String &name, const packed_float4x2 &m) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  float4x2  " << name << ";\n";
        return _layout.Put( &m, Sizeof(m), 16_b );
    }

    uint  ScriptBuffer::Float4x3 (const String &name, const packed_float4x3 &m) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  float4x3  " << name << ";\n";
        return _layout.Put( &m, Sizeof(m), 16_b );
    }

    uint  ScriptBuffer::Float4x4 (const String &name, const packed_float4x4 &m) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  float4x4  " << name << ";\n";
        return _layout.Put( &m, Sizeof(m), 16_b );
    }

/*
=================================================
    Int*
=================================================
*/
    uint  ScriptBuffer::Int1 (const String &name, int x) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  int  " << name << ";\n";
        return _layout.Put( &x, Sizeof(x), 4_b );
    }

    uint  ScriptBuffer::Int2v (const String &name, const packed_int2 &v) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  int2  " << name << ";\n";
        return _layout.Put( &v, Sizeof(v), 8_b );
    }

    uint  ScriptBuffer::Int3v (const String &name, const packed_int3 &v) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  int3  " << name << ";\n";
        return _layout.Put( &v, Sizeof(v), 16_b );
    }

    uint  ScriptBuffer::Int4v (const String &name, const packed_int4 &v) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  int4  " << name << ";\n";
        return _layout.Put( &v, Sizeof(v), 16_b );
    }

/*
=================================================
    UInt*
=================================================
*/
    uint  ScriptBuffer::UInt1 (const String &name, uint x) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  uint  " << name << ";\n";
        return _layout.Put( &x, Sizeof(x), 4_b );
    }

    uint  ScriptBuffer::UInt2v (const String &name, const packed_uint2 &v) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  uint2  " << name << ";\n";
        return _layout.Put( &v, Sizeof(v), 8_b );
    }

    uint  ScriptBuffer::UInt3v (const String &name, const packed_uint3 &v) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  uint3  " << name << ";\n";
        return _layout.Put( &v, Sizeof(v), 16_b );
    }

    uint  ScriptBuffer::UInt4v (const String &name, const packed_uint4 &v) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  uint4  " << name << ";\n";
        return _layout.Put( &v, Sizeof(v), 16_b );
    }

/*
=================================================
    ULong*
=================================================
*/
    uint  ScriptBuffer::ULong1 (const String &name, ulong x) __Th___
    {
        _InitConstDataFromScriptLayout();
        _layout.source << "  ulong  " << name << ";\n";
        return _layout.Put( &x, Sizeof(x), 8_b );
    }

/*
=================================================
    Float*Array (vec)
=================================================
*/
    uint  ScriptBuffer::Float1Array (const String &name, const ScriptArray<float> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  float  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 4_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Float2Array (const String &name, const ScriptArray<packed_float2> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  float2  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 8_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Float3Array (const String &name, const ScriptArray<packed_float3> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  float3  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Float4Array (const String &name, const ScriptArray<packed_float4> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  float4  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

/*
=================================================
    Float*Array (matrix)
=================================================
*/
    uint  ScriptBuffer::Float2x2Array (const String &name, const ScriptArray<packed_float2x2> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  float2x2  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Float2x3Array (const String &name, const ScriptArray<packed_float2x3> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  float2x3  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Float2x4Array (const String &name, const ScriptArray<packed_float2x4> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  float2x4  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Float3x2Array (const String &name, const ScriptArray<packed_float3x2> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  float3x2  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Float3x3Array (const String &name, const ScriptArray<packed_float3x3> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  float3x3  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Float3x4Array (const String &name, const ScriptArray<packed_float3x4> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  float3x4  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Float4x2Array (const String &name, const ScriptArray<packed_float4x2> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  float4x2  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Float4x3Array (const String &name, const ScriptArray<packed_float4x3> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  float4x3  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Float4x4Array (const String &name, const ScriptArray<packed_float4x4> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  float4x4  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

/*
=================================================
    Int*Array
=================================================
*/
    uint  ScriptBuffer::Int1Array (const String &name, const ScriptArray<int> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  int  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 4_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Int2Array (const String &name, const ScriptArray<packed_int2> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  int2  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 8_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Int3Array (const String &name, const ScriptArray<packed_int3> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  int3  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

    uint  ScriptBuffer::Int4Array (const String &name, const ScriptArray<packed_int4> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  int4  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

/*
=================================================
    UInt*Array
=================================================
*/
    uint  ScriptBuffer::UInt1Array (const String &name, const ScriptArray<uint> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  uint  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 4_b ));
        }
        return result;
    }

    uint  ScriptBuffer::UInt2Array (const String &name, const ScriptArray<packed_uint2> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  uint2  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 8_b ));
        }
        return result;
    }

    uint  ScriptBuffer::UInt3Array (const String &name, const ScriptArray<packed_uint3> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  uint3  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

    uint  ScriptBuffer::UInt4Array (const String &name, const ScriptArray<packed_uint4> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  uint4  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
        }
        return result;
    }

/*
=================================================
    ULong*Array
=================================================
*/
    uint  ScriptBuffer::ULong1Array (const String &name, const ScriptArray<ulong> &arr) __Th___
    {
        CHECK_THROW_MSG( not arr.empty() );
        _InitConstDataFromScriptLayout();
        _layout.source << "  ulong  " << name << " [" << ToString(arr.size()) << "];\n";

        uint    result = UMax;
        for (auto x : arr) {
            result = Min( result, _layout.Put( &x, Sizeof(x), 8_b ));
        }
        return result;
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptBuffer::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptBuffer>    binder{ se };
        binder.CreateRef();
        binder.AddFactoryCtor( &ScriptBuffer_Ctor1,     {"sizeInBytes"} );
        binder.AddFactoryCtor( &ScriptBuffer_Ctor2,     {"filenameInVFS"} );

        binder.Comment( "Set resource name. It is used for debugging." );
        binder.AddMethod( &ScriptBuffer::Name,                  "Name",             {} );

        binder.Comment( "Set explicit name of the 'ShaderStructType' which will be created for buffer data layout.\n"
                        "It is used when buffer is passed to the pipeline which is explicitly declared (in 'pipelines' folder)\n"
                        "so typename must match in 'Layout()' and in 'ds.StorageBuffer()' call in pipeline script." );
        binder.AddMethod( &ScriptBuffer::SetLayoutName,         "LayoutName",       {"typeName"} );

        binder.Comment( "Allow to declare array of struct with constant or dynamic size.\n"
                        "Layout will be '{ <typeName>  elements [<count>]; }'.\n"
                        "'typeName' must be previously declared or one of built-in type:\n"
                        "\t'DispatchIndirectCommand', 'DrawIndirectCommand', 'DrawIndexedIndirectCommand',\n"
                        "\t'DrawMeshTasksIndirectCommand', 'TraceRayIndirectCommand', 'ASBuildIndirectCommand'\n"
                        "\t'AccelStructInstance'." );
        binder.AddMethod( &ScriptBuffer::SetArrayLayout1,       "ArrayLayout",      {"typeName", "count"} );
        binder.AddMethod( &ScriptBuffer::SetArrayLayout2,       "ArrayLayout",      {"typeName", "count"} );

        binder.Comment( "Allow to declare array of struct with constant or dynamic size.\n"
                        "Created a new structure with type 'typeName' and fields in 'source'.\n"
                        "See field declaration rules for 'ShaderStructType::Set()' method in [pipeline_compiler.as](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/pipeline_compiler.as).");
        binder.AddMethod( &ScriptBuffer::SetArrayLayout3,       "ArrayLayout",      {"typeName", "source", "count"} );
        binder.AddMethod( &ScriptBuffer::SetArrayLayout4,       "ArrayLayout",      {"typeName", "source", "count"} );

        binder.AddMethod( &ScriptBuffer::SetArrayLayout5,       "ArrayLayout",      {"typeName", "arrayElementSource", "staticSource", "count"} );

        binder.Comment( "Allow to declare single structure as a buffer layout.\n"
                        "'typeName' must be previously declared or one of built-in type (see 'ArrayLayout')." );
        binder.AddMethod( &ScriptBuffer::SetLayout1,            "UseLayout",        {"typeName"} );

        binder.Comment( "Created a new structure with type 'typeName' and fields in 'source'.\n"
                        "See field declaration rules for 'ShaderStructType::Set()' method in [pipeline_compiler.as](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/pipeline_compiler.as).");
        binder.AddMethod( &ScriptBuffer::SetLayout2,            "UseLayout",        {"typeName", "source"} );

        binder.Comment( "Returns buffer device address.\n"
                        "Requires 'GL_EXT_buffer_reference extension' in GLSL.\n"
                        "It passed as 'uint64' type so you should cast it to buffer reference type." );
        binder.AddMethod( &ScriptBuffer::GetDeviceAddress,      "DeviceAddress",    {} );

        binder.Comment( "Force enable buffer content history.\n"
                        "It store copy of the buffer content on last N frames." );
        binder.AddMethod( &ScriptBuffer::EnableHistory,         "EnableHistory",    {} );

        binder.Comment( "Call this method if 'DeviceAddress()' of another buffer is used in current buffer to avoid missed synchronizations." );
        binder.AddMethod( &ScriptBuffer::AddReference,          "AddReference",     {} );

        binder.Comment( "Build buffer data layout with initial content.\n"
                        "Returns offset in bytes where data is begin." );

        binder.AddMethod( &ScriptBuffer::Float1,        "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float2,        "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float3,        "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float4,        "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float2v,       "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float3v,       "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float4v,       "Float",        {} );

        binder.AddMethod( &ScriptBuffer::Float2x2,      "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float2x3,      "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float2x4,      "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float3x2,      "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float3x3,      "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float3x4,      "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float4x2,      "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float4x3,      "Float",        {} );
        binder.AddMethod( &ScriptBuffer::Float4x4,      "Float",        {} );

        binder.AddMethod( &ScriptBuffer::Int1,          "Int",          {} );
        binder.AddMethod( &ScriptBuffer::Int2,          "Int",          {} );
        binder.AddMethod( &ScriptBuffer::Int3,          "Int",          {} );
        binder.AddMethod( &ScriptBuffer::Int4,          "Int",          {} );
        binder.AddMethod( &ScriptBuffer::Int2v,         "Int",          {} );
        binder.AddMethod( &ScriptBuffer::Int3v,         "Int",          {} );
        binder.AddMethod( &ScriptBuffer::Int4v,         "Int",          {} );

        binder.AddMethod( &ScriptBuffer::UInt1,         "Uint",         {} );
        binder.AddMethod( &ScriptBuffer::UInt2,         "Uint",         {} );
        binder.AddMethod( &ScriptBuffer::UInt3,         "Uint",         {} );
        binder.AddMethod( &ScriptBuffer::UInt4,         "Uint",         {} );
        binder.AddMethod( &ScriptBuffer::UInt2v,        "Uint",         {} );
        binder.AddMethod( &ScriptBuffer::UInt3v,        "Uint",         {} );
        binder.AddMethod( &ScriptBuffer::UInt4v,        "Uint",         {} );

        binder.AddMethod( &ScriptBuffer::ULong1,        "ULong",        {} );

        binder.AddMethod( &ScriptBuffer::Float1Array,   "FloatArray",   {} );
        binder.AddMethod( &ScriptBuffer::Float2Array,   "FloatArray",   {} );
        binder.AddMethod( &ScriptBuffer::Float3Array,   "FloatArray",   {} );
        binder.AddMethod( &ScriptBuffer::Float4Array,   "FloatArray",   {} );

        binder.AddMethod( &ScriptBuffer::Float2x2Array, "FloatArray",   {} );
        binder.AddMethod( &ScriptBuffer::Float2x3Array, "FloatArray",   {} );
        binder.AddMethod( &ScriptBuffer::Float2x4Array, "FloatArray",   {} );
        binder.AddMethod( &ScriptBuffer::Float3x2Array, "FloatArray",   {} );
        binder.AddMethod( &ScriptBuffer::Float3x3Array, "FloatArray",   {} );
        binder.AddMethod( &ScriptBuffer::Float3x4Array, "FloatArray",   {} );
        binder.AddMethod( &ScriptBuffer::Float4x2Array, "FloatArray",   {} );
        binder.AddMethod( &ScriptBuffer::Float4x3Array, "FloatArray",   {} );
        binder.AddMethod( &ScriptBuffer::Float4x4Array, "FloatArray",   {} );

        binder.AddMethod( &ScriptBuffer::Int1Array,     "IntArray",     {} );
        binder.AddMethod( &ScriptBuffer::Int2Array,     "IntArray",     {} );
        binder.AddMethod( &ScriptBuffer::Int3Array,     "IntArray",     {} );
        binder.AddMethod( &ScriptBuffer::Int4Array,     "IntArray",     {} );

        binder.AddMethod( &ScriptBuffer::UInt1Array,    "UIntArray",    {} );
        binder.AddMethod( &ScriptBuffer::UInt2Array,    "UIntArray",    {} );
        binder.AddMethod( &ScriptBuffer::UInt3Array,    "UIntArray",    {} );
        binder.AddMethod( &ScriptBuffer::UInt4Array,    "UIntArray",    {} );

        binder.AddMethod( &ScriptBuffer::ULong1Array,   "ULongArray",   {} );
    }

/*
=================================================
    _InitConstDataFromScriptLayout
=================================================
*/
    void  ScriptBuffer::_InitConstDataFromScriptLayout () __Th___
    {
        _SetType( EBufferType::ConstDataFromScript );  // throw

        if ( _layout.typeName.empty() )
            _layout.typeName = "ConstLayout-"s << ToString<16>( BitCast<usize>(this) );
    }

/*
=================================================
    _InitMutableDataFromScriptLayout
=================================================
*/
    void  ScriptBuffer::_InitMutableDataFromScriptLayout () __Th___
    {
        _SetType( EBufferType::MutableDataFromScript );  // throw

        if ( _layout.typeName.empty() )
            _layout.typeName = "MutableLayout-"s << ToString<16>( BitCast<usize>(this) );
    }

/*
=================================================
    ToResource
=================================================
*/
    RC<Buffer>  ScriptBuffer::ToResource () __Th___
    {
        if ( _resource )
            return _resource;

        AddLayoutReflection();  // throw

        Buffer::EBufferFlags    flags = Default;

        CHECK_ERR_MSG( _resUsage != Default, "failed to create buffer '"s << _dbgName << "'" );
        for (auto usage : BitfieldIterate( _resUsage ))
        {
            switch_enum( usage )
            {
                case EResourceUsage::ComputeRead :      _desc.usage |= EBufferUsage::Storage | EBufferUsage::TransferSrc;       break;
                case EResourceUsage::ComputeWrite :     _desc.usage |= EBufferUsage::Storage;                                   break;

                case EResourceUsage::UploadedData :     _desc.usage |= EBufferUsage::TransferDst;                               break;
                case EResourceUsage::WillReadback :     _desc.usage |= EBufferUsage::TransferSrc;                               break;

                case EResourceUsage::VertexInput :      _desc.usage |= EBufferUsage::Vertex | EBufferUsage::Index;              break;

                case EResourceUsage::IndirectBuffer :   _desc.usage |= EBufferUsage::Indirect;                                  break;
                case EResourceUsage::ASBuild :          _desc.usage |= EBufferUsage::ASBuild_ReadOnly;                          break;
                case EResourceUsage::ShaderAddress :    _desc.usage |= EBufferUsage::ShaderAddress;                             break;

                case EResourceUsage::WithHistory :      flags |= Buffer::EBufferFlags::WithHistory;                             break;
                case EResourceUsage::Transfer :         _desc.usage |= EBufferUsage::Transfer;                                  break;

                case EResourceUsage::Unknown :
                case EResourceUsage::Sampled :
                case EResourceUsage::GenMipmaps :
                case EResourceUsage::ColorAttachment :
                case EResourceUsage::DepthStencil :
                case EResourceUsage::ComputeRW :
                case EResourceUsage::Present :
                default :                               RETURN_ERR( "unsupported usage" );
            }
            switch_end
        }

        switch_enum( _type )
        {
            case EBufferType::ConstDataFromScript :
            {
                CHECK_THROW( HasLayout() );
                _desc.size = Bytes{_layout._data.size()};
                break;
            }
            case EBufferType::ConstDataFromFile :               break;
            case EBufferType::MutableData_NonInitialized :      break;
            case EBufferType::MutableDataFromScript :           break;
            case EBufferType::Unknown :                         break;
        }
        switch_end

        // allow 'FillBuffer' on init
        _desc.usage |= EBufferUsage::TransferDst;

        auto&               res_mngr    = GraphicsScheduler().GetResourceManager();
        Renderer&           renderer    = ScriptExe::ScriptResourceApi::GetRenderer(); // throw
        GfxMemAllocatorPtr  gfx_alloc   = _dynCount ? renderer.GetDynamicAllocator() : renderer.GetAllocator();

        Buffer::LoadOp      load_op;
        load_op.clear       = (not _filename.IsDefined()) and _layout._data.empty();
        load_op.filename    = _filename;
        load_op.data        = RVRef(_layout._data);

        Bytes   elem_size;

        if ( HasLayout() )
        {
            using namespace AE::PipelineCompiler;

            auto    storage = ObjectStorage::Instance();
            if ( storage )
            {
                auto&   st_types    = storage->structTypes;
                auto    it          = st_types.find( GetTypeName() );
                CHECK_THROW_MSG( it != st_types.end(),
                    "Can't find ShaderStructType '"s << GetTypeName() << "'" );

                if ( _dynCount )
                {
                    UNTESTED;
                    CHECK_THROW_MSG( it->second->HasDynamicArray() );
                    CHECK_THROW_MSG( _desc.size == 0 );

                    elem_size = it->second->ArrayStride();
                    _desc.size = elem_size;
                }

                if ( _staticCount > 0 )
                {
                    const bool  is_array = (_staticCount != UMax);
                    CHECK_THROW_MSG( is_array == it->second->HasDynamicArray() );
                    CHECK_THROW_MSG( _desc.size == 0 );

                    _desc.size = it->second->TotalSize( is_array ? _staticCount : 0 );
                }
            }
            ASSERT( not (_dynCount or _staticCount > 0) or storage );
        }

        CHECK_THROW_MSG( _desc.size > 0,
            "Buffer '"s << _dbgName << "' size is 0" );
        CHECK_THROW_MSG( res_mngr.IsSupported( _desc ),
            "Buffer '"s << _dbgName << "' description is not supported by GPU device" );

        const ShaderStructName  struct_type {GetTypeName()};
        Buffer::IDs_t           buf_ids;

        if ( AllBits( flags, Buffer::EBufferFlags::WithHistory ))
        {
            for (auto& id : buf_ids) {
                id = res_mngr.CreateBuffer( _desc, _dbgName, gfx_alloc );
                CHECK_ERR( id );
            }
        }
        else
        {
            buf_ids[0] = res_mngr.CreateBuffer( _desc, _dbgName, gfx_alloc );
            CHECK_ERR( buf_ids[0] );

            for (usize i = 1; i < buf_ids.size(); ++i) {
                buf_ids[i] = res_mngr.AcquireResource( buf_ids[0] );
            }
        }

        _resource = MakeRCTh<Buffer>( RVRef(buf_ids), _desc, elem_size, RVRef(load_op), struct_type,
                                      renderer, (_dynCount ? _dynCount->Get() : null), _dbgName, flags, RVRef(_refBuffers) );  // throw
        return _resource;
    }

/*
=================================================
    AddLayoutReflection
=================================================
*/
    void  ScriptBuffer::AddLayoutReflection () C_Th___
    {
        using namespace AE::PipelineCompiler;

        if ( not HasLayout() )
            return;

        const auto  AddStructType = [] (const String &typeName, const String &source, Bool srcIsOptional = False{})
        {{
            auto    storage     = ObjectStorage::Instance();
            if ( not storage )
                return;

            auto&   st_types    = storage->structTypes;
            auto    it          = st_types.find( typeName );

            if ( it != st_types.end() )
            {
                const String    tmp_typename = "_Temp_"s + typeName;

                if ( st_types.contains( tmp_typename ))
                    return; // double check

                // compare 'source' with existing structure
                if ( not source.empty() )
                {
                    ShaderStructTypePtr     tmp{ new ShaderStructType{ tmp_typename }};
                    tmp->Set( EStructLayout::Std430, source );

                    CHECK( it->second->Compare( *tmp ));

                    st_types.erase( tmp_typename );
                }
                return;
            }

            if ( source.empty() and srcIsOptional )
                return;

            ShaderStructTypePtr     st{ new ShaderStructType{ typeName }};
            st->Set( EStructLayout::Std430, source );
        }};

        if ( _IsArray() )
        {
            String  src;
            src << _layout.staticSrc << '\n';
            src << _layout.typeName << "  elements [];";

            AddStructType( _layout.typeName, _layout.source, True{"opt"} );
            AddStructType( GetTypeName(), src );
        }
        else
        {
            AddStructType( _layout.typeName, _layout.source );
        }
    }

/*
=================================================
    GetFieldOffset
=================================================
*/
    Bytes  ScriptBuffer::GetFieldOffset (const String &name) __Th___
    {
        auto*   field = GetField( name ).GetIf< PipelineCompiler::ShaderStructType::Field >();  // throw
        CHECK_THROW_MSG( field != null );
        return field->offset;
    }

/*
=================================================
    GetFieldStructName
=================================================
*/
    StringView  ScriptBuffer::GetFieldStructName (const String &name) __Th___
    {
        auto*   field = GetField( name ).GetIf< PipelineCompiler::ShaderStructType::Field >();  // throw
        CHECK_THROW_MSG( field != null );
        return  field->stType ? field->stType->Typename() : Default;
    }

/*
=================================================
    GetFieldType
=================================================
*/
    uint  ScriptBuffer::GetFieldType (const String &name) __Th___
    {
        auto*   field = GetField( name ).GetIf< PipelineCompiler::ShaderStructType::Field >();  // throw
        CHECK_THROW_MSG( field != null );
        CHECK_THROW_MSG( field->IsScalar() );
        CHECK_THROW_MSG( not field->stType );
        return uint(field->type);
    }

/*
=================================================
    GetField
=================================================
*/
    AnyTypeCRef  ScriptBuffer::GetField (const String &name) __Th___
    {
        using namespace AE::PipelineCompiler;

        CHECK_THROW_MSG( HasLayout() );

        auto&   st_types    = ObjectStorage::Instance()->structTypes;
        auto    it          = st_types.find( GetTypeName() );

        CHECK_THROW_MSG( it != st_types.end() )

        for (auto& field : it->second->Fields())
        {
            if ( field.name == name )
                return AnyTypeCRef{field};
        }

        CHECK_THROW_MSG( false,
            "Failed to find field '"s << name << "' for type '" << GetTypeName() << "'" );
    }


} // AE::ResEditor
