// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/FileStream.h"

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/EditorUI.h"

#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/EnumBinder.h"

#include "res_editor/Scripting/ScriptBasePass.cpp.h"
#include "res_editor/_data/cpp/types.h"

namespace AE::ResEditor
{   
namespace
{
} // namespace


/*
=================================================
    InputController
=================================================
*/
    void  ScriptScene::InputController (const ScriptBaseControllerPtr &value) __Th___
    {
        CHECK_THROW_MSG( value );

        _controller = value;
    }

/*
=================================================
    InputGeometry*
=================================================
*/
    void  ScriptScene::InputGeometry1 (const ScriptGeomSourcePtr &geom, const packed_float3 &pos) __Th___
    {
        CHECK_THROW_MSG( geom );

        _geomInstances.emplace_back( geom, pos );
    }

    void  ScriptScene::InputGeometry2 (const ScriptGeomSourcePtr &geom) __Th___
    {
        return InputGeometry1( geom, packed_float3{} );
    }

/*
=================================================
    AddGraphicsPass
=================================================
*/
    ScriptSceneGraphicsPass*  ScriptScene::AddGraphicsPass (const String &name) __Th___
    {
        CHECK_THROW_MSG( not _geomInstances.empty() );

        ++_passCount;
        return ScriptRC{ new ScriptSceneGraphicsPass{ ScriptScenePtr{this}, name }}.Detach();
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptScene::Bind (const ScriptEnginePtr &se) __Th___
    {
        using namespace Scripting;

        ClassBinder<ScriptScene>    binder{ se };
        binder.CreateRef();

        binder.AddMethod( &ScriptScene::InputGeometry1,     "Input"             );
        binder.AddMethod( &ScriptScene::InputGeometry2,     "Input"             );
        binder.AddMethod( &ScriptScene::InputController,    "Input"             );
        binder.AddMethod( &ScriptScene::AddGraphicsPass,    "AddGraphicsPass"   );
    }

/*
=================================================
    ToScene
=================================================
*/
    RC<SceneData>  ScriptScene::ToScene () __Th___
    {
        if ( _scene )
            return _scene;

        CHECK_THROW_MSG( _controller );
        CHECK_THROW_MSG( not _geomInstances.empty() );
        CHECK_THROW_MSG( _passCount > 0 );

        _scene = MakeRC<SceneData>();

        for (auto& src : _geomInstances)
        {
            auto    geom    = src.geom->ToGeomSource();  // throw
            CHECK_THROW( geom );

            auto&   dst     = _scene->_geomInstances.emplace_back();
            dst.position    = src.pos;
            dst.geometry    = geom;
        }

        return _scene;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ScriptSceneGraphicsPass::ScriptSceneGraphicsPass (ScriptScenePtr scene, const String &passName) __Th___ :
        ScriptBaseRenderPass{Default},
        _scene{scene}, _controller{_scene->GetController()}, _passName{passName}
    {
        _dbgName = passName;

        ScriptExe::ScriptPassApi::AddPass( ScriptBasePassPtr{this} );
    }

/*
=================================================
    AddPipeline
=================================================
*/
    void  ScriptSceneGraphicsPass::AddPipeline (const String &pplnFile) __Th___
    {
        Path    file_path = ScriptExe::ScriptPassApi::ToPipelinePath( Path{pplnFile} );  // throw

        if ( _uniquePplns.insert( file_path ).second )
            _pipelines.push_back( file_path );
    }

/*
=================================================
    AddPipelines
=================================================
*/
    void  ScriptSceneGraphicsPass::AddPipelines (const String &pplnsFolder) __Th___
    {
        Path    folder = ScriptExe::ScriptPassApi::ToPipelineFolder( Path{pplnsFolder} );  // throw

        for (auto& path : FileSystem::EnumRecursive( folder )) {
            if ( _uniquePplns.insert( path ).second )
                _pipelines.push_back( path );
        }
    }

/*
=================================================
    InputController
=================================================
*/
    void  ScriptSceneGraphicsPass::InputController (const ScriptBaseControllerPtr &value) __Th___
    {
        CHECK_THROW_MSG( value );

        _controller = value;
    }

/*
=================================================
    ToPass
=================================================
*/
    RC<IPass>  ScriptSceneGraphicsPass::ToPass () C_Th___
    {
        RC<SceneGraphicsPass>   result      = MakeRC<SceneGraphicsPass>();
        auto&                   res_mngr    = RenderTaskScheduler().GetResourceManager();
        Renderer&               renderer    = ScriptExe::ScriptPassApi::GetRenderer();  // throw
        auto&                   materials   = result->_materials;
        PipelineNames_t         ppln_names;

        result->_scene  = _scene->ToScene();                    // throw
        result->_rtech  = _CompilePipelines( OUT ppln_names );  // throw

        CHECK_THROW( ppln_names.size() == _scene->_geomInstances.size() );
        materials.reserve( ppln_names.size() );

        for (usize i = 0; i < ppln_names.size(); ++i)
        {
            const auto&     geom    = _scene->_geomInstances[i].geom;
            const auto&     names   = ppln_names[i];
            auto            mtr     = geom->ToMaterial( result->_rtech.rtech, names );  // throw
            CHECK_THROW( mtr );
            materials.push_back( mtr );
        }

        result->_rpDesc.renderPassName  = RenderPassName{"rp"};
        result->_rpDesc.subpassName     = SubpassName{"main"};
        result->_rpDesc.packId          = result->_rtech.packId;
        result->_rpDesc.layerCount      = 1_layer;

        result->_ubuffer = res_mngr.CreateBuffer( BufferDesc{ SizeOf<ShaderTypes::ScenePassUB>, EBufferUsage::Uniform | EBufferUsage::TransferDst },
                                                  "ScenePassUB", renderer.GetAllocator() );
        CHECK_ERR( result->_ubuffer );

        CHECK_THROW( res_mngr.CreateDescriptorSets( OUT result->_descSets.data(), result->_descSets.size(), result->_rtech.packId, DSLayoutName{"pass.ds"} ));

        for (usize i = 0; i < _output.size(); ++i)
        {
            auto&   src = _output[i];
            auto    rt  = src.rt->ToResource();
            CHECK_ERR( rt );

            ImageViewDesc   view;
            view.viewType       = EImage_2D;
            view.baseLayer      = src.layer;
            view.layerCount     = 1;
            view.baseMipmap     = src.mipmap;
            view.mipmapCount    = 1;

            rt = rt->CreateView( view, rt->GetName() );
            CHECK_ERR( rt );

            auto&   dst = result->_renderTargets.emplace_back();
            dst.name    = AttachmentName{src.name};
            dst.image   = rt;
            dst.clear   = src.clear;
        }
        CHECK_ERR( not result->_renderTargets.empty() );

        result->_dbgName    = this->_dbgName;
        result->_dbgColor   = this->_dbgColor;

        if ( this->_controller )
        {
            RC<DynamicDim>  dyn_dim;

            for (auto& img : _output) {
                if ( img.rt->IsMutableDimension() ) {
                    dyn_dim = img.rt->DynamicDimension();
                    break;
                }
            }
            if ( not dyn_dim )
                dyn_dim = MakeRC<DynamicDim>( _output.front().rt->Dimension2() );

            result->_controller = this->_controller->ToController( dyn_dim );  // throw
            CHECK_ERR( result->_controller );
        }

        // add debug modes
        EnumBitSet<IPass::EDebugMode>   dbg_modes;  // TODO
        UIInteraction::Instance().AddPassDbgInfo( result.get(), dbg_modes, EShaderStages::Fragment );

        _AddSlidersToUIInteraction( result.get() );
        _CopyConstants( OUT result->_shConst );

        AE_LOGI( "Compiled: "s << _dbgName );
        return result;
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptSceneGraphicsPass::Bind (const ScriptEnginePtr &se) __Th___
    {
        using namespace Scripting;

        ClassBinder<ScriptSceneGraphicsPass>    binder{ se };
        binder.CreateRef();
        _BindBaseRenderPass( binder, False{"withBlending"} );

        binder.AddMethod( &ScriptSceneGraphicsPass::InputController,    "Input"         );

        binder.AddMethod( &ScriptSceneGraphicsPass::AddPipeline,        "AddPipeline"   );
        binder.AddMethod( &ScriptSceneGraphicsPass::AddPipelines,       "AddPipelines"  );
    }

} // AE::ResEditor


#include "res_editor/Scripting/PassCommon.inl.h"

namespace AE::ResEditor
{
    using namespace AE::PipelineCompiler;

/*
=================================================
    _CreateUBType
=================================================
*/
    auto  ScriptSceneGraphicsPass::_CreateUBType () __Th___
    {
        auto&   obj_storage = *ObjectStorage::Instance();
        auto    it          = obj_storage.structTypes.find( "ScenePassUB" );

        if ( it != obj_storage.structTypes.end() )
            return it->second;

        ShaderStructTypePtr st{ new ShaderStructType{"ScenePassUB"}};
        st->Set( EStructLayout::Std140, R"#(
                // view //
                float2      resolution;
                float       time;

                // controller //
                CameraData  camera;

                // sliders //
                float4      floatSliders [4];
                int4        intSliders [4];
                float4      colors [4];

                // constants //
                float4      floatConst [4];
                int4        intConst [4];
            )#");

        STATIC_ASSERT( UIInteraction::MaxSlidersPerType == 4 );
        STATIC_ASSERT( IPass::Constants::MaxCount == 4 );
        return st;
    }

/*
=================================================
    GetShaderTypes
=================================================
*/
    void  ScriptSceneGraphicsPass::GetShaderTypes (INOUT CppStructsFromShaders &data) __Th___
    {
        auto    st = _CreateUBType();   // throw

        CHECK_THROW( st->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
    }

/*
=================================================
    _CompilePipelines2
=================================================
*/
    void  ScriptSceneGraphicsPass::_CompilePipelines2 (ScriptEnginePtr se, OUT PipelineNames_t &pplnNames) C_Th___
    {
        auto&           storage = *ObjectStorage::Instance();
        const String    subpass = "main";

        CompatibleRenderPassDescPtr compat_rp{ new CompatibleRenderPassDesc{ "compat.rp" }};
        compat_rp->AddFeatureSet( "Default" );
        compat_rp->AddSubpass( subpass );
        {
            for (usize i = 0; i < _output.size(); ++i)
            {
                RPAttachmentPtr     att     = compat_rp->AddAttachment2( _output[i].name );
                const auto          desc    = _output[i].rt->ToResource()->GetImageDesc();
                const bool          is_ds   = _output[i].rt->IsDepthStencil();

                att->format     = desc.format;
                att->samples    = desc.samples;
                att->AddUsage( subpass, is_ds ? EAttachment::DepthStencil : EAttachment::Color );
            }
        }{
            RenderPassSpecPtr   rp_spec = compat_rp->AddSpecialization2( "rp" );

            for (usize i = 0; i < _output.size(); ++i)
            {
                RPAttachmentSpecPtr att     = rp_spec->AddAttachment2( _output[i].name );
                const bool          is_ds   = _output[i].rt->IsDepthStencil();
                const auto          state   = is_ds ?
                                                EResourceState::DepthStencilAttachment_RW | EResourceState::DSTestBeforeFS | EResourceState::DSTestAfterFS :
                                                EResourceState::ColorAttachment_RW;

                att->loadOp     = EAttachmentLoadOp::Load;
                att->storeOp    = EAttachmentStoreOp::Store;

                if ( _output[i].HasClearValue() )
                {
                    att->loadOp = EAttachmentLoadOp::Clear;
                    att->AddLayout( "ExternalIn", EResourceState::Invalidate );
                }
                att->AddLayout( subpass, state );
            }
        }


        RenderTechniquePtr  rtech{ new RenderTechnique{ "rtech" }};
        {
            RTGraphicsPassPtr   pass = rtech->AddGraphicsPass2( subpass );
            pass->SetRenderPass( "rp", subpass );
        }

        {
            Unused( _CreateUBType() );  // throw

            DescriptorSetLayoutPtr  ds_layout{ new DescriptorSetLayout{ "pass.ds" }};
            ds_layout->AddFeatureSet( "Default" );

            ds_layout->AddUniformBuffer( uint(EShaderStages::AllGraphics), "passUB", ArraySize{1}, "ScenePassUB", EResourceState::ShaderUniform );
        }

        for (auto& inst : _scene->_geomInstances) {
            inst.geom->AddLayoutReflection();  // throw
        }

        for (auto& ppln : _pipelines) {
            if ( not storage.CompilePipeline( se, ppln ))
                continue;
        }

        CHECK_THROW( not storage.gpipelines.empty() or
                     not storage.mpipelines.empty() );

        pplnNames.reserve( _scene->_geomInstances.size() );

        for (auto& inst : _scene->_geomInstances)
        {
            auto    names = inst.geom->FindMaterialPipeline();  // throw
            CHECK_THROW( not names.empty() );
            pplnNames.push_back( RVRef(names) );
        }
    }

/*
=================================================
    _CompilePipelines
=================================================
*/
    RTechInfo  ScriptSceneGraphicsPass::_CompilePipelines (OUT PipelineNames_t &pplnNames) C_Th___
    {
        return ScriptExe::ScriptPassApi::ConvertAndLoad(
                    [this, &pplnNames] (ScriptEnginePtr se){ _CompilePipelines2( se, OUT pplnNames ); });
    }


} // AE::ResEditor
