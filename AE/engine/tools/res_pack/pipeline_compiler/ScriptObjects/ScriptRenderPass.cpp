// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/ScriptRenderPass.h"
#include "ScriptObjects/Common.inl.h"

AE_DECL_SCRIPT_TYPE(    AE::PipelineCompiler::EAttachment,              "EAttachment"           );

AE_DECL_SCRIPT_OBJ_RC(  AE::PipelineCompiler::RPAttachment,             "Attachment"            );
AE_DECL_SCRIPT_OBJ_RC(  AE::PipelineCompiler::RPAttachmentSpec,         "AttachmentSpec"        );
AE_DECL_SCRIPT_OBJ_RC(  AE::PipelineCompiler::RenderPassSpec,           "RenderPass"            );
AE_DECL_SCRIPT_OBJ_RC(  AE::PipelineCompiler::CompatibleRenderPassDesc, "CompatibleRenderPass"  );
AE_DECL_SCRIPT_OBJ(     AE::PipelineCompiler::RPAttachment::ShaderIO,   "ShaderIO"              );


namespace AE::Base
{
    ND_ inline String  ToString (PipelineCompiler::EAttachment value)
    {
        using PipelineCompiler::EAttachment;
        switch_enum( value )
        {
            case EAttachment::Invalidate :      return "Invalidate";
            case EAttachment::Color :           return "Color";
            case EAttachment::ColorResolve :    return "ColorResolve";
            case EAttachment::ReadWrite :       return "ReadWrite";
            case EAttachment::Input :           return "Input";
            case EAttachment::Depth :           return "Depth";
            case EAttachment::Preserve :        return "Preserve";
            case EAttachment::ShadingRate :     return "ShadingRate";
            case EAttachment::Unknown :         return "Unknown";
            case EAttachment::_Count :          break;
        }
        switch_end
        RETURN_ERR( "unknown attachment usage" );
    }

} // AE::Base


namespace AE::PipelineCompiler
{
namespace
{
/*
=================================================
    CompatibleRenderPassDesc_Ctor
=================================================
*/
    static CompatibleRenderPassDesc*  CompatibleRenderPassDesc_Ctor (const String &name) __Th___ {
        return CompatibleRenderPassDescPtr{new CompatibleRenderPassDesc{ name }}.Detach();
    }

/*
=================================================
    ShaderIO_Ctor*
=================================================
*/
    static void  ShaderIO_Ctor (void* mem, const String &name, Optional<EShaderIO> type, uint index)
    {
        ObjectStorage::Instance()->AddName<ShaderIOName>( name );

        if ( type.has_value() )
        {
            CHECK_THROW_MSG( *type != Default );
            CHECK_THROW_MSG( *type < EShaderIO::_Count );
        }
        PlacementNew<RPAttachment::ShaderIO>( OUT mem, ShaderIOName{name}, type.value_or(EShaderIO::Unknown), index );
    }

    static void  ShaderIO_Ctor1 (void* mem, const String &name)
    {
        ShaderIO_Ctor( OUT mem, name, NullOptional, UMax );
    }

    static void  ShaderIO_Ctor2 (void* mem, const String &name, EShaderIO type)
    {
        ShaderIO_Ctor( OUT mem, name, type, UMax );
    }

    static void  ShaderIO_Ctor3 (void* mem, uint index, const String &name, EShaderIO type)
    {
        ShaderIO_Ctor( OUT mem, name, type, index );
    }

    static void  ShaderIO_Ctor4 (void* mem, uint index, const String &name)
    {
        ShaderIO_Ctor( OUT mem, name, NullOptional, index );
    }


    static constexpr char   c_Subpass_ExternalIn[]  = "ExternalIn";
    static constexpr char   c_Subpass_ExternalOut[] = "ExternalOut";

    StaticAssert( SubpassName{c_Subpass_ExternalIn} == Subpass_ExternalIn );
    StaticAssert( SubpassName{c_Subpass_ExternalOut} == Subpass_ExternalOut );
    StaticAssert( alignof(RPAttachment::ShaderIO) == 4 );

} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
    AddUsage
=================================================
*/
    void  RPAttachment::AddUsage (const String &subpassName, EAttachment usage) __Th___
    {
        CHECK_THROW_MSG( usage != EAttachment::ShadingRate );
        return _AddUsage( subpassName, usage, NullOptional, NullOptional, packed_uint2{} );
    }

    void  RPAttachment::AddUsage2 (const String &subpassName, EAttachment usage, const ShaderIO &inOrOut) __Th___
    {
        CHECK_THROW_MSG( usage != EAttachment::ShadingRate );
        return _AddUsage( subpassName, usage, inOrOut, NullOptional, packed_uint2{} );
    }

    void  RPAttachment::AddUsage3 (const String &subpassName, EAttachment usage, const ShaderIO &in, const ShaderIO &out) __Th___
    {
        CHECK_THROW_MSG( usage != EAttachment::ShadingRate );
        return _AddUsage( subpassName, usage, in, out, packed_uint2{} );
    }

    void  RPAttachment::AddUsage4 (const String &subpassName, EAttachment usage, const packed_uint2 &texelSize) __Th___
    {
        CHECK_THROW_MSG( usage == EAttachment::ShadingRate );
        return _AddUsage( subpassName, usage, NullOptional, NullOptional, texelSize );
    }

    void  RPAttachment::_AddUsage (const String &subpassName, EAttachment usage, Optional<ShaderIO> inVar, Optional<ShaderIO> outVar,
                                   const packed_uint2 &texelSize) __Th___
    {
        CHECK_THROW_MSG( usageMap.size() < GraphicsConfig::MaxSubpasses );
        CHECK_THROW_MSG( usage < EAttachment::_Count );
        CHECK_THROW_MSG( usage != Default );

        auto&   storage = *ObjectStorage::Instance();
        storage.AddName<SubpassName>( subpassName );

        const SubpassName   sp_name {subpassName};
        usize               subpass_idx = UMax;

        ASSERT( _compat != null );
        if ( _compat != null )
        {
            auto    iter = _compat->_subpassMap.find( sp_name );
            CHECK_THROW_MSG( iter != _compat->_subpassMap.end(),
                "subpass '"s << subpassName << "' is not defined in render pass" );

            subpass_idx = iter->second;
            for (usize i = subpass_idx + 1; i < _compat->_subpasses.size(); ++i)
            {
                CHECK_THROW_MSG( not usageMap.contains( _compat->_subpasses[i].name ),
                    "subpass '"s << subpassName << "' must be placed before subpass '" << storage.GetName( _compat->_subpasses[i].name ) << "'" );
            }
        }

        auto [iter, inserted] = usageMap.emplace( sp_name, Usage{usage} );
        CHECK_THROW_MSG( inserted,
            "subpass '"s << subpassName << "' is already has usage" );

        switch_enum( usage )
        {
            case EAttachment::Color :
            case EAttachment::ColorResolve :
                if ( outVar.has_value() )
                {
                    CHECK_THROW_MSG( not inVar.has_value(), "color attachment requires single shader IO" );
                    iter->second.output = *outVar;
                }else
                if ( inVar.has_value() )
                {
                    CHECK_THROW_MSG( not outVar.has_value(), "color attachment requires single shader IO" );
                    iter->second.output = *inVar;
                }else
                {
                    // set default
                    ShaderIO_Ctor1( OUT &iter->second.output, storage.GetName( _name ));
                }
                break;

            case EAttachment::ReadWrite :
                if ( inVar.has_value() and outVar.has_value() )
                {
                    iter->second.input  = *inVar;
                    iter->second.output = *outVar;
                }
                else
                {
                    CHECK_THROW_MSG( inVar.has_value() or outVar.has_value(),
                        "for 'ReadWrite' usage both input and output shader IO must be defined" );

                    // set default
                    ShaderIO_Ctor1( OUT &iter->second.input, storage.GetName( _name ));
                    ShaderIO_Ctor1( OUT &iter->second.output, storage.GetName( _name ));
                }
                break;

            case EAttachment::Input :
                if ( outVar.has_value() )
                {
                    CHECK_THROW_MSG( not inVar.has_value(), "input attachment requires single shader IO" );
                    iter->second.input = *outVar;
                }else
                if ( inVar.has_value() )
                {
                    CHECK_THROW_MSG( not outVar.has_value(), "input attachment requires single shader IO" );
                    iter->second.input = *inVar;
                }else
                {
                    // set default
                    ShaderIO_Ctor1( OUT &iter->second.input, storage.GetName( _name ));
                }
                break;

            case EAttachment::ShadingRate :
                CHECK_THROW_MSG( not inVar.has_value() and not outVar.has_value() );
                CHECK_THROW_MSG( All(IsNotZero( texelSize )));
                CHECK_THROW_MSG( All(IsPowerOfTwo( texelSize )));
                iter->second.texelSize = texelSize;
                break;

            case EAttachment::Invalidate :
            case EAttachment::Depth :
            case EAttachment::Preserve :
            case EAttachment::Unknown :
            case EAttachment::_Count :
            default :
                CHECK_THROW_MSG( not inVar.has_value() and not outVar.has_value(),
                    "attachment usage '"s << Base::ToString(usage) << "' is not compatible with ShaderIO" );
                break;
        }
        switch_end

        CHECK_THROW_MSG( format != Default, "pixel format must be defined" );
        if ( format < EPixelFormat::_Count ) {
            CHECK_THROW_MSG( not EPixelFormat_IsCompressed( format ), "unsupported pixel format '"s << Base::ToString(format) << "'" );
        }else{
            CHECK_THROW_MSG( format == EPixelFormat::SwapchainColor, "unsupported pixel format" );
        }

        // validate input/output
        const auto  CheckIOIndex = [] (auto& assignedAttachments, ShaderIO &io, StringView msg)
        {{
            if ( io.index != UMax )
            {
                CHECK_THROW_MSG( io.index < GraphicsConfig::MaxColorAttachments,
                    String{msg} << " index " << Base::ToString(io.index) << " is too big" );
                CHECK_THROW_MSG( not assignedAttachments.test( io.index ),
                    String{msg} << " index "s << Base::ToString(io.index) << " is already used" );
                assignedAttachments.set( io.index );
            }
            else
            {
                const int   bit_idx = BitScanForward( ~assignedAttachments.to_ulong() );
                CHECK_THROW_MSG( bit_idx >= 0 and bit_idx < int(GraphicsConfig::MaxColorAttachments), "(internal error)" );
                io.index = uint(bit_idx);
                CHECK_THROW_MSG( not assignedAttachments.test( io.index ), "(internal error)" );
                assignedAttachments.set( io.index );
            }
        }};

        const auto  CheckIOType = [this] (INOUT EShaderIO &type)
        {{
            const EShaderIO     io_type = format == EPixelFormat::SwapchainColor ? EShaderIO::AnyColor : EPixelFormat_ToShaderIO( format );

            if ( type == Default )
                type = io_type;
            else
            if ( io_type != type )
            {
                CHECK_THROW_MSG( EShaderIO_IsConvertible( io_type, type ),
                    "shader IO type '"s << Base::ToString(type) << "' is not compatible with pixel format '"
                    << Base::ToString(format) << "', expected '" << Base::ToString(io_type) << "'" );
            }
        }};

        if ( iter->second.input.IsDefined() and _compat != null )
        {
            auto&   sp = _compat->_subpasses[subpass_idx];
            CheckIOIndex( sp.assignedInputAttachment, iter->second.input, "input" );
            CheckIOType( iter->second.input.type );
        }

        if ( iter->second.output.IsDefined() and _compat != null )
        {
            auto&   sp = _compat->_subpasses[subpass_idx];
            CheckIOIndex( sp.assignedColorAttachment, iter->second.output, "output" );
            CheckIOType( iter->second.output.type );
        }
    }

/*
=================================================
    Validate
=================================================
*/
    bool  RPAttachment::Validate () const
    {
        CHECK_ERR( _compat != null );

        auto&           storage         = *ObjectStorage::Instance();
        const bool      is_sw_fmt       = (format == EPixelFormat::SwapchainColor);
        const bool      is_compressed   = (format < EPixelFormat::_Count ? EPixelFormat_IsCompressed( format ) : false);
        const String    msg             = "in compatible render pass '"s << storage.GetName( _compat->_name ) << "' attachment '" << storage.GetName( _name ) << "' ";
        bool            is_valid        = true;

        if ( usageMap.empty() )
        {
            AE_LOGE( String{msg} << "at least one usage must be defined" );
            is_valid = false;
        }

        if ( (not is_sw_fmt and format >= EPixelFormat::_Count) or is_compressed )
        {
            AE_LOGE( String{msg} << "has unsupported pixel format '" << Base::ToString(format) << "'" );
            is_valid = false;
        }

        return is_valid;
    }

/*
=================================================
    Print
=================================================
*/
    String  RPAttachment::ToString (StringView padding) const
    {
        auto&   storage = *ObjectStorage::Instance();
        String  str;

        str         << padding << "format:  " << Base::ToString( format )
            << '\n' << padding << "samples: " << Base::ToString( samples.Get() );

        ASSERT( _compat != null );
        if ( _compat != null and not usageMap.empty() )
        {
            str << '\n' << padding << "usage:   ";
            for (auto& sp : _compat->_subpasses)
            {
                auto    iter = usageMap.find( sp.name );
                if ( iter != usageMap.end() )
                    str << '\n' << padding << "  " << storage.GetName( sp.name ) << " : " << Base::ToString( iter->second.type );
            }
            str << "\n";
        }
        return str;
    }

    void  RPAttachment::Print () const
    {
        AE_LOGI( ToString( "" ));
    }

/*
=================================================
    IsColor
=================================================
*/
    bool  RPAttachment::IsColor () const
    {
        return format == EPixelFormat::SwapchainColor or EPixelFormat_IsColor( format );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    AddLayout
=================================================
*/
    void  RPAttachmentSpec::AddLayout (const String &subpassName, EResourceState state) __Th___
    {
        CHECK_THROW_MSG( layouts.size() < GraphicsConfig::MaxSubpasses );

        auto&   storage = *ObjectStorage::Instance();
        storage.AddName<SubpassName>( subpassName );

        CHECK_THROW_MSG( EResourceState_Validate( state ),
            "in subpass '"s << subpassName << "' state (" << Base::ToString(state) << ") is not valid" );

        const SubpassName   sp_name {subpassName};

        if ( sp_name == Subpass_ExternalIn )
        {
            CHECK_THROW_MSG( layouts.empty(),
                "subpass '"s << subpassName << "' must be first" );
        }

        ASSERT( _compat != null );
        if ( _compat != null and sp_name != Subpass_ExternalIn and sp_name != Subpass_ExternalOut )
        {
            auto    iter = _compat->_subpassMap.find( sp_name );
            CHECK_THROW_MSG( iter != _compat->_subpassMap.end(),
                "subpass '"s << subpassName << "' is not defined in render pass" );

            for (usize i = iter->second + 1; i < _compat->_subpasses.size(); ++i)
            {
                CHECK_THROW_MSG( not layouts.contains( _compat->_subpasses[i].name ),
                    "subpass '"s << subpassName << "' must be placed before subpass '" << storage.GetName( _compat->_subpasses[i].name ) << "'" );
            }

            CHECK_THROW_MSG( not layouts.contains( Subpass_ExternalOut ),
                "subpass '"s << subpassName << "' must be placed before final subpass" );

            // validate resource state
            {
                auto    att_it = _compat->_attachments.find( _name );
                CHECK_THROW_MSG( att_it != _compat->_attachments.end(),
                    "attachment '"s << storage.GetName( _name ) << "' is not exist" );

                auto    usage_it = att_it->second->usageMap.find( sp_name );
                CHECK_THROW_MSG( usage_it != att_it->second->usageMap.end(),
                    "attachment '"s << storage.GetName( _name ) << "' does not define usage for subpass '" << subpassName << "'" );

                const auto              access      = ToEResState( state );
                const bool              invalidate  = AllBits( state, _EResState::Invalidate );
                const EResourceState    shaders     = state & EResourceState::AllShaders;
                const bool              is_color    = att_it->second->IsColor();

                const String    msg = "in subpass '"s << subpassName << "', attachment '" << storage.GetName( _name ) << "' with usage '" << Base::ToString( usage_it->second.type ) << "' ";

                switch_enum( usage_it->second.type )
                {
                    case EAttachment::Invalidate :
                    {
                        CHECK_THROW_MSG( access == Default and invalidate,
                            String{msg} << "allow only 'Invalidate' state" );
                        CHECK_THROW_MSG( shaders == Default,
                            String{msg} << "must not contain shader stages" );
                        break;
                    }
                    case EAttachment::Color :
                    {
                        CHECK_THROW_MSG( access == _EResState::ColorAttachment,
                            String{msg} << "allow only 'ColorAttachment' state" );
                        CHECK_THROW_MSG( shaders == Default,
                            String{msg} << "must not contain shader stages" );
                        break;
                    }
                    case EAttachment::ReadWrite :
                    {
                        if ( is_color ) {
                            CHECK_THROW_MSG( access == _EResState::InputColorAttachment_RW,
                                String{msg} << "allow only 'InputColorAttachment_RW' state" );
                        }else{
                            CHECK_THROW_MSG( access == _EResState::InputDepthStencilAttachment_RW,
                                String{msg} << "allow only 'InputDepthStencilAttachment_RW' state" );
                        }
                        CHECK_THROW_MSG( AnyBits( shaders, EResourceState::PostRasterizationShaders ),
                            String{msg} << "must contains 'Tile' or 'Fragment' shader stage" );
                        break;
                    }
                    case EAttachment::ColorResolve:
                    {
                        CHECK_THROW_MSG( access == _EResState::ColorAttachment,
                            String{msg} << "allow only 'ColorAttachment' state" );
                        CHECK_THROW_MSG( shaders == Default,
                            String{msg} << "must not contain shader stages" );
                        break;
                    }
                    case EAttachment::Input :
                    {
                        if ( is_color ) {
                            CHECK_THROW_MSG( access == _EResState::InputColorAttachment,
                                String{msg} << "allow only 'InputColorAttachment' state" );
                        }else{
                            CHECK_THROW_MSG( access == _EResState::InputDepthStencilAttachment,
                                String{msg} << "allow only 'InputDepthStencilAttachment' state" );
                        }
                        CHECK_THROW_MSG( AnyBits( shaders, EResourceState::PostRasterizationShaders ),
                            String{msg} << "must contains Tile or Fragment shader stage" );
                        break;
                    }
                    case EAttachment::Depth :
                    {
                        const EResourceState    ds_stages = state & (EResourceState::DSTestBeforeFS | EResourceState::DSTestAfterFS);

                        CHECK_THROW_MSG( access == _EResState::DepthStencilTest             or
                                         access == _EResState::DepthStencilAttachment_RW    or
                                         access == _EResState::DepthTest_StencilRW          or
                                         access == _EResState::DepthRW_StencilTest,
                            String{msg} << "allow all combinations of DepthStencilAttachment states" );
                        CHECK_THROW_MSG( ds_stages != Default,
                            String{msg} << "requires DSTestBeforeFS or DSTestAfterFS stages" );
                        CHECK_THROW_MSG( shaders == Default,
                            String{msg} << "must not contain shader stages" );
                        break;
                    }
                    case EAttachment::Preserve :
                    {
                        CHECK_THROW_MSG( access == _EResState::Preserve,
                            String{msg} << "allow only 'Preserve' state" );
                        CHECK_THROW_MSG( shaders == Default,
                            String{msg} << "must not contain shader stages" );
                        break;
                    }
                    case EAttachment::ShadingRate :
                    {
                        CHECK_THROW_MSG( access == _EResState::ShadingRateImage,
                            String{msg} << "allow only 'ShadingRateImage' state" );
                        CHECK_THROW_MSG( shaders == Default,
                            String{msg} << "must not contain shader stages" );
                        break;
                    }

                    case EAttachment::Unknown :
                    case EAttachment::_Count :
                    default :
                        CHECK_THROW_MSG( false, "unknown attachment usage type" );
                        break;
                }
                switch_end
            }
        }

        CHECK_THROW_MSG( layouts.emplace( sp_name, Layout{ state }).second,
            "subpass '"s << subpassName << "' is already has layout" );
    }

    void  RPAttachmentSpec::AddLayout2 (const String &subpassName, uint state) __Th___
    {
        return AddLayout( subpassName, EResourceState(state) );
    }

/*
=================================================
    GenOptimalLayouts
=================================================
*/
    void  RPAttachmentSpec::GenOptimalLayouts () __Th___
    {
        return GenOptimalLayouts2( EResourceState::_InvalidState, EResourceState::_InvalidState );
    }

    void  RPAttachmentSpec::GenOptimalLayouts2 (EResourceState initialState, EResourceState finalState) __Th___
    {
        const EResourceState    PreserveAttachment  = EResourceState::Preserve;
        const EResourceState    DS_State            = EResourceState::DepthStencilAttachment_RW     | EResourceState::DSTestAfterFS | EResourceState::DSTestBeforeFS;
        const EResourceState    ColorDiscard        = EResourceState::ColorAttachment_Blend         | EResourceState::Invalidate;
        const EResourceState    DepthDiscard        = EResourceState::DepthStencilAttachment_Read   | EResourceState::DSTestAfterFS | EResourceState::DSTestBeforeFS | EResourceState::Invalidate;
        const EResourceState    InColAtt_State      = EResourceState::InputColorAttachment          | EResourceState::FragmentShader;
        const EResourceState    InDSAtt_State       = EResourceState::InputDepthStencilAttachment   | EResourceState::FragmentShader;   // TODO: TileShader
        const EResourceState    RWInColAtt_State    = EResourceState::InputColorAttachment_RW       | EResourceState::FragmentShader;

        auto&   storage = *ObjectStorage::Instance();

        auto    att_it = _compat->_attachments.find( _name );
        CHECK_THROW_MSG( att_it != _compat->_attachments.end(),
            "Attachment '"s << storage.GetName( _name ) << "' is not exists in CompatibleRenderPass '" << storage.GetName( _compat->_name ) << "'" );

        const auto&     rt          = att_it->second;
        const bool      is_color    = rt->IsColor();
        SubpassName     prev_sp     = Subpass_ExternalIn;

        Array<EResourceState>   rt_states;

        if ( initialState != EResourceState::_InvalidState )
        {
            auto [layout_it, layout_inserted] = this->layouts.emplace( Subpass_ExternalIn, RPAttachmentSpec::Layout{} );
            CHECK_THROW_MSG( layout_inserted,
                "Initial layout for attachment '"s << storage.GetName( _name ) << "' is already exists" );
            layout_it->second.state = initialState;
            rt_states.push_back( initialState );
        }

        for (auto& sp : _compat->_subpasses)
        {
            EResourceState  new_state;
            auto            usage       = rt->usageMap.find( sp.name );
            const bool      has_content = rt_states.empty() ? false : not AllBits( rt_states.back(), EResourceState::Invalidate );

            // if usage for subpass is not defined then content of the attachment may be invalidated
            if ( rt_states.empty() )
                new_state = is_color ? ColorDiscard : DepthDiscard;
            else
                new_state = rt_states.back();   // keep previous layout

            if ( usage != rt->usageMap.end() )
            {
                switch_enum( usage->second.type )
                {
                    case EAttachment::Invalidate :
                        new_state |= EResourceState::Invalidate;
                        break;

                    case EAttachment::Color :
                        CHECK_THROW_MSG( is_color,
                            "ColorAttachment '"s << storage.GetName( _name ) << "' in subpass '" << storage.GetName( sp.name ) <<
                            "' must have color format, but current format is " << Base::ToString( rt->format ));
                        new_state = EResourceState::ColorAttachment_Blend;
                        break;

                    case EAttachment::ReadWrite :
                        CHECK_THROW_MSG( is_color,
                            "RWInputAttachment '"s << storage.GetName( _name ) << "' in subpass '" << storage.GetName( sp.name ) <<
                            "' must have color format, but current format is " << Base::ToString( rt->format ));
                        new_state = RWInColAtt_State;
                        break;

                    case EAttachment::ColorResolve:
                        CHECK_THROW_MSG( is_color,
                            "ColorResolveAttachment '"s << storage.GetName( _name ) << "' in subpass '" << storage.GetName( sp.name ) <<
                            "' must have color format, but current format is " << Base::ToString( rt->format ));
                        CHECK_THROW_MSG( has_content,
                            "ColorResolveAttachment '"s << storage.GetName( _name ) << "' in subpass '" << storage.GetName( sp.name ) <<
                            "' have previous state (" << Base::ToString( new_state ) << ") which invalidate content, " <<
                            "add 'Preserve' usage for previous subpass '" << storage.GetName( prev_sp ) << "'" );
                        new_state = EResourceState::ColorAttachment_Blend;
                        break;

                    case EAttachment::Input :
                        CHECK_THROW_MSG( has_content,
                            "InputAttachment '"s << storage.GetName( _name ) << "' in subpass '" << storage.GetName( sp.name ) <<
                            "' have previous state (" << Base::ToString( new_state ) << ") which invalidate content, " <<
                            "add 'Preserve' usage for previous subpass '" << storage.GetName( prev_sp ) << "'" );
                        new_state = is_color ? InColAtt_State : InDSAtt_State;
                        break;

                    case EAttachment::Depth :
                        CHECK_THROW_MSG( not is_color,
                            "DepthStencilAttachment '"s << storage.GetName( _name ) << "' in subpass '" << storage.GetName( sp.name ) <<
                            "' must have depth/stencil format, but current format is " << Base::ToString( rt->format ));
                        new_state = DS_State;
                        break;

                    case EAttachment::Preserve :
                        CHECK_THROW_MSG( has_content,
                            "PreserveAttachment '"s << storage.GetName( _name ) << "' in subpass '" << storage.GetName( sp.name ) <<
                            "' have previous state (" << Base::ToString( new_state ) << ") which invalidate content, " <<
                            "add 'Preserve' usage for previous subpass '" << storage.GetName( prev_sp ) << "'" );
                        new_state = PreserveAttachment;
                        break;

                    case EAttachment::ShadingRate :
                        CHECK_THROW_MSG( rt->format == EPixelFormat::R8U );
                        CHECK_THROW_MSG( has_content,
                            "ShadingRateAttachment '"s << storage.GetName( _name ) << "' in subpass '" << storage.GetName( sp.name ) <<
                            "' have previous state (" << Base::ToString( new_state ) << ") which invalidate content, " <<
                            "add 'Preserve' usage for previous subpass '" << storage.GetName( prev_sp ) << "'" );
                        new_state = EResourceState::ShadingRateImage;
                        break;

                    case EAttachment::Unknown :
                    case EAttachment::_Count :
                    default :
                        CHECK_THROW_MSG( false, "unknown attachment usage type" );
                        break;
                }
                switch_end

                auto [layout_it, layout_inserted] = this->layouts.emplace( sp.name, RPAttachmentSpec::Layout{} );
                CHECK_THROW_MSG( layout_inserted,
                    "Layout for subpass '"s << storage.GetName( sp.name ) << "' and attachment '" << storage.GetName( _name ) << "' is already exists" );

                layout_it->second.state = new_state;
            }
            else
            {
                // TODO: check for content lost

                new_state |= EResourceState::Invalidate;
            }

            rt_states.push_back( new_state );
            prev_sp = sp.name;
        }

        if ( finalState != EResourceState::_InvalidState )
        {
            if ( EResourceState_IsReadOnly( finalState ) and not rt_states.empty() )
            {
                CHECK_THROW_MSG( not AllBits( rt_states.back(), EResourceState::Invalidate ),
                    "Attachment '"s << storage.GetName( _name ) << "' final state (" << Base::ToString( rt_states.back() ) <<
                    ") has read-only access, but current content of attachment is invalidated" );
            }

            auto [layout_it, layout_inserted] = this->layouts.emplace( Subpass_ExternalOut, RPAttachmentSpec::Layout{} );
            CHECK_THROW_MSG( layout_inserted,
                "Final layout for attachment '"s << storage.GetName( _name ) << "' is already exists" );
            layout_it->second.state = finalState;
            rt_states.push_back( finalState );
        }

        if ( _compat->_subpasses.size() > 1 )
        {
            if ( AnyBits( rt_states[0], EResourceState::Invalidate ) and AnyBits( rt_states[1], EResourceState::Invalidate ))
                this->loadOp = EAttachmentLoadOp::Invalidate;
            else
            if ( AnyBits( rt_states[0], EResourceState::Invalidate ))
                this->loadOp = EAttachmentLoadOp::Clear;
            else
                this->loadOp = EAttachmentLoadOp::Load;

            this->storeOp = AnyBits( rt_states.back(),  EResourceState::Invalidate ) ? EAttachmentStoreOp::Invalidate : EAttachmentStoreOp::Store;
        }
        else
        {
            this->loadOp    = AnyBits( rt_states.front(), EResourceState::Invalidate ) ? EAttachmentLoadOp::Clear       : EAttachmentLoadOp::Load;
            this->storeOp   = AnyBits( rt_states.back(),  EResourceState::Invalidate ) ? EAttachmentStoreOp::Invalidate : EAttachmentStoreOp::Store;
        }
    }

    void  RPAttachmentSpec::GenOptimalLayouts3 (uint initialState, uint finalState) __Th___
    {
        return GenOptimalLayouts2( EResourceState(initialState), EResourceState(finalState) );
    }

/*
=================================================
    Print
=================================================
*/
    String  RPAttachmentSpec::ToString (StringView padding) const
    {
        auto&   storage = *ObjectStorage::Instance();
        String  str;

        str         << padding << "loadOp:  " << Base::ToString( loadOp )
            << '\n' << padding << "storeOp: " << Base::ToString( storeOp );

        ASSERT( _compat != null );
        if ( _compat != null and not layouts.empty() )
        {
            str << '\n' << padding << "layouts: ";
            for (auto& sp : _compat->_subpasses)
            {
                auto    iter = layouts.find( sp.name );
                if ( iter != layouts.end() )
                    str << '\n' << padding << "  " << storage.GetName( sp.name ) << " : " << Base::ToString( iter->second.state );
            }
            str << "\n";
        }
        return str;
    }

    void  RPAttachmentSpec::Print () const
    {
        AE_LOGI( ToString( "" ));
    }

/*
=================================================
    Validate
=================================================
*/
    bool  RPAttachmentSpec::Validate (RenderPassName::Ref rpName)
    {
        return  _ValidatePass1( rpName ) and
                _ValidatePass2( rpName );
    }

/*
=================================================
    _ValidatePass1
=================================================
*/
    bool  RPAttachmentSpec::_ValidatePass1 (RenderPassName::Ref rpName) const
    {
        auto&               storage         = *ObjectStorage::Instance();
        bool                is_valid        = true;
        const bool          store_op_store  = storeOp == EAttachmentStoreOp::Store or storeOp == EAttachmentStoreOp::StoreCustomSamplePositions;
        EAttachmentLoadOp   subpass_load_op = loadOp;
        const String        msg             = "in render pass '"s << storage.GetName( rpName ) << "' attachment '" << storage.GetName( _name ) << "' ";

        const auto  AddUsage = [&] (uint subpassIdx, SubpassName::Ref spName, const EAttachment usage, const EResourceState state)
        {{
            Unused( usage );

            String  msg2 = String{msg} << "in subpass '" << storage.GetName( spName ) << "' ";

            // check subpass load op
            {
                if ( ToEResState(state) == _EResState::Preserve )
                {
                    if ( subpass_load_op != EAttachmentLoadOp::Load )
                    {
                        AE_LOGE( String{msg} << "in subpass '" << storage.GetName( spName ) << "' have 'Preserve' state but previous content is invalid" );
                        is_valid = false;
                    }
                }

                if ( EResourceState_IsReadOnly( state ))
                {
                    if ( not AnyEqual( subpass_load_op, EAttachmentLoadOp::Load, EAttachmentLoadOp::Clear ))
                    {
                        AE_LOGE( String{msg} << "with state '" << Base::ToString(state) << "' in subpass '" << storage.GetName( spName ) <<
                                 "' have read access but previous content is invalid" );
                        is_valid = false;
                    }
                }
            }

            if ( spName == Subpass_ExternalOut and store_op_store and
                 not AnyEqual( subpass_load_op, EAttachmentLoadOp::Load, EAttachmentLoadOp::Clear ))
            {
                AE_LOGE( String{msg} << "have 'storeOp = Store' but previous content is invalid" );
                is_valid = false;
            }

            if ( storeOp == EAttachmentStoreOp::None and EResourceState_HasWriteAccess( state ))
            {
                AE_LOGE( String{msg} << "in subpass '" << storage.GetName( spName ) << "' have write access which is not allowed when 'storeOp = None'" );
                is_valid = false;
            }

            // update load op for next subpass
            if ( AllBits( state, EResourceState::Invalidate ))
            {
                if ( subpassIdx == 0 and subpass_load_op == EAttachmentLoadOp::Clear )
                {
                    subpass_load_op = EAttachmentLoadOp::Load;
                }
                else
                {
                    if ( subpassIdx == 1 and subpass_load_op == EAttachmentLoadOp::Load )
                    {
                        AE_LOGE( String{msg} << "in subpass '" << storage.GetName( spName ) << "' invalidate content in first subpass when 'loadOp = Load', "
                                 "set 'Preserve' usage instead" );
                        is_valid = false;
                    }
                    subpass_load_op = EAttachmentLoadOp::Invalidate;
                }
            }
            else
            if ( ToEResState(state) == _EResState::Preserve )
            {
                subpass_load_op = EAttachmentLoadOp::Load;
            }
            else
            {
                if ( EResourceState_HasWriteAccess( state ))
                    subpass_load_op = EAttachmentLoadOp::Load;
            }
        }};


        auto    att_it = _compat->_attachments.find( _name );
        CHECK_ERR_MSG( att_it != _compat->_attachments.end(),
            "Attachment '"s << storage.GetName( _name ) << "' is not exists in CompatibleRenderPass '" << storage.GetName( _compat->_name ) << "'" );

        // initial layout
        {
            auto    layout_it = layouts.find( Subpass_ExternalIn );
            if ( layout_it != layouts.end() )
            {
                AddUsage( 0, Subpass_ExternalIn, EAttachment::Unknown, layout_it->second.state );

                if ( AllBits( layout_it->second.state, EResourceState::Invalidate ))
                {
                    if ( loadOp == EAttachmentLoadOp::Load )
                    {
                        AE_LOGE( String{msg} << "with 'loadOp = Load' must not have 'Invalidate' in initial state" );
                        is_valid = false;
                    }
                }
                else
                if ( ToEResState( layout_it->second.state ) == _EResState::Preserve )
                {
                    AE_LOGE( String{msg} << "must not have 'Preserve' in initial state" );
                    is_valid = false;
                }
            }
        }

        uint    subpass_idx = 0;
        for (auto& sp : _compat->_subpasses)
        {
            ++subpass_idx;

            auto    usage_it    = att_it->second->usageMap.find( sp.name );
            auto    layout_it   = layouts.find( sp.name );

            if ( usage_it == att_it->second->usageMap.end() )
            {
                CHECK_ERR( layout_it == layouts.end() );
                AddUsage( subpass_idx, sp.name, EAttachment::Invalidate, EResourceState::Invalidate );
                continue;
            }

            if ( layout_it == layouts.end() )
            {
                AE_LOGE( String{msg} << "does not have specialization for subpass '"
                         << storage.GetName( sp.name ) << "' with usage '" << Base::ToString( usage_it->second.type ) << "'" );
                is_valid = false;
                continue;
            }

            AddUsage( subpass_idx, sp.name, usage_it->second.type, layout_it->second.state );
        }

        // final layout
        {
            auto    layout_it = layouts.find( Subpass_ExternalOut );
            if ( layout_it != layouts.end() )
            {
                AddUsage( ++subpass_idx, Subpass_ExternalOut, EAttachment::Unknown, layout_it->second.state );

                if ( AllBits( layout_it->second.state, EResourceState::Invalidate ))
                {
                    if ( storeOp != EAttachmentStoreOp::Invalidate )
                    {
                        AE_LOGE( String{msg} << "with 'storeOp != Invalidate' must not have 'Invalidate' in final state" );
                        is_valid = false;
                    }
                }
                else
                if ( ToEResState( layout_it->second.state ) == _EResState::Preserve )
                {
                    AE_LOGE( String{msg} << "must not have 'Preserve' in final state" );
                    is_valid = false;
                }
                else
                if ( storeOp == EAttachmentStoreOp::Invalidate )
                {
                    AE_LOGE( String{msg} << "with 'storeOp = Invalidate' must have 'Invalidate' in final state" );
                    is_valid = false;
                }
            }
        }

        return is_valid;
    }

/*
=================================================
    _ValidatePass2
=================================================
*/
    bool  RPAttachmentSpec::_ValidatePass2 (RenderPassName::Ref )
    {
        const EResourceState    mask0       = EResourceState(_EResState::AccessMask);
        const EResourceState    mask1       = EResourceState(_EResState::AccessMask | _EResState::DSTestAfterFS | _EResState::DSTestBeforeFS);
        const EResourceState    mask2       = EResourceState(~_EResState::AccessMask);
        EResourceState          prev_state  = Default;

        // final layout
        {
            auto    layout_it = layouts.find( Subpass_ExternalOut );
            if ( layout_it != layouts.end() )
                prev_state = layout_it->second.state;
        }

        for (const auto& sp : Reverse(_compat->_subpasses))
        {
            auto    layout_it = layouts.find( sp.name );
            if ( layout_it != layouts.end() )
            {
                if ( (layout_it->second.state & mask0) == Default )
                    layout_it->second.state = (prev_state & mask1) | (layout_it->second.state & mask2);

                prev_state = layout_it->second.state;
            }
        }

        // initial layout
        {
            auto    layout_it = layouts.find( Subpass_ExternalIn );
            if ( layout_it != layouts.end() )
            {
                if ( (layout_it->second.state & mask0) == Default )
                    layout_it->second.state = (prev_state & mask1) | (layout_it->second.state & mask2);
            }
        }

        return true;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    AddAttachment
=================================================
*/
    RPAttachmentSpecPtr  RenderPassSpec::AddAttachment2 (const String &attachmentName) __Th___
    {
        CHECK_THROW_MSG( _attachments.size() < GraphicsConfig::MaxAttachments );

        auto&   storage = *ObjectStorage::Instance();
        storage.AddName<AttachmentName>( attachmentName );

        const AttachmentName    att_name {attachmentName};

        ASSERT( _compat != null );
        if ( _compat != null )
        {
            CHECK_THROW_MSG( _compat->_attachments.contains( att_name ),
                "Attachment '"s << attachmentName << "' is not exists in render pass '" << storage.GetName( _compat->_name ) << "'" );
        }

        auto [iter, inserted] = _attachments.emplace( att_name, RPAttachmentSpecPtr{new RPAttachmentSpec{ att_name, _compat }} );
        CHECK_THROW_MSG( inserted,
            "attachment '"s << attachmentName << "' is already exists" );

        ASSERT( iter->second->_name == att_name );

        return iter->second;
    }

    RPAttachmentSpec*  RenderPassSpec::AddAttachment (const String &attachmentName) __Th___
    {
        return AddAttachment2( attachmentName ).Detach();
    }

/*
=================================================
    Print
=================================================
*/
    String  RenderPassSpec::ToString (StringView padding) const
    {
        auto&   storage = *ObjectStorage::Instance();
        String  str;

        for (auto& [name, rt] : _attachments)
        {
            str << padding << "AttachmentSpec {"
                << '\n' << padding << "  name:    " << storage.GetName( name ) << '\n'
                << rt->ToString( String{"  "} << padding )
                << padding << "}\n";
        }
        return str;
    }

    void  RenderPassSpec::Print () const
    {
        AE_LOGI( ToString( "" ));
    }

/*
=================================================
    GenOptimalLayouts
=================================================
*/
    void  RenderPassSpec::GenOptimalLayouts () __Th___
    {
        CHECK_THROW_MSG( _compat != null );
        CHECK_THROW_MSG( _attachments.empty() );
        CHECK_THROW_MSG( not _compat->_subpasses.empty() );
        CHECK_THROW_MSG( _compat->_attachments.size() < GraphicsConfig::MaxAttachments );

        auto&   storage = *ObjectStorage::Instance();

        for (auto& [name, rt] : _compat->_attachments)
        {
            auto [iter, inserted] = _attachments.emplace( name, RPAttachmentSpecPtr{new RPAttachmentSpec{ name, _compat }} );
            CHECK_THROW_MSG( inserted,
                "attachment '"s << storage.GetName(name) << "' is already exists, use one of 'GenOptimalLayouts()' or 'AddAttachment()'" );

            iter->second->GenOptimalLayouts();  // throw
        }
    }

/*
=================================================
    Validate
=================================================
*/
    bool  RenderPassSpec::Validate () const
    {
        for (auto& att : _attachments)
        {
            CHECK_ERR( att.second->Validate( _name ));
        }
        return true;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    CompatibleRenderPassDesc::CompatibleRenderPassDesc () :
        CompatibleRenderPassDesc{ "<unknown>" }
    {}

    CompatibleRenderPassDesc::CompatibleRenderPassDesc (const String &name) __Th___ :
        _name{ CompatRenderPassName{name} },
        _features{ ObjectStorage::Instance()->GetDefaultFeatureSets() }
    {
        ObjectStorage::Instance()->AddName<CompatRenderPassName>( name );
        CHECK_THROW_MSG( ObjectStorage::Instance()->compatibleRPs.emplace( _name, CompatibleRenderPassDescPtr{this} ).second,
            "CompatibleRenderPass with name '"s << name << "' is already defined" );
    }

/*
=================================================
    AddFeatureSet
=================================================
*/
    void  CompatibleRenderPassDesc::AddFeatureSet (const String &name) __Th___
    {
        auto&   storage = *ObjectStorage::Instance();
        auto    fs_it   = storage.featureSets.find( FeatureSetName{name} );
        CHECK_THROW_MSG( fs_it != storage.featureSets.end(),
            "FeatureSet with name '"s << name << "' is not found" );

        _features.push_back( fs_it->second );
    }

/*
=================================================
    Print
=================================================
*/
    String  CompatibleRenderPassDesc::ToString (StringView padding) const
    {
        auto&   storage = *ObjectStorage::Instance();
        String  str;

        str         << padding << "CompatibleRenderPass {"
            << '\n' << padding << "  name:        " << storage.GetName( _name );

        if ( not _attachments.empty() )
        {
            str << '\n' << padding << "  attachments: ";
            for (auto& [name, rt] : _attachments)
            {
                str << '\n' << padding << "    " << storage.GetName( name ) << " {\n"
                    << rt->ToString( String{"    "} << padding )
                    << padding << "  }";
            }
        }
        return str;
    }

    void  CompatibleRenderPassDesc::Print () const
    {
        AE_LOGI( ToString( "" ));
    }

/*
=================================================
    Validate
=================================================
*/
    bool  CompatibleRenderPassDesc::Validate () __Th___
    {
        auto&   storage = *ObjectStorage::Instance();

        for (const auto& att : _attachments)
        {
            CHECK_ERR( att.second->Validate() );
        }

        for (const auto& sp : _subpasses)
        {
            for (auto& [att_name, att] : _attachments)
            {
                auto    usage_it = att->usageMap.find( sp.name );
                if ( usage_it == att->usageMap.end() )
                    continue;

                switch_enum( usage_it->second.type )
                {
                    case EAttachment::Color :
                    case EAttachment::ColorResolve :
                    case EAttachment::ReadWrite :
                    case EAttachment::Input :
                    case EAttachment::DepthStencil :
                        if ( att->format != EPixelFormat::SwapchainColor ) {
                            TestFeature_PixelFormat( _features, &FeatureSet::attachmentFormats, att->format, "attachmentFormats",
                                                     ", which used in Attachment '"s << storage.GetName(att_name) << "' in CompatibleRenderPass '" <<
                                                     storage.GetName(_name) << "' with subpass '" << storage.GetName(sp.name) << "'" );  // throw
                        }
                        break;

                    case EAttachment::ShadingRate :
                        TEST_FEATURE( _features, attachmentFragmentShadingRate );
                        TestFeature_PixelFormat( _features, &FeatureSet::attachmentFormats, att->format, "attachmentFormats",
                                                 ", which used in Attachment '"s << storage.GetName(att_name) << "' in CompatibleRenderPass '" <<
                                                 storage.GetName(_name) << "' with subpass '" << storage.GetName(sp.name) << "'" );     // throw
                        CHECK_THROW_MSG( att->format == EPixelFormat::R8U );

                        for (auto& fs : _features) {
                            if ( fs->fs.attachmentFragmentShadingRate == EFeature::RequireTrue )
                            {
                                const uint2     texel_size      = usage_it->second.texelSize;
                                const uint2     min_texel_size  = fs->fs.fragmentShadingRateTexelSize.Min();
                                const uint2     max_texel_size  = fs->fs.fragmentShadingRateTexelSize.Max();
                                const uint      aspect          = fs->fs.fragmentShadingRateTexelSize.MaxAspect();

                                CHECK_THROW_MSG( All( texel_size >= min_texel_size ),
                                    "ShadingRateAttachment '"s << storage.GetName( att_name ) << "' texelSize" << Base::ToString(texel_size) <<
                                    " must be >= minTexelSize" << Base::ToString(min_texel_size) << " in feature sets" );
                                CHECK_THROW_MSG( All( texel_size <= max_texel_size ),
                                    "ShadingRateAttachment '"s << storage.GetName( att_name ) << "' texelSize" << Base::ToString(texel_size) <<
                                    " must be <= maxTexelSize" << Base::ToString(min_texel_size) << " in feature sets" );
                                CHECK_THROW_MSG( ((texel_size.x / texel_size.y) <= aspect) or ((texel_size.y / texel_size.x) <= aspect),
                                    "ShadingRateAttachment '"s << storage.GetName( att_name ) << "' texelSize" << Base::ToString(texel_size) <<
                                    " aspect must be <= maxAspectRatio(" << Base::ToString(aspect) << ") in feature sets" );
                            }
                        }
                        break;

                    case EAttachment::Preserve :
                    case EAttachment::_Count :
                    case EAttachment::Unknown :
                    case EAttachment::Invalidate :
                        break;
                }
                switch_end
            }
        }

        CHECK_ERR( not _specializations.empty() );
        for (const auto& spec : _specializations)
        {
            CHECK_ERR( spec.second->Validate() );
        }
        return true;
    }

/*
=================================================
    AddSpecialization
=================================================
*/
    RenderPassSpecPtr  CompatibleRenderPassDesc::AddSpecialization2 (const String &rpName) __Th___
    {
        CHECK_THROW_MSG( _specializations.size() < 10'000 );

        CHECK_THROW_MSG( _state <= EState::AddSpecializations );
        _state = EState::AddSpecializations;

        auto&   storage = *ObjectStorage::Instance();
        storage.AddName<RenderPassName>( rpName );

        const RenderPassName    rp_name {rpName};

        auto [iter, inserted] = _specializations.emplace( rp_name, RenderPassSpecPtr{new RenderPassSpec{ rp_name, this }} );
        CHECK_THROW_MSG( inserted );

        CHECK_THROW_MSG( storage.renderPassToCompat.emplace( rp_name, _name ).second );

        return iter->second;
    }

    RenderPassSpec*  CompatibleRenderPassDesc::AddSpecialization (const String &rpName) __Th___
    {
        return AddSpecialization2( rpName ).Detach();
    }

/*
=================================================
    AddAttachment
=================================================
*/
    RPAttachmentPtr  CompatibleRenderPassDesc::AddAttachment2 (const String &attachmentName) __Th___
    {
        CHECK_THROW_MSG( _attachments.size() < GraphicsConfig::MaxAttachments );

        CHECK_THROW_MSG( _state <= EState::AddAttachments,
            "AddAttachment() must be used before AddSpecialization()" );

        _state = EState::AddAttachments;

        ObjectStorage::Instance()->AddName<AttachmentName>( attachmentName );

        const AttachmentName    att_name {attachmentName};

        auto [iter, inserted] = _attachments.emplace( att_name, RPAttachmentPtr{new RPAttachment{ att_name, this, uint(_attachments.size()) }} );
        CHECK_THROW_MSG( inserted );

        return iter->second;
    }

    RPAttachment*  CompatibleRenderPassDesc::AddAttachment (const String &attachmentName) __Th___
    {
        return AddAttachment2( attachmentName ).Detach();
    }

/*
=================================================
    AddSubpass
=================================================
*/
    void  CompatibleRenderPassDesc::AddSubpass (const String &subpassName) __Th___
    {
        CHECK_ERRV( _subpassMap.size() < GraphicsConfig::MaxSubpasses );

        const SubpassName   sp_name {subpassName};
        CHECK_THROW_MSG( sp_name != Subpass_ExternalIn, "subpass name must not be 'ExternalIn'" );
        CHECK_THROW_MSG( sp_name != Subpass_ExternalOut, "subpass name must not be 'ExternalOut'" );

        CHECK_THROW_MSG( _state <= EState::AddSubpasses,
            "AddSubpass() must be used before AddAttachment() and AddSpecialization()" );

        _state = EState::AddSubpasses;

        ObjectStorage::Instance()->AddName<SubpassName>( subpassName );

        bool inserted = _subpassMap.emplace( sp_name, uint(_subpasses.size()) ).second;
        CHECK_THROW_MSG( inserted,
            "subpass '"s << subpassName << "' is already exists" );

        _subpasses.push_back( SubpassInfo{ sp_name });
    }

/*
=================================================
    GetRenderPass
=================================================
*/
    RenderPassSpecPtr  CompatibleRenderPassDesc::GetRenderPass (RenderPassName::Ref name) const
    {
        auto    it = _specializations.find( name );
        if ( it != _specializations.end() )
            return it->second;
        else
            return null;
    }

/*
=================================================
    IsFirstSubpass
=================================================
*/
    bool  CompatibleRenderPassDesc::IsFirstSubpass (SubpassName::Ref name) const
    {
        auto    it = _subpassMap.find( name );
        if ( it != _subpassMap.end() )
            return it->second == 0;
        else
            return false;
    }

/*
=================================================
    Bind
=================================================
*/
    void  CompatibleRenderPassDesc::Bind (const ScriptEnginePtr &se) __Th___
    {
        // constants
        {
            const auto& c = *ObjectStorage::Instance()->_compatRPConstPtr;

            se->AddConstProperty( c.subpass_ExternalIn,         "InitialLayout"         );
            se->AddConstProperty( c.subpass_ExternalIn,         "Subpass_ExternalIn"    );
            se->AddConstProperty( c.subpass_ExternalOut,        "FinalLayout"           );
            se->AddConstProperty( c.subpass_ExternalOut,        "Subpass_ExternalOut"   );
            se->AddConstProperty( c.subpass_Main,               "Subpass_Main"          );

            se->AddConstProperty( c.attachment_Color,           "Attachment_Color"      );
            se->AddConstProperty( c.attachment_Depth,           "Attachment_Depth"      );
            se->AddConstProperty( c.attachment_DepthStencil,    "Attachment_DepthStencil" );
        }

        // attachment usage enum
        {
            EnumBinder<EAttachment>     binder{ se };
            binder.Create();
            binder.Comment( "Discard previous content. Used as optimization for TBDR architectures." );
            binder.AddValue( "Invalidate",      EAttachment::Invalidate     );

            binder.Comment( "Color attachment." );
            binder.AddValue( "Color",           EAttachment::Color          );

            binder.Comment( "Used as input attachment and color attachment." );
            binder.AddValue( "ReadWrite",       EAttachment::ReadWrite      );

            binder.Comment( "Resolve attachment - will get content from multisampled color attachment." );
            binder.AddValue( "ColorResolve",    EAttachment::ColorResolve   );

            binder.Comment( "Input attachment." );
            binder.AddValue( "Input",           EAttachment::Input          );

            binder.Comment( "Depth attachment." );
            binder.AddValue( "Depth",           EAttachment::Depth          );

            binder.Comment( "Keep attachment content between passes." );
            binder.AddValue( "Preserve",        EAttachment::Preserve       );

            binder.Comment( "Depth and stencil attachment." );
            binder.AddValue( "DepthStencil",    EAttachment::DepthStencil   );

            binder.Comment( "Fragment shading rate attachment." );
            binder.AddValue( "ShadingRate",     EAttachment::ShadingRate    );

            StaticAssert( uint(EAttachment::_Count) == 8 );
        }

        // shader IO
        {
            using ShaderIO = RPAttachment::ShaderIO;
            ClassBinder<ShaderIO>   binder{ se };
            binder.CreateClassValue();

            binder.AddConstructor( &ShaderIO_Ctor1, {"shaderVariableName"} );
            binder.AddConstructor( &ShaderIO_Ctor2, {"shaderVariableName", "type"} );
            binder.AddConstructor( &ShaderIO_Ctor3, {"colorOrInputAttachmentIndex", "shaderVariableName", "type"} );
            binder.AddConstructor( &ShaderIO_Ctor4, {"colorOrInputAttachmentIndex", "shaderVariableName"} );
        }

        // attachment
        {
            ClassBinder<RPAttachment>   binder{ se };
            binder.CreateRef();

            binder.Comment( "Attachment format." );
            binder.AddProperty( &RPAttachment::format,  "format" );

            binder.Comment( "Number of samples for multisampling." );
            binder.AddProperty( &RPAttachment::samples, "samples" );

            binder.Comment( "Attachment usage in subpass." );
            binder.AddMethod( &RPAttachment::AddUsage,  "Usage",    {"subpass", "usage"} );
            binder.AddMethod( &RPAttachment::AddUsage2, "Usage",    {"subpass", "usage", "inOrOut"} );
            binder.AddMethod( &RPAttachment::AddUsage3, "Usage",    {"subpass", "usage", "in", "out" } );
            binder.AddMethod( &RPAttachment::AddUsage4, "Usage",    {"subpass", "usage", "shadingRateTexelSize"} );

            binder.Comment( "For debugging: print information to the log." );
            binder.AddMethod( &RPAttachment::Print,     "Print",    {} );
        }

        // attachment specialization
        {
            ClassBinder<RPAttachmentSpec>   binder{ se };
            binder.CreateRef();

            binder.Comment( "Load operation.\n"
                            "Specify how image content will be loaded from global memory to tile memory." );
            binder.AddProperty( &RPAttachmentSpec::loadOp,              "loadOp" );

            binder.Comment( "Store operation.\n"
                            "Specify how image content will be stored from tile memory to global memory." );
            binder.AddProperty( &RPAttachmentSpec::storeOp,             "storeOp" );

            binder.Comment( "Set image layout in subpass." );
            binder.AddMethod( &RPAttachmentSpec::AddLayout,             "Layout",               {"subpass", "state"} );
            binder.AddMethod( &RPAttachmentSpec::AddLayout2,            "Layout",               {"subpass", "state"} );

            binder.Comment( "Generate optimal layouts for current attachment." );
            binder.AddMethod( &RPAttachmentSpec::GenOptimalLayouts,     "GenOptimalLayouts",    {} );
            binder.AddMethod( &RPAttachmentSpec::GenOptimalLayouts2,    "GenOptimalLayouts",    {"initialState", "finalState"} );
            binder.AddMethod( &RPAttachmentSpec::GenOptimalLayouts3,    "GenOptimalLayouts",    {"initialState", "finalState"} );

            binder.Comment( "For debugging: print information to the log." );
            binder.AddMethod( &RPAttachmentSpec::Print,                 "Print",                {} );
        }

        // render pass specialization
        {
            ClassBinder<RenderPassSpec>     binder{ se };
            binder.CreateRef();

            binder.Comment( "Create specialization for attachment to set layout per subpass and load/store operations.\n"
                            "Specialization contains params which can not break render pass compatibility." );
            binder.AddMethod( &RenderPassSpec::AddAttachment,       "AddAttachment",        {"name"} );

            binder.Comment( "Generate optimal layouts for all attachments. Used instead of specialization." );
            binder.AddMethod( &RenderPassSpec::GenOptimalLayouts,   "GenOptimalLayouts",    {} );

            binder.Comment( "For debugging: print information to the log." );
            binder.AddMethod( &RenderPassSpec::Print,               "Print",                {} );
        }

        // compatible render pass
        {
            ClassBinder<CompatibleRenderPassDesc>   binder{ se };
            binder.CreateRef();

            binder.Comment( "Create compatible render pass.\n"
                            "Name may be used in C++ code to create graphics/mesh/tile pipeline." );
            binder.AddFactoryCtor( &CompatibleRenderPassDesc_Ctor, {"name"} );

            binder.Comment( "Create render pass specialization.\n"
                            "Name is used in C++ code to begin render pass (in 'RenderPassDesc')." );
            binder.AddMethod( &CompatibleRenderPassDesc::AddSpecialization, "AddSpecialization",    {"rpName"} );

            binder.Comment( "Create render pass attachment.\n"
                            "Name is used in C++ code to bind image to attachment (in 'RenderPassDesc')." );
            binder.AddMethod( &CompatibleRenderPassDesc::AddAttachment,     "AddAttachment",        {"attachmentName"} );

            binder.Comment( "Create render pass subpass.\n"
                            "Name may be used in C++ code to create graphics/mesh/tile pipeline." );
            binder.AddMethod( &CompatibleRenderPassDesc::AddSubpass,        "AddSubpass",           {"subpassName"} );

            binder.Comment( "Add FeatureSet to the render pass.\n"
                            "Render pass can use only features that are enabled in at least one FeatureSet." );
            binder.AddMethod( &CompatibleRenderPassDesc::AddFeatureSet,     "AddFeatureSet",        {"fsName"} );

            binder.Comment( "For debugging: print information to the log." );
            binder.AddMethod( &CompatibleRenderPassDesc::Print,             "Print",                {} );
        }
    }

} // AE::PipelineCompiler
