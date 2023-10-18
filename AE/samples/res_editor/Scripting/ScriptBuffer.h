// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"
#include "res_editor/Resources/Buffer.h"

namespace AE::ResEditor
{

    //
    // Buffer
    //

    class ScriptBuffer final : public EnableScriptRC
    {
    // types
    public:
        enum class EBufferType
        {
            Unknown,
            ConstDataFromFile,
            MutableData_NonInitialized,
            ConstDataFromScript,
            MutableDataFromScript,
        };

    private:
        struct BufferLayout
        {
        // variables
            Array<ubyte>        _data;
            String              source;
            String              staticSrc;
            Bytes               _align  = 4_b;
            String              typeName;           // ShaderStructName

        // methods
            BufferLayout () {}
            explicit BufferLayout (const String &type) : typeName{type} {}

            ND_ uint    Put (const void* data, Bytes dataSize, Bytes align);
            ND_ uint    Put (Bytes dataSize, Bytes align);

            ND_ bool    Empty ()        const { return _data.empty() or source.empty(); }
        };


    // variables
    private:
        const VFS::FileName     _filename;
        BufferLayout            _layout;
        BufferDesc              _desc;
        BufferViewDesc          _viewDesc;
        EBufferType             _type           = Default;
        EResourceUsage          _resUsage       = Default;
        uint                    _texbufType     = 0;        // PipelineCompiler::EImageType
        String                  _dbgName;

        ScriptDynamicUIntPtr    _dynCount;
        uint                    _staticCount    = 0;

        RC<Buffer>              _resource;
        Array<RC<Buffer>>       _refBuffers;                // if this buffer uses device address for another buffer, they must be in 'ShaderAddress' state


    // methods
    public:
        ScriptBuffer ()                                                                         __Th___;
        ScriptBuffer (Bytes size)                                                               __Th___;
        ScriptBuffer (const String &filename)                                                   __Th___;
        ~ScriptBuffer ();

        void  Name (const String &name)                                                         __Th___;

        void  AddUsage (EResourceUsage usage)                                                   __Th___;
        void  EnableHistory ()                                                                  __Th___ { AddUsage( EResourceUsage::WithHistory ); }

        void  SetSize (Bytes size, const String &typeName = Default)                            __Th___;
        void  SetLayoutName (const String &typeName)                                            __Th___;
        void  SetLayout1 (const String &typeName)                                               __Th___;
        void  SetLayout2 (const String &typeName, const String &source)                         __Th___;
        void  SetArrayLayout1 (const String &typeName, uint count)                              __Th___;
        void  SetArrayLayout2 (const String &typeName, const ScriptDynamicUIntPtr &count)       __Th___;
        void  SetArrayLayout3 (const String &typeName, const String &source, uint count)        __Th___;
        void  SetArrayLayout4 (const String &typeName, const String &source,
                               const ScriptDynamicUIntPtr &count)                               __Th___;
        void  SetArrayLayout5 (const String &typeName, const String &source,
                               const String &staticSrc, uint count)                             __Th___;

        ND_ StringView      GetName ()                                                          C_NE___ { return _dbgName; }
        ND_ bool            HasLayout ()                                                        C_NE___ { return not _layout.typeName.empty(); }
        ND_ String          GetTypeName ()                                                      C_NE___;
        ND_ uint            TexelBufferType ()                                                  C_NE___ { ASSERT( not HasLayout() );  return _texbufType; }
        ND_ bool            IsDynamicSize ()                                                    C_NE___ { return _dynCount != null; }
        ND_ ulong           GetDeviceAddress ()                                                 __Th___;
        ND_ EPixelFormat    GetViewFormat ()                                                    C_Th___;    // TODO
        ND_ bool            WithHistory ()                                                      C_NE___ { return AllBits( _resUsage, EResourceUsage::WithHistory ); }

        ND_ Bytes           GetFieldOffset (const String &name)                                 __Th___;
        ND_ uint            GetFieldType (const String &name)                                   __Th___;    // PipelineCompiler::EValueType
        ND_ StringView      GetFieldStructName (const String &name)                             __Th___;
        ND_ AnyTypeCRef     GetField (const String &name)                                       __Th___;    // PipelineCompiler::ShaderStructType::Field


        void  AddReference (const ScriptBufferPtr &buf)                                         __Th___;


      // Build buffer layout with initial data.
      // Returns initial offset in bytes.

        uint  Float1  (const String &name, float x)                                             __Th___;
        uint  Float2  (const String &name, float x, float y)                                    __Th___ { return Float2v( name, packed_float2{ x, y }); }
        uint  Float3  (const String &name, float x, float y, float z)                           __Th___ { return Float3v( name, packed_float3{ x, y, z }); }
        uint  Float4  (const String &name, float x, float y, float z, float w)                  __Th___ { return Float4v( name, packed_float4{ x, y, z, w }); }
        uint  Float2v (const String &name, const packed_float2 &v)                              __Th___;
        uint  Float3v (const String &name, const packed_float3 &v)                              __Th___;
        uint  Float4v (const String &name, const packed_float4 &v)                              __Th___;

        uint  Float2x2 (const String &name, const packed_float2x2 &m)                           __Th___;
        uint  Float2x3 (const String &name, const packed_float2x3 &m)                           __Th___;
        uint  Float2x4 (const String &name, const packed_float2x4 &m)                           __Th___;
        uint  Float3x2 (const String &name, const packed_float3x2 &m)                           __Th___;
        uint  Float3x3 (const String &name, const packed_float3x3 &m)                           __Th___;
        uint  Float3x4 (const String &name, const packed_float3x4 &m)                           __Th___;
        uint  Float4x2 (const String &name, const packed_float4x2 &m)                           __Th___;
        uint  Float4x3 (const String &name, const packed_float4x3 &m)                           __Th___;
        uint  Float4x4 (const String &name, const packed_float4x4 &m)                           __Th___;

        uint  Int1  (const String &name, int x)                                                 __Th___;
        uint  Int2  (const String &name, int x, int y)                                          __Th___ { return Int2v( name, packed_int2{ x, y }); }
        uint  Int3  (const String &name, int x, int y, int z)                                   __Th___ { return Int3v( name, packed_int3{ x, y, z }); }
        uint  Int4  (const String &name, int x, int y, int z, int w)                            __Th___ { return Int4v( name, packed_int4{ x, y, z, w }); }
        uint  Int2v (const String &name, const packed_int2 &v)                                  __Th___;
        uint  Int3v (const String &name, const packed_int3 &v)                                  __Th___;
        uint  Int4v (const String &name, const packed_int4 &v)                                  __Th___;

        uint  UInt1  (const String &name, uint x)                                               __Th___;
        uint  UInt2  (const String &name, uint x, uint y)                                       __Th___ { return UInt2v( name, packed_uint2{ x, y }); }
        uint  UInt3  (const String &name, uint x, uint y, uint z)                               __Th___ { return UInt3v( name, packed_uint3{ x, y, z }); }
        uint  UInt4  (const String &name, uint x, uint y, uint z, uint w)                       __Th___ { return UInt4v( name, packed_uint4{ x, y, z, w }); }
        uint  UInt2v (const String &name, const packed_uint2 &v)                                __Th___;
        uint  UInt3v (const String &name, const packed_uint3 &v)                                __Th___;
        uint  UInt4v (const String &name, const packed_uint4 &v)                                __Th___;

        uint  ULong1  (const String &name, ulong x)                                             __Th___;

        uint  Float1Array (const String &name, const ScriptArray<float> &arr)                   __Th___;
        uint  Float2Array (const String &name, const ScriptArray<packed_float2> &arr)           __Th___;
        uint  Float3Array (const String &name, const ScriptArray<packed_float3> &arr)           __Th___;
        uint  Float4Array (const String &name, const ScriptArray<packed_float4> &arr)           __Th___;

        uint  Float2x2Array (const String &name, const ScriptArray<packed_float2x2> &m)         __Th___;
        uint  Float2x3Array (const String &name, const ScriptArray<packed_float2x3> &m)         __Th___;
        uint  Float2x4Array (const String &name, const ScriptArray<packed_float2x4> &m)         __Th___;
        uint  Float3x2Array (const String &name, const ScriptArray<packed_float3x2> &m)         __Th___;
        uint  Float3x3Array (const String &name, const ScriptArray<packed_float3x3> &m)         __Th___;
        uint  Float3x4Array (const String &name, const ScriptArray<packed_float3x4> &m)         __Th___;
        uint  Float4x2Array (const String &name, const ScriptArray<packed_float4x2> &m)         __Th___;
        uint  Float4x3Array (const String &name, const ScriptArray<packed_float4x3> &m)         __Th___;
        uint  Float4x4Array (const String &name, const ScriptArray<packed_float4x4> &m)         __Th___;

        uint  Int1Array (const String &name, const ScriptArray<int> &arr)                       __Th___;
        uint  Int2Array (const String &name, const ScriptArray<packed_int2> &arr)               __Th___;
        uint  Int3Array (const String &name, const ScriptArray<packed_int3> &arr)               __Th___;
        uint  Int4Array (const String &name, const ScriptArray<packed_int4> &arr)               __Th___;

        uint  UInt1Array (const String &name, const ScriptArray<uint> &arr)                     __Th___;
        uint  UInt2Array (const String &name, const ScriptArray<packed_uint2> &arr)             __Th___;
        uint  UInt3Array (const String &name, const ScriptArray<packed_uint3> &arr)             __Th___;
        uint  UInt4Array (const String &name, const ScriptArray<packed_uint4> &arr)             __Th___;

        uint  ULong1Array (const String &name, const ScriptArray<ulong> &arr)                   __Th___;


        static void  Bind (const ScriptEnginePtr &se)                                           __Th___;

            void        AddLayoutReflection ()                                                  C_Th___;

        ND_ RC<Buffer>  ToResource ()                                                           __Th___;


    private:
        ND_ bool  _IsArray ()                                                                   C_NE___;

        void  _SetType (EBufferType type)                                                       __Th___;

        void  _InitConstDataFromScriptLayout ()                                                 __Th___;
        void  _InitMutableDataFromScriptLayout ()                                               __Th___;

        static void  _ValidateResourceUsage (EResourceUsage usage)                              __Th___;
    };


} // AE::ResEditor
