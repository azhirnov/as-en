// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/DescriptorSetLayout.h"

namespace AE::PipelineCompiler
{
    struct ScriptVertexBufferInput;
    using VertexBufferInputPtr = ScriptRC< ScriptVertexBufferInput >;


    //
    // Vertex Divisor
    //
    struct VertexDivisor
    {
    // variables
        uint        value   = 0;

    // methods
        VertexDivisor ()                                __NE___ {}
        explicit VertexDivisor (uint v)                 __NE___ : value{v} {}

        static void  Bind (const ScriptEnginePtr &se)   __Th___;
    };



    //
    // Vertex Buffer Input
    //

    struct ScriptVertexBufferInput final : EnableScriptRC
    {
    // types
    private:
        struct Buffer
        {
            ShaderStructTypePtr     ptr;
            uint                    glslLoc     = UMax;
            uint                    mslIndex    = UMax;
            ubyte                   index       = UMax;
            Bytes16u                stride;
            EVertexInputRate        rate        = EVertexInputRate::Vertex;
            uint                    divisor     = 0;
        };

        using VB_t          = FlatHashMap< String, Buffer >;
        using VertexAttrib  = SerializableGraphicsPipeline::VertexAttrib;
        using VertexInput   = GraphicsPipelineDesc::VertexInput;
        using VertexBuffer  = GraphicsPipelineDesc::VertexBuffer;


    // variables
    private:
        Array< ScriptFeatureSetPtr >    _features;

        VB_t            _buffers;
        const String    _name;

        String          _glsl;
        uint            _glslLoc    = 0;

        String          _msl;
        uint            _mslIndex   = 0;


    // methods
    public:
        ScriptVertexBufferInput () {}
        ScriptVertexBufferInput (const String &name);

        void  AddFeatureSet (const String &name)                                                            __Th___;

        void  Add1 (const String &bufferName, const String &typeName, uint stride)                          __Th___;
        void  Add2 (const String &bufferName, const ShaderStructTypePtr &ptr, uint stride)                  __Th___;
        void  Add3 (const String &bufferName, const String &typeName)                                       __Th___;
        void  Add4 (const String &bufferName, const ShaderStructTypePtr &ptr)                               __Th___;
        void  Add5 (const String &bufferName, const String &typeName, const Align &align)                   __Th___;
        void  Add6 (const String &bufferName, const ShaderStructTypePtr &ptr, const Align &align)           __Th___;

        // with vertex divisor
        void  AddD1  (const String &bufferName, const String &typeName, uint stride, const VertexDivisor &div)                  __Th___;
        void  AddD2 (const String &bufferName, const ShaderStructTypePtr &ptr, uint stride, const VertexDivisor &div)           __Th___;
        void  AddD3 (const String &bufferName, const String &typeName, const VertexDivisor &div)                                __Th___;
        void  AddD4 (const String &bufferName, const ShaderStructTypePtr &ptr, const VertexDivisor &div)                        __Th___;
        void  AddD5 (const String &bufferName, const String &typeName, const Align &align, const VertexDivisor &div)            __Th___;
        void  AddD6 (const String &bufferName, const ShaderStructTypePtr &ptr, const Align &align, const VertexDivisor &div)    __Th___;

        ND_ bool  IsSameAttribs (const ScriptVertexBufferInput &rhs)                                        const;

            void  Get (OUT ArrayView<VertexAttrib> &attribs)                                                C_Th___;
        ND_ bool  Get (OUT ArrayView<VertexInput> &vertexInput, OUT ArrayView<VertexBuffer> &vertexBuffers) const;

        ND_ uint            GlslLocations ()                                                                const   { return _glslLoc; }
        ND_ String          ToGLSL ()                                                                       const;
        ND_ String          ToMSL ()                                                                        const;

        ND_ StringView      Name ()                                                                         const   { return _name; }
        ND_ VB_t const&     Buffers ()                                                                      const   { return _buffers; }

        static void  Bind (const ScriptEnginePtr &se)                                                       __Th___;

    private:
        void  _Get (OUT Array<VertexAttrib> &attribs)                                                       const;

        void  _Add (const String &bufferName, const ShaderStructTypePtr &ptr, uint stride, Optional<VertexDivisor> divisor) __Th___;
    };


} // AE::PipelineCompiler
