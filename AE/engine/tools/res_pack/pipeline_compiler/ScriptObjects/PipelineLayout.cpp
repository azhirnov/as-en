// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "ScriptObjects/DescriptorSetLayout.h"
#include "ScriptObjects/Common.inl.h"

namespace AE::PipelineCompiler
{
namespace
{
    static PipelineLayout*  PipelineLayout_Ctor (const String &name) {
        return PipelineLayoutPtr{ new PipelineLayout{ name }}.Detach();
    }

/*
=================================================
    IsStd***
=================================================
*/
    ND_ static bool  IsStd140OrMetal (EStructLayout layout)
    {
        return AnyEqual( layout, EStructLayout::Compatible_Std140, EStructLayout::Std140, EStructLayout::Metal );
    }

    ND_ static bool  IsStd430 (EStructLayout layout)
    {
        return AnyEqual( layout, EStructLayout::Compatible_Std430, EStructLayout::Std430 );
    }

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    PipelineLayout::PipelineLayout (const String &name) __Th___ :
        _features{ ObjectStorage::Instance()->GetDefaultFeatureSets() },
        _name{ name }
    {
        ObjectStorage::Instance()->AddName<PipelineLayoutName>( name );
        CHECK_THROW_MSG( ObjectStorage::Instance()->pplnLayouts.emplace( _name, PipelineLayoutPtr{this} ).second,
            "PipelineLayout with name '"s << name << "' is already defined" );
    }

/*
=================================================
    AddFeatureSet
=================================================
*/
    void  PipelineLayout::AddFeatureSet (const String &name) __Th___
    {
        CHECK_THROW_MSG( _dsLayouts.empty(), "Add all feature sets before descriptor set definition" );
        CHECK_THROW_MSG( not _uid.has_value() );

        auto&   storage = *ObjectStorage::Instance();
        auto    fs_it   = storage.featureSets.find( FeatureSetName{name} );
        CHECK_THROW_MSG( fs_it != storage.featureSets.end(),
            "FeatureSet with name '"s << name << "' is not found" );

        _features.push_back( fs_it->second );
    }

/*
=================================================
    Define
=================================================
*/
    void  PipelineLayout::Define (const String &value) __Th___
    {
        _defines << '\n' << value;
    }

/*
=================================================
    AddDSLayout
=================================================
*/
    void  PipelineLayout::AddDSLayout (uint index, const String &typeName) __Th___
    {
        return AddDSLayout2( typeName, index, typeName );
    }

    void  PipelineLayout::AddDSLayout2 (const String &name, uint index, const String &typeName) __Th___
    {
        CHECK_THROW_MSG( index < GraphicsConfig::MaxDescriptorSets );
        CHECK_THROW_MSG( not _uid.has_value() );

        auto&   storage = *ObjectStorage::Instance();
        auto    ds_it   = storage.dsLayouts.find( typeName );
        CHECK_THROW_MSG( ds_it != storage.dsLayouts.end(),
            "DescriptorSetLayout '"s << typeName << "' is not exists" );

        return AddDSLayout4( name, index, ds_it->second );
    }

/*
=================================================
    AddDSLayout
=================================================
*/
    void  PipelineLayout::AddDSLayout3  (uint index, const DescriptorSetLayoutPtr &dsPtr) __Th___
    {
        CHECK_THROW_MSG( dsPtr );
        return AddDSLayout4( String{dsPtr->Name()}, index, dsPtr );
    }

    void  PipelineLayout::AddDSLayout4 (const String &name, uint index, const DescriptorSetLayoutPtr &dsPtr) __Th___
    {
        CHECK_THROW_MSG( dsPtr );

        auto&   storage = *ObjectStorage::Instance();
        storage.AddName<DescriptorSetName>( name );

        for (auto& ds : _dsLayouts)
        {
            if ( auto ptr = ds.Get<0>() )
            {
                CHECK_THROW_MSG( name != storage.GetName( ds.Get<1>() ),
                    "descriptor set name '"s << name << "' is already used" );

                for (auto& un_name : ptr->_uniqueNames)
                {
                    CHECK_THROW_MSG( not dsPtr->_uniqueNames.contains( un_name ),
                        "uniform with name '"s << un_name << "' is already exists in DS '" << ptr->_name << "'" );
                }
            }
        }

        _features.insert( _features.end(), dsPtr->_features.begin(), dsPtr->_features.end() );
        ScriptFeatureSet::Minimize( INOUT _features );

        _dsLayouts.resize( Max( index+1, _dsLayouts.size() ));
        CHECK_THROW_MSG( not _dsLayouts[index].Get<0>(),
            "DS index "s << ToString(index) << " is already used" );

        _dsLayouts[index] = Tuple{ dsPtr, DescriptorSetName{name} };
    }

/*
=================================================
    AddPushConst
=================================================
*/
    void  PipelineLayout::AddPushConst1 (const String &name, const String &structName, EShader stage) __Th___
    {
        auto&   struct_types = ObjectStorage::Instance()->structTypes;

        ObjectStorage::Instance()->AddName<ShaderStructName>( structName );

        auto    st_it = struct_types.find( structName );
        CHECK_THROW_MSG( st_it != struct_types.end(),
            "ShaderStructType '"s << structName << "' is not exists" );

        return AddPushConst2( name, st_it->second, stage );
    }

    void  PipelineLayout::AddPushConst2 (const String &name, const ShaderStructTypePtr &ptr, EShader stage) __Th___
    {
        CHECK_THROW_MSG( stage != Default );
        CHECK_THROW_MSG( ptr );
        CHECK_THROW_MSG( not ptr->HasDynamicArray() );

        ObjectStorage::Instance()->AddName<PushConstantName>( name );

        for (auto& pc : _pushConstants)
        {
            CHECK_THROW_MSG( pc.Get<0>() != name,
                "push constant name '"s << name << "' is already used" );
        }

        if ( IsStd430( ptr->Layout() ))
        {
            TEST_FEATURE( _features, scalarBlockLayout,
                ", PushConstant '"s << name << "' with struct '" << ptr->Name() <<
                "' with Std430 layout requires 'scalarBlockLayout'" );
        }
        else
        {
            CHECK_THROW_MSG( IsStd140OrMetal( ptr->Layout() ),
                "PushConstant '"s << name << "' with struct '" << ptr->Name() << "' reuuires Std140 layout" );
        }

        _pushConstants.push_back( Tuple{ name, ptr, stage });
    }

/*
=================================================
    AddDebugDSLayout
=================================================
*/
    void  PipelineLayout::AddDebugDSLayout1 (const EShaderOpt dbgMode, const uint stages) __Th___
    {
        return AddDebugDSLayout2( uint(_dsLayouts.size()), dbgMode, stages );
    }

    void  PipelineLayout::AddDebugDSLayout2 (const uint index, const EShaderOpt dbgMode, const uint inStages) __Th___
    {
        CHECK_THROW_MSG( inStages != 0 );

        const String    un_name         = "dbg_ShaderTrace";
        const Bytes     static_size     = 16_b;
        const Bytes     array_stride    = 4_b;
        String          name            = "DbgDS_";

        CHECK_THROW_MSG( not _dbgInfo.IsDefined(), "debug descriptor set is already defined" );
        CHECK_THROW_MSG( (dbgMode & ~EShaderOpt::_ShaderTrace_Mask) == Default, "only 'ShaderTrace_Mask' flags are supported" );

        switch ( dbgMode & EShaderOpt::_ShaderTrace_Mask )
        {
            case EShaderOpt::Trace :            name << "Trace";    break;
            case EShaderOpt::FnProfiling :      name << "FnProf";   break;
            case EShaderOpt::TimeHeatMap :      name << "TmProf";   break;
            default :                           CHECK_THROW_MSG( false, "unknown shader trace mode" );
        }

        name << "_";

        for (auto stages = EShaderStages(inStages); stages != Zero;)
        {
            BEGIN_ENUM_CHECKS();
            switch ( ExtractBit( INOUT stages ))
            {
                case EShaderStages::Vertex :            name << "Vs";   break;
                case EShaderStages::TessControl :       name << "Tc";   break;
                case EShaderStages::TessEvaluation :    name << "Te";   break;
                case EShaderStages::Geometry :          name << "Gs";   break;
                case EShaderStages::Fragment :          name << "Fg";   break;
                case EShaderStages::Compute :           name << "Cs";   break;
                case EShaderStages::Tile :              name << "Tl";   break;
                case EShaderStages::MeshTask :          name << "Mt";   break;
                case EShaderStages::Mesh :              name << "Ms";   break;
                case EShaderStages::RayGen :            name << "Rg";   break;
                case EShaderStages::RayAnyHit :         name << "Rah";  break;
                case EShaderStages::RayClosestHit :     name << "Rch";  break;
                case EShaderStages::RayMiss :           name << "Rm";   break;
                case EShaderStages::RayIntersection :   name << "Ri";   break;
                case EShaderStages::RayCallable :       name << "Rc";   break;

                case EShaderStages::All :
                case EShaderStages::GraphicsStages :
                case EShaderStages::MeshStages :
                case EShaderStages::AllGraphics :
                case EShaderStages::AllRayTracing :
                case EShaderStages::Unknown :
                default :
                    CHECK_THROW_MSG( false, "unknown shader stage" );
            }
            END_ENUM_CHECKS();
        }

        // TODO

        auto&   storage     = *ObjectStorage::Instance();
        auto&   ds_layouts  = storage.dsLayouts;
        auto    ds_it       = ds_layouts.find( name );

        DescriptorSetLayoutPtr  ds_ptr;

        // create new DS layout
        if ( ds_it == ds_layouts.end() )
        {
            ds_ptr = DescriptorSetLayoutPtr{ new DescriptorSetLayout{ name }};
            ds_ptr->AddDebugStorageBuffer( un_name, EShaderStages(inStages), static_size, array_stride );
            CHECK_THROW_MSG( ds_ptr->Build() );
        }else{
            ds_ptr = ds_it->second;
        }

        storage.AddName< DescriptorSetName >( _DbgShaderTraceStr );

        _dsLayouts.resize( Max( index+1, _dsLayouts.size() ));
        CHECK_THROW_MSG( not _dsLayouts[index].Get<0>() );

        _dsLayouts[index]   = Tuple{ ds_ptr, _DbgShaderTrace };
        _dbgInfo.dsIndex    = index;
        _dbgInfo.mode       = dbgMode;
        _dbgInfo.stages     = EShaderStages(inStages);
    }

/*
=================================================
    ToGLSL
=================================================
*/
    String  PipelineLayout::ToGLSL (EShaderStages stages, INOUT UniqueTypes_t &uniqueTypes) C_Th___
    {
        String  types;
        String  str;
        uint    idx         = 0;
        auto&   storage     = *ObjectStorage::Instance();

        for (auto& ds : _dsLayouts)
        {
            if ( ds.Get<1>() == _DbgShaderTrace )
                continue;

            if ( auto ptr = ds.Get<0>() )
            {
                String  decl;
                ptr->ToGLSL( stages, idx, INOUT types, OUT decl, INOUT uniqueTypes );

                str << "//---------------------\n// ds[" << ToString(idx) << "], name: '" << storage.GetName( ds.Get<1>() ) << "', type: '" << ptr->_name << "'\n"
                    << decl
                    << "//---------------------\n\n";
            }
            ++idx;
        }

        if ( not _pushConstants.empty() )
        {
            Bytes   pc_offset;

            for (auto& pc : _pushConstants)
            {
                if ( AnyBits( EShaderStages(0) | pc.Get<2>(), stages ))
                {
                    String  fields;
                    pc.Get<1>()->AddUsage( ShaderStructType::EUsage::BufferLayout );
                    CHECK_THROW_MSG( pc.Get<1>()->ToGLSL( true, INOUT types, OUT fields, INOUT uniqueTypes, pc_offset ));

                    str << "layout(push_constant) uniform AE_Type_" << pc.Get<1>()->Typename() << " {\n" << fields << "} " << pc.Get<0>() << ";\n\n";
                }

                pc_offset += pc.Get<1>()->StaticSize();
            }
        }

        return types + str;
    }

/*
=================================================
    ToMSL
=================================================
*/
    void  PipelineLayout::ToMSL (EShaderStages stages, INOUT UniqueTypes_t &uniqueTypes, INOUT String &typesStr, INOUT String &declStr) C_Th___
    {
        CHECK_THROW_MSG( IsSingleBitSet( stages ));

        MSLBindings msl_bindings;

        for (auto& ds : _dsLayouts)
        {
            CHECK_THROW_MSG( ds.Get<1>() != _DbgShaderTrace, "shader trace is not supported in Metal" );

            if ( auto ptr = ds.Get<0>() )
            {
                String  decl;
                ptr->ToMSL( stages, INOUT msl_bindings, INOUT typesStr, OUT decl, INOUT uniqueTypes );

                declStr << decl;
            }
        }

        if ( not _pushConstants.empty() )
        {
            declStr << "  /* push constants */\n";

            for (auto& pc : _pushConstants)
            {
                if ( AnyBits( EShaderStages(0) | pc.Get<2>(), stages ))
                {
                    pc.Get<1>()->AddUsage( ShaderStructType::EUsage::BufferLayout );
                    CHECK_THROW_MSG( pc.Get<1>()->ToMSL( INOUT typesStr, INOUT uniqueTypes ));

                    declStr << "  constant " << pc.Get<1>()->Typename() << " " << pc.Get<0>() << " [[buffer(" << ToString( msl_bindings.bufferIdx++ ) << ")]],\n";
                }
            }
        }
    }

/*
=================================================
    Bind
=================================================
*/
    void  PipelineLayout::Bind (const ScriptEnginePtr &se) __Th___
    {
        ClassBinder<PipelineLayout> binder{ se };
        binder.CreateRef();

        binder.Comment( "Create pipeline template.\n"
                        "Name is used only in script." );
        binder.AddFactoryCtor( &PipelineLayout_Ctor, {"name"} );

        binder.Comment( "Add FeatureSet to the pipeline layout." );
        binder.AddMethod( &PipelineLayout::AddFeatureSet,       "AddFeatureSet",    {"fsName"} );

        binder.Comment( "Add descriptor set layout." );
        binder.AddMethod( &PipelineLayout::AddDSLayout,         "DSLayout",         {"index", "dslName"} );
        binder.AddMethod( &PipelineLayout::AddDSLayout2,        "DSLayout",         {"dsName", "index", "dslName"} );
        binder.AddMethod( &PipelineLayout::AddDSLayout3,        "DSLayout",         {"index", "dsl"} );
        binder.AddMethod( &PipelineLayout::AddDSLayout4,        "DSLayout",         {"dsName", "index", "dsl"} );

        binder.Comment( "Add descriptor set layout for shader debugging." );
        binder.AddMethod( &PipelineLayout::AddDebugDSLayout1,   "AddDebugDSLayout", {"dbgMode", "shaderStages"} );
        binder.AddMethod( &PipelineLayout::AddDebugDSLayout2,   "AddDebugDSLayout", {"index", "dbgMode", "shaderStages"} );

        binder.Comment( "Set push constant layout for specific shader stage.\n"
                        "Push constants are native in Vulkan and emulated in Metal." );
        binder.AddMethod( &PipelineLayout::AddPushConst1,       "PushConst",        {"pcName", "structName", "stage"} );
        binder.AddMethod( &PipelineLayout::AddPushConst2,       "PushConst",        {"pcName", "type", "stage"} );

        binder.Comment( "Add macros which will be used in shader.\n"
                        "Format: MACROS = value; DEF" );
        binder.AddMethod( &PipelineLayout::Define,              "Define",           {} );
    }

/*
=================================================
    Build
=================================================
*/
    bool  PipelineLayout::Build ()
    {
        using MSLBindingsPerState_t = FixedMap< EShaderStages, MSLBindings, uint(EShader::_Count) >;

        if ( _uid.has_value() )
            return true;

        uint                    idx             = 0;
        DescriptorCount_t       total           = {};
        PerStageDescCount_t     per_stage;
        MSLBindingsPerState_t   msl_per_stage;
        const bool              is_metal        = AnyEqual( ObjectStorage::Instance()->target, ECompilationTarget::Metal_Mac, ECompilationTarget::Metal_iOS );

        for (auto& ds : _dsLayouts)
        {
            if ( auto ptr = ds.Get<0>() )
            {
                CHECK_ERR( ptr->Build() );
                CHECK_ERR( ptr->_uid.has_value() );

                PipelineLayoutDesc::DescSetLayout   dsl;
                dsl.vkIndex = idx;
                dsl.uid     = *ptr->_uid;

                // Metal
                if ( is_metal )
                {
                    for (EShaderStages stages = ptr->GetStages(); stages != Default;)
                    {
                        const EShaderStages stage           = ExtractBit( INOUT stages );
                        auto&               msl_bindings    = msl_per_stage( stage );
                        ubyte*              dst             = dsl.mtlIndex.PtrForShader( stage );

                        CHECK_ERR( dst != null, "unsupported shader stage for Metal" );
                        CHECK_ERR( CheckCast( OUT *dst, msl_bindings.BufferCount() ));

                        CHECK_ERR( ptr->CountMSLBindings( stage, INOUT msl_bindings ));
                    }

                    CHECK_ERR( CheckCast( dsl.mtlIndex.BindingIndex(), idx ));
                }

                CHECK_ERR( _desc.descrSets.emplace( DescriptorSetName{ds.Get<1>()}, dsl ).second );

                ptr->CountDescriptors( INOUT total, INOUT per_stage );

                Define( ptr->GetDefines() );
            }
            ++idx;
        }

        ScriptFeatureSet::Minimize( INOUT _features );
        CHECK_ERR( DescriptorSetLayout::CheckDescriptorLimits( total, per_stage, _features, ("In PipelineLayout '"s << _name << "'") ));

        // TODO: check metal limits

        Bytes   pc_offset;

        for (auto& src : _pushConstants)
        {
            auto&       msl_bindings    = msl_per_stage( EShaderStages(0) | src.Get<2>() );
            const uint  buf_idx         = msl_bindings.bufferIdx++;

            if ( is_metal )
                CHECK_ERR( buf_idx < 0xFF );

            PushConstants::PushConst    pc;
            pc.vulkanOffset     = pc_offset;
            pc.metalBufferId    = ubyte(buf_idx);
            pc.size             = src.Get<1>()->StaticSize();
            pc.typeName         = ShaderStructName{src.Get<1>()->Typename()};
            pc.stage            = src.Get<2>();

            pc_offset += pc.size;

            _desc.pushConstants.items.emplace( PushConstantName{src.Get<0>()}, pc );
        }

        ASSERT( _pushConstants.size() == _desc.pushConstants.items.size() );

        TestFeature_Min( _features, &FeatureSet::minDescriptorSets,     ushort(idx),    "minDescriptorSets",    "DescriptorLayouts" );
        TestFeature_Min( _features, &FeatureSet::minPushConstantsSize,  uint(pc_offset),"minPushConstantsSize", "PushConstantsSize" );

        if ( is_metal )
        {
            for (auto [stage, bindings] : msl_per_stage)
            {
                CHECK_THROW_MSG( bindings.BufferCount()  <= MetalLimits::maxBuffers );
                CHECK_THROW_MSG( bindings.SamplerCount() <= MetalLimits::maxSamplers );
                CHECK_THROW_MSG( bindings.textureIdx     <= MetalLimits::maxImages );
            }
        }

        _uid = ObjectStorage::Instance()->pplnStorage->AddPipelineLayout( _desc );
        return true;
    }

/*
=================================================
    MslDescSetAsArgumentBuffer
=================================================
*/
    bool  PipelineLayout::MslDescSetAsArgumentBuffer () const
    {
        const bool  is_metal = AnyEqual( ObjectStorage::Instance()->target, ECompilationTarget::Metal_Mac, ECompilationTarget::Metal_iOS );

        if ( not is_metal )
            return false;

        uint    with_arg_buf    = 0;
        uint    without_arg_buf = 0;

        for (auto& ds : _dsLayouts)
        {
            if ( auto ptr = ds.Get<0>() )
            {
                const auto  usage       = ptr->GetUsage();
                const bool  is_arg_buf  = AllBits( usage, EDescSetUsage::ArgumentBuffer );

                with_arg_buf    += uint(is_arg_buf);
                without_arg_buf += uint(not is_arg_buf);
            }
        }

        if ( (with_arg_buf == 0 and without_arg_buf == 0) or
             ((with_arg_buf != 0) != (without_arg_buf != 0)) )
        {
            return with_arg_buf != 0;
        }

        String  msg = "PipelineLayout '"s << _name << "' has DS with adn without usage: ArgumentBuffer which is not compatible with SpvToMsl:";

        for (auto& ds : _dsLayouts)
        {
            if ( auto ptr = ds.Get<0>() )
            {
                msg << "\n  '" << ptr->Name() << "' usage: " << ToString( ptr->GetUsage() );
            }
        }
        CHECK_THROW_MSG( false, msg );
        return false;
    }


} // AE::PipelineCompiler
