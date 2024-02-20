// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/BasePipeline.h"
#include "ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
namespace
{
/*
=================================================
    BuildMSLEntry
=================================================
*/
    ND_ static String  BuildMSLEntry (EShaderVersion version, const ScriptShaderPtr &inShader, String entry_res, String entry_args,
                                      ArrayView<ScriptFeatureSetPtr> features) __Th___
    {
        const bool  has_attribs = uint(version & ~EShaderVersion::_Mask) >= 0x23;

        String  str;
        str << "#define AE_ENTRY()\n";

        if ( has_attribs )
            str << inShader->ThreadgroupsMSL( features );

        switch_enum( inShader->type )
        {
            case EShader::Vertex :      str << (has_attribs ? "[[vertex]]"   : "vertex");       break;
            case EShader::Fragment :    str << (has_attribs ? "[[fragment]]" : "fragment");     break;
            case EShader::Compute :     str << (has_attribs ? "[[kernel]"    : "kernel");       break;
            case EShader::Tile :        str << (has_attribs ? "[[kernel]"    : "kernel");       break;
            case EShader::MeshTask :    str << "[[object]]";                                    break;
            case EShader::Mesh :        str << "[[mesh]]";                                      break;

            case EShader::TessControl :
            case EShader::TessEvaluation :
            case EShader::Geometry :
            case EShader::RayGen :
            case EShader::RayAnyHit :
            case EShader::RayClosestHit :
            case EShader::RayMiss :
            case EShader::RayIntersection :
            case EShader::RayCallable :
            case EShader::_Count :
            case EShader::Unknown :
            default :                   CHECK_THROW_MSG( false, "unsupported shader type" );
        }
        switch_end
        str << " \n";

        // TODO:
        //  mesh_grid_properties
        //  set_threadgroups_per_grid()

        if ( entry_res.empty() )
            entry_res = "void";

        str << entry_res << "  Main (\n" << entry_args;

        // remove ',\n'
        if ( not str.empty() and str.back() == '\n' )
        {
            str.pop_back();
            str.pop_back();
        }

        for (usize i = 0; i < str.size(); ++i)
        {
            const char  c = str[i];
            if ( c == '\n' )
            {
                str.insert( str.begin() + i, '\\' );
                ++i;
            }
        }
        str << ")\n";
        str << "//----------\n\n";
        return str;
    }
}

/*
=================================================
    constructor
=================================================
*/
    BasePipelineTmpl::BasePipelineTmpl (const String &name) __Th___ :
        _name{ PipelineTmplName{name} },
        _nameStr{ name },
        _filename{ ObjectStorage::Instance()->pipelineFilename },
        _features{ ObjectStorage::Instance()->GetDefaultFeatureSets() }
    {
        ObjectStorage::Instance()->AddName<PipelineTmplName>( name );
        CHECK_THROW_MSG( ObjectStorage::Instance()->pplnTmplNames.insert( name ).second,
            "Pipeline with name '"s << name << "' is already defined" );
    }

/*
=================================================
    _Define
=================================================
*/
    void  BasePipelineTmpl::_Define (const String &value) __Th___
    {
        CHECK_THROW_MSG( not AnyBits( _states, EStateBits::HasShaders ),
            "can not add global definition when one of the shaders is set" );

        _defines << '\n' << value;

        _states |= EStateBits::Define;
    }

/*
=================================================
    _Include
=================================================
*/
    void  BasePipelineTmpl::_Include (const String &value) __Th___
    {
        CHECK_THROW_MSG( not AnyBits( _states, EStateBits::HasShaders ),
            "can not add global include when one of the shaders is set" );

        for (auto& inc : _includes) {
            if ( inc == value )
                return;
        }

        _includes.push_back( value );

        _states |= EStateBits::Include;
    }

/*
=================================================
    _CompileShader
=================================================
*/
    void  BasePipelineTmpl::_CompileShader (INOUT CompiledShaderPtr &outShader, const ScriptShaderPtr &inShader,
                                            const ShaderStructTypePtr &shaderInput, const ShaderStructTypePtr &shaderOutput,
                                            const Optional<FragOutput_t> &fragOut, const VertexBufferInputPtr &vbInput) __Th___
    {
        CHECK_THROW_MSG( inShader );
        CHECK_THROW_MSG( inShader->type != Default );
        CHECK_THROW_MSG( not outShader,
            String{ToString(inShader->type)} << " is already defined" );
        CHECK_THROW_MSG( not AnyBits( _states, EStateBits::HasSpec ),
            "can not add new shader when one of pipeline specializations is added" );

        const EShaderStages     stage   = EShaderStages::Unknown | inShader->type;
        const EShaderVersion    version = (inShader->version == Default ? _GetShaderVersion() : inShader->version);

        if ( AllBits( version, EShaderVersion::_SPIRV, EShaderVersion::_Mask ))
        {
            PipelineLayout::UniqueTypes_t   unique_types;

            String  resources   = inShader->SpecToGLSL();
            uint    dbg_ds_idx  = UMax;

            if ( vbInput )
                resources << vbInput->ToGLSL();

            if ( shaderInput )
                resources << shaderInput->ToShaderIO_GLSL( inShader->type, true, INOUT unique_types );

            if ( shaderOutput )
                resources << shaderOutput->ToShaderIO_GLSL( inShader->type, false, INOUT unique_types );

            bool    use_arg_buf = false;
            if ( _layoutPtr )
            {
                resources << _layoutPtr->ToGLSL( stage, INOUT unique_types );

                const auto& dbg_ds      = _layoutPtr->GetDebugDS();
                const bool  same_stage  = AllBits( dbg_ds.stages, stage );
                const auto  dbg_mode    = inShader->options & EShaderOpt::_ShaderTrace_Mask;

                dbg_ds_idx  = dbg_ds.dsIndex;
                use_arg_buf = _layoutPtr->MslDescSetAsArgumentBuffer();

                if ( same_stage )
                {
                    CHECK_THROW_MSG( (dbg_mode != Default) == dbg_ds.IsDefined(),
                        "Shader must have EShaderOpt with Trace/FnProfiling/TimeHeatMap or DebugDSLayout is not defined" );
                    CHECK_THROW_MSG( dbg_mode == dbg_ds.mode,
                        "shader debug mode is not compatible with DebugDSLayout in pipeline layout" );
                }
            }

            if ( fragOut.has_value() )
                resources << _FragOutputToGLSL( *fragOut );

            ObjectStorage::Instance()->CompileShaderGLSL( INOUT outShader, inShader, version, _defines, resources, _includes, _features, dbg_ds_idx, use_arg_buf );
        }
        else
        if ( AllBits( version, EShaderVersion::_Metal_iOS, EShaderVersion::_Mask ) or
             AllBits( version, EShaderVersion::_Metal_Mac, EShaderVersion::_Mask ) or
             AllBits( version, EShaderVersion::_Metal,     EShaderVersion::_Mask ))
        {
            PipelineLayout::UniqueTypes_t   unique_types;

            String  resources   = inShader->SpecToMSL();
            String  entry_args;
            String  entry_res;

            if ( vbInput )
            {
                resources  << vbInput->ToMSL();
                entry_args << "  VertexInput In [[stage_in]],\n";
            }

            if ( shaderInput )
            {
                resources  << shaderInput->ToShaderIO_MSL( inShader->type, true, INOUT unique_types );
                entry_args << "  " << shaderInput->Typename() << " In [[stage_in]],\n";
            }

            if ( shaderOutput )
            {
                resources << shaderOutput->ToShaderIO_MSL( inShader->type, false, INOUT unique_types );
                resources << "#define AE_STAGE_OUT  " << shaderOutput->Typename() << "\n\n";
                entry_res  = shaderOutput->Typename();
            }

            if ( shaderOutput and inShader->type == EShader::Mesh )
            {
                resources << inShader->MeshOutToMSL( String{shaderOutput->Typename()}, "" );
                entry_res.clear();
            }

            if ( shaderInput and inShader->type == EShader::Mesh )
                resources << "  const " << shaderInput->Typename() << "& In [[payload]],\n";

            if ( shaderOutput and inShader->type == EShader::MeshTask )
                entry_args << "  object_data " << shaderOutput->Typename() << "& Out [[payload]],\n";

            if ( _layoutPtr )
            {
                _layoutPtr->ToMSL( stage, INOUT unique_types, INOUT resources, INOUT entry_args );
                CHECK_THROW_MSG( not _layoutPtr->GetDebugDS().IsDefined(), "Debug DSLayout is not supported in Metal" );
            }

            if ( fragOut.has_value() )
            {
                resources << _FragOutputToMSL( *fragOut );
                resources << "#define AE_STAGE_OUT  FragmentOutput\n\n";
                entry_res = "FragmentOutput";
            }

            // for compute / mesh
            if (String comp_in = inShader->InputToMSL(); not comp_in.empty() )
                entry_args << inShader->InputToMSL();

            resources << "\n"
                << BuildMSLEntry( version, inShader, RVRef(entry_res), RVRef(entry_args), _features );

            ObjectStorage::Instance()->CompileShaderMSL( INOUT outShader, inShader, version, _defines, resources, _includes, _features );
        }
        else
        {
            CHECK_THROW_MSG( false, "unsupported shader version" );
        }

        _states |= EStateBits::HasShaders;
    }

/*
=================================================
    _OnAddSpecialization
=================================================
*/
    void  BasePipelineTmpl::_OnAddSpecialization () __Th___
    {
        _states |= EStateBits::HasSpec;
    }

/*
=================================================
    _SetLayout
=================================================
*/
    void  BasePipelineTmpl::_SetLayout (const String &name) __Th___
    {
        auto&   storage = *ObjectStorage::Instance();
        auto    iter    = storage.pplnLayouts.find( PipelineLayoutName{name} );
        CHECK_THROW_MSG( iter != storage.pplnLayouts.end(),
            "PipelineLayout with name '"s << name << "' is not found" );

        return _SetLayout( iter->second );
    }

    void  BasePipelineTmpl::_SetLayout (PipelineLayoutPtr ptr) __Th___
    {
        CHECK_THROW_MSG( not AnyBits( _states, EStateBits::HasShaders ),
            "can not set pipeline layout when one of the shaders is set" );

        CHECK_THROW_MSG( not _layoutUID.has_value() );
        CHECK_THROW_MSG( not _layoutPtr );
        CHECK_THROW_MSG( ptr );

        CHECK_THROW_MSG( ptr->Build() );
        CHECK_THROW_MSG( ptr->GetLayout().has_value() );

        _Define( ptr->GetDefines() );

        _features.insert( _features.end(), ptr->GetFeatures().begin(), ptr->GetFeatures().end() );
        ScriptFeatureSet::Minimize( INOUT _features );

        _layoutPtr = ptr;
        _layoutUID = *_layoutPtr->GetLayout();
    }

/*
=================================================
    GetLayout
=================================================
*/
    PipelineLayoutPtr  BasePipelineTmpl::GetLayout () C_Th___
    {
        CHECK_THROW_MSG( _layoutUID.has_value() );
        CHECK_THROW_MSG( _layoutPtr );
        return _layoutPtr;
    }

/*
=================================================
    _AddLayout
=================================================
*/
    bool  BasePipelineTmpl::_AddLayout (CompiledShaderPtr shader)
    {
        if ( not shader )
            return true;

        for (auto& ds : shader->reflection.layout.descrSets)
        {
            if ( ds.bindingIndex >= _dsLayoutsFromReflection.size() )
            {
                CHECK( ds.layout.uniforms.empty() );
                continue;
            }

            if ( not _dsLayoutNames[ds.bindingIndex].IsDefined() )
            {
                _dsLayoutNames[ds.bindingIndex] = ds.name;
            }
            else
                CHECK_ERR( _dsLayoutNames[ds.bindingIndex] == ds.name );

            CHECK_ERR( _dsLayoutsFromReflection[ds.bindingIndex].Merge( ds.layout ));
            _dsLayoutsFromReflection[ds.bindingIndex].SortUniforms();
        }

        CHECK_ERR( _pushConstants.Merge( shader->reflection.layout.pushConstants ));
        return true;
    }

/*
=================================================
    _GetLayout
=================================================
*/
    PipelineLayoutUID  BasePipelineTmpl::_GetLayout ()
    {
        CHECK_ERR( _layoutUID.has_value() );
        return *_layoutUID;
    }

/*
=================================================
    _BuildLayout
=================================================
*/
    bool  BasePipelineTmpl::_BuildLayout ()
    {
        auto&   storage         = *ObjectStorage::Instance();
        auto&   ppln_storage    = *storage.pplnStorage;

        if ( _layoutPtr )
        {
            // compare layouts
            CHECK_ERR( _layoutUID.has_value() );
            CHECK_ERR( _stages != Default );

            const auto& desc = _layoutPtr->GetDesc();

            // validate
            for (auto [ds_name, ds] : desc.descrSets)
            {
                auto*   ds_ptr = ppln_storage.GetDescriptorSetLayout( ds.uid );
                CHECK_ERR( ds_ptr != null );

                for (auto& [un_name, un] : ds_ptr->uniforms)
                {
                    if ( not AllBits( _stages, un.stages ))
                    {
                        AE_LOGI( "Pipeline '"s << GetName() << "' with layout '" << _layoutPtr->Name() << "' in DSLayout '" <<
                                 storage.GetName( ds_name ) << "', uniform '" << storage.GetName( un_name ) << "' has stages (" << ToString(un.stages) <<
                                 ") which is not compatible with exists stages (" << ToString(_stages) << ")" );
                    }
                }
            }

            if ( storage.target == ECompilationTarget::Vulkan )
            {
                CHECK_ERR( desc.pushConstants == _pushConstants );

                for (usize i = 0; i < _dsLayoutsFromReflection.size(); ++i)
                {
                    const DescriptorSetLayoutDesc*  ds_ptr = null;

                    for (auto [name, ds] : desc.descrSets)
                    {
                        if ( ds.vkIndex == i )
                        {
                            ds_ptr = ppln_storage.GetDescriptorSetLayout( ds.uid );
                            CHECK_ERR( ds_ptr != null );
                            break;
                        }
                    }

                    // Metal reflection is not supported
                    if ( _dsLayoutsFromReflection[i].uniforms.empty() )
                    {
                        CHECK_ERR( ds_ptr == null );
                        continue;
                    }

                    CHECK_ERR( ds_ptr != null );
                    CHECK_ERR( DescriptorSetLayoutDesc::IsCompatible( _dsLayoutsFromReflection[i], *ds_ptr ));
                }
            }
        }
        else
        {
            // build new layout
            CHECK_ERR( not _layoutUID.has_value() );

            PipelineLayoutDesc  desc;
            desc.pushConstants = _pushConstants;

            for (usize i = 0; i < _dsLayoutsFromReflection.size(); ++i)
            {
                if ( _dsLayoutsFromReflection[i].uniforms.empty() )
                    continue;

                auto [dst, inserted] = desc.descrSets.insert_or_assign( _dsLayoutNames[i], PipelineLayoutDesc::DescSetLayout{} );
                CHECK_ERR( inserted );

                dst->second.vkIndex = uint(i);
                dst->second.uid     = ppln_storage.AddDescriptorSetLayout( RVRef(_dsLayoutsFromReflection[i]) );
            }

            _dsLayoutsFromReflection.fill( Default );
            _dsLayoutNames.fill( Default );

            _layoutUID = ppln_storage.AddPipelineLayout( RVRef(desc) );
        }
        return true;
    }

/*
=================================================
    _AddFeatureSet
=================================================
*/
    void  BasePipelineTmpl::_AddFeatureSet (const String &name) __Th___
    {
        CHECK_THROW_MSG( not AnyBits( _states, EStateBits::HasShaders ),
            "can not add feature set when shaders is already set" );

        CHECK_THROW_MSG( not AnyBits( _states, EStateBits::HasSpec ),
            "can not add feature set when one of pipeline specializations is added" );

        _states |= EStateBits::HasFeatures;

        const auto& feat_sets = ObjectStorage::Instance()->featureSets;

        auto    iter = feat_sets.find( FeatureSetName{name} );
        CHECK_THROW_MSG( iter != feat_sets.end(),
            "FeatureSet with name '"s << name << "' is not found" );

        _features.push_back( iter->second );
        ScriptFeatureSet::Minimize( INOUT _features );
    }

/*
=================================================
    _FragmentOutputFromRenderTech
=================================================
*/
    bool  BasePipelineTmpl::_FragmentOutputFromRenderTech (const String &renTechName, const String &passName, OUT FragOutput_t &fragOut) __Th___
    {
        auto&   storage = *ObjectStorage::Instance();
        storage.AddName<RenderTechName>( renTechName );     // throw
        storage.AddName<RenderTechPassName>( passName );    // throw

        const auto& rtech_map   = storage.rtechMap;
        auto        rt_it       = rtech_map.find( renTechName );
        CHECK_ERR_MSG( rt_it != rtech_map.end(),
            "RenderTechnique with name '"s << renTechName << "' is not found" );

        auto    pass_ptr = rt_it->second->GetPass( passName );
        CHECK_ERR_MSG( pass_ptr,
            "RenderTechPass '"s << passName << "' is not exists in RenderTechnique '" << renTechName << "'" );

        auto*   gpass_ptr = DynCast<RTGraphicsPass>( pass_ptr.Get() );
        CHECK_ERR_MSG( gpass_ptr != null,
            "RenderTechPass '"s << passName << "' in RenderTechnique '" << renTechName << "' is not a graphics pass" );

        if ( not _FragmentOutputFromRenderPass( storage.GetName( gpass_ptr->GetRenderPassName() ), storage.GetName( gpass_ptr->GetSubpassName() ), OUT fragOut ))
        {
            RETURN_ERR( "in RenderTechnique '"s << renTechName << "' Pass '" << passName << "'" );
        }
        return true;
    }

/*
=================================================
    _FragmentOutputFromRenderPass
=================================================
*/
    bool  BasePipelineTmpl::_FragmentOutputFromRenderPass (const String &rPassName, const String &subpassName, OUT FragOutput_t &fragOut) __Th___
    {
        auto&   storage = *ObjectStorage::Instance();
        storage.AddName<RenderPassName>( rPassName ); // throw

        auto    rp_it = storage.renderPassToCompat.find( RenderPassName{ rPassName });
        CHECK_ERR_MSG( rp_it != storage.renderPassToCompat.end(),
            "RenderPass with name '"s << rPassName << "' is not found" );

        if ( not _FragmentOutputFromCompatRenderPass( storage.GetName( rp_it->second ), subpassName, OUT fragOut ))
        {
            RETURN_ERR( "in RenderPass '"s << rPassName << "'" );
        }
        return true;
    }

/*
=================================================
    _FragmentOutputFromCompatRenderPass
=================================================
*/
    bool  BasePipelineTmpl::_FragmentOutputFromCompatRenderPass (const String &compatRPassName, const String &subpassName, OUT FragOutput_t &fragOut) __Th___
    {
        auto&   storage = *ObjectStorage::Instance();
        storage.AddName<CompatRenderPassName>( compatRPassName );   // throw
        storage.AddName<SubpassName>( subpassName );                // throw

        auto    compat_it = storage.compatibleRPs.find( CompatRenderPassName{ compatRPassName });
        CHECK_ERR_MSG( compat_it != storage.compatibleRPs.end(),
            "CompatibleRenderPass with name '"s << compatRPassName << "' is not found" );

        const auto& subpass_map = compat_it->second->_subpassMap;
        auto        sp_it       = subpass_map.find( SubpassName{ subpassName });
        CHECK_ERR_MSG( sp_it != subpass_map.end(),
            "Subpass '"s << subpassName << "' is not exists in CompatibleRenderPass '" << compatRPassName << "'" );

        for (const auto& [name, att] : compat_it->second->_attachments)
        {
            auto    sp_att_it = att->usageMap.find( SubpassName{ subpassName });
            CHECK_ERR_MSG( sp_att_it != att->usageMap.end(),
                "Subpass '"s << subpassName << "' is not defined for Attachment '" << storage.GetName( name ) << "'" );

            const auto& usage   = sp_att_it->second;
            if ( not usage.output.IsDefined() )
                continue;

            CHECK_ERR_MSG( AnyEqual( usage.type, EAttachment::Color ),
                "Attachment '"s << storage.GetName( name ) << "' is not a color attachment" );

            fragOut.push_back( usage.output );
        }
        return true;
    }

/*
=================================================
    _FragOutputToGLSL
=================================================
*/
    String  BasePipelineTmpl::_FragOutputToGLSL (const FragOutput_t &fragOut) __Th___
    {
        auto&   storage = *ObjectStorage::Instance();
        String  str;

        for (auto& fo : fragOut)
        {
            str << "layout(location=" << ToString( fo.index ) << ") out ";

            switch_enum( fo.type )
            {
                case EShaderIO::Float :
                case EShaderIO::UFloat :
                case EShaderIO::Half :
                case EShaderIO::UNorm :
                case EShaderIO::SNorm :
                case EShaderIO::sRGB :
                case EShaderIO::AnyColor :      str << "vec4";  break;

                case EShaderIO::Int :           str << "ivec4"; break;
                case EShaderIO::UInt :          str << "uvec4"; break;

                case EShaderIO::Depth :
                case EShaderIO::Stencil :
                case EShaderIO::DepthStencil:
                case EShaderIO::Unknown :
                case EShaderIO::_Count :
                default :                       CHECK_THROW_MSG( false, "unknown ShaderIO type" );
            }
            switch_end

            str << "  " << storage.GetName( fo.name ) << ";\n";
        }
        str << "\n";
        return str;
    }

/*
=================================================
    _FragOutputToMSL
=================================================
*/
    String  BasePipelineTmpl::_FragOutputToMSL (const FragOutput_t &fragOut) __Th___
    {
        auto&   storage = *ObjectStorage::Instance();
        String  str;

        str << "struct FragmentOutput\n{\n";

        for (auto& fo : fragOut)
        {
            str << "\t";
            switch_enum( fo.type )
            {
                case EShaderIO::UFloat :
                case EShaderIO::UNorm :
                case EShaderIO::SNorm :
                case EShaderIO::sRGB :          // TODO: srgba8unorm<float4>
                case EShaderIO::Float :
                case EShaderIO::AnyColor :      str << "float4";    break;
                case EShaderIO::Half :          str << "half4";     break;

                case EShaderIO::Int :           str << "int4";      break;
                case EShaderIO::UInt :          str << "uint4";     break;

                case EShaderIO::Depth :
                case EShaderIO::Stencil :
                case EShaderIO::DepthStencil:
                case EShaderIO::Unknown :
                case EShaderIO::_Count :
                default :                       CHECK_THROW_MSG( false, "unknown ShaderIO type" );
            }
            switch_end

            str << "  " << storage.GetName( fo.name ) << "  [[color(" << ToString( fo.index ) << ")]];\n";
        }
        str << "};\n\n";
        return str;
    }

/*
=================================================
    _GetShaderVersion
=================================================
*/
    EShaderVersion  BasePipelineTmpl::_GetShaderVersion () const
    {
        const auto  target      = ObjectStorage::Instance()->target;
        const auto  sh_version  = ObjectStorage::Instance()->shaderVersion;

        if ( sh_version != Default )
            return sh_version;

        const auto  sh_ver      = GetMaxValueFromFeatures( _features, &FeatureSet::maxShaderVersion );
        const uint  min_spirv   = sh_ver.spirv;
        const uint  min_metal   = sh_ver.metal;

        if ( min_spirv != UMax )
        {
            ASSERT( min_spirv >= 100 and min_spirv <= 150 );
            return EShaderVersion(min_spirv) | EShaderVersion::_SPIRV;
        }

        if ( target == ECompilationTarget::Vulkan )
            return EShaderVersion::SPIRV_1_0;

        if ( target == ECompilationTarget::Metal_iOS )
        {
            if ( min_metal != UMax )
            {
                ASSERT( min_metal >= 200 and min_metal <= 240 );
                return EShaderVersion(min_metal) | EShaderVersion::_Metal_iOS;
            }
            return EShaderVersion::Metal_iOS_2_0;
        }

        if ( target == ECompilationTarget::Metal_Mac )
        {
            if ( min_metal != UMax )
            {
                ASSERT( min_metal >= 200 and min_metal <= 240 );
                return EShaderVersion(min_metal) | EShaderVersion::_Metal_Mac;
            }
            return EShaderVersion::Metal_Mac_2_0;
        }

        RETURN_ERR( "can not deduce shader version", Default );
    }

/*
=================================================
    _FragOutToSubpassShaderIO
=================================================
*/
    bool  BasePipelineTmpl::_FragOutToSubpassShaderIO (const FragOutput_t &fragOut, OUT SubpassShaderIO &outFragIO) const
    {
        outFragIO = Default;

        for (auto& col : fragOut)
        {
            outFragIO.colorAttachments.resize( Max( outFragIO.colorAttachments.size(), col.index+1 ));
            CHECK_ERR( not outFragIO.colorAttachments.at<0>( col.index ).IsDefined() );

            outFragIO.colorAttachments.set( col.index, ShaderIOName::Optimized_t{col.name}, col.type );
        }

        if ( _layoutPtr )
        {
            auto&   desc_sets       = _layoutPtr->GetDesc().descrSets;
            auto&   storage         = *ObjectStorage::Instance();
            auto&   ppln_storage    = *storage.pplnStorage;

            for (auto [ds_name, ds] : desc_sets)
            {
                auto*   ds_ptr = ppln_storage.GetDescriptorSetLayout( ds.uid );
                CHECK_ERR( ds_ptr != null );

                for (auto& [un_name, un] : ds_ptr->uniforms)
                {
                    if_unlikely( un.type == EDescriptorType::SubpassInput )
                    {
                        outFragIO.inputAttachments.resize( Max( outFragIO.inputAttachments.size(), un.image.subpassInputIdx + 1u ));
                        CHECK_ERR( not outFragIO.inputAttachments.at<0>( un.image.subpassInputIdx ).IsDefined() );

                        outFragIO.inputAttachments.set( un.image.subpassInputIdx,
                                                        ShaderIOName::Optimized_t{ storage.GetName( un_name )},
                                                        EImageType_ToShaderIO( un.image.type ));
                    }
                }
            }
        }

        ASSERT( fragOut.empty() == outFragIO.colorAttachments.empty() );
        return true;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    BasePipelineSpec::BasePipelineSpec (const BasePipelineTmpl* tmpl, const String &name) __Th___ :
        _name{ PipelineName{name} },
        _tmpl{ tmpl },
        _options{ ObjectStorage::Instance()->defaultPipelineOpt }
    {
        ObjectStorage::Instance()->AddName<PipelineName>( name );
    //  CHECK_THROW_MSG( ObjectStorage::Instance()->pplnSpecNames.insert( name ).second,
    //      "PipelineSpec with name '"s << name << "' is already defined" );
    }

/*
=================================================
    _OnBuild
=================================================
*/
    void  BasePipelineSpec::_OnBuild (PipelineSpecUID uid)
    {
        _linkedRTechs.clear();

        _uid = uid;
    }

/*
=================================================
    _SetSpecValue
=================================================
*/
    void  BasePipelineSpec::_SetSpecValue (INOUT GraphicsPipelineDesc::SpecValues_t &values, const String &name, uint value) __Th___
    {
        ObjectStorage::Instance()->AddName<SpecializationName>( name );

        auto [iter, inserted] = values.insert_or_assign( SpecializationName{name}, value );
        CHECK_THROW_MSG( inserted,
            "Specialization with name '"s << name << "' is already exists" );
    }

/*
=================================================
    _AddToRenderTech
=================================================
*/
    void  BasePipelineSpec::_AddToRenderTech (const String &rtechName, const String &passName) __Th___
    {
        const auto& rtech_map = ObjectStorage::Instance()->rtechMap;

        auto    rt_iter = rtech_map.find( rtechName );
        CHECK_THROW_MSG( rt_iter != rtech_map.end(),
            "Render technique '"s << rtechName << "' is not exists" );

        auto    pass    = rt_iter->second->GetPass( passName );
        CHECK_THROW_MSG( pass,
            "Render technique pass '"s << passName << "' is not exists in render technique '" << rtechName << "'" );

        _linkedRTechs.push_back( rt_iter->second );

        pass->AddPipeline( BasePipelineSpecPtr{this} );
    }

/*
=================================================
    _SetOptions
=================================================
*/
    void  BasePipelineSpec::_SetOptions (EPipelineOpt value) __Th___
    {
        _options = value;

        for (auto opt : BitfieldIterate( _options ))
        {
            switch_enum( opt )
            {
                case EPipelineOpt::Optimize :
                    break;  // ok

                case EPipelineOpt::CS_DispatchBase :
                    CHECK_THROW_MSG( AllBits( GetBase()->GetStages(), EShaderStages::Compute ),
                        "CS_DispatchBase requires Compute stage" );
                    break;

                case EPipelineOpt::RT_NoNullAnyHitShaders :
                case EPipelineOpt::RT_NoNullClosestHitShaders :
                case EPipelineOpt::RT_NoNullMissShaders :
                case EPipelineOpt::RT_NoNullIntersectionShaders :
                case EPipelineOpt::RT_SkipTriangles :
                case EPipelineOpt::RT_SkipAABBs :
                    CHECK_THROW_MSG( AnyBits( GetBase()->GetStages(), EShaderStages::AllRayTracing ),
                        ToString(opt) << " requires any RayTracing stage" );
                    break;

                case EPipelineOpt::DontCompile :
                case EPipelineOpt::CaptureStatistics :
                case EPipelineOpt::CaptureInternalRepresentation :
                    break;

                case EPipelineOpt::_Last :
                case EPipelineOpt::All :
                case EPipelineOpt::Unknown :
                default :   CHECK_THROW_MSG( false, "unknown pipeline option" );
            }
            switch_end
        }
    }

/*
=================================================
    _ValidateRenderState
=================================================
*/
    void  BasePipelineSpec::_ValidateRenderState (const EPipelineDynamicState dynamicState, const RenderState &state, ArrayView<ScriptFeatureSetPtr> features) __Th___
    {
        if ( state.rasterization.rasterizerDiscard )
        {
            CHECK_THROW_MSG( state.color == Default );
            CHECK_THROW_MSG( state.depth == Default );
            CHECK_THROW_MSG( state.stencil == Default );
        }

        for (auto dyn_st : BitfieldIterate( dynamicState ))
        {
            switch_enum( dyn_st )
            {
                case EPipelineDynamicState::StencilCompareMask :
                case EPipelineDynamicState::StencilWriteMask :
                case EPipelineDynamicState::StencilReference :
                    CHECK_THROW_MSG( state.stencil.enabled );
                    break;

                case EPipelineDynamicState::DepthBias :
                case EPipelineDynamicState::BlendConstants :
                //case EPipelineDynamicState::DepthBounds :
                case EPipelineDynamicState::RTStackSize :
                case EPipelineDynamicState::FragmentShadingRate :
                    break;  // skip

                case EPipelineDynamicState::Unknown :
                case EPipelineDynamicState::_Last :
                case EPipelineDynamicState::All :
                case EPipelineDynamicState::GraphicsPipelineMask :
                default :
                    CHECK_THROW_MSG( false, "unknown pipeline dynamic state" );
                    break;
            }
            switch_end
        }

        if ( state.rasterization.polygonMode != EPolygonMode::Fill )
            TEST_FEATURE( features, fillModeNonSolid, ", 'rasterization.polygonMode' must be Fill" );
    }

/*
=================================================
    _ValidateRenderPass
=================================================
*/
    void  BasePipelineSpec::_ValidateRenderPass (const RenderState &state, CompatRenderPassName::Optimized_t renderPass,
                                                 SubpassName::Optimized_t subpass, ArrayView<ScriptFeatureSetPtr> features) __Th___
    {
        auto&   storage = *ObjectStorage::Instance();
        auto    rp_it   = storage.compatibleRPs.find( CompatRenderPassName{renderPass} );
        CHECK_THROW_MSG( rp_it != storage.compatibleRPs.end(),
            "CompatibleRenderPass '"s << storage.GetName(renderPass) << "' with subpass '" << storage.GetName(subpass) << "' is not exists" );

        for (auto& [name, att] : rp_it->second->_attachments)
        {
            auto    sp_it = att->usageMap.find( SubpassName{subpass} );
            CHECK_THROW_MSG( sp_it != att->usageMap.end() );

            switch ( sp_it->second.type )
            {
                case EAttachment::Color :
                case EAttachment::ReadWrite :
                {
                    CHECK_THROW_MSG( att->index < state.color.buffers.size() );

                    auto&   cb = state.color.buffers[ att->index ];
                    if ( cb.blend ) {
                        TestFeature_PixelFormat( features, &FeatureSet::attachmentBlendFormats, att->format, "attachmentBlendFormats",
                                                 ", which used in Attachment '"s << storage.GetName(name) << "' in CompatibleRenderPass '" <<
                                                 storage.GetName(renderPass) << "' with subpass '" << storage.GetName(subpass) << "'" );  // throw
                    }
                    break;
                }
            }
        }
    }

/*
=================================================
    _SetLocalGroupSize
=================================================
*/
    void  BasePipelineSpec::_SetLocalGroupSize (String prefix, const uint3 &spec, const uint3 &maxSize, uint maxTotalSize,
                                                const uint3 &inSize, OUT packed_ushort3 &outSize) __Th___
    {
        uint    total_size = 1;

        if ( spec.x != UMax )
        {
            CHECK_THROW_MSG( inSize.x > 0 and inSize.x <= maxSize.x,
                prefix << "X (" << ToString(inSize.x) << ") must be in range [0, " << ToString(maxSize.x) << ")" );

            outSize.x = ushort(inSize.x);
            total_size *= outSize.x;
        }
        else
        {
            CHECK_THROW_MSG( inSize.x == UMax, prefix << "specialization for X is not supported" );
            outSize.x = UMax;
        }

        if ( spec.y != UMax )
        {
            CHECK_THROW_MSG( inSize.y > 0 and inSize.y <= maxSize.y,
                prefix << "Y (" << ToString(inSize.y) << ") must be in range [0, " << ToString(maxSize.y) << ")" );

            outSize.y = ushort(inSize.y);
            total_size *= outSize.y;
        }
        else
        {
            CHECK_THROW_MSG( inSize.y == UMax, prefix << "specialization for Y is not supported" );
            outSize.y = UMax;
        }

        if ( spec.z != UMax )
        {
            CHECK_THROW_MSG( inSize.z > 0 and inSize.z <= maxSize.z,
                prefix << "Z (" << ToString(inSize.z) << ") must be in range [0, " << ToString(maxSize.z) << ")" );

            outSize.z = ushort(inSize.z);
            total_size *= outSize.z;
        }
        else
        {
            CHECK_THROW_MSG( inSize.z == UMax, prefix << "specialization for Z is not supported" );
            outSize.z = UMax;
        }

        CHECK_THROW_MSG( total_size <= maxTotalSize,
            prefix << "local thread total size (" << ToString(total_size) << ") must be <= " << ToString(maxTotalSize) );
    }

/*
=================================================
    _CheckDepthStencil
=================================================
*/
    void  BasePipelineSpec::_CheckDepthStencil (const Graphics::RenderState &rs, const SubpassShaderIO &fragIO,
                                                const CompatRenderPassName::Optimized_t &rpName, const SubpassName::Optimized_t &subpass)
    {
        auto&       storage     = *ObjectStorage::Instance();
        const bool  req_depth   = rs.depth.test or rs.depth.write;
        const bool  req_stencil = rs.stencil.enabled;

        storage.TestRenderPass( rpName, subpass, fragIO, req_depth, req_stencil );  // throw
    }


} // AE::PipelineCompiler
