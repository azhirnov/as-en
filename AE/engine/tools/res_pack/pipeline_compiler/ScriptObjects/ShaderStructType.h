#pragma once

#include "ScriptObjects/ScriptFeatureSet.h"

namespace AE::PipelineCompiler
{

    //
    // Array Size Helper
    //
    struct ArraySize
    {
    // variables
        uint        value   = 0;

    // methods
        ArraySize () {}
        explicit ArraySize (uint v) : value{v} {}

        static void  Bind (const ScriptEnginePtr &se) __Th___;
    };



    //
    // Align Helper
    //
    struct Align
    {
    // variables
        uint        value   = 0;

    // methods
        Align () {}
        explicit Align (uint v) : value{v} {}

        static void  Bind (const ScriptEnginePtr &se) __Th___;
    };


    struct ShaderStructType;
    using ShaderStructTypePtr = ScriptRC< ShaderStructType >;



    //
    // Shader Structure Type
    //
    struct ShaderStructType final : EnableScriptRC
    {
    // types
    public:
        enum class EUsage
        {
            Unknown         = 0,
            ShaderIO        = 1 << 0,
            VertexAttribs   = 1 << 1,   // vertex input in shader
            VertexLayout    = 1 << 2,   // vertex format in buffer
            BufferLayout    = 1 << 3,
            BufferReference = 1 << 4,
            _Last,
            All             = ((_Last - 1) << 1) - 1
        };

        struct Field
        {
            String              name;
            EValueType          type        = Default;
            ShaderStructTypePtr stType;
            uint                arraySize   = 0;        // 0 - non-array, UMax - dynamic
            ubyte               rows        = 0;
            ubyte               cols        = 0;
            bool                packed      : 1;
            bool                pointer     : 1;
            bool                address     : 1;        // typed device address
            bool                padding     : 1;
            Bytes               size;
            Bytes               align;
            Bytes               offset;

            Field () : packed{false}, pointer{false}, address{false}, padding{false} {}

            ND_ bool    IsScalar ()                     const   { return (rows == 1)  and (cols == 1) and (not IsStruct()); }
            ND_ bool    IsVec ()                        const   { return (rows >  1)  and (cols == 1) and (not IsStruct()); }
            ND_ bool    IsMat ()                        const   { return (cols >  1)  and (not IsStruct()); }
            ND_ bool    IsStruct ()                     const   { return bool(stType) and (not IsDeviceAddress()); }
            ND_ bool    IsBufferRef ()                  const   { return bool(stType) and address; }
            ND_ bool    IsPointer ()                    const   { return pointer; }
            ND_ bool    IsDeviceAddress ()              const   { return address or (type == EValueType::DeviceAddress); }      // typed or untyped
            ND_ bool    IsUntypedDeviceAddress ()       const   { return (not address) and (type == EValueType::DeviceAddress); }
            ND_ bool    IsDynamicArray ()               const   { return arraySize == UMax; }
            ND_ bool    IsArray ()                      const   { return arraySize != 0; }
            ND_ bool    IsStaticArray ()                const   { return (arraySize != 0) and (not IsDynamicArray()); }
            ND_ bool    operator == (const Field &rhs)  const;
        };

    private:
        struct ValidationData
        {
            // in
            ArrayView<ScriptFeatureSetPtr>  features;
            EStructLayout                   layout;
            Bytes                           baseOffset;

            // mutable
            Bytes       mslOffset;
            Bytes       glslOffset;
            Bytes       cppOffset;

            ValidationData (ArrayView<ScriptFeatureSetPtr> inFeatures, EStructLayout inLayout) :
                features{inFeatures}, layout{inLayout} {}
        };

    public:
        using UniqueTypes_t = HashSet< String >;
        using VertexInput   = GraphicsPipelineDesc::VertexInput;

        struct Constants
        {
            struct TypeInfo
            {
                EValueType      type;
                uint            size;
                uint            align;
            };
            const FlatHashMap< StringView, TypeInfo >       typeNames;
            const FlatHashMap< StringView, StringView >     renameMap;

            Constants ();
        };


    // variables
    private:
        const String    _originName;    // ShaderStructName
        const String    _typeName;

        Array<Field>    _fields;
        Bytes           _align;
        Bytes           _structAlign;   // align for array or ...
        Bytes           _size;
        EStructLayout   _layout     = EStructLayout::Compatible_Std140;
        mutable EUsage  _usage      = Default;

        Array< ScriptFeatureSetPtr >    _features;


    // methods
    public:
        ShaderStructType () {}
        explicit ShaderStructType (const String &name)                                                  __Th___;

        void  AddFeatureSet (const String &name)                                                        __Th___;
        void  Set (EStructLayout layout, const String &fields)                                          __Th___;
        void  Set2 (const String &fields)                                                               __Th___;

        ND_ String          FieldsToString ()                                                           C_Th___;

        ND_ StringView      Name ()                                                                     const   { return _originName; }
        ND_ StringView      Typename ()                                                                 const   { return _typeName; }
        ND_ bool            HasDynamicArray ()                                                          const   { return _fields.size() > 0 and _fields.back().IsDynamicArray(); }

        ND_ Bytes           TotalSize (uint arraySize)                                                  const;
        ND_ Bytes           StaticSize ()                                                               const   { return AlignUp( _size, _align ); }
        ND_ Bytes           ArrayStride ()                                                              const   { return HasDynamicArray() ? _fields.back().size : 0_b; }
        ND_ Bytes           Align ()                                                                    const   { return _structAlign; }
        ND_ EStructLayout   Layout ()                                                                   const   { return _layout; }
        ND_ EUsage          Usage ()                                                                    const   { return _usage; }

            void  AddUsage (EUsage usage);
        ND_ bool  Compare (const ShaderStructType &rhs)                                                 const;

        ND_ ArrayView<ScriptFeatureSetPtr>  Features ()                                                 const   { return _features; }
        ND_ ArrayView<Field>                Fields ()                                                   const   { return _fields; }

        ND_ bool  FieldCount (INOUT usize &count)                                                       const;

        ND_ bool  ToGLSL (bool withOffsets, INOUT String &types, INOUT String &fields, INOUT UniqueTypes_t &uniqueTypes, Bytes baseOffset = 0_b) const;
        ND_ bool  ToGLSL (bool withOffsets, INOUT String &types, INOUT String &fields, Bytes baseOffset = 0_b) const;

        ND_ bool  StructToGLSL (INOUT String &types, INOUT UniqueTypes_t &uniqueTypes)                  const;

        ND_ bool  ToMSL (INOUT String &types, INOUT UniqueTypes_t &uniqueTypes)                         const;
        ND_ bool  ToMSL (INOUT String &types)                                                           const;

        ND_ bool  ToCPP (INOUT String &types, INOUT UniqueTypes_t &uniqueTypes)                         const;
        ND_ bool  ToCPP (INOUT String &types)                                                           const;

        ND_ String  VertexInputToGLSL (const String &prefix, INOUT uint &loc)                           C_Th___;
        ND_ String  VertexInputToMSL (const String &prefix, INOUT uint &index)                          C_Th___;

        ND_ String  ToShaderIO_GLSL (EShader shaderType, bool input, INOUT UniqueTypes_t &uniqueTypes)  C_Th___;
        ND_ String  ToShaderIO_MSL (EShader shaderType, bool input, INOUT UniqueTypes_t &uniqueTypes)   C_Th___;

        void  GetVertexInput (INOUT uint &loc, INOUT Array<VertexInput> &arr)                           C_Th___;

        static void  Bind (const ScriptEnginePtr &se)                                                   __Th___;

    private:
        ND_ uint    _StaticSize ()                                                                      const   { return uint(StaticSize()); }
        ND_ uint    _ArrayStride ()                                                                     const   { return uint(ArrayStride()); }

        static void  _ParseFields (const String &fields, OUT Array<Field> &outFields)                   __Th___;
        static void  _CalcOffsets (StringView, EStructLayout layout, INOUT Array<Field> &fields,
                                   OUT Bytes &maxAlign, OUT Bytes &structAlign, OUT Bytes &totalSize)   __Th___;

        static void  _Validate (StringView, StringView, ArrayView<Field> fields, ValidationData &data)  __Th___;
        static void  _ValidateOffsets (const ValidationData &data, Bytes offset)                        __Th___;
        static void  _AddPadding (EStructLayout layout, INOUT Array<Field> &fields)                     __Th___;

        ND_ static EValueType  _VertexToAttrib (EValueType type)                                        __Th___;

        ND_ String  _VertexInputToGLSL (const String &prefix, INOUT uint &loc)                          C_Th___;
        ND_ String  _VertexInputToMSL (const String &prefix, INOUT uint &index)                         C_Th___;

        ND_ String  _ToShaderIO_GLSL (const String &prefix, INOUT uint &loc, bool useLocations)         C_Th___;
        ND_ String  _ToShaderIO_MSL (const String &prefix)                                              C_Th___;

        ND_ static SizeAndAlign  _GetCPPSizeAndAlign2 (const Field &field);
        ND_ static SizeAndAlign  _GetCPPSizeAndAlign (const Field &field, EStructLayout layout);

        ND_ static SizeAndAlign  _GetMSLSizeAndAlign2 (const Field &field);
        ND_ static SizeAndAlign  _GetMSLSizeAndAlign (const Field &field, EStructLayout layout);

        ND_ static SizeAndAlign  _GetGLSLSizeAndAlign2 (const Field &field);
        ND_ static SizeAndAlign  _GetGLSLSizeAndAlign (const Field &field, EStructLayout layout);

        ND_ static bool  _CreatePackedTypeGLSL1 (INOUT String &outTypes, StringView packedTypeName, StringView memberTypeName, StringView dstType, const Field &);
        ND_ static bool  _CreatePackedTypeGLSL2 (INOUT String &outTypes, StringView packedTypeName, StringView memberTypeName, StringView dstType, const Field &);
        ND_ static bool  _CreatePackedTypeMSL (INOUT String &outTypes, StringView packedTypeName, StringView memberTypeName, StringView dstType, const Field &);
    };

    AE_BIT_OPERATORS( ShaderStructType::EUsage );


    inline void  ShaderStructType::AddUsage (EUsage usage)  { _usage |= usage; }

} // AE::PipelineCompiler
