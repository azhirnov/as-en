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
    static ScriptTiledTerrain*  ScriptTiledTerrain_Ctor1 (ScriptTiledTerrain::EMode mode) {
        return ScriptRC<ScriptTiledTerrain>{ new ScriptTiledTerrain{ mode, ScriptDynamicFloat4Ptr{new ScriptDynamicFloat4{MakeRC<DynamicFloat4>()}} }}.Detach();
    }

    static ScriptTiledTerrain*  ScriptTiledTerrain_Ctor2 (ScriptTiledTerrain::EMode mode, const ScriptDynamicFloat4Ptr &dynRegion) {
        return ScriptRC<ScriptTiledTerrain>{ new ScriptTiledTerrain{ mode, dynRegion }}.Detach();
    }

    static ScriptSceneGeometry*  ScriptSceneGeometry_Ctor1 (const String &filename) {
        return ScriptRC<ScriptSceneGeometry>{ new ScriptSceneGeometry{ filename }}.Detach();
    }

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
        CHECK_THROW( not pipelines.empty() );
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
        CHECK_THROW( not pipelines.empty() );
    }

    static void  _FindPipelinesWithoutVB (OUT Array<MeshPipelineSpecPtr> &pipelines) __Th___
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
        CHECK_THROW( not pipelines.empty() );
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

        inPipelines = RVRef(out_pplns);
        CHECK_THROW( not inPipelines.empty() );
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

        inPipelines = RVRef(out_pplns);
        CHECK_THROW( not inPipelines.empty() );
    }

/*
=================================================
    _FindPipelinesByTextures
=================================================
*/
    template <typename V, typename PplnSpec>
    static void  _FindPipelinesByTextures (StringView dsName, const FlatHashMap<String,V> &texNames, INOUT Array<PplnSpec> &inPipelines) __Th___
    {
        Array<PplnSpec>                         out_pplns;
        const DescriptorSetName                 req_ds_name {dsName};
        FlatHashSet<UniformName::Optimized_t>   tex_names;

        for (auto& [name, val] : texNames) {
            tex_names.insert( UniformName::Optimized_t{name} );
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

        inPipelines = RVRef(out_pplns);
        CHECK_THROW( not inPipelines.empty() );
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

        auto*   begin   = &mtr.GetSettings().albedo;
        auto*   end     = &mtr.GetSettings().opticalDepth;

        for (; begin < end; ++begin)
        {
            if ( auto* tex = UnionGet<MtrTexture>( *begin ))
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

        inPipelines = RVRef(out_pplns);
        CHECK_THROW( not inPipelines.empty() );
    }

/*
=================================================
    _FindPipelinesByBuffers
=================================================
*/
    template <typename V, typename PplnSpec>
    static void  _FindPipelinesByBuffers (StringView dsName, const FlatHashMap<String,V> &bufNames, INOUT Array<PplnSpec> &inPipelines) __Th___
    {
        Array<PplnSpec>                         out_pplns;
        const DescriptorSetName                 req_ds_name {dsName};
        FlatHashSet<UniformName::Optimized_t>   buf_names;

        for (auto& [name, val] : bufNames) {
            buf_names.insert( UniformName::Optimized_t{name} );
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
                    if ( un.type != EDescriptorType::StorageBuffer )
                    {
                        CHECK( not buf_names.contains( un_name ));
                        continue;
                    }
                    counter += usize(buf_names.contains( un_name ));
                }

                if ( counter == buf_names.size() ) {
                    out_pplns.push_back( ppln );
                    break;
                }
            }
        }

        CHECK_THROW( not out_pplns.empty() );
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

} // namespace
//-----------------------------------------------------------------------------



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
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ScriptTiledTerrain::ScriptTiledTerrain (EMode mode, const ScriptDynamicFloat4Ptr &dynRegion) __Th___ :
        _mode{mode},
        _dynRegion{ dynRegion }
    {
        CHECK_THROW_MSG( _dynRegion and _dynRegion->Get(), "Dynamic region must not be null" );
    }

/*
=================================================
    SetGenerator
=================================================
*/
    void  ScriptTiledTerrain::SetGenerator (const ScriptBasePassPtr &passGroup) __Th___
    {
        CHECK_THROW_MSG( not _generator,
            "Tile generator is already defined" );
        CHECK_THROW_MSG( passGroup );
        CHECK_THROW_MSG( ScriptExe::ScriptResourceApi::IsPassGroup( passGroup ));

        _generator = passGroup;
    }

/*
=================================================
    AddLayer
=================================================
*/
    void  ScriptTiledTerrain::AddLayer (const String &name, const ScriptImagePtr &image) __Th___
    {
        CHECK_THROW_MSG( not _layers.contains( name ),
            "Layer '"s << name << "' is already defined" );
        CHECK_THROW_MSG( image );
        CHECK_THROW_MSG( not image->IsMutableDimension() );

        _layers.emplace( name, image );
    }

/*
=================================================
    SetGridSize
=================================================
*/
    void  ScriptTiledTerrain::SetGridSize (uint value) __Th___
    {
        CHECK_THROW_MSG( _mode == EMode::Chunk3D,
            "Grid size can be specified only for 'Chunk3D' mode" );
        CHECK_THROW_MSG( value >= 2 );

        _vertsPerEdge = value;
    }

/*
=================================================
    DynamicRegion
=================================================
*/
    ScriptDynamicFloat4*  ScriptTiledTerrain::DynamicRegion () __Th___
    {
        return ScriptDynamicFloat4Ptr{_dynRegion}.Detach();
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptTiledTerrain::Bind (const ScriptEnginePtr &se) __Th___
    {
        using namespace Scripting;
        {
            EnumBinder<EMode>   binder{ se };
            binder.Create();
            binder.AddValue( "Tile2D",  EMode::Tile2D );
            binder.AddValue( "Chunk3D", EMode::Chunk3D );
            STATIC_ASSERT( uint(EMode::_Count) == 2 );
        }{
            ClassBinder<ScriptTiledTerrain> binder{ se };
            binder.CreateRef();
            ScriptGeomSource::_BindBase( binder );
            binder.AddFactoryCtor( &ScriptTiledTerrain_Ctor1 );
            binder.AddFactoryCtor( &ScriptTiledTerrain_Ctor2 );
            binder.AddMethod( &ScriptTiledTerrain::SetGenerator,    "Generator"     );
            binder.AddMethod( &ScriptTiledTerrain::AddLayer,        "Layer"         );
            binder.AddMethod( &ScriptTiledTerrain::SetGridSize,     "GridSize"      );
            binder.AddMethod( &ScriptTiledTerrain::DynamicRegion,   "DynamicRegion" );
        }
    }

/*
=================================================
    ToGeomSource
=================================================
*/
    RC<IGeomSource>  ScriptTiledTerrain::ToGeomSource () __Th___
    {
        if ( _geomSrc )
            return _geomSrc;

        CHECK_THROW_MSG( _dynRegion );
        CHECK_THROW_MSG( _generator );
        CHECK_THROW_MSG( not _layers.empty() );

        auto&       res_mngr    = RenderTaskScheduler().GetResourceManager();
        Renderer&   renderer    = ScriptExe::ScriptResourceApi::GetRenderer();  // throw
        auto        result      = MakeRC<TiledTerrain>( renderer, DynCast<PassGroup>(_generator->ToPass()), _mode, _vertsPerEdge, _dynRegion->Get() );

        for (auto& [name, image] : _layers)
        {
            auto&   dst = result->_layers.emplace_back();
            dst.Get<0>() = UniformName{name};
            {
                ImageDesc   desc = image->Description();
                desc.SetArrayLayers( TiledTerrain::LayerCount );
                desc.SetAllMipmaps();

                CHECK_THROW_MSG( desc.imageDim == EImageDim_2D );
                CHECK_THROW_MSG( res_mngr.IsSupported( desc ),
                    "TerrainImage '"s << name << "' description is not supported by GPU device" );

                dst.Get<1>().image = res_mngr.CreateImage( desc, "Layer: "s << name, renderer.GetAllocator() );
                CHECK_THROW_MSG( dst.Get<1>().image );
            }{
                ImageViewDesc   desc{ image->Description() };
                desc.viewType = EImage_2DArray;

                dst.Get<1>().view = res_mngr.CreateImageView( desc, dst.Get<1>().image, "Layer: "s << name );
                CHECK_THROW_MSG( dst.Get<1>().view );
            }
            dst.Get<2>() = image->ToResource();
        }

        _geomSrc = result;
        return _geomSrc;
    }

/*
=================================================
    FindMaterialPipeline
=================================================
*/
    ScriptGeomSource::PipelineNames_t  ScriptTiledTerrain::FindMaterialPipeline () C_Th___
    {
        Array<GraphicsPipelineSpecPtr>  pipelines;
        _FindPipelinesByVB( "VB{2DGridVertex}", OUT pipelines );                        // throw
        _FindPipelinesByUB( "material", "TiledTerrainMaterialUB", INOUT pipelines );    // throw
        _FindPipelinesByTextures( "material", _layers, INOUT pipelines );               // throw
        return _GetSuitablePipeline( pipelines );
    }

/*
=================================================
    ToMaterial
=================================================
*/
    RC<IGSMaterials>  ScriptTiledTerrain::ToMaterial (RenderTechPipelinesPtr rtech, const PipelineNames_t &names) C_Th___
    {
        CHECK_THROW( _geomSrc );
        CHECK_THROW( rtech );
        CHECK_THROW( names.size() == 1 );

        auto        result      = MakeRC<TiledTerrain::Material>();
        auto&       res_mngr    = RenderTaskScheduler().GetResourceManager();

        result->rtech   = rtech;
        result->ppln    = rtech->GetGraphicsPipeline( names[0] );
        CHECK_THROW( result->ppln );

        StructSet( result->descSet, result->mtrDSIndex ) = res_mngr.CreateDescriptorSet( result->ppln, DescriptorSetName{"material"} );
        CHECK_THROW( result->descSet );

        result->passDSIndex = GetDescSetBinding( res_mngr, result->ppln, DescriptorSetName{"pass"} );

    //  result->ubuffer = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ShaderTypes::TiledTerrainMaterialUB>, EBufferUsage::Uniform | EBufferUsage::TransferDst },
    //                                           "TiledTerrainMaterialUB", renderer.GetAllocator() );
    //  CHECK_THROW( result->ubuffer );

        return result;
    }

/*
=================================================
    _CreateUBType
=================================================
*/
    auto  ScriptTiledTerrain::_CreateUBType () __Th___
    {
        auto&   obj_storage = *ObjectStorage::Instance();
        auto    it          = obj_storage.structTypes.find( "TiledTerrainMaterialUB" );

        if ( it != obj_storage.structTypes.end() )
            return it->second;

        ShaderStructTypePtr st{ new ShaderStructType{"TiledTerrainMaterialUB"}};
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
    void  ScriptTiledTerrain::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
    {
        auto    st = _CreateUBType();   // throw

        CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
    }
//-----------------------------------------------------------------------------



/*
=================================================
    AddTexture*
=================================================
*/
    void  ScriptSphericalCube::AddTexture1 (const String &name, const ScriptImagePtr &tex) __Th___
    {
        return AddTexture2( name, tex, "" );
    }

    void  ScriptSphericalCube::AddTexture2 (const String &name, const ScriptImagePtr &tex, const String &sampler) __Th___
    {
        CHECK_THROW_MSG( tex );
        CHECK_THROW_MSG( not _geomSrc );
        CHECK_THROW_MSG( not _textures.contains( name ), "uniform '"s << name << "' is already exists" );

        tex->AddUsage( EResourceUsage::Sampled );

        auto&   dst = _textures[ name ];
        dst.image   = tex;
        dst.sampler = sampler;
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

        binder.AddMethod( &ScriptSphericalCube::AddTexture1,        "AddTexture"    );
        binder.AddMethod( &ScriptSphericalCube::AddTexture2,        "AddTexture"    );
        binder.AddMethod( &ScriptSphericalCube::SetDetailLevel1,    "DetailLevel"   );
        binder.AddMethod( &ScriptSphericalCube::SetDetailLevel2,    "DetailLevel"   );
        binder.AddMethod( &ScriptSphericalCube::SetTessLevel1,      "TessLevel"     );
        binder.AddMethod( &ScriptSphericalCube::SetTessLevel2,      "TessLevel"     );
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
        _FindPipelinesByVB( "VB{SphericalCubeVertex}", OUT pipelines );                 // throw
        _FindPipelinesByUB( "material", "SphericalCubeMaterialUB", INOUT pipelines );   // throw
        _FindPipelinesByTextures( "material", _textures, INOUT pipelines );             // throw
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
                                                    ppln, DescriptorSetName{"material"} ));

        result->passDSIndex = GetDescSetBinding( res_mngr, ppln, DescriptorSetName{"pass"} );

        result->ubuffer = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ShaderTypes::SphericalCubeMaterialUB>, EBufferUsage::Uniform | EBufferUsage::TransferDst },
                                                 "SphericalCubeMaterialUB", renderer.GetAllocator() );
        CHECK_THROW( result->ubuffer );

        for (auto& src : _textures)
        {
            auto&   dst = result->textures.emplace_back();
            dst.first   = UniformName{src.first};
            dst.second  = src.second.image->ToResource();
            CHECK_THROW( dst.second );
        }

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

/*
=================================================
    DrawIndexedCmd3::SetIndexBuffer*
=================================================
*/
    void  ScriptUniGeometry::DrawIndexedCmd3::SetIndexBuffer1 (EIndex type, const ScriptBufferPtr &ibuf)
    {
        SetIndexBuffer2( type, ibuf, 0 );
    }

    void  ScriptUniGeometry::DrawIndexedCmd3::SetIndexBuffer2 (EIndex type, const ScriptBufferPtr &ibuf, ulong offset)
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not indexBuffer );
        CHECK_THROW_MSG( type == EIndex::UShort or type == EIndex::UInt );
        indexType           = type;
        indexBuffer         = ibuf;
        indexBufferOffset   = offset;
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

/*
=================================================
    DrawIndirectCmd3::SetIndirectBuffer*
=================================================
*/
    void  ScriptUniGeometry::DrawIndirectCmd3::SetIndirectBuffer1 (const ScriptBufferPtr &ibuf)
    {
        SetIndirectBuffer2( ibuf, 0 );
    }

    void  ScriptUniGeometry::DrawIndirectCmd3::SetIndirectBuffer2 (const ScriptBufferPtr &ibuf, ulong offset)
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not indirectBuffer );
        indirectBuffer          = ibuf;
        indirectBufferOffset    = offset;
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

/*
=================================================
    DrawIndexedIndirectCmd3::SetIndexBuffer*
=================================================
*/
    void  ScriptUniGeometry::DrawIndexedIndirectCmd3::SetIndexBuffer1 (EIndex type, const ScriptBufferPtr &ibuf)
    {
        SetIndexBuffer2( type, ibuf, 0 );
    }

    void  ScriptUniGeometry::DrawIndexedIndirectCmd3::SetIndexBuffer2 (EIndex type, const ScriptBufferPtr &ibuf, ulong offset)
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not indexBuffer );
        CHECK_THROW_MSG( type == EIndex::UShort or type == EIndex::UInt );
        indexType           = type;
        indexBuffer         = ibuf;
        indexBufferOffset   = offset;
    }

/*
=================================================
    DrawIndexedIndirectCmd3::SetIndirectBuffer*
=================================================
*/
    void  ScriptUniGeometry::DrawIndexedIndirectCmd3::SetIndirectBuffer1 (const ScriptBufferPtr &ibuf)
    {
        SetIndirectBuffer2( ibuf, 0 );
    }

    void  ScriptUniGeometry::DrawIndexedIndirectCmd3::SetIndirectBuffer2 (const ScriptBufferPtr &ibuf, ulong offset)
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not indirectBuffer );
        indirectBuffer          = ibuf;
        indirectBufferOffset    = offset;
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

/*
=================================================
    DrawMeshTasksIndirectCmd3::SetIndirectBuffer*
=================================================
*/
    void  ScriptUniGeometry::DrawMeshTasksIndirectCmd3::SetIndirectBuffer1 (const ScriptBufferPtr &ibuf)
    {
        SetIndirectBuffer2( ibuf, 0 );
    }

    void  ScriptUniGeometry::DrawMeshTasksIndirectCmd3::SetIndirectBuffer2 (const ScriptBufferPtr &ibuf, ulong offset)
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not indirectBuffer );
        indirectBuffer          = ibuf;
        indirectBufferOffset    = offset;
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

/*
=================================================
    DrawIndirectCountCmd3::SetIndirectBuffer*
=================================================
*/
    void  ScriptUniGeometry::DrawIndirectCountCmd3::SetIndirectBuffer1 (const ScriptBufferPtr &ibuf)
    {
        SetIndirectBuffer2( ibuf, 0 );
    }

    void  ScriptUniGeometry::DrawIndirectCountCmd3::SetIndirectBuffer2 (const ScriptBufferPtr &ibuf, ulong offset)
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not indirectBuffer );
        indirectBuffer          = ibuf;
        indirectBufferOffset    = offset;
    }

/*
=================================================
    DrawIndirectCountCmd3::SetCountBuffer*
=================================================
*/
    void  ScriptUniGeometry::DrawIndirectCountCmd3::SetCountBuffer1 (const ScriptBufferPtr &cbuf)
    {
        SetCountBuffer2( cbuf, 0 );
    }

    void  ScriptUniGeometry::DrawIndirectCountCmd3::SetCountBuffer2 (const ScriptBufferPtr &cbuf, ulong offset)
    {
        CHECK_THROW_MSG( cbuf );
        CHECK_THROW_MSG( not countBuffer );
        countBuffer         = cbuf;
        countBufferOffset   = offset;
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

/*
=================================================
    DrawIndexedIndirectCountCmd3::SetIndexBuffer*
=================================================
*/
    void  ScriptUniGeometry::DrawIndexedIndirectCountCmd3::SetIndexBuffer1 (EIndex type, const ScriptBufferPtr &ibuf)
    {
        SetIndexBuffer2( type, ibuf, 0 );
    }

    void  ScriptUniGeometry::DrawIndexedIndirectCountCmd3::SetIndexBuffer2 (EIndex type, const ScriptBufferPtr &ibuf, ulong offset)
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not indexBuffer );
        CHECK_THROW_MSG( type == EIndex::UShort or type == EIndex::UInt );
        indexType           = type;
        indexBuffer         = ibuf;
        indexBufferOffset   = offset;
    }

/*
=================================================
    DrawIndexedIndirectCountCmd3::SetIndirectBuffer*
=================================================
*/
    void  ScriptUniGeometry::DrawIndexedIndirectCountCmd3::SetIndirectBuffer1 (const ScriptBufferPtr &ibuf)
    {
        SetIndirectBuffer2( ibuf, 0 );
    }

    void  ScriptUniGeometry::DrawIndexedIndirectCountCmd3::SetIndirectBuffer2 (const ScriptBufferPtr &ibuf, ulong offset)
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not indirectBuffer );
        indirectBuffer          = ibuf;
        indirectBufferOffset    = offset;
    }

/*
=================================================
    DrawIndexedIndirectCountCmd3::SetCountBuffer*
=================================================
*/
    void  ScriptUniGeometry::DrawIndexedIndirectCountCmd3::SetCountBuffer1 (const ScriptBufferPtr &cbuf)
    {
        SetCountBuffer2( cbuf, 0 );
    }

    void  ScriptUniGeometry::DrawIndexedIndirectCountCmd3::SetCountBuffer2 (const ScriptBufferPtr &cbuf, ulong offset)
    {
        CHECK_THROW_MSG( cbuf );
        CHECK_THROW_MSG( not countBuffer );
        countBuffer         = cbuf;
        countBufferOffset   = offset;
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

/*
=================================================
    DrawMeshTasksIndirectCountCmd3::SetIndirectBuffer*
=================================================
*/
    void  ScriptUniGeometry::DrawMeshTasksIndirectCountCmd3::SetIndirectBuffer1 (const ScriptBufferPtr &ibuf)
    {
        SetIndirectBuffer2( ibuf, 0 );
    }

    void  ScriptUniGeometry::DrawMeshTasksIndirectCountCmd3::SetIndirectBuffer2 (const ScriptBufferPtr &ibuf, ulong offset)
    {
        CHECK_THROW_MSG( ibuf );
        CHECK_THROW_MSG( not indirectBuffer );
        indirectBuffer          = ibuf;
        indirectBufferOffset    = offset;
    }

/*
=================================================
    DrawMeshTasksIndirectCountCmd3::SetCountBuffer*
=================================================
*/
    void  ScriptUniGeometry::DrawMeshTasksIndirectCountCmd3::SetCountBuffer1 (const ScriptBufferPtr &cbuf)
    {
        SetCountBuffer2( cbuf, 0 );
    }

    void  ScriptUniGeometry::DrawMeshTasksIndirectCountCmd3::SetCountBuffer2 (const ScriptBufferPtr &cbuf, ulong offset)
    {
        CHECK_THROW_MSG( cbuf );
        CHECK_THROW_MSG( not countBuffer );
        countBuffer         = cbuf;
        countBufferOffset   = offset;
    }
//-----------------------------------------------------------------------------


/*
=================================================
    constructor
=================================================
*/
    ScriptUniGeometry::ScriptUniGeometry ()
    {}

/*
=================================================
    AddBuffer
=================================================
*/
    void  ScriptUniGeometry::AddBuffer (const String &unName, const ScriptBufferPtr &buf) __Th___
    {
        CHECK_THROW_MSG( not unName.empty() );
        CHECK_THROW_MSG( buf );

        buf->AddUsage( EResourceUsage::ComputeRead );

        CHECK_THROW_MSG( _meshes.emplace( unName, buf ).second,
            "Buffer with name '"s << unName << "' is already defined" );
    }

/*
=================================================
    AddTexture
=================================================
*/
    void  ScriptUniGeometry::AddTexture (const String &unName, const ScriptImagePtr &img) __Th___
    {
        CHECK_THROW_MSG( not unName.empty() );
        CHECK_THROW_MSG( img );

        img->AddUsage( EResourceUsage::Sampled );

        CHECK_THROW_MSG( _textures.emplace( unName, img ).second,
            "Texture with name '"s << unName << "' is already defined" );
    }

/*
=================================================
    Draw*
=================================================
*/
    void  ScriptUniGeometry::Draw1 (const DrawCmd3 &cmd)
    {
        CHECK_THROW_MSG( cmd.vertexCount > 0 or cmd.dynVertexCount );
        CHECK_THROW_MSG( cmd.instanceCount > 0 or cmd.dynInstanceCount );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw2 (const DrawIndexedCmd3 &cmd)
    {
        CHECK_THROW_MSG( cmd.indexBuffer );
        CHECK_THROW_MSG( cmd.indexCount > 0 );
        CHECK_THROW_MSG( cmd.instanceCount > 0 );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw3 (const DrawIndirectCmd3 &cmd)
    {
        // TODO: check feature
        CHECK_THROW_MSG( cmd.indirectBuffer );
        CHECK_THROW_MSG( cmd.drawCount > 0 or cmd.dynDrawCount );
        CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawIndirectCommand)) );
        CHECK_THROW_MSG( cmd.stride % 4 == 0, "Stride must be multiple of 4" );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw4 (const DrawIndexedIndirectCmd3 &cmd)
    {
        // TODO: check feature
        CHECK_THROW_MSG( cmd.indexBuffer );
        CHECK_THROW_MSG( cmd.indirectBuffer );
        CHECK_THROW_MSG( cmd.drawCount > 0 or cmd.dynDrawCount );
        CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawIndexedIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawIndexedIndirectCommand)) );
        CHECK_THROW_MSG( cmd.stride % 4 == 0, "Stride must be multiple of 4" );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw5 (const DrawMeshTasksCmd3 &cmd)
    {
        auto&   ext = RenderTaskScheduler().GetDevice().GetExtensions();
        CHECK_THROW_MSG( ext.meshShader );

        CHECK_THROW_MSG( All( uint3{cmd.taskCount} > uint3{0} ));

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw6 (const DrawMeshTasksIndirectCmd3 &cmd)
    {
        auto&   ext = RenderTaskScheduler().GetDevice().GetExtensions();
        CHECK_THROW_MSG( ext.meshShader );

        CHECK_THROW_MSG( cmd.indirectBuffer );
        CHECK_THROW_MSG( cmd.drawCount > 0 or cmd.dynDrawCount );
        CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawMeshTasksIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawMeshTasksIndirectCommand)) );
        CHECK_THROW_MSG( cmd.stride % 4 == 0, "Stride must be multiple of 4" );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw7 (const DrawIndirectCountCmd3 &cmd)
    {
        auto&   ext = RenderTaskScheduler().GetDevice().GetExtensions();
        CHECK_THROW_MSG( ext.drawIndirectCount );

        CHECK_THROW_MSG( cmd.indirectBuffer );
        CHECK_THROW_MSG( cmd.countBuffer );
        CHECK_THROW_MSG( cmd.maxDrawCount > 0 or cmd.dynMaxDrawCount );
        CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawIndirectCommand)) );
        CHECK_THROW_MSG( cmd.stride % 4 == 0, "Stride must be multiple of 4" );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw8 (const DrawIndexedIndirectCountCmd3 &cmd)
    {
        auto&   ext = RenderTaskScheduler().GetDevice().GetExtensions();
        CHECK_THROW_MSG( ext.drawIndirectCount );

        CHECK_THROW_MSG( cmd.indexBuffer );
        CHECK_THROW_MSG( cmd.indirectBuffer );
        CHECK_THROW_MSG( cmd.countBuffer );
        CHECK_THROW_MSG( cmd.maxDrawCount > 0 or cmd.dynMaxDrawCount );
        CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawIndexedIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawIndexedIndirectCommand)) );
        CHECK_THROW_MSG( cmd.stride % 4 == 0, "Stride must be multiple of 4" );

        _drawCommands.push_back( cmd );
    }

    void  ScriptUniGeometry::Draw9 (const DrawMeshTasksIndirectCountCmd3 &cmd)
    {
        auto&   ext = RenderTaskScheduler().GetDevice().GetExtensions();
        CHECK_THROW_MSG( ext.drawIndirectCount and ext.meshShader );

        CHECK_THROW_MSG( cmd.indirectBuffer );
        CHECK_THROW_MSG( cmd.countBuffer );
        CHECK_THROW_MSG( cmd.maxDrawCount > 0 or cmd.dynMaxDrawCount );
        CHECK_THROW_MSG( cmd.stride >= sizeof(Graphics::DrawMeshTasksIndirectCommand), "Stride must be >= "s << ToString(sizeof(Graphics::DrawMeshTasksIndirectCommand)) );
        CHECK_THROW_MSG( cmd.stride % 4 == 0, "Stride must be multiple of 4" );

        _drawCommands.push_back( cmd );
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
        binder.AddMethod( &DrawCmd3::SetDynVertexCount,                             "VertexCount"       );
        binder.AddMethod( &DrawCmd3::SetDynInstanceCount,                           "InstanceCount"     );
        binder.AddProperty( &DrawCmd3::vertexCount,                                 "vertexCount"       );
        binder.AddProperty( &DrawCmd3::instanceCount,                               "instanceCount"     );
        binder.AddProperty( &DrawCmd3::firstVertex,                                 "firstVertex"       );
        binder.AddProperty( &DrawCmd3::firstInstance,                               "firstInstance"     );
    }

    void  ScriptUniGeometry::DrawIndexedCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawIndexedCmd3> binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawIndexedCmd3::SetDynIndexCount,                       "IndexCount"        );
        binder.AddMethod( &DrawIndexedCmd3::SetDynInstanceCount,                    "InstanceCount"     );
        binder.AddMethod( &DrawIndexedCmd3::SetIndexBuffer1,                        "IndexBuffer"       );
        binder.AddMethod( &DrawIndexedCmd3::SetIndexBuffer2,                        "IndexBuffer"       );
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
        binder.AddMethod( &DrawIndirectCmd3::SetDynDrawCount,                       "DrawCount"         );
        binder.AddMethod( &DrawIndirectCmd3::SetIndirectBuffer1,                    "IndirectBuffer"    );
        binder.AddMethod( &DrawIndirectCmd3::SetIndirectBuffer2,                    "IndirectBuffer"    );
        binder.AddProperty( &DrawIndirectCmd3::drawCount,                           "drawCount"         );
        binder.AddProperty( &DrawIndirectCmd3::stride,                              "stride"            );
    }

    void  ScriptUniGeometry::DrawIndexedIndirectCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawIndexedIndirectCmd3> binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawIndexedIndirectCmd3::SetDynDrawCount,                "DrawCount"         );
        binder.AddMethod( &DrawIndexedIndirectCmd3::SetIndexBuffer1,                "IndexBuffer"       );
        binder.AddMethod( &DrawIndexedIndirectCmd3::SetIndexBuffer2,                "IndexBuffer"       );
        binder.AddMethod( &DrawIndexedIndirectCmd3::SetIndirectBuffer1,             "IndirectBuffer"    );
        binder.AddMethod( &DrawIndexedIndirectCmd3::SetIndirectBuffer2,             "IndirectBuffer"    );
        binder.AddProperty( &DrawIndexedIndirectCmd3::drawCount,                    "drawCount"         );
        binder.AddProperty( &DrawIndexedIndirectCmd3::stride,                       "stride"            );
    }

    void  ScriptUniGeometry::DrawMeshTasksCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawMeshTasksCmd3>   binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawMeshTasksCmd3::SetDynTaskCount,                      "TaskCount"         );
        binder.AddProperty( &DrawMeshTasksCmd3::taskCount,                          "taskCount"         );
    }

    void  ScriptUniGeometry::DrawMeshTasksIndirectCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawMeshTasksIndirectCmd3>   binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawMeshTasksIndirectCmd3::SetDynDrawCount,              "DrawCount"         );
        binder.AddMethod( &DrawMeshTasksIndirectCmd3::SetIndirectBuffer1,           "IndirectBuffer"    );
        binder.AddMethod( &DrawMeshTasksIndirectCmd3::SetIndirectBuffer2,           "IndirectBuffer"    );
        binder.AddProperty( &DrawMeshTasksIndirectCmd3::drawCount,                  "drawCount"         );
        binder.AddProperty( &DrawMeshTasksIndirectCmd3::stride,                     "stride"            );
    }

    void  ScriptUniGeometry::DrawIndirectCountCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawIndirectCountCmd3>   binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawIndirectCountCmd3::SetDynMaxDrawCount,               "MaxDrawCount"      );
        binder.AddMethod( &DrawIndirectCountCmd3::SetIndirectBuffer1,               "IndirectBuffer"    );
        binder.AddMethod( &DrawIndirectCountCmd3::SetIndirectBuffer2,               "IndirectBuffer"    );
        binder.AddMethod( &DrawIndirectCountCmd3::SetCountBuffer1,                  "CountBuffer"       );
        binder.AddMethod( &DrawIndirectCountCmd3::SetCountBuffer2,                  "CountBuffer"       );
        binder.AddProperty( &DrawIndirectCountCmd3::maxDrawCount,                   "maxDrawCount"      );
        binder.AddProperty( &DrawIndirectCountCmd3::stride,                         "stride"            );
    }

    void  ScriptUniGeometry::DrawIndexedIndirectCountCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawIndexedIndirectCountCmd3>    binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawIndexedIndirectCountCmd3::SetDynMaxDrawCount,        "MaxDrawCount"      );
        binder.AddMethod( &DrawIndexedIndirectCountCmd3::SetIndexBuffer1,           "IndexBuffer"       );
        binder.AddMethod( &DrawIndexedIndirectCountCmd3::SetIndexBuffer2,           "IndexBuffer"       );
        binder.AddMethod( &DrawIndexedIndirectCountCmd3::SetIndirectBuffer1,        "IndirectBuffer"    );
        binder.AddMethod( &DrawIndexedIndirectCountCmd3::SetIndirectBuffer2,        "IndirectBuffer"    );
        binder.AddMethod( &DrawIndexedIndirectCountCmd3::SetCountBuffer1,           "CountBuffer"       );
        binder.AddMethod( &DrawIndexedIndirectCountCmd3::SetCountBuffer2,           "CountBuffer"       );
        binder.AddProperty( &DrawIndexedIndirectCountCmd3::maxDrawCount,            "maxDrawCount"      );
        binder.AddProperty( &DrawIndexedIndirectCountCmd3::stride,                  "stride"            );
    }

    void  ScriptUniGeometry::DrawMeshTasksIndirectCountCmd3::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<DrawMeshTasksIndirectCountCmd3>  binder{ se };
        binder.CreateClassValue();
        binder.AddMethod( &DrawMeshTasksIndirectCountCmd3::SetDynMaxDrawCount,      "MaxDrawCount"      );
        binder.AddMethod( &DrawMeshTasksIndirectCountCmd3::SetIndirectBuffer1,      "IndirectBuffer"    );
        binder.AddMethod( &DrawMeshTasksIndirectCountCmd3::SetIndirectBuffer2,      "IndirectBuffer"    );
        binder.AddMethod( &DrawMeshTasksIndirectCountCmd3::SetCountBuffer1,         "CountBuffer"       );
        binder.AddMethod( &DrawMeshTasksIndirectCountCmd3::SetCountBuffer2,         "CountBuffer"       );
        binder.AddProperty( &DrawMeshTasksIndirectCountCmd3::maxDrawCount,          "maxDrawCount"      );
        binder.AddProperty( &DrawMeshTasksIndirectCountCmd3::stride,                "stride"            );
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

        binder.AddMethod( &ScriptUniGeometry::AddBuffer,    "Buffer"    );
        binder.AddMethod( &ScriptUniGeometry::AddTexture,   "Texture"   );

        binder.AddMethod( &ScriptUniGeometry::Draw1,        "Draw"      );
        binder.AddMethod( &ScriptUniGeometry::Draw2,        "Draw"      );
        binder.AddMethod( &ScriptUniGeometry::Draw3,        "Draw"      );
        binder.AddMethod( &ScriptUniGeometry::Draw4,        "Draw"      );
        binder.AddMethod( &ScriptUniGeometry::Draw5,        "Draw"      );
        binder.AddMethod( &ScriptUniGeometry::Draw6,        "Draw"      );
        binder.AddMethod( &ScriptUniGeometry::Draw7,        "Draw"      );
        binder.AddMethod( &ScriptUniGeometry::Draw8,        "Draw"      );
        binder.AddMethod( &ScriptUniGeometry::Draw9,        "Draw"      );
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
                    cmd.indexType           = src.indexType;
                    cmd.indexBufferPtr      = src.indexBuffer->ToResource();        CHECK_THROW( cmd.indexBufferPtr );
                    cmd.indexBufferOffset   = Bytes{src.indexBufferOffset};
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
                    cmd.indirectBufferPtr   = src.indirectBuffer->ToResource();     CHECK_THROW( cmd.indirectBufferPtr );
                    cmd.indirectBufferOffset= Bytes{src.indirectBufferOffset};
                    cmd.drawCount           = src.drawCount;
                    cmd.dynDrawCount        = src.dynDrawCount ? src.dynDrawCount->Get() : null;
                    cmd.stride              = Bytes{src.stride};
                    dst = cmd;
                },

                [&] (const DrawIndexedIndirectCmd3 &src) {
                    UnifiedGeometry::DrawIndexedIndirectCmd2    cmd;
                    cmd.indexType           = src.indexType;
                    cmd.indexBufferPtr      = src.indexBuffer->ToResource();        CHECK_THROW( cmd.indexBufferPtr );
                    cmd.indexBufferOffset   = Bytes{src.indexBufferOffset};
                    cmd.indirectBufferPtr   = src.indirectBuffer->ToResource();     CHECK_THROW( cmd.indirectBufferPtr );
                    cmd.indirectBufferOffset= Bytes{src.indirectBufferOffset};
                    cmd.drawCount           = src.drawCount;
                    cmd.dynDrawCount        = src.dynDrawCount ? src.dynDrawCount->Get() : null;
                    cmd.stride              = Bytes{src.stride};
                    dst = cmd;
                },

                [&] (const DrawMeshTasksIndirectCmd3 &src) {
                    UnifiedGeometry::DrawMeshTasksIndirectCmd2  cmd;
                    cmd.indirectBufferPtr   = src.indirectBuffer->ToResource();     CHECK_THROW( cmd.indirectBufferPtr );
                    cmd.indirectBufferOffset= Bytes{src.indirectBufferOffset};
                    cmd.drawCount           = src.drawCount;
                    cmd.dynDrawCount        = src.dynDrawCount ? src.dynDrawCount->Get() : null;
                    cmd.stride              = Bytes{src.stride};
                    dst = cmd;
                },

                [&] (const DrawIndirectCountCmd3 &src) {
                    UnifiedGeometry::DrawIndirectCountCmd2  cmd;
                    cmd.indirectBufferPtr   = src.indirectBuffer->ToResource();     CHECK_THROW( cmd.indirectBufferPtr );
                    cmd.indirectBufferOffset= Bytes{src.indirectBufferOffset};
                    cmd.countBufferPtr      = src.countBuffer->ToResource();        CHECK_THROW( cmd.countBufferPtr );
                    cmd.countBufferOffset   = Bytes{src.countBufferOffset};
                    cmd.maxDrawCount        = src.maxDrawCount;
                    cmd.dynMaxDrawCount     = src.dynMaxDrawCount ? src.dynMaxDrawCount->Get() : null;
                    cmd.stride              = Bytes{src.stride};
                    dst = cmd;
                },

                [&] (const DrawIndexedIndirectCountCmd3 &src) {
                    UnifiedGeometry::DrawIndexedIndirectCountCmd2   cmd;
                    cmd.indexType           = src.indexType;
                    cmd.indexBufferPtr      = src.indexBuffer->ToResource();        CHECK_THROW( cmd.indexBufferPtr );
                    cmd.indexBufferOffset   = Bytes{src.indexBufferOffset};
                    cmd.indirectBufferPtr   = src.indirectBuffer->ToResource();     CHECK_THROW( cmd.indirectBufferPtr );
                    cmd.indirectBufferOffset= Bytes{src.indirectBufferOffset};
                    cmd.countBufferPtr      = src.countBuffer->ToResource();        CHECK_THROW( cmd.countBufferPtr );
                    cmd.countBufferOffset   = Bytes{src.countBufferOffset};
                    cmd.maxDrawCount        = src.maxDrawCount;
                    cmd.dynMaxDrawCount     = src.dynMaxDrawCount ? src.dynMaxDrawCount->Get() : null;
                    cmd.stride              = Bytes{src.stride};
                    dst = cmd;
                },

                [&] (const DrawMeshTasksIndirectCountCmd3 &src) {
                    UnifiedGeometry::DrawMeshTasksIndirectCountCmd2 cmd;
                    cmd.indirectBufferPtr   = src.indirectBuffer->ToResource();     CHECK_THROW( cmd.indirectBufferPtr );
                    cmd.indirectBufferOffset= Bytes{src.indirectBufferOffset};
                    cmd.countBufferPtr      = src.countBuffer->ToResource();        CHECK_THROW( cmd.countBufferPtr );
                    cmd.countBufferOffset   = Bytes{src.countBufferOffset};
                    cmd.maxDrawCount        = src.maxDrawCount;
                    cmd.dynMaxDrawCount     = src.dynMaxDrawCount ? src.dynMaxDrawCount->Get() : null;
                    cmd.stride              = Bytes{src.stride};
                    dst = cmd;
                });
        }

        for (auto& [name, buf] : _meshes)
        {
            auto    res = buf->ToResource();
            CHECK_THROW( res );
            result->_meshes.emplace_back( UniformName{name}, res );
        }

        for (auto& [name, tex] : _textures)
        {
            auto    res = tex->ToResource();
            CHECK_THROW( res );
            result->_textures.emplace_back( UniformName{name}, res );
        }

        _geomSrc = result;
        return _geomSrc;
    }

/*
=================================================
    AddLayoutReflection
=================================================
*/
    void  ScriptUniGeometry::AddLayoutReflection () C_Th___
    {
        for (auto& [name, buf] : _meshes) {
            buf->AddLayoutReflection();
        }
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
            _FindPipelinesWithoutVB( OUT pipelines );
            _FindPipelinesByUB( "material", "UnifiedGeometryMaterialUB", INOUT pipelines ); // throw
            _FindPipelinesByTextures( "material", _textures, INOUT pipelines );             // throw
            _FindPipelinesByBuffers( "material", _meshes, INOUT pipelines );                // throw
            auto    tmp = _GetSuitablePipeline( pipelines );
            CHECK_THROW_MSG( not tmp.empty() );
            return tmp.front();
        }};

        const auto  GetGraphicsPipeline = [this] () -> PipelineName
        {{
            Array<GraphicsPipelineSpecPtr>  pipelines;
            _FindPipelinesWithoutVB( OUT pipelines );
            _FindPipelinesByUB( "material", "UnifiedGeometryMaterialUB", INOUT pipelines ); // throw
            _FindPipelinesByTextures( "material", _textures, INOUT pipelines );             // throw
            _FindPipelinesByBuffers( "material", _meshes, INOUT pipelines );                // throw
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
                CHECK_THROW( result->mtrDSIndex == GetDescSetBinding( res_mngr, ppln, DescriptorSetName{"material"} ));
                CHECK_THROW( result->passDSIndex == GetDescSetBinding( res_mngr, ppln, DescriptorSetName{"pass"} ));
            }else{
                ds_inited = true;
                CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->mtrDSIndex, OUT result->descSets.data(), result->descSets.size(),
                                                            ppln, DescriptorSetName{"material"} ));
                result->passDSIndex = GetDescSetBinding( res_mngr, ppln, DescriptorSetName{"pass"} );
            }
            result->pplns.push_back( ppln );
        }};

        const auto  FindGraphicsPpln = [&] (const PipelineName &name)
        {{
            auto    ppln = rtech->GetGraphicsPipeline( name );
            CHECK_THROW( ppln );

            if ( ds_inited ){
                CHECK_THROW( result->mtrDSIndex == GetDescSetBinding( res_mngr, ppln, DescriptorSetName{"material"} ));
                CHECK_THROW( result->passDSIndex == GetDescSetBinding( res_mngr, ppln, DescriptorSetName{"pass"} ));
            }else{
                ds_inited = true;
                CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->mtrDSIndex, OUT result->descSets.data(), result->descSets.size(),
                                                            ppln, DescriptorSetName{"material"} ));
                result->passDSIndex = GetDescSetBinding( res_mngr, ppln, DescriptorSetName{"pass"} );
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
    ScriptSceneGeometry::ScriptSceneGeometry () __Th___
    {}

    ScriptSceneGeometry::ScriptSceneGeometry (const String &filename) __Th___
    {
        _dbgName = Path{filename}.filename().replace_extension("").string().substr( 0, ResNameMaxLen );

        _scenePath = FileSystem::ToAbsolute( filename );

        CHECK_THROW_MSG( FileSystem::IsFile( _scenePath ),
            "File '"s << ToString(_scenePath) << "' is not exists" );
    }

/*
=================================================
    destructor
=================================================
*/
    ScriptSceneGeometry::~ScriptSceneGeometry ()
    {}

/*
=================================================
    Name
=================================================
*/
    void  ScriptSceneGeometry::Name (const String &name) __Th___
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
    SetTexturePrefix
=================================================
*/
    void  ScriptSceneGeometry::SetTexturePrefix (const String &value) __Th___
    {
        CHECK_THROW_MSG( not _geomSrc,
            "resource is already created, can not change texture prefix" );
        CHECK_THROW_MSG( _texPrefix.empty(), "already specified" );

        _texPrefix = value;
    }

/*
=================================================
    GetOpaqueRTGeometry
=================================================
*/
    ScriptRTGeometry*  ScriptSceneGeometry::GetOpaqueRTGeometry () __Th___
    {
        CHECK_THROW_MSG( not _geomSrc,
            "resource is already created, can not create RTScene" );

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
    ScriptRTGeometry*  ScriptSceneGeometry::GetTranslucentRTGeometry () __Th___
    {
        CHECK_THROW_MSG( not _geomSrc,
            "resource is already created, can not create RTGeometry" );

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
    SetTransform
=================================================
*/
    void  ScriptSceneGeometry::SetTransform (const packed_float4x4 &value) __Th___
    {
        CHECK_THROW_MSG( not _geomSrc,
            "resource is already created, can not set initial transform" );

        _initialTransform = float4x4{value};
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptSceneGeometry::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptSceneGeometry> binder{ se };
        binder.CreateRef( 0, False{"no ctor"} );
        ScriptGeomSource::_BindBase( binder );
        binder.AddFactoryCtor( &ScriptSceneGeometry_Ctor1 );

        binder.AddMethod( &ScriptSceneGeometry::Name,                       "Name"                  );
        binder.AddMethod( &ScriptSceneGeometry::SetTexturePrefix,           "TexturePrefix"         );
        binder.AddMethod( &ScriptSceneGeometry::SetTransform,               "InitialTransform"      );
        binder.AddMethod( &ScriptSceneGeometry::GetOpaqueRTGeometry,        "OpaqueRTGeometry"      );
        binder.AddMethod( &ScriptSceneGeometry::GetTranslucentRTGeometry,   "TranslucentRTGeometry" );
    }

/*
=================================================
    GetShaderTypes
=================================================
*/
    void  ScriptSceneGeometry::GetShaderTypes (INOUT CppStructsFromShaders &) __Th___
    {
    }

/*
=================================================
    ToGeomSource
=================================================
*/
    RC<IGeomSource>  ScriptSceneGeometry::ToGeomSource () __Th___
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

        _geomSrc = MakeRC<ModelGeomSource>( renderer, _intermScene, _initialTransform );
        return _geomSrc;
    }

/*
=================================================
    FindMaterialPipeline
=================================================
*/
    ScriptGeomSource::PipelineNames_t  ScriptSceneGeometry::FindMaterialPipeline () C_Th___
    {
        CHECK_THROW_MSG( _intermScene );

    #if 0
        ScriptGeomSource::PipelineNames_t   ppln_per_mtr;

        _intermScene->ForEachModel(
            [&ppln_per_mtr] (const ResLoader::IntermScene::ModelData &model)
            {
                auto    mesh    = model.levels[ uint(ResLoader::EDetailLevel::High) ].mesh;
                auto    mtr     = model.levels[ uint(ResLoader::EDetailLevel::High) ].mtr;

                CHECK_THROW_MSG( mesh and mtr );
                CHECK_THROW_MSG( mesh->Attribs() != null );
                CHECK_THROW_MSG( mesh->Topology() == EPrimitive::TriangleList );

                Array<GraphicsPipelineSpecPtr>  pipelines;
                _FindPipelinesByVB( _AttribsToVBName( *mesh->Attribs() ), OUT pipelines );  // throw
                _FindPipelinesBySB( "material", "ModelNodeArray", INOUT pipelines );        // throw
                _FindPipelinesBySB( "material", "ModelMeshArray", INOUT pipelines );        // throw
                _FindPipelinesBySB( "material", "ModelMaterialArray", INOUT pipelines );    // throw
                _FindPipelinesByPC( "draw_model.pc", INOUT pipelines );                     // throw
                _FindPipelinesByMaterial( "material", *mtr, INOUT pipelines );              // throw

                auto    tmp = _GetSuitablePipeline( pipelines );
                CHECK_THROW_MSG( tmp.size() == 1 );
                ppln_per_mtr.push_back( tmp[0] );
            });

        return ppln_per_mtr;
    #else

        Array<GraphicsPipelineSpecPtr>  pipelines;
        _FindPipelinesByVB( "VB{Posf3, Normf3, UVf2}", OUT pipelines );             // throw
    //  _FindPipelinesWithoutVB( OUT pipelines );
    //  _FindPipelinesBySB( "material", "ModelNodeArray", INOUT pipelines );        // throw
    //  _FindPipelinesBySB( "material", "ModelMeshArray", INOUT pipelines );        // throw
    //  _FindPipelinesBySB( "material", "ModelMaterialArray", INOUT pipelines );    // throw
        _FindPipelinesByPC( "ModelNode", INOUT pipelines );                         // throw
    //  _FindPipelinesByMaterial( "material", *mtr, INOUT pipelines );              // throw
        return _GetSuitablePipeline( pipelines );
    #endif
    }

/*
=================================================
    ToMaterial
=================================================
*/
    RC<IGSMaterials>  ScriptSceneGeometry::ToMaterial (RenderTechPipelinesPtr rtech, const PipelineNames_t &names) C_Th___
    {
        CHECK_THROW( _geomSrc );
        CHECK_THROW( rtech );
        CHECK_THROW( names.size() == 1 );

        auto    result      = MakeRC<ModelGeomSource::Material>();
        auto&   res_mngr    = RenderTaskScheduler().GetResourceManager();

        result->rtech   = rtech;
        result->ppln    = rtech->GetGraphicsPipeline( names[0] );
        CHECK_THROW( result->ppln );

        CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->mtrDSIndex, OUT result->descSets.data(), result->descSets.size(),
                                                    result->ppln, DescriptorSetName{"material"} ));

        result->passDSIndex = GetDescSetBinding( res_mngr, result->ppln, DescriptorSetName{"pass"} );

        return result;
    }

/*
=================================================
    _AttribsToVBName
=================================================
*/
    String  ScriptSceneGeometry::_AttribsToVBName (const ResLoader::IntermVertexAttribs &) __Th___
    {
        // TODO
        return "VB{Pos3_Norm3_UV2}";
    }


} // AE::ResEditor
