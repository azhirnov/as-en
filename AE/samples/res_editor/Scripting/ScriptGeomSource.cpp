// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"

#include "res_editor/GeomSource/SphericalCube.h"
#include "res_editor/GeomSource/UnifiedGeometry.h"
#include "res_editor/GeomSource/ModelGeomSource.h"

#include "res_editor/_data/cpp/types.h"
#include "res_editor/_ui_data/cpp/types.h"

#include "res_editor/Scripting/PassCommon.inl.h"

#include "res_loaders/Intermediate/IntermScene.h"
#include "res_loaders/Assimp/AssimpLoader.h"

namespace AE::ResEditor
{
    using namespace AE::PipelineCompiler;

namespace
{
    static ScriptModelGeometrySrc*  ScriptSceneGeometry_Ctor1 (const String &filename) {
        return ScriptRC<ScriptModelGeometrySrc>{ new ScriptModelGeometrySrc{ filename }}.Detach();
    }

    static const StringView     c_MtrDS     = "material";
    static const StringView     c_PassDS    = "pass";

} // namespace
//-----------------------------------------------------------------------------


namespace
{
/*
=================================================
    ScriptGeomSource_ToBase
=================================================
*/
    template <typename T>
    static ScriptGeomSource*  ScriptGeomSource_ToBase (T* ptr)
    {
        STATIC_ASSERT( IsBaseOf< ScriptGeomSource, T >);

        ScriptGeomSourcePtr  result{ ptr };
        return result.Detach();
    }


/*
=================================================
    GetDescSetBinding
=================================================
*/
    template <typename ResMngr, typename PplnID>
    ND_ static DescSetBinding  GetDescSetBinding (ResMngr &resMngr, PplnID pplnId, const DescriptorSetName &name) __Th___
    {
        auto*   ppln = resMngr.GetResource( pplnId );
        CHECK_THROW( ppln != null );

        auto*   layout = resMngr.GetResource( ppln->LayoutID() );
        CHECK_THROW( layout != null );

        DescriptorSetLayoutID   dsl;
        DescSetBinding          binding;
        CHECK_THROW( layout->GetDescriptorSetLayout( name, OUT dsl, OUT binding ));

        return binding;
    }

/*
=================================================
    _FindPipelinesByVB
=================================================
*/
    static void  _FindPipelinesByVB (StringView vbName, OUT Array<GraphicsPipelineSpecPtr> &pipelines) __Th___
    {
        auto&   storage = *ObjectStorage::Instance();
        pipelines.clear();

        for (auto& [tname, ppln] : storage.gpipelines)
        {
            for (auto& spec : ppln->GetSpecializations())
            {
                if ( spec->GetVertexBuffers() == null )
                    continue;

                if ( vbName == spec->GetVertexBuffers()->Name() )
                    pipelines.push_back( spec );
            }
        }
        CHECK_THROW_MSG( not pipelines.empty(),
            "Can't find pipelines with vertex buffer type '"s << vbName << "'" );
    }

/*
=================================================
    _FindPipelinesWithoutVB
=================================================
*/
    static void  _FindPipelinesWithoutVB (OUT Array<GraphicsPipelineSpecPtr> &pipelines) __Th___
    {
        auto&   storage = *ObjectStorage::Instance();
        pipelines.clear();

        for (auto& [tname, ppln] : storage.gpipelines)
        {
            for (auto& spec : ppln->GetSpecializations())
            {
                if ( spec->GetVertexBuffers() == null )
                    pipelines.push_back( spec );
            }
        }
        CHECK_THROW_MSG( not pipelines.empty(),
            "Can't find graphics pipelines without vertex buffer" );
    }

/*
=================================================
    _GetMeshPipelines
=================================================
*/
    static void  _GetMeshPipelines (OUT Array<MeshPipelineSpecPtr> &pipelines) __Th___
    {
        auto&   storage = *ObjectStorage::Instance();
        pipelines.clear();

        for (auto& [tname, ppln] : storage.mpipelines)
        {
            for (auto& spec : ppln->GetSpecializations())
            {
                pipelines.push_back( spec );
            }
        }
        CHECK_THROW_MSG( not pipelines.empty(), "Can't find mesh pipelines" );
    }
/*
=================================================
    _FindPipelinesByBuf
=================================================
*/
    template <typename PplnSpec>
    static void  _FindPipelinesByBuf (StringView dsName, StringView bufTypeName, EDescriptorType descType, INOUT Array<PplnSpec> &inPipelines) __Th___
    {
        Array<PplnSpec>             out_pplns;
        const DescriptorSetName     req_ds_name     {dsName};
        const ShaderStructName      req_buf_name    {bufTypeName};

        for (auto& ppln : inPipelines)
        {
            auto    ppln_layout = ppln->GetBase()->GetLayout();
            if ( not ppln_layout )
                continue;

            for (auto& [dsl, ds_name] : ppln_layout->Layouts())
            {
                if ( ds_name != req_ds_name )
                    continue;

                for (auto& [un_name, un] : dsl->GetUniforms())
                {
                    if ( un.type != descType )
                        continue;

                    if ( un.buffer.typeName == req_buf_name ) {
                        out_pplns.push_back( ppln );
                        break;
                    }
                }
            }
        }

        CHECK_THROW_MSG( not out_pplns.empty(),
            "Can't find pipelines with DS '"s << dsName << "' and " <<
            (descType == EDescriptorType::UniformBuffer ? "UniformBuffer" :
             descType == EDescriptorType::StorageBuffer ? "StorageBuffer" : "<unknown>") << 
            " with type '" << bufTypeName << "'" );

        inPipelines = RVRef(out_pplns);
    }

    template <typename PplnSpec>
    static void  _FindPipelinesByUB (StringView dsName, StringView ubTypeName, INOUT Array<PplnSpec> &pipelines) __Th___
    {
        _FindPipelinesByBuf( dsName, ubTypeName, EDescriptorType::UniformBuffer, INOUT pipelines );
    }

    template <typename PplnSpec>
    static void  _FindPipelinesBySB (StringView dsName, StringView sbTypeName, INOUT Array<PplnSpec> &pipelines) __Th___
    {
        _FindPipelinesByBuf( dsName, sbTypeName, EDescriptorType::StorageBuffer, INOUT pipelines );
    }

/*
=================================================
    _FindPipelinesBySampledImage
=================================================
*/
    template <typename PplnSpec>
    static void  _FindPipelinesBySampledImage (StringView dsName, StringView imgName, EImageType imgType, uint arraySize, INOUT Array<PplnSpec> &inPipelines) __Th___
    {
        Array<PplnSpec>             out_pplns;
        const DescriptorSetName     req_ds_name {dsName};
        const UniformName           req_un_name {imgName};

        for (auto& ppln : inPipelines)
        {
            auto    ppln_layout = ppln->GetBase()->GetLayout();
            if ( not ppln_layout )
                continue;

            for (auto& [dsl, ds_name] : ppln_layout->Layouts())
            {
                if ( ds_name != req_ds_name )
                    continue;

                for (auto& [un_name, un] : dsl->GetUniforms())
                {
                    if ( un.type != EDescriptorType::SampledImage )
                        continue;

                    if ( un.image.type  == imgType      and
                         un_name        == req_un_name  and
                         un.arraySize   == arraySize )
                    {
                        out_pplns.push_back( ppln );
                        break;
                    }
                }
            }
        }

        CHECK_THROW_MSG( not out_pplns.empty(),
            "Can't find pipelines with DS '"s << dsName << "' and SampledImage '" << imgName << "' with type '" <<
            EImageType_ToString( imgType ) << "', array size " << ToString(arraySize) );

        inPipelines = RVRef(out_pplns);
    }

/*
=================================================
    _FindPipelinesBySampler
=================================================
*/
    template <typename PplnSpec>
    static void  _FindPipelinesBySampler (StringView dsName, StringView uniformName, INOUT Array<PplnSpec> &inPipelines) __Th___
    {
        Array<PplnSpec>             out_pplns;
        const DescriptorSetName     req_ds_name {dsName};
        const UniformName           req_un_name {uniformName};

        for (auto& ppln : inPipelines)
        {
            auto    ppln_layout = ppln->GetBase()->GetLayout();
            if ( not ppln_layout )
                continue;

            for (auto& [dsl, ds_name] : ppln_layout->Layouts())
            {
                if ( ds_name != req_ds_name )
                    continue;

                for (auto& [un_name, un] : dsl->GetUniforms())
                {
                    if ( un.type != EDescriptorType::ImmutableSampler )
                        continue;

                    if ( un_name == req_un_name )
                    {
                        out_pplns.push_back( ppln );
                        break;
                    }
                }
            }
        }

        CHECK_THROW_MSG( not out_pplns.empty(),
            "Can't find pipelines with DS '"s << dsName << "' and ImmutableSampler '" << uniformName << "'" );

        inPipelines = RVRef(out_pplns);
    }

/*
=================================================
    _FindPipelinesByPC
=================================================
*/
    template <typename PplnSpec>
    static void  _FindPipelinesByPC (StringView pcTypeName, INOUT Array<PplnSpec> &inPipelines) __Th___
    {
        Array<PplnSpec> out_pplns;

        for (auto& ppln : inPipelines)
        {
            auto    ppln_layout = ppln->GetBase()->GetLayout();
            if ( not ppln_layout )
                continue;

            for (auto& [name, type, shader] : ppln_layout->PushConstants())
            {
                if ( type->Name() == pcTypeName ) {
                    out_pplns.push_back( ppln );
                    break;
                }
            }
        }

        CHECK_THROW_MSG( not out_pplns.empty(),
            "Can't find pipelines with PushConstant '"s << pcTypeName << "'" );

        inPipelines = RVRef(out_pplns);
    }

/*
=================================================
    _FindPipelinesByResources
=================================================
*/
    template <typename PplnSpec>
    static void  _FindPipelinesByResources (StringView dsName, const ScriptPassArgs::Arguments_t &args, INOUT Array<PplnSpec> &inPipelines) __Th___
    {
        auto&                                               storage     = *ObjectStorage::Instance();
        Array<PplnSpec>                                     out_pplns;
        const DescriptorSetName                             req_ds_name {dsName};
        FlatHashMap< UniformName::Optimized_t, StringView > tex_names;
        FlatHashSet< UniformName::Optimized_t >             buf_names;
        FlatHashSet< UniformName::Optimized_t >             texbuf_names;
        FlatHashSet< UniformName::Optimized_t >             img_names;
        FlatHashSet< UniformName::Optimized_t >             rtas_names;

        for (auto& arg : args)
        {
            Visit( arg.res,
                [&] (ScriptBufferPtr buf) {
                    if ( buf->HasLayout() )
                        buf_names.insert( UniformName::Optimized_t{arg.name} );
                    else
                        texbuf_names.insert( UniformName::Optimized_t{arg.name} );
                },
                [&] (ScriptImagePtr) {
                    if ( arg.samplerName.empty() )
                        img_names.emplace( UniformName::Optimized_t{arg.name} );
                    else
                        tex_names.emplace( UniformName::Optimized_t{arg.name}, arg.samplerName );
                },
                [&] (ScriptVideoImagePtr video) {
                    tex_names.emplace( UniformName::Optimized_t{arg.name}, arg.samplerName );
                },
                [&] (ScriptRTScenePtr) {
                    rtas_names.insert( UniformName::Optimized_t{arg.name} );
                },
                [] (NullUnion) {
                    CHECK_THROW_MSG( false, "unsupported argument type" );
                }
            );
        }

        for (auto& ppln : inPipelines)
        {
            auto    ppln_layout = ppln->GetBase()->GetLayout();
            if ( not ppln_layout )
                continue;

            for (auto& [dsl, ds_name] : ppln_layout->Layouts())
            {
                if ( ds_name != req_ds_name )
                    continue;

                usize   tex_counter     = 0;
                usize   buf_counter     = 0;
                usize   texbuf_counter  = 0;
                usize   img_counter     = 0;
                usize   rtas_counter    = 0;

                for (auto& [un_name, un] : dsl->GetUniforms())
                {
                    // storage image
                    if ( un.type == EDescriptorType::StorageImage ) {
                        img_counter += usize(img_names.contains( un_name ));
                        continue;
                    }else{
                        CHECK_THROW_MSG( not img_names.contains( un_name ),
                            "Required storage image '"s << storage.GetName( un_name ) << "' declared with different type" );
                    }

                    // storage buffer
                    if ( un.type == EDescriptorType::StorageBuffer ) {
                        buf_counter += usize(buf_names.contains( un_name ));
                        continue;
                    }else{
                        CHECK_THROW_MSG( not buf_names.contains( un_name ),
                            "Required storage buffer '"s << storage.GetName( un_name ) << "' declared with different type" );
                    }

                    // storage texel buffer
                    if ( un.type == EDescriptorType::StorageTexelBuffer ) {
                        texbuf_counter += usize(texbuf_names.contains( un_name ));
                        continue;
                    }else{
                        CHECK_THROW_MSG( not texbuf_names.contains( un_name ),
                            "Required storage texel buffer '"s << storage.GetName( un_name ) << "' declared with different type" );
                    }

                    // ray tracing scene
                    if ( un.type == EDescriptorType::RayTracingScene ) {
                        rtas_counter += usize(rtas_names.contains( un_name ));
                        continue;
                    }else{
                        CHECK_THROW_MSG( not rtas_names.contains( un_name ),
                            "Required ray tracing scene '"s << storage.GetName( un_name ) << "' declared with different type" );
                    }

                    // texture
                    if ( un.type == EDescriptorType::CombinedImage_ImmutableSampler )
                    {
                        if ( auto it = tex_names.find( un_name );  it != tex_names.end() )
                        {
                            String  samp_name = storage.GetName(dsl->GetSampler( un ));
                            CHECK_THROW_MSG( it->second == samp_name,
                                "Texture '"s << storage.GetName( un_name ) << "' requires sampler '" << it->second << "' but declared sampler is '" << samp_name << "'" );
                            ++tex_counter;
                        }
                        continue;
                    }else{
                        CHECK_THROW_MSG( not tex_names.contains( un_name ),
                            "Required combined image '"s << storage.GetName( un_name ) << "' declared with different type" );
                    }
                }

                if ( tex_counter    == tex_names.size()     and
                     buf_counter    == buf_names.size()     and
                     texbuf_counter == texbuf_names.size()  and
                     img_counter    == img_names.size()     and
                     rtas_counter   == rtas_names.size() )
                {
                    out_pplns.push_back( ppln );
                    break;
                }
            }
        }

        if ( out_pplns.empty() )
        {
            String  str = "Can't find pipelines with DS '"s << dsName << "' and all resources:";
            for (auto& arg : args)
            {
                Visit( arg.res,
                    [&] (ScriptBufferPtr buf) {
                        if ( buf->HasLayout() )
                            str << "\n  StorageBuffer '" << arg.name << "'";
                        else
                            str << "\n  TexelStorageBuffer '" << arg.name << "'";
                    },
                    [&] (ScriptImagePtr) {
                        if ( arg.samplerName.empty() )
                            str << "\n  StorageImage '" << arg.name << "'";
                        else
                            str << "\n  CombinedImage '" << arg.name << "' with immutable sampler '" << arg.samplerName << "'";
                    },
                    [&] (ScriptVideoImagePtr video) {
                        str << "\n  CombinedImage '" << arg.name << "' with immutable sampler '" << arg.samplerName << "'";
                    },
                    [&] (ScriptRTScenePtr) {
                        str << "\n  RayTracingScene '" << arg.name << "'";
                    },
                    [] (NullUnion) {
                        CHECK_THROW_MSG( false, "unsupported argument type" );
                    }
                );
            }
            CHECK_THROW_MSG( false, str );
        }

        inPipelines = RVRef(out_pplns);
    }

/*
=================================================
    _FindPipelinesByMaterial
=================================================
*/
    template <typename PplnSpec>
    static void  _FindPipelinesByMaterial (StringView dsName, const ResLoader::IntermMaterial &mtr, INOUT Array<PplnSpec> &inPipelines) __Th___
    {
        using MtrTexture = ResLoader::IntermMaterial::MtrTexture;

        Array<PplnSpec>                         out_pplns;
        const DescriptorSetName                 req_ds_name {dsName};
        FlatHashSet<UniformName::Optimized_t>   tex_names;

        for (auto& param : mtr.GetSettings().GetParams())
        {
            if ( auto* tex = UnionGet<MtrTexture>( param ))
                tex_names.emplace( UniformName::Optimized_t{tex->name} );
        }

        for (auto& ppln : inPipelines)
        {
            auto    ppln_layout = ppln->GetBase()->GetLayout();
            if ( not ppln_layout )
                continue;

            for (auto& [dsl, ds_name] : ppln_layout->Layouts())
            {
                if ( ds_name != req_ds_name )
                    continue;

                usize   counter = 0;
                for (auto& [un_name, un] : dsl->GetUniforms())
                {
                    if ( un.type != EDescriptorType::CombinedImage_ImmutableSampler and
                         un.type != EDescriptorType::SampledImage )
                    {
                        CHECK( not tex_names.contains( un_name ));
                        continue;
                    }
                    counter += usize(tex_names.contains( un_name ));
                }

                if ( counter == tex_names.size() ) {
                    out_pplns.push_back( ppln );
                    break;
                }
            }
        }

        if ( out_pplns.empty() )
        {
            String  str = "Can't find pipelines with DS '"s << dsName << "' and all resources:";

            for (auto& param : mtr.GetSettings().GetParams())
            {
                if ( auto* tex = UnionGet<MtrTexture>( param ))
                    str << "\n  SampledImage '" << tex->name << "'";
            }

            CHECK_THROW_MSG( false, str );
        }

        inPipelines = RVRef(out_pplns);
    }

/*
=================================================
    _GetSuitablePipeline
=================================================
*/
    template <typename PplnSpec>
    ND_ static ScriptGeomSource::PipelineNames_t  _GetSuitablePipeline (Array<PplnSpec> &pipelines)
    {
        if ( pipelines.size() > 1 )
            AE_LOGI( "More than one pipeline are match the requirements" );

        if ( not pipelines.empty() )
            return ScriptGeomSource::PipelineNames_t{ pipelines.front()->Name() };

        return Default;
    }

/*
=================================================
    _GetSuitablePipelineAndDS
=================================================
*/
    template <typename PplnSpec>
    static void  _GetSuitablePipelineAndDS (Array<PplnSpec> &pipelines, StringView dsName,
                                            OUT ScriptGeomSource::PipelineNames_t &name, OUT DSLayoutName &dslName)
    {
        if ( pipelines.size() > 1 )
            AE_LOGI( "More than one pipeline are match the requirements" );

        if ( not pipelines.empty() )
        {
            name = ScriptGeomSource::PipelineNames_t{ pipelines.front()->Name() };

            auto    pl = pipelines.front()->GetBase()->GetLayout();
            CHECK_THROW( pl );

            const DescriptorSetName req_ds_name {dsName};
            dslName = Default;

            for (auto& [dsl, ds_name] : pl->Layouts())
            {
                if ( ds_name == req_ds_name )
                {
                    dslName = DSLayoutName{dsl->Name()};
                    return;
                }
            }
            CHECK_THROW_MSG( false, "Can't find DS with name '"s << dsName << "'" );
        }
    }

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ScriptGeomSource::ScriptGeomSource () :
        _args{ [this](ScriptPassArgs::Argument &arg) { _OnAddArg( arg ); }}
    {}

/*
=================================================
    Bind
=================================================
*/
    void  ScriptGeomSource::Bind (const ScriptEnginePtr &se) __Th___
    {
        using namespace Scripting;

        ClassBinder<ScriptGeomSource>   binder{ se };
        binder.CreateRef( 0, False{"no ctor"} );
    }

/*
=================================================
    _BindBase
=================================================
*/
    template <typename B>
    void  ScriptGeomSource::_BindBase (B &binder) __Th___
    {
        using T = typename B::Class_t;

        binder.Operators().ImplCast( &ScriptGeomSource_ToBase<T> );

        binder.Comment( "Add resource to all shaders in the current pass.\n"
                        "In - resource is used for read access.\n"
                        "Out - resource is used for write access.\n" );
        binder.AddMethod( &ScriptGeomSource::ArgSceneIn,        "ArgIn",    {"uniformName", "resource"} );

        binder.AddMethod( &ScriptGeomSource::ArgBufferIn,       "ArgIn",    {"uniformName", "resource"} );
        binder.AddMethod( &ScriptGeomSource::ArgBufferOut,      "ArgOut",   {"uniformName", "resource"} );
        binder.AddMethod( &ScriptGeomSource::ArgBufferInOut,    "ArgInOut", {"uniformName", "resource"} );

        binder.AddMethod( &ScriptGeomSource::ArgImageIn,        "ArgIn",    {"uniformName", "resource"} );
        binder.AddMethod( &ScriptGeomSource::ArgImageOut,       "ArgOut",   {"uniformName", "resource"} );
        binder.AddMethod( &ScriptGeomSource::ArgImageInOut,     "ArgInOut", {"uniformName", "resource"} );

        binder.AddMethod( &ScriptGeomSource::ArgTextureIn,      "ArgIn",    {"uniformName", "resource", "samplerName"} );
        binder.AddMethod( &ScriptGeomSource::ArgVideoIn,        "ArgIn",    {"uniformName", "resource", "samplerName"} );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _OnAddArg
=================================================
*/
    void  ScriptSphericalCube::_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th___
    {
        CHECK_THROW_MSG( not _geomSrc );
    }

/*
=================================================
    SetDetailLevel*
=================================================
*/
    void  ScriptSphericalCube::SetDetailLevel1 (uint maxLod) __Th___
    {
        return SetDetailLevel2( 0, maxLod );
    }

    void  ScriptSphericalCube::SetDetailLevel2 (uint minLod, uint maxLod) __Th___
    {
        CHECK_THROW_MSG( not _geomSrc );
        CHECK_THROW_MSG( _minLod == 0 and _maxLod == 0, "already specified" );

        _minLod = minLod;
        _maxLod = maxLod;
    }

/*
=================================================
    SetTessLevel*
=================================================
*/
    void  ScriptSphericalCube::SetTessLevel1 (float level) __Th___
    {
        return SetTessLevel2( ScriptDynamicFloatPtr{ new ScriptDynamicFloat{ new DynamicFloat{level} }});
    }

    void  ScriptSphericalCube::SetTessLevel2 (const ScriptDynamicFloatPtr &level) __Th___
    {
        CHECK_THROW_MSG( not _geomSrc );
        CHECK_THROW_MSG( level and level->Get() );
        CHECK_THROW_MSG( not _tessLevel, "already specified" );

        _tessLevel = level->Get();
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptSphericalCube::Bind (const ScriptEnginePtr &se) __Th___
    {
        using namespace Scripting;

        ClassBinder<ScriptSphericalCube>    binder{ se };
        binder.CreateRef();
        ScriptGeomSource::_BindBase( binder );

        binder.Comment( "Set detail level of the sphere.\n"
                        "Vertex count: (lod+2)^2, index count: 6*(lod+1)^2." );
        binder.AddMethod( &ScriptSphericalCube::SetDetailLevel1,    "DetailLevel",  {"maxLOD"} );
        binder.AddMethod( &ScriptSphericalCube::SetDetailLevel2,    "DetailLevel",  {"minLOD", "maxLOD"} );

        binder.Comment( "Set constant or dynamic tessellation level." );
        binder.AddMethod( &ScriptSphericalCube::SetTessLevel1,      "TessLevel",    {"level"} );
        binder.AddMethod( &ScriptSphericalCube::SetTessLevel2,      "TessLevel",    {"level"} );
    }

/*
=================================================
    ToGeomSource
=================================================
*/
    RC<IGeomSource>  ScriptSphericalCube::ToGeomSource () __Th___
    {
        if ( _geomSrc )
            return _geomSrc;

        Renderer&   renderer    = ScriptExe::ScriptResourceApi::GetRenderer();  // throw
        auto        result      = MakeRC<SphericalCube>( renderer, _minLod, _maxLod, _tessLevel );

        _args.InitResources( result->_resources );

        _geomSrc = result;
        return _geomSrc;
    }

/*
=================================================
    FindMaterialPipeline
=================================================
*/
    ScriptGeomSource::PipelineNames_t  ScriptSphericalCube::FindMaterialPipeline () C_Th___
    {
        Array<GraphicsPipelineSpecPtr>  pipelines;
        _FindPipelinesByVB( "VB{SphericalCubeVertex}", OUT pipelines );             // throw
        _FindPipelinesByUB( c_MtrDS, "SphericalCubeMaterialUB", INOUT pipelines );  // throw
        _FindPipelinesByResources( c_MtrDS, _args.Args(), INOUT pipelines );        // throw
        return _GetSuitablePipeline( pipelines );
    }

/*
=================================================
    ToMaterial
=================================================
*/
    RC<IGSMaterials>  ScriptSphericalCube::ToMaterial (RenderTechPipelinesPtr rtech, const PipelineNames_t &names) C_Th___
    {
        CHECK_THROW( _geomSrc );
        CHECK_THROW( rtech );
        CHECK_THROW( names.size() == 1 );

        auto        result      = MakeRC<SphericalCube::Material>();
        auto&       res_mngr    = RenderTaskScheduler().GetResourceManager();
        Renderer&   renderer    = ScriptExe::ScriptResourceApi::GetRenderer();  // throw

        auto    ppln = rtech->GetGraphicsPipeline( names[0] );
        CHECK_THROW( ppln );

        result->rtech   = rtech;
        result->ppln    = ppln;

        CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->mtrDSIndex, OUT result->descSets.data(), result->descSets.size(),
                                                    ppln, DescriptorSetName{c_MtrDS} ));

        result->passDSIndex = GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_PassDS} );

        result->ubuffer = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ShaderTypes::SphericalCubeMaterialUB>, EBufferUsage::Uniform | EBufferUsage::TransferDst },
                                                 "SphericalCubeMaterialUB", renderer.GetAllocator() );
        CHECK_THROW( result->ubuffer );

        return result;
    }

/*
=================================================
    _CreateUBType
=================================================
*/
    auto  ScriptSphericalCube::_CreateUBType () __Th___
    {
        auto&   obj_storage = *ObjectStorage::Instance();
        auto    it          = obj_storage.structTypes.find( "SphericalCubeMaterialUB" );

        if ( it != obj_storage.structTypes.end() )
            return it->second;

        ShaderStructTypePtr st{ new ShaderStructType{"SphericalCubeMaterialUB"}};
        st->Set( EStructLayout::Std140, R"#(
                float4x4    transform;
                float       tessLevel;
            )#");

        return st;
    }

/*
=================================================
    GetShaderTypes
=================================================
*/
    void  ScriptSphericalCube::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
    {
        auto    st = _CreateUBType();   // throw

        CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
    }
//-----------------------------------------------------------------------------



namespace
{
/*
=================================================
    DrawCmd_SetIndexBuffer*
=================================================
*/
    template <typename DrawCmd>
    void  DrawCmd_SetIndexBuffer2 (DrawCmd &cmd, EIndex type, const ScriptBufferPtr &ibuf, ulong offset) __Th___
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not cmd._indexBuffer );
        CHECK_THROW_MSG( type == EIndex::UShort or type == EIndex::UInt );

        ibuf->AddUsage( EResourceUsage::VertexInput );

        cmd._indexType          = type;
        cmd._indexBuffer        = ibuf;
        cmd._indexBufferOffset  = offset;
        cmd._indexBufferField   = "";
    }

    template <typename DrawCmd>
    void  DrawCmd_SetIndexBuffer1 (DrawCmd &cmd, EIndex type, const ScriptBufferPtr &ibuf) __Th___
    {
        DrawCmd_SetIndexBuffer2( cmd, type, ibuf, 0 );
    }

    template <typename DrawCmd>
    void  DrawCmd_SetIndexBuffer3 (DrawCmd &cmd, const ScriptBufferPtr &ibuf, const String &field) __Th___
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not cmd._indexBuffer );
        CHECK_THROW_MSG( not field.empty() );

        ibuf->AddUsage( EResourceUsage::VertexInput );

        cmd._indexType          = Default;
        cmd._indexBuffer        = ibuf;
        cmd._indexBufferOffset  = 0;
        cmd._indexBufferField   = field;
    }

/*
=================================================
    DrawCmd_GetIndexBufferOffset
=================================================
*/
    template <typename DrawCmd>
    ND_ Bytes  DrawCmd_GetIndexBufferOffset (DrawCmd &cmd) __Th___
    {
        CHECK_THROW_MSG( cmd._indexBuffer );

        if ( not cmd._indexBufferField.empty() )
        {
            CHECK_THROW_MSG( cmd._indexBufferOffset == 0 );
            cmd._indexBuffer->AddLayoutReflection();
            return cmd._indexBuffer->GetFieldOffset( cmd._indexBufferField );
        }
        else
            return Bytes{cmd._indexBufferOffset};
    }

/*
=================================================
    DrawCmd_GetIndexBufferType
=================================================
*/
    template <typename DrawCmd>
    ND_ EIndex  DrawCmd_GetIndexBufferType  (DrawCmd &cmd) __Th___
    {
        using namespace AE::PipelineCompiler;
        CHECK_THROW_MSG( cmd._indexBuffer );

        if ( not cmd._indexBufferField.empty() )
        {
            CHECK_THROW_MSG( cmd._indexType == Default );
            cmd._indexBuffer->AddLayoutReflection();

            auto*   field = cmd._indexBuffer->GetField( cmd._indexBufferField ).GetIf< ShaderStructType::Field >();
            CHECK_THROW_MSG( field != null );
            CHECK_THROW_MSG( field->IsArray() );
            CHECK_THROW_MSG( field->IsScalar() or field->IsVec() );
            CHECK_THROW_MSG( AnyEqual( field->rows, 1, 2, 4 ));

            switch ( field->type ) {
                case EValueType::UInt16 :   return EIndex::UShort;
                case EValueType::UInt32 :   return EIndex::UInt;
            }

            CHECK_THROW_MSG( false,
                "IndexBuffer '"s << cmd._indexBuffer->GetName() << "' field '" << cmd._indexBufferField << 
                "' must be array of scalar/vec1/vec2/vec4 with uint16/uint32 type" );
        }
        else
            return cmd._indexType;
    }

/*
=================================================
    DrawCmd_SetIndirectBuffer*
=================================================
*/
    template <typename DrawCmd>
    void  DrawCmd_SetIndirectBuffer2 (DrawCmd &cmd, const ScriptBufferPtr &ibuf, ulong offset) __Th___
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not cmd._indirectBuffer );
        cmd._indirectBuffer         = ibuf;
        cmd._indirectBufferOffset   = offset;
    }

    template <typename DrawCmd>
    void  DrawCmd_SetIndirectBuffer1 (DrawCmd &cmd, const ScriptBufferPtr &ibuf) __Th___
    {
        DrawCmd_SetIndirectBuffer2( cmd, ibuf, 0 );
    }

    template <typename DrawCmd>
    void  DrawCmd_SetIndirectBuffer3 (DrawCmd &cmd, const ScriptBufferPtr &ibuf, const String &field) __Th___
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not cmd._indirectBuffer );
        CHECK_THROW_MSG( not field.empty() );
        cmd._indirectBuffer         = ibuf;
        cmd._indirectBufferOffset   = 0;
        cmd._indirectBufferField    = field;
    }

/*
=================================================
    DrawCmd_GetIndirectBufferOffset
=================================================
*/
    template <typename DrawCmd>
    ND_ Bytes  DrawCmd_GetIndirectBufferOffset (DrawCmd &cmd, StringView cmdName) __Th___
    {
        CHECK_THROW_MSG( cmd._indirectBuffer );

        if ( not cmd._indirectBufferField.empty() )
        {
            CHECK_THROW_MSG( cmd._indirectBufferOffset == 0 );
            cmd._indirectBuffer->AddLayoutReflection();

            CHECK_THROW_MSG( cmd._indirectBuffer->GetFieldStructName( cmd._indirectBufferField ) == cmdName,
                "Buffer '"s << cmd._indirectBuffer->GetName() << "' field '" << cmd._indirectBufferField <<
                "' must have '" << cmdName << "' type to use it as IndirectBuffer" );

            return cmd._indirectBuffer->GetFieldOffset( cmd._indirectBufferField );
        }
        else
            return Bytes{cmd._indirectBufferOffset};
    }

/*
=================================================
    DrawCmd_SetCountBuffer*
=================================================
*/
    template <typename DrawCmd>
    void  DrawCmd_SetCountBuffer2 (DrawCmd &cmd, const ScriptBufferPtr &cbuf, ulong offset) __Th___
    {
        CHECK_THROW_MSG( cbuf );
        CHECK_THROW_MSG( not cmd._countBuffer );
        cmd._countBuffer        = cbuf;
        cmd._countBufferOffset  = offset;
    }

    template <typename DrawCmd>
    void  DrawCmd_SetCountBuffer1 (DrawCmd &cmd, const ScriptBufferPtr &cbuf) __Th___
    {
        DrawCmd_SetCountBuffer2( cmd, cbuf, 0 );
    }

    template <typename DrawCmd>
    void  DrawCmd_SetCountBuffer3 (DrawCmd &cmd, const ScriptBufferPtr &ibuf, const String &field) __Th___
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not cmd._countBuffer );
        CHECK_THROW_MSG( not field.empty() );
        cmd._countBuffer        = ibuf;
        cmd._countBufferOffset  = 0;
        cmd._countBufferField   = field;
    }

/*
=================================================
    DrawCmd_GetCountBufferOffset
=================================================
*/
    template <typename DrawCmd>
    ND_ Bytes  DrawCmd_GetCountBufferOffset (DrawCmd &cmd) __Th___
    {
        CHECK_THROW_MSG( cmd._countBuffer );

        if ( not cmd._countBufferField.empty() )
        {
            CHECK_THROW_MSG( cmd._countBufferOffset == 0 );
            cmd._countBuffer->AddLayoutReflection();

            CHECK_THROW_MSG( cmd._countBuffer->GetFieldType( cmd._countBufferField ) == uint(PipelineCompiler::EValueType::UInt32),
                "Buffer '"s << cmd._countBuffer->GetName() << "' field '" << cmd._countBufferField <<
                "' must have 'Uint32' type to use it as CountBuffer" );

            return cmd._countBuffer->GetFieldOffset( cmd._countBufferField );
        }
        else
            return Bytes{cmd._countBufferOffset};
    }


} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
    DrawCmd3::SetDyn*
=================================================
*/
    void  ScriptUniGeometry::DrawCmd3::SetDynVertexCount (const ScriptDynamicUIntPtr &ptr)
    {
        CHECK_THROW_MSG( ptr );
        CHECK_THROW_MSG( not dynVertexCount );
        dynVertexCount = ptr;
    }

    void  ScriptUniGeometry::DrawCmd3::SetDynInstanceCount (const ScriptDynamicUIntPtr &ptr)
    {
        CHECK_THROW_MSG( ptr );
        CHECK_THROW_MSG( not dynInstanceCount );
        dynInstanceCount = ptr;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    DrawIndexedCmd3::SetDyn*
=================================================
*/
    void  ScriptUniGeometry::DrawIndexedCmd3::SetDynIndexCount (const ScriptDynamicUIntPtr &ptr)
    {
        CHECK_THROW_MSG( ptr );
        CHECK_THROW_MSG( not dynIndexCount );
        dynIndexCount = ptr;
    }

    void  ScriptUniGeometry::DrawIndexedCmd3::SetDynInstanceCount (const ScriptDynamicUIntPtr &ptr)
    {
        CHECK_THROW_MSG( ptr );
        CHECK_THROW_MSG( not dynInstanceCount );
        dynInstanceCount = ptr;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    DrawIndirectCmd3::SetDyn*
=================================================
*/
    void  ScriptUniGeometry::DrawIndirectCmd3::SetDynDrawCount (const ScriptDynamicUIntPtr &ptr)
    {
        CHECK_THROW_MSG( ptr );
        CHECK_THROW_MSG( not dynDrawCount );
        dynDrawCount    = ptr;
        drawCount       = 0;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    DrawIndexedIndirectCmd3::SetDyn*
=================================================
*/
    void  ScriptUniGeometry::DrawIndexedIndirectCmd3::SetDynDrawCount (const ScriptDynamicUIntPtr &ptr)
    {
        CHECK_THROW_MSG( ptr );
        CHECK_THROW_MSG( not dynDrawCount );
        dynDrawCount    = ptr;
        drawCount       = 0;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    DrawMeshTasksCmd3::SetDynTaskCount
=================================================
*/
    void  ScriptUniGeometry::DrawMeshTasksCmd3::SetDynTaskCount (const ScriptDynamicUInt3Ptr &ptr)
    {
        CHECK_THROW_MSG( ptr );
        CHECK_THROW_MSG( not dynTaskCount );
        dynTaskCount = ptr;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    DrawMeshTasksIndirectCmd3::SetDyn*
=================================================
*/
    void  ScriptUniGeometry::DrawMeshTasksIndirectCmd3::SetDynDrawCount (const ScriptDynamicUIntPtr &ptr)
    {
        CHECK_THROW_MSG( ptr );
        CHECK_THROW_MSG( not dynDrawCount );
        dynDrawCount    = ptr;
        drawCount       = 0;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    DrawIndirectCountCmd3::SetDyn*
=================================================
*/
    void  ScriptUniGeometry::DrawIndirectCountCmd3::SetDynMaxDrawCount (const ScriptDynamicUIntPtr &ptr)
    {
        CHECK_THROW_MSG( ptr );
        CHECK_THROW_MSG( not dynMaxDrawCount );
        dynMaxDrawCount = ptr;
        maxDrawCount    = 0;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    DrawIndexedIndirectCountCmd3::SetDyn*
=================================================
*/
    void  ScriptUniGeometry::DrawIndexedIndirectCountCmd3::SetDynMaxDrawCount (const ScriptDynamicUIntPtr &ptr)
    {
        CHECK_THROW_MSG( ptr );
        CHECK_THROW_MSG( not dynMaxDrawCount );
        dynMaxDrawCount = ptr;
        maxDrawCount    = 0;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    DrawMeshTasksIndirectCountCmd3::SetDyn*
=================================================
*/
    void  ScriptUniGeometry::DrawMeshTasksIndirectCountCmd3::SetDynMaxDrawCount (const ScriptDynamicUIntPtr &ptr)
    {
        CHECK_THROW_MSG( ptr );
        CHECK_THROW_MSG( not dynMaxDrawCount );
        dynMaxDrawCount = ptr;
        maxDrawCount    = 0;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Draw*
=================================================
*/
    void  ScriptUniGeometry::Draw1 (const DrawCmd3 &cmd)
    {
        CHECK_THROW_MSG( not _geomSrc );

        CHECK_THROW_MSG( cmd.vertexCount > 0 or cmd.dynVertexCount );
        CHECK_THROW_MSG( cmd.instanceCount > 0 or cmd.dynInstanceCount );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw2 (const DrawIndexedCmd3 &cmd)
    {
        CHECK_THROW_MSG( not _geomSrc );

        CHECK_THROW_MSG( cmd._indexBuffer );
        CHECK_THROW_MSG( cmd.indexCount > 0 );
        CHECK_THROW_MSG( cmd.instanceCount > 0 );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw3 (const DrawIndirectCmd3 &cmd)
    {
        CHECK_THROW_MSG( not _geomSrc );

        CHECK_THROW_MSG( cmd._indirectBuffer );
        CHECK_THROW_MSG( cmd.drawCount > 0 or cmd.dynDrawCount );
        CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawIndirectCommand)) );
        CHECK_THROW_MSG( IsAligned( cmd.stride, 4 ), "Stride must be multiple of 4" );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw4 (const DrawIndexedIndirectCmd3 &cmd)
    {
        CHECK_THROW_MSG( not _geomSrc );

        CHECK_THROW_MSG( cmd._indexBuffer );
        CHECK_THROW_MSG( cmd._indirectBuffer );
        CHECK_THROW_MSG( cmd.drawCount > 0 or cmd.dynDrawCount );
        CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawIndexedIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawIndexedIndirectCommand)) );
        CHECK_THROW_MSG( IsAligned( cmd.stride, 4 ), "Stride must be multiple of 4" );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw5 (const DrawMeshTasksCmd3 &cmd)
    {
        CHECK_THROW_MSG( not _geomSrc );

        auto&   fs = RenderTaskScheduler().GetFeatureSet();
        CHECK_THROW_MSG( fs.meshShader == EFeature::RequireTrue );

        CHECK_THROW_MSG( All( uint3{cmd.taskCount} > uint3{0} ));

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw6 (const DrawMeshTasksIndirectCmd3 &cmd)
    {
        CHECK_THROW_MSG( not _geomSrc );

        auto&   fs = RenderTaskScheduler().GetFeatureSet();
        CHECK_THROW_MSG( fs.meshShader == EFeature::RequireTrue );

        CHECK_THROW_MSG( cmd._indirectBuffer );
        CHECK_THROW_MSG( cmd.drawCount > 0 or cmd.dynDrawCount );
        CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawMeshTasksIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawMeshTasksIndirectCommand)) );
        CHECK_THROW_MSG( IsAligned( cmd.stride, 4 ), "Stride must be multiple of 4" );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw7 (const DrawIndirectCountCmd3 &cmd)
    {
        CHECK_THROW_MSG( not _geomSrc );

        auto&   fs = RenderTaskScheduler().GetFeatureSet();
        CHECK_THROW_MSG( fs.drawIndirectCount == EFeature::RequireTrue );

        CHECK_THROW_MSG( cmd._indirectBuffer );
        CHECK_THROW_MSG( cmd._countBuffer );
        CHECK_THROW_MSG( cmd.maxDrawCount > 0 or cmd.dynMaxDrawCount );
        CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawIndirectCommand)) );
        CHECK_THROW_MSG( IsAligned( cmd.stride, 4 ), "Stride must be multiple of 4" );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw8 (const DrawIndexedIndirectCountCmd3 &cmd)
    {
        CHECK_THROW_MSG( not _geomSrc );

        auto&   fs = RenderTaskScheduler().GetFeatureSet();
        CHECK_THROW_MSG( fs.drawIndirectCount == EFeature::RequireTrue );

        CHECK_THROW_MSG( cmd._indexBuffer );
        CHECK_THROW_MSG( cmd._indirectBuffer );
        CHECK_THROW_MSG( cmd._countBuffer );
        CHECK_THROW_MSG( cmd.maxDrawCount > 0 or cmd.dynMaxDrawCount );
        CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawIndexedIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawIndexedIndirectCommand)) );
        CHECK_THROW_MSG( IsAligned( cmd.stride, 4 ), "Stride must be multiple of 4" );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw9 (const DrawMeshTasksIndirectCountCmd3 &cmd)
    {
        CHECK_THROW_MSG( not _geomSrc );

        auto&   fs = RenderTaskScheduler().GetFeatureSet();
        CHECK_THROW_MSG( fs.drawIndirectCount == EFeature::RequireTrue and fs.meshShader == EFeature::RequireTrue );

        CHECK_THROW_MSG( cmd._indirectBuffer );
        CHECK_THROW_MSG( cmd._countBuffer );
        CHECK_THROW_MSG( cmd.maxDrawCount > 0 or cmd.dynMaxDrawCount );
        CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawMeshTasksIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawMeshTasksIndirectCommand)) );
        CHECK_THROW_MSG( IsAligned( cmd.stride, 4 ), "Stride must be multiple of 4" );

        _drawCommands.push_back( cmd );
    }

/*
=================================================
    _OnAddArg
=================================================
*/
    void  ScriptUniGeometry::_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th___
    {
        CHECK_THROW_MSG( not _geomSrc );
    }

/*
=================================================
    Clone
=================================================
*/
    ScriptUniGeometry*  ScriptUniGeometry::Clone () C_Th___
    {
        ScriptRC<ScriptUniGeometry> result{ new ScriptUniGeometry{} };

        result->_args.CopyFrom( this->_args );
        result->_drawCommands   = this->_drawCommands;

        return result.Detach();
    }

/*
=================================================
    Bind*
=================================================
*/
    void  ScriptUniGeometry::DrawCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawCmd3>    binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawCmd3::SetDynVertexCount,                             "VertexCount",      {} );
        binder.AddMethod( &DrawCmd3::SetDynInstanceCount,                           "InstanceCount",    {} );
        binder.AddProperty( &DrawCmd3::vertexCount,                                 "vertexCount"       );
        binder.AddProperty( &DrawCmd3::instanceCount,                               "instanceCount"     );
        binder.AddProperty( &DrawCmd3::firstVertex,                                 "firstVertex"       );
        binder.AddProperty( &DrawCmd3::firstInstance,                               "firstInstance"     );
    }

    void  ScriptUniGeometry::DrawIndexedCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawIndexedCmd3> binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawIndexedCmd3::SetDynIndexCount,                       "IndexCount",       {} );
        binder.AddMethod( &DrawIndexedCmd3::SetDynInstanceCount,                    "InstanceCount",    {} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer1<DrawIndexedCmd3>,      "IndexBuffer",      {"type", "buffer"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer2<DrawIndexedCmd3>,      "IndexBuffer",      {"type", "buffer", "offset"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer3<DrawIndexedCmd3>,      "IndexBuffer",      {"buffer", "field"} );
        binder.AddProperty( &DrawIndexedCmd3::indexCount,                           "indexCount"        );
        binder.AddProperty( &DrawIndexedCmd3::instanceCount,                        "instanceCount"     );
        binder.AddProperty( &DrawIndexedCmd3::firstIndex,                           "firstIndex"        );
        binder.AddProperty( &DrawIndexedCmd3::vertexOffset,                         "vertexOffset"      );
        binder.AddProperty( &DrawIndexedCmd3::firstInstance,                        "firstInstance"     );
    }

    void  ScriptUniGeometry::DrawIndirectCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawIndirectCmd3>    binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawIndirectCmd3::SetDynDrawCount,                       "DrawCount",        {} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer1<DrawIndirectCmd3>,  "IndirectBuffer",   {"buffer"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer2<DrawIndirectCmd3>,  "IndirectBuffer",   {"buffer", "offset"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer3<DrawIndirectCmd3>,  "IndirectBuffer",   {"buffer", "field"} );
        binder.AddProperty( &DrawIndirectCmd3::drawCount,                           "drawCount"         );
        binder.AddProperty( &DrawIndirectCmd3::stride,                              "stride"            );
    }

    void  ScriptUniGeometry::DrawIndexedIndirectCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawIndexedIndirectCmd3> binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawIndexedIndirectCmd3::SetDynDrawCount,                        "DrawCount",        {} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer1<DrawIndexedIndirectCmd3>,      "IndexBuffer",      {"type", "buffer"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer2<DrawIndexedIndirectCmd3>,      "IndexBuffer",      {"type", "buffer", "offset"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer3<DrawIndexedIndirectCmd3>,      "IndexBuffer",      {"buffer", "field"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer1<DrawIndexedIndirectCmd3>,   "IndirectBuffer",   {"buffer"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer2<DrawIndexedIndirectCmd3>,   "IndirectBuffer",   {"buffer", "offset"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer3<DrawIndexedIndirectCmd3>,   "IndirectBuffer",   {"buffer", "field"} );
        binder.AddProperty( &DrawIndexedIndirectCmd3::drawCount,                            "drawCount"         );
        binder.AddProperty( &DrawIndexedIndirectCmd3::stride,                               "stride"            );
    }

    void  ScriptUniGeometry::DrawMeshTasksCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawMeshTasksCmd3>   binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawMeshTasksCmd3::SetDynTaskCount,                              "TaskCount",        {} );
        binder.AddProperty( &DrawMeshTasksCmd3::taskCount,                                  "taskCount"         );
    }

    void  ScriptUniGeometry::DrawMeshTasksIndirectCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawMeshTasksIndirectCmd3>   binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawMeshTasksIndirectCmd3::SetDynDrawCount,                      "DrawCount",        {} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer1<DrawMeshTasksIndirectCmd3>, "IndirectBuffer",   {"buffer"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer2<DrawMeshTasksIndirectCmd3>, "IndirectBuffer",   {"buffer", "offset"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer3<DrawMeshTasksIndirectCmd3>, "IndirectBuffer",   {"buffer", "field"} );
        binder.AddProperty( &DrawMeshTasksIndirectCmd3::drawCount,                          "drawCount"         );
        binder.AddProperty( &DrawMeshTasksIndirectCmd3::stride,                             "stride"            );
    }

    void  ScriptUniGeometry::DrawIndirectCountCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawIndirectCountCmd3>   binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawIndirectCountCmd3::SetDynMaxDrawCount,                       "MaxDrawCount",     {} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer1<DrawIndirectCountCmd3>,     "IndirectBuffer",   {"buffer"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer2<DrawIndirectCountCmd3>,     "IndirectBuffer",   {"buffer", "offset"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer3<DrawIndirectCountCmd3>,     "IndirectBuffer",   {"buffer", "field"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer1<DrawIndirectCountCmd3>,        "CountBuffer",      {"buffer"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer2<DrawIndirectCountCmd3>,        "CountBuffer",      {"buffer", "offset"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer3<DrawIndirectCountCmd3>,        "CountBuffer",      {"buffer", "field"} );
        binder.AddProperty( &DrawIndirectCountCmd3::maxDrawCount,                           "maxDrawCount"      );
        binder.AddProperty( &DrawIndirectCountCmd3::stride,                                 "stride"            );
    }

    void  ScriptUniGeometry::DrawIndexedIndirectCountCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawIndexedIndirectCountCmd3>    binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawIndexedIndirectCountCmd3::SetDynMaxDrawCount,                    "MaxDrawCount",     {} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer1<DrawIndexedIndirectCountCmd3>,     "IndexBuffer",      {"type", "buffer"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer2<DrawIndexedIndirectCountCmd3>,     "IndexBuffer",      {"type", "buffer", "offset"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndexBuffer3<DrawIndexedIndirectCountCmd3>,     "IndexBuffer",      {"buffer", "field"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer1<DrawIndexedIndirectCountCmd3>,  "IndirectBuffer",   {"buffer"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer2<DrawIndexedIndirectCountCmd3>,  "IndirectBuffer",   {"buffer", "offset"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer3<DrawIndexedIndirectCountCmd3>,  "IndirectBuffer",   {"buffer", "field"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer1<DrawIndexedIndirectCountCmd3>,     "CountBuffer",      {"buffer"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer2<DrawIndexedIndirectCountCmd3>,     "CountBuffer",      {"buffer", "offset"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer3<DrawIndexedIndirectCountCmd3>,     "CountBuffer",      {"buffer", "field"} );
        binder.AddProperty( &DrawIndexedIndirectCountCmd3::maxDrawCount,                        "maxDrawCount"      );
        binder.AddProperty( &DrawIndexedIndirectCountCmd3::stride,                              "stride"            );
    }

    void  ScriptUniGeometry::DrawMeshTasksIndirectCountCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawMeshTasksIndirectCountCmd3>  binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawMeshTasksIndirectCountCmd3::SetDynMaxDrawCount,                      "MaxDrawCount",     {} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer1<DrawMeshTasksIndirectCountCmd3>,    "IndirectBuffer",   {"buffer"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer2<DrawMeshTasksIndirectCountCmd3>,    "IndirectBuffer",   {"buffer", "offset"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetIndirectBuffer3<DrawMeshTasksIndirectCountCmd3>,    "IndirectBuffer",   {"buffer", "field"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer1<DrawMeshTasksIndirectCountCmd3>,       "CountBuffer",      {"buffer"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer2<DrawMeshTasksIndirectCountCmd3>,       "CountBuffer",      {"buffer", "offset"} );
        binder.AddMethodFromGlobal( &DrawCmd_SetCountBuffer3<DrawMeshTasksIndirectCountCmd3>,       "CountBuffer",      {"buffer", "field"} );
        binder.AddProperty( &DrawMeshTasksIndirectCountCmd3::maxDrawCount,                          "maxDrawCount"      );
        binder.AddProperty( &DrawMeshTasksIndirectCountCmd3::stride,                                "stride"            );
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptUniGeometry::Bind (const ScriptEnginePtr &se) __Th___
    {
        DrawCmd3::Bind( se );
        DrawIndexedCmd3::Bind( se );
        DrawIndirectCmd3::Bind( se );
        DrawIndexedIndirectCmd3::Bind( se );
        DrawMeshTasksCmd3::Bind( se );
        DrawMeshTasksIndirectCmd3::Bind( se );
        DrawIndirectCountCmd3::Bind( se );
        DrawIndexedIndirectCountCmd3::Bind( se );
        DrawMeshTasksIndirectCountCmd3::Bind( se );

        Scripting::ClassBinder<ScriptUniGeometry>   binder{ se };
        binder.CreateRef();
        ScriptGeomSource::_BindBase( binder );

        binder.AddMethod( &ScriptUniGeometry::Draw1,    "Draw", {} );
        binder.AddMethod( &ScriptUniGeometry::Draw2,    "Draw", {} );
        binder.AddMethod( &ScriptUniGeometry::Draw3,    "Draw", {} );
        binder.AddMethod( &ScriptUniGeometry::Draw4,    "Draw", {} );
        binder.AddMethod( &ScriptUniGeometry::Draw5,    "Draw", {} );
        binder.AddMethod( &ScriptUniGeometry::Draw6,    "Draw", {} );
        binder.AddMethod( &ScriptUniGeometry::Draw7,    "Draw", {} );
        binder.AddMethod( &ScriptUniGeometry::Draw8,    "Draw", {} );
        binder.AddMethod( &ScriptUniGeometry::Draw9,    "Draw", {} );

        binder.AddMethod( &ScriptUniGeometry::Clone,    "Clone", {} );
    }

/*
=================================================
    _CreateUBType
=================================================
*/
    auto  ScriptUniGeometry::_CreateUBType () __Th___
    {
        auto&   obj_storage = *ObjectStorage::Instance();
        auto    it          = obj_storage.structTypes.find( "UnifiedGeometryMaterialUB" );

        if ( it != obj_storage.structTypes.end() )
            return it->second;

        ShaderStructTypePtr st{ new ShaderStructType{"UnifiedGeometryMaterialUB"}};
        st->Set( EStructLayout::Std140, R"#(
                float4x4    transform;
            )#");

        return st;
    }

/*
=================================================
    GetShaderTypes
=================================================
*/
    void  ScriptUniGeometry::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
    {
        auto    st = _CreateUBType();   // throw

        CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
    }

/*
=================================================
    ToGeomSource
=================================================
*/
    RC<IGeomSource>  ScriptUniGeometry::ToGeomSource () __Th___
    {
        if ( _geomSrc )
            return _geomSrc;

        CHECK_THROW_MSG( not _drawCommands.empty() );

        Renderer&   renderer    = ScriptExe::ScriptResourceApi::GetRenderer();  // throw
        auto        result      = MakeRC<UnifiedGeometry>( renderer );

        for (auto& src : _drawCommands)
        {
            auto&   dst = result->_drawCommands.emplace_back();
            Visit( src,
                [&] (const DrawCmd3 &src) {
                    UnifiedGeometry::DrawCmd2   cmd;
                    cmd.dynVertexCount      = src.dynVertexCount ? src.dynVertexCount->Get() : null;
                    cmd.dynInstanceCount    = src.dynInstanceCount ? src.dynInstanceCount->Get() : null;
                    cmd.vertexCount         = src.vertexCount;
                    cmd.instanceCount       = src.instanceCount;
                    cmd.firstVertex         = src.firstVertex;
                    cmd.firstInstance       = src.firstInstance;
                    dst = cmd;
                },

                [&] (const DrawIndexedCmd3 &src) {
                    UnifiedGeometry::DrawIndexedCmd2    cmd;
                    cmd.indexType           = DrawCmd_GetIndexBufferType( src );
                    cmd.indexBufferPtr      = src._indexBuffer->ToResource();       CHECK_THROW( cmd.indexBufferPtr );
                    cmd.indexBufferOffset   = DrawCmd_GetIndexBufferOffset( src );
                    cmd.dynIndexCount       = src.dynIndexCount ? src.dynIndexCount->Get() : null;
                    cmd.dynInstanceCount    = src.dynInstanceCount ? src.dynInstanceCount->Get() : null;
                    cmd.indexCount          = src.indexCount;
                    cmd.instanceCount       = src.instanceCount;
                    cmd.firstIndex          = src.firstIndex;
                    cmd.vertexOffset        = src.vertexOffset;
                    cmd.firstInstance       = src.firstInstance;
                    dst = cmd;
                },

                [&] (const DrawMeshTasksCmd3 &src) {
                    UnifiedGeometry::DrawMeshTasksCmd2  cmd;
                    cmd.dynTaskCount        = src.dynTaskCount ? src.dynTaskCount->Get() : null;
                    cmd.taskCount           = uint3{ src.taskCount };
                    dst = cmd;
                },

                [&] (const DrawIndirectCmd3 &src) {
                    UnifiedGeometry::DrawIndirectCmd2   cmd;
                    cmd.indirectBufferPtr   = src._indirectBuffer->ToResource();    CHECK_THROW( cmd.indirectBufferPtr );
                    cmd.indirectBufferOffset= DrawCmd_GetIndirectBufferOffset( src, "DrawIndirectCommand" );
                    cmd.drawCount           = src.drawCount;
                    cmd.dynDrawCount        = src.dynDrawCount ? src.dynDrawCount->Get() : null;
                    cmd.stride              = Bytes{src.stride};
                    dst = cmd;
                },

                [&] (const DrawIndexedIndirectCmd3 &src) {
                    UnifiedGeometry::DrawIndexedIndirectCmd2    cmd;
                    cmd.indexType           = DrawCmd_GetIndexBufferType( src );
                    cmd.indexBufferPtr      = src._indexBuffer->ToResource();       CHECK_THROW( cmd.indexBufferPtr );
                    cmd.indexBufferOffset   = DrawCmd_GetIndexBufferOffset( src );
                    cmd.indirectBufferPtr   = src._indirectBuffer->ToResource();    CHECK_THROW( cmd.indirectBufferPtr );
                    cmd.indirectBufferOffset= DrawCmd_GetIndirectBufferOffset( src, "DrawIndexedIndirectCommand" );
                    cmd.drawCount           = src.drawCount;
                    cmd.dynDrawCount        = src.dynDrawCount ? src.dynDrawCount->Get() : null;
                    cmd.stride              = Bytes{src.stride};
                    dst = cmd;
                },

                [&] (const DrawMeshTasksIndirectCmd3 &src) {
                    UnifiedGeometry::DrawMeshTasksIndirectCmd2  cmd;
                    cmd.indirectBufferPtr   = src._indirectBuffer->ToResource();    CHECK_THROW( cmd.indirectBufferPtr );
                    cmd.indirectBufferOffset= DrawCmd_GetIndirectBufferOffset( src, "DrawMeshTasksIndirectCommand" );
                    cmd.drawCount           = src.drawCount;
                    cmd.dynDrawCount        = src.dynDrawCount ? src.dynDrawCount->Get() : null;
                    cmd.stride              = Bytes{src.stride};
                    dst = cmd;
                },

                [&] (const DrawIndirectCountCmd3 &src) {
                    UnifiedGeometry::DrawIndirectCountCmd2  cmd;
                    cmd.indirectBufferPtr   = src._indirectBuffer->ToResource();    CHECK_THROW( cmd.indirectBufferPtr );
                    cmd.indirectBufferOffset= DrawCmd_GetIndirectBufferOffset( src, "DrawIndirectCommand" );
                    cmd.countBufferPtr      = src._countBuffer->ToResource();       CHECK_THROW( cmd.countBufferPtr );
                    cmd.countBufferOffset   = DrawCmd_GetCountBufferOffset( src );
                    cmd.maxDrawCount        = src.maxDrawCount;
                    cmd.dynMaxDrawCount     = src.dynMaxDrawCount ? src.dynMaxDrawCount->Get() : null;
                    cmd.stride              = Bytes{src.stride};
                    dst = cmd;
                },

                [&] (const DrawIndexedIndirectCountCmd3 &src) {
                    UnifiedGeometry::DrawIndexedIndirectCountCmd2   cmd;
                    cmd.indexType           = DrawCmd_GetIndexBufferType( src );
                    cmd.indexBufferPtr      = src._indexBuffer->ToResource();       CHECK_THROW( cmd.indexBufferPtr );
                    cmd.indexBufferOffset   = DrawCmd_GetIndexBufferOffset( src );
                    cmd.indirectBufferPtr   = src._indirectBuffer->ToResource();    CHECK_THROW( cmd.indirectBufferPtr );
                    cmd.indirectBufferOffset= DrawCmd_GetIndirectBufferOffset( src, "DrawIndexedIndirectCommand" );
                    cmd.countBufferPtr      = src._countBuffer->ToResource();       CHECK_THROW( cmd.countBufferPtr );
                    cmd.countBufferOffset   = DrawCmd_GetCountBufferOffset( src );
                    cmd.maxDrawCount        = src.maxDrawCount;
                    cmd.dynMaxDrawCount     = src.dynMaxDrawCount ? src.dynMaxDrawCount->Get() : null;
                    cmd.stride              = Bytes{src.stride};
                    dst = cmd;
                },

                [&] (const DrawMeshTasksIndirectCountCmd3 &src) {
                    UnifiedGeometry::DrawMeshTasksIndirectCountCmd2 cmd;
                    cmd.indirectBufferPtr   = src._indirectBuffer->ToResource();    CHECK_THROW( cmd.indirectBufferPtr );
                    cmd.indirectBufferOffset= DrawCmd_GetIndirectBufferOffset( src, "DrawMeshTasksIndirectCommand" );
                    cmd.countBufferPtr      = src._countBuffer->ToResource();       CHECK_THROW( cmd.countBufferPtr );
                    cmd.countBufferOffset   = DrawCmd_GetCountBufferOffset( src );
                    cmd.maxDrawCount        = src.maxDrawCount;
                    cmd.dynMaxDrawCount     = src.dynMaxDrawCount ? src.dynMaxDrawCount->Get() : null;
                    cmd.stride              = Bytes{src.stride};
                    dst = cmd;
                });
        }

        _args.InitResources( result->_resources );

        _geomSrc = result;
        return _geomSrc;
    }

/*
=================================================
    FindMaterialPipeline
=================================================
*/
    ScriptGeomSource::PipelineNames_t  ScriptUniGeometry::FindMaterialPipeline () C_Th___
    {
        CHECK_THROW_MSG( not _drawCommands.empty() );

        const auto  GetMeshPipeline = [this] () -> PipelineName
        {{
            Array<MeshPipelineSpecPtr>      pipelines;
            _GetMeshPipelines( OUT pipelines );
            _FindPipelinesByUB( c_MtrDS, "UnifiedGeometryMaterialUB", INOUT pipelines );    // throw
            _FindPipelinesByResources( c_MtrDS, _args.Args(), INOUT pipelines );            // throw
            auto    tmp = _GetSuitablePipeline( pipelines );
            CHECK_THROW_MSG( not tmp.empty() );
            return tmp.front();
        }};

        const auto  GetGraphicsPipeline = [this] () -> PipelineName
        {{
            Array<GraphicsPipelineSpecPtr>  pipelines;
            _FindPipelinesWithoutVB( OUT pipelines );
            _FindPipelinesByUB( c_MtrDS, "UnifiedGeometryMaterialUB", INOUT pipelines );    // throw
            _FindPipelinesByResources( c_MtrDS, _args.Args(), INOUT pipelines );            // throw
            auto    tmp = _GetSuitablePipeline( pipelines );
            CHECK_THROW_MSG( not tmp.empty() );
            return tmp.front();
        }};

        PipelineNames_t     result;
        result.reserve( _drawCommands.size() );

        for (auto& src : _drawCommands)
        {
            Visit( src,
                [&] (const DrawCmd3 &)                          { result.push_back( GetGraphicsPipeline() ); },
                [&] (const DrawIndexedCmd3 &)                   { result.push_back( GetGraphicsPipeline() ); },
                [&] (const DrawIndirectCmd3 &)                  { result.push_back( GetGraphicsPipeline() ); },
                [&] (const DrawIndexedIndirectCmd3 &)           { result.push_back( GetGraphicsPipeline() ); },
                [&] (const DrawIndirectCountCmd3 &)             { result.push_back( GetGraphicsPipeline() ); },
                [&] (const DrawIndexedIndirectCountCmd3 &)      { result.push_back( GetGraphicsPipeline() ); },
                [&] (const DrawMeshTasksCmd3 &)                 { result.push_back( GetMeshPipeline() ); },
                [&] (const DrawMeshTasksIndirectCmd3 &)         { result.push_back( GetMeshPipeline() ); },
                [&] (const DrawMeshTasksIndirectCountCmd3 &)    { result.push_back( GetMeshPipeline() ); }
            );
        }
        return result;
    }

/*
=================================================
    ToMaterial
=================================================
*/
    RC<IGSMaterials>  ScriptUniGeometry::ToMaterial (RenderTechPipelinesPtr rtech, const PipelineNames_t &names) C_Th___
    {
        CHECK_THROW( _geomSrc );
        CHECK_THROW( rtech );
        CHECK_THROW( names.size() == _drawCommands.size() );

        auto        result          = MakeRC<UnifiedGeometry::Material>();
        auto&       res_mngr        = RenderTaskScheduler().GetResourceManager();
        Renderer&   renderer        = ScriptExe::ScriptResourceApi::GetRenderer();  // throw
        bool        ds_inited       = false;

        const auto  FindMeshPpln    = [&] (const PipelineName &name)
        {{
            auto    ppln = rtech->GetMeshPipeline( name );
            CHECK_THROW( ppln );

            if ( ds_inited ){
                CHECK_THROW( result->mtrDSIndex == GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_MtrDS} ));
                CHECK_THROW( result->passDSIndex == GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_PassDS} ));
            }else{
                ds_inited = true;
                CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->mtrDSIndex, OUT result->descSets.data(), result->descSets.size(),
                                                            ppln, DescriptorSetName{c_MtrDS} ));
                result->passDSIndex = GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_PassDS} );
            }
            result->pplns.push_back( ppln );
        }};

        const auto  FindGraphicsPpln = [&] (const PipelineName &name)
        {{
            auto    ppln = rtech->GetGraphicsPipeline( name );
            CHECK_THROW( ppln );

            if ( ds_inited ){
                CHECK_THROW( result->mtrDSIndex == GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_MtrDS} ));
                CHECK_THROW( result->passDSIndex == GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_PassDS} ));
            }else{
                ds_inited = true;
                CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->mtrDSIndex, OUT result->descSets.data(), result->descSets.size(),
                                                            ppln, DescriptorSetName{c_MtrDS} ));
                result->passDSIndex = GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_PassDS} );
            }
            result->pplns.push_back( ppln );
        }};

        result->rtech = rtech;

        for (usize i = 0; i < _drawCommands.size(); ++i)
        {
            const auto  name = names[i];
            Visit( _drawCommands[i],
                [&] (const DrawCmd3 &)                      { FindGraphicsPpln( name ); },
                [&] (const DrawIndexedCmd3 &)               { FindGraphicsPpln( name ); },
                [&] (const DrawIndirectCmd3 &)              { FindGraphicsPpln( name ); },
                [&] (const DrawIndexedIndirectCmd3 &)       { FindGraphicsPpln( name ); },
                [&] (const DrawIndirectCountCmd3 &)         { FindGraphicsPpln( name ); },
                [&] (const DrawIndexedIndirectCountCmd3 &)  { FindGraphicsPpln( name ); },
                [&] (const DrawMeshTasksCmd3 &)             { FindMeshPpln( name ); },
                [&] (const DrawMeshTasksIndirectCmd3 &)     { FindMeshPpln( name ); },
                [&] (const DrawMeshTasksIndirectCountCmd3 &){ FindMeshPpln( name ); }
            );
        }

        result->ubuffer = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ShaderTypes::UnifiedGeometryMaterialUB>, EBufferUsage::Uniform | EBufferUsage::TransferDst },
                                                 "UnifiedGeometryMaterialUB", renderer.GetAllocator() );
        CHECK_THROW( result->ubuffer );

        return result;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ScriptModelGeometrySrc::ScriptModelGeometrySrc () __Th___
    {}

    ScriptModelGeometrySrc::ScriptModelGeometrySrc (const String &filename) __Th___ :
        _scenePath{ ScriptExe::ScriptResourceApi::ToAbsolute( filename )}  // throw
    {
        _dbgName = Path{filename}.filename().replace_extension("").string().substr( 0, ResNameMaxLen );
    }

/*
=================================================
    destructor
=================================================
*/
    ScriptModelGeometrySrc::~ScriptModelGeometrySrc ()
    {}

/*
=================================================
    Name
=================================================
*/
    void  ScriptModelGeometrySrc::Name (const String &name) __Th___
    {
        CHECK_THROW_MSG( not _geomSrc,
            "resource is already created, can not change debug name" );

        _dbgName = name;

        if ( _opaqueRTGeom )
            _opaqueRTGeom->Name( name + "-Opaque" );

        if ( _translucentRTGeom )
            _translucentRTGeom->Name( _dbgName + "-Translucent" );
    }

/*
=================================================
    AddTextureSearchDir
=================================================
*/
    void  ScriptModelGeometrySrc::AddTextureSearchDir (const String &value) __Th___
    {
        CHECK_THROW_MSG( not _geomSrc,
            "resource is already created, can not change texture prefix" );

        _texSearchDirs.push_back( FileSystem::ToAbsolute( value )); // TODO: VFS

        CHECK_THROW_MSG( FileSystem::IsDirectory( _texSearchDirs.back() ),
            "Folder '"s << ToString(_texSearchDirs.back()) << "' is not exists" );
    }

/*
=================================================
    GetOpaqueRTGeometry
=================================================
*/
    ScriptRTGeometry*  ScriptModelGeometrySrc::GetOpaqueRTGeometry () __Th___
    {
        CHECK_THROW_MSG( not _geomSrc,
            "resource is already created, can not create RTGeometry for Model" );

        if ( not _opaqueRTGeom )
        {
            _opaqueRTGeom.Attach( new ScriptRTGeometry{} );
            _opaqueRTGeom->Name( _dbgName + "-Opaque" );

            // TODO

            _opaqueRTGeom->MakeImmutable();
        }

        return ScriptRTGeometryPtr{_opaqueRTGeom}.Detach();
    }

/*
=================================================
    GetTranslucentRTGeometry
=================================================
*/
    ScriptRTGeometry*  ScriptModelGeometrySrc::GetTranslucentRTGeometry () __Th___
    {
        CHECK_THROW_MSG( not _geomSrc,
            "resource is already created, can not create RTGeometry for Model" );

        if ( not _translucentRTGeom )
        {
            _translucentRTGeom.Attach( new ScriptRTGeometry{} );
            _translucentRTGeom->Name( _dbgName + "-Translucent" );

            // TODO

            _translucentRTGeom->MakeImmutable();
        }

        return ScriptRTGeometryPtr{_translucentRTGeom}.Detach();
    }

/*
=================================================
    SetInitialTransform
=================================================
*/
    void  ScriptModelGeometrySrc::SetInitialTransform (const packed_float4x4 &value) __Th___
    {
        CHECK_THROW_MSG( not _geomSrc,
            "resource is already created, can not set initial transform" );

        _initialTransform = float4x4{value};
    }

/*
=================================================
    _OnAddArg
=================================================
*/
    void  ScriptModelGeometrySrc::_OnAddArg (INOUT ScriptPassArgs::Argument &) C_Th___
    {
        CHECK_THROW_MSG( not _geomSrc );
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptModelGeometrySrc::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptModelGeometrySrc>  binder{ se };
        binder.CreateRef( 0, False{"no ctor"} );
        ScriptGeomSource::_BindBase( binder );
        binder.AddFactoryCtor( &ScriptSceneGeometry_Ctor1,  {"scenePathInVFS"} );

        binder.Comment( "Set resource name. It is used for debugging." );
        binder.AddMethod( &ScriptModelGeometrySrc::Name,                        "Name",                 {} );

        binder.Comment( "Add directory where to search required textures." );
        binder.AddMethod( &ScriptModelGeometrySrc::AddTextureSearchDir,         "TextureSearchDir",     {"folder"} );

        binder.Comment( "Set transformation for model root node." );
        binder.AddMethod( &ScriptModelGeometrySrc::SetInitialTransform,         "InitialTransform",     {} );

        binder.Comment( "Returns RTGeometry to use it in ray tracing." );
        binder.AddMethod( &ScriptModelGeometrySrc::GetOpaqueRTGeometry,         "OpaqueRTGeometry",     {} );
        binder.AddMethod( &ScriptModelGeometrySrc::GetTranslucentRTGeometry,    "TranslucentRTGeometry",{} );
    }

/*
=================================================
    GetShaderTypes
=================================================
*/
    void  ScriptModelGeometrySrc::GetShaderTypes (INOUT CppStructsFromShaders &) __Th___
    {
    }

/*
=================================================
    ToGeomSource
=================================================
*/
    RC<IGeomSource>  ScriptModelGeometrySrc::ToGeomSource () __Th___
    {
        using namespace ResLoader;

        if ( _geomSrc )
            return _geomSrc;

        IModelLoader::Config    cfg;

        _intermScene.reset( new IntermScene{} );

        #ifdef AE_ENABLE_ASSIMP
            AssimpLoader    loader;
            CHECK_THROW_MSG( loader.LoadModel( *_intermScene, _scenePath, cfg ),
                "failed to load model from '"s << ToString(_scenePath) << "'" );
        #endif

        Renderer&   renderer = ScriptExe::ScriptResourceApi::GetRenderer(); // throw

        // search in scene dir
        _texSearchDirs.push_back( _scenePath.parent_path() );

        _geomSrc = MakeRC<ModelGeomSource>( renderer, _intermScene, _initialTransform, _texSearchDirs, _maxTextures );
        return _geomSrc;
    }

/*
=================================================
    FindMaterialPipeline
=================================================
*/
    ScriptGeomSource::PipelineNames_t  ScriptModelGeometrySrc::FindMaterialPipeline () C_Th___
    {
        CHECK_THROW_MSG( _intermScene );

        ScriptGeomSource::PipelineNames_t   ppln_per_mtr;
        DSLayoutName                        shared_mtr_dsl;

        _intermScene->ForEachModel(
            [&ppln_per_mtr, this, &shared_mtr_dsl] (const ResLoader::IntermScene::ModelData &model)
            {
                auto    mesh    = model.levels[ uint(ResLoader::EDetailLevel::High) ].mesh;
                auto    mtr     = model.levels[ uint(ResLoader::EDetailLevel::High) ].mtr;

                CHECK_THROW_MSG( mesh and mtr );
                CHECK_THROW_MSG( mesh->Attribs() != null );
                CHECK_THROW_MSG( mesh->Topology() == EPrimitive::TriangleList );

                Array<GraphicsPipelineSpecPtr>  pipelines;
                _FindPipelinesByVB( _AttribsToVBName( *mesh->Attribs() ), OUT pipelines );                              // throw
                _FindPipelinesByPC( "model.pc", INOUT pipelines );                                                      // throw
                _FindPipelinesBySB( c_MtrDS, "ModelNodeArray", INOUT pipelines );                                       // throw
                _FindPipelinesBySampledImage( c_MtrDS, "un_AlbedoMaps",
                                              EImageType::Img2D | EImageType::Float, _maxTextures, INOUT pipelines );   // throw
                _FindPipelinesBySampler( c_MtrDS, "un_AlbedoSampler", INOUT pipelines );                                // throw
                _FindPipelinesByResources( c_MtrDS, _args.Args(), INOUT pipelines );                                    // throw

                ScriptGeomSource::PipelineNames_t   ppln_name;
                DSLayoutName                        mtr_dsl;
                _GetSuitablePipelineAndDS( pipelines, c_MtrDS, OUT ppln_name, OUT mtr_dsl );                            // throw

                if ( shared_mtr_dsl == Default )
                    shared_mtr_dsl = mtr_dsl;   // init
                else
                    CHECK_THROW_MSG( shared_mtr_dsl == mtr_dsl, "All pipelines must use same DescriptorSetLayout" );

                ppln_per_mtr.push_back( ppln_name[0] );
            });

        CHECK_THROW_MSG( shared_mtr_dsl.IsDefined() );
        CHECK_THROW_MSG( not ppln_per_mtr.empty() );

        return ppln_per_mtr;
    }

/*
=================================================
    ToMaterial
=================================================
*/
    RC<IGSMaterials>  ScriptModelGeometrySrc::ToMaterial (RenderTechPipelinesPtr rtech, const PipelineNames_t &names) C_Th___
    {
        CHECK_THROW( _geomSrc );
        CHECK_THROW( rtech );
        CHECK_THROW( names.size() > 0 );

        auto    result      = MakeRC<ModelGeomSource::Material>();
        auto&   res_mngr    = RenderTaskScheduler().GetResourceManager();

        result->rtech   = rtech;
        result->pplns.reserve( names.size() );

        _intermScene->ForEachModel(
            [&, i = 0u, init_ds = true] (const ResLoader::IntermScene::ModelData &) mutable
            {
                CHECK_THROW_MSG( i < names.size() );

                auto    ppln = rtech->GetGraphicsPipeline( names[i++] );
                CHECK_THROW( ppln );

                if ( init_ds )
                {
                    init_ds = false;
                    CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->mtrDSIndex, OUT result->descSets.data(), result->descSets.size(),
                                                                ppln, DescriptorSetName{c_MtrDS} ));

                    result->passDSIndex = GetDescSetBinding( res_mngr, ppln, DescriptorSetName{c_PassDS} );  // throw
                    result->pcIndex     = res_mngr.GetPushConstantIndex< ShaderTypes::model_pc >( ppln, PushConstantName{"pc"} );
                    CHECK_THROW( result->pcIndex );
                }

                result->pplns.push_back( ppln );
            });

        CHECK_THROW( result->pplns.size() == names.size() );
        CHECK_THROW( result->passDSIndex );
        CHECK_THROW( result->descSets[0] );

        return result;
    }

/*
=================================================
    _AttribsToVBName
----
    vertex buffer types defined in
    [file](https://github.com/azhirnov/as-en/blob/dev/AE/samples/res_editor/_data/pipelines/ModelReflection.as)
=================================================
*/
    String  ScriptModelGeometrySrc::_AttribsToVBName (const ResLoader::IntermVertexAttribs &attribs) __Th___
    {
        using namespace AE::ResLoader;

        const bool  has_position    = attribs.HasVertex( VertexAttributeName::Position,     EVertexType::Float3 );
        const bool  has_normal      = attribs.HasVertex( VertexAttributeName::Normal,       EVertexType::Float3 );
        const bool  has_tangent     = attribs.HasVertex( VertexAttributeName::Tangent,      EVertexType::Float3 );
        const bool  has_bitangent   = attribs.HasVertex( VertexAttributeName::BiTangent,    EVertexType::Float3 );
        const bool  has_uv0_2       = attribs.HasVertex( VertexAttributeName::TextureUVs[0],EVertexType::Float2 );

        if ( has_position and has_normal and has_uv0_2 )
            return "VB{Posf3, Normf3, UVf2}";

        if ( has_position and has_normal )
            return "VB{Posf3, Normf3}";

        if ( has_position )
            return "VB{Posf3}";

        CHECK_THROW_MSG( false, "No compatible attribs found." );
    }


} // AE::ResEditor
