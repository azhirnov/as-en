// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Passes/BuildRTAS.h"

#include "res_editor/Core/EditorUI.h"
#include "res_editor/Scripting/PassCommon.inl.h"

namespace AE::ResEditor
{
namespace {
    static ScriptExe*  s_scriptExe = null;
}

    using namespace AE::Scripting;
    using namespace AE::PipelineCompiler;


    //
    // Present Pass
    //
    class ScriptExe::ScriptPresent final : public ScriptBasePass
    {
    private:
        ScriptImagePtr      rt;
        ImageLayer          layer;
        MipmapLevel         mipmap;
        RC<DynamicDim>      dynSize;

    public:
        ScriptPresent (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap, RC<DynamicDim> dynSize) :
            ScriptBasePass{EFlags::Unknown}, rt{rt}, layer{layer}, mipmap{mipmap}, dynSize{dynSize} {}

        RC<IPass>  ToPass () C_Th_OV;

        void  _OnAddArg (INOUT ScriptPassArgs::Argument &)  C_Th_OV {}
    };

/*
=================================================
    ScriptPresent::ToPass
=================================================
*/
    RC<IPass>  ScriptExe::ScriptPresent::ToPass () C_Th___
    {
        Array< RC<Image> >  src;

        ImageViewDesc   desc;
        desc.baseLayer  = layer;
        desc.baseMipmap = mipmap;

        RC<Image>   img = rt->ToResource();
        CHECK_ERR( img );

        img = img->CreateView( desc, "PresentSrc" );
        CHECK_ERR( img );

        src.push_back( img );

        auto    fmode = UIInteraction::Instance().GetFilterMode();

        return MakeRC<ResEditor::Present>( RVRef(src), "Present", dynSize, fmode );
    }
//-----------------------------------------------------------------------------



    //
    // Dbg View Pass
    //
    class ScriptExe::ScriptDbgView final : public ScriptBasePass
    {
    private:
        ScriptImagePtr      rt;
        ImageLayer          layer;
        MipmapLevel         mipmap;
        DebugView::EFlags   flags;
        uint                index;

    public:
        ScriptDbgView (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap, DebugView::EFlags flags, uint idx) :
            ScriptBasePass{EFlags::Unknown}, rt{rt}, layer{layer}, mipmap{mipmap}, flags{flags}, index{idx} {}

        RC<IPass>  ToPass () C_Th_OV;

        void  _OnAddArg (INOUT ScriptPassArgs::Argument &)  C_Th_OV {}
    };

/*
=================================================
    ScriptDbgView::ToPass
=================================================
*/
    RC<IPass>  ScriptExe::ScriptDbgView::ToPass () C_Th___
    {
        return MakeRC<ResEditor::DebugView>( rt->ToResource(), index, flags, layer, mipmap,
                                             s_scriptExe->_GetRenderer(), "DbgView" );
    }
//-----------------------------------------------------------------------------



    //
    // Generate Mipmaps Pass
    //
    class ScriptExe::ScriptGenMipmaps final : public ScriptBasePass
    {
    private:
        ScriptImagePtr      rt;

    public:
        ScriptGenMipmaps (const ScriptImagePtr &rt) : ScriptBasePass{EFlags::Unknown}, rt{rt} {}

        RC<IPass>  ToPass () C_Th_OV;

        void  _OnAddArg (INOUT ScriptPassArgs::Argument &)  C_Th_OV {}
    };

/*
=================================================
    ScriptGenMipmaps::ToPass
=================================================
*/
    RC<IPass>  ScriptExe::ScriptGenMipmaps::ToPass () C_Th___
    {
        return MakeRC<ResEditor::GenerateMipmapsPass>( rt->ToResource(), "GenMipmaps" );
    }
//-----------------------------------------------------------------------------



    //
    // Copy Image Pass
    //
    class ScriptExe::ScriptCopyImage final : public ScriptBasePass
    {
    private:
        ScriptImagePtr      src;
        ScriptImagePtr      dst;

    public:
        ScriptCopyImage (const ScriptImagePtr &src, const ScriptImagePtr &dst) :
            ScriptBasePass{EFlags::Unknown}, src{src}, dst{dst} {}

        RC<IPass>  ToPass () C_Th_OV;

        void  _OnAddArg (INOUT ScriptPassArgs::Argument &)  C_Th_OV {}
    };

/*
=================================================
    ScriptCopyImage::ToPass
=================================================
*/
    RC<IPass>  ScriptExe::ScriptCopyImage::ToPass () C_Th___
    {
        return MakeRC<ResEditor::CopyImagePass>( src->ToResource(), dst->ToResource(), "CopyImage" );
    }
//-----------------------------------------------------------------------------



    //
    // Clear Image Pass
    //
    class ScriptExe::ScriptClearImage final : public ScriptBasePass
    {
    private:
        ScriptImagePtr                  image;
        ClearImagePass::ClearValue_t    value;

    public:
        ScriptClearImage (const ScriptImagePtr &image, ClearImagePass::ClearValue_t value) :
            ScriptBasePass{EFlags::Unknown}, image{image}, value{value} {}

        RC<IPass>  ToPass () C_Th_OV;

        void  _OnAddArg (INOUT ScriptPassArgs::Argument &)  C_Th_OV {}
    };

/*
=================================================
    ScriptClearImage::ToPass
=================================================
*/
    RC<IPass>  ScriptExe::ScriptClearImage::ToPass () C_Th___
    {
        return MakeRC<ResEditor::ClearImagePass>( image->ToResource(), value, "ClearImage" );
    }
//-----------------------------------------------------------------------------



    //
    // Clear Buffer Pass
    //
    class ScriptExe::ScriptClearBuffer final : public ScriptBasePass
    {
    private:
        ScriptBufferPtr     buffer;
        uint                value;

    public:
        ScriptClearBuffer (const ScriptBufferPtr &buffer, uint value) :
            ScriptBasePass{EFlags::Unknown}, buffer{buffer}, value{value} {}

        RC<IPass>  ToPass () C_Th_OV;

        void  _OnAddArg (INOUT ScriptPassArgs::Argument &)  C_Th_OV {}
    };

/*
=================================================
    ScriptClearBuffer::ToPass
=================================================
*/
    RC<IPass>  ScriptExe::ScriptClearBuffer::ToPass () C_Th___
    {
        return MakeRC<ResEditor::ClearBufferPass>( buffer->ToResource(), value, "ClearBuffer" );
    }
//-----------------------------------------------------------------------------



    //
    // Build Ray Tracing Geometry Pass
    //
    class ScriptExe::ScriptBuildRTGeometry final : public ScriptBasePass
    {
    private:
        ScriptRTGeometryPtr _dstGeometry;
        const bool          _indirect;


    public:
        ScriptBuildRTGeometry (ScriptRTGeometryPtr  dstGeometry,
                               bool                 indirect) :
            ScriptBasePass{EFlags::Unknown},
            _dstGeometry{ dstGeometry },
            _indirect{ indirect }
        {}

        RC<IPass>  ToPass () C_Th_OV;

        void  _OnAddArg (INOUT ScriptPassArgs::Argument &)  C_Th_OV {}
    };

/*
=================================================
    ScriptBuildRTGeometry::ToPass
=================================================
*/
    RC<IPass>  ScriptExe::ScriptBuildRTGeometry::ToPass () C_Th___
    {
        CHECK_THROW( s_scriptExe != null );

        RC<RTGeometry>  dst_geom;
        s_scriptExe->_RunWithPipelineCompiler(
            [&] () {
                dst_geom = _dstGeometry->ToResource();
            });

        return MakeRC<ResEditor::BuildRTGeometry>(
                    dst_geom, _indirect,
                    "BuildRTGeometry" );
    }
//-----------------------------------------------------------------------------



    //
    // Build Ray Tracing Scene Pass
    //
    class ScriptExe::ScriptBuildRTScene final : public ScriptBasePass
    {
    private:
        ScriptRTScenePtr    _dstScene;
        const bool          _indirect;


    public:
        ScriptBuildRTScene (ScriptRTScenePtr    dstScene,
                            bool                indirect) :
            ScriptBasePass{EFlags::Unknown},
            _dstScene{ dstScene },
            _indirect{ indirect }
        {}

        RC<IPass>  ToPass () C_Th_OV;

        void  _OnAddArg (INOUT ScriptPassArgs::Argument &)  C_Th_OV {}
    };

/*
=================================================
    ScriptBuildRTScene::ToPass
=================================================
*/
    RC<IPass>  ScriptExe::ScriptBuildRTScene::ToPass () C_Th___
    {
        CHECK_THROW( s_scriptExe != null );

        RC<RTScene>     dst_scene;
        s_scriptExe->_RunWithPipelineCompiler(
            [&] () {
                dst_scene = _dstScene->ToResource();
            });

        return MakeRC<ResEditor::BuildRTScene>(
                    dst_scene, _indirect,
                    "BuildRTScene" );
    }
//-----------------------------------------------------------------------------



    //
    // Pass Group
    //
    class ScriptExe::ScriptPassGroup final : public ScriptBasePass
    {
    private:
        Array< ScriptBasePassPtr >  _passes;
        const PassGroup::EFlags     _flags;
        RC<Renderer>                _renderer;
        mutable RC<IPass>           _result;

    public:
        ScriptPassGroup (PassGroup::EFlags flags, RC<Renderer> renderer) :
            ScriptBasePass{EFlags::Unknown}, _flags{flags}, _renderer{RVRef(renderer)}
        {}

            void  Add (ScriptBasePassPtr pass)                          { _passes.push_back( RVRef(pass) ); }
            void  _OnAddArg (INOUT ScriptPassArgs::Argument &)  C_Th_OV {}

        ND_ ArrayView<ScriptBasePassPtr>    GetPasses ()        const   { return _passes; }
        ND_ RC<IPass>                       ToPass ()           C_Th_OV;
    };

/*
=================================================
    ScriptPassGroup::ToPass
=================================================
*/
    RC<IPass>  ScriptExe::ScriptPassGroup::ToPass () C_Th___
    {
        if ( _result )
            return _result;

        CHECK_THROW( not _passes.empty() );

        RC<PassGroup>   pg = MakeRC<PassGroup>( _flags, _renderer->GetResourceQueue() );

        for (auto& script_pass : _passes) {
            pg->AddPass( script_pass->ToPass() );  // throw
        }

        _result = pg;
        return pg;
    }
//-----------------------------------------------------------------------------



    struct ScriptExe::SamplerConsts
    {
        const String    NearestClamp            {"NearestClamp"};
        const String    NearestRepeat           {"NearestRepeat"};
        const String    NearestMirrorRepeat     {"NearestMirrorRepeat"};
        const String    LinearClamp             {"LinearClamp"};
        const String    LinearRepeat            {"LinearRepeat"};
        const String    LinearMirrorRepeat      {"LinearMirrorRepeat"};
        const String    LinearMipmapClamp       {"LinearMipmapClamp"};
        const String    LinearMipmapRepeat      {"LinearMipmapRepeat"};
        const String    LinearMipmapMirrorRepeat{"LinearMipmapMirrorRepeat"};
        const String    Anisotropy8Repeat       {"Anisotropy8Repeat"};
        const String    Anisotropy8MirrorRepeat {"Anisotropy8MirrorRepeat"};
        const String    Anisotropy8Clamp        {"Anisotropy8Clamp"};
        const String    Anisotropy16Repeat      {"Anisotropy16Repeat"};
        const String    Anisotropy16MirrorRepeat{"Anisotropy16MirrorRepeat"};
        const String    Anisotropy16Clamp       {"Anisotropy16Clamp"};
    };
/*
=================================================
    constructor
=================================================
*/
    ScriptExe::ScriptExe (Config cfg) :
        _sampConsts{ new SamplerConsts{} },
        _config{ RVRef(cfg) }
    {
        EXLOCK( _guard );

        s_scriptExe = this;

        try{
            // bind RE scripts
            _engine = MakeRC<ScriptEngine>();
            CHECK_THROW( _engine->Create( True{"gen cpp header"} ));

            // init pipeline compiler
            ObjectStorage   obj_storage;
            {
                obj_storage.target              = ECompilationTarget::Vulkan;
                obj_storage.shaderVersion       = EShaderVersion::SPIRV_1_5;
                obj_storage.defaultFeatureSet   = "DefaultFS";

                obj_storage.spirvCompiler       = MakeUnique<SpirvCompiler>( Array<Path>{} );
                obj_storage.spirvCompiler->SetDefaultResourceLimits();

                ObjectStorage::SetInstance( &obj_storage );

                ScriptFeatureSetPtr fs {new ScriptFeatureSet{ "DefaultFS" }};
                fs->fs = RenderTaskScheduler().GetFeatureSet();
            }

            // bind pipeline compiler scripts
            _engine2 = MakeRC<ScriptEngine>();
            ObjectStorage::Bind( _engine2 );
            _Bind_Constants( _engine2 );

            ObjectStorage::Bind_EImageType( _engine );

            // save RE shader types as C++ types
            if ( not _config.cppTypesFolder.empty() )
            {
                FileSystem::CreateDirectory( _config.cppTypesFolder );
                _SaveCppStructs( _config.cppTypesFolder / "vk_types.h" );   // TODO: metal?
            }

            if ( FileSystem::IsDirectory( _config.scriptHeaderOutFolder ))
                CHECK( _engine2->SaveCppHeader( _config.scriptHeaderOutFolder / "pipeline_compiler.as" ));

            ObjectStorage::SetInstance( null );

            CoreBindings::BindString( _engine );
            _Bind_Constants( _engine );
            _Bind( _engine, _config );
        }
        catch(...)
        {
            _engine  = null;
            _engine2 = null;

            AE_LOGE( "Failed to initialize script executor" );
        }
    }

/*
=================================================
    destructor
=================================================
*/
    ScriptExe::~ScriptExe ()
    {
        EXLOCK( _guard );
        s_scriptExe = null;
    }

/*
=================================================
    Run
=================================================
*/
    RC<Renderer>  ScriptExe::Run (const Path &filePath, const ScriptConfig &cfg) __NE___
    {
        AE_LOGI( "------------------------------------------" );

        ASSERT( _guard.is_unlocked() );
        EXLOCK( _guard );
        CHECK_ERR( not _tempData );

        _tempData.reset( new TempData{} );
        _tempData->renderer = MakeRC<Renderer>( _rand.Uniform( 0u, 0xFFFF'FFFFu ));
        _tempData->currPath.push_back( FileSystem::ToAbsolute( filePath ));
        _tempData->dependencies.push_back( _tempData->currPath.front() );
        _tempData->cfg = cfg;

        for (auto& p : _tempData->cfg.shaderDirs) {
            p = FileSystem::ToAbsolute( p );
        }

        RC<Renderer>    result;

        if ( _Run2( filePath ))
        {
            result = _tempData->renderer;
            //result->SetDependencies( RVRef(_tempData->dependencies) );

            _AddSlidersToUIInteraction( *_tempData, result.get() );
        }

        _tempData.reset();
        return result;
    }

/*
=================================================
    _AddSlidersToUIInteraction
=================================================
*/
    void  ScriptExe::_AddSlidersToUIInteraction (TempData &data, Renderer* renderer)
    {
        if ( data.sliders.empty() )
            return;

        UIInteraction::PerPassSlidersInfo   info;
        SliderCounter_t                     slider_idx {};
        Renderer::Sliders_t                 dyn_sliders;

        for (const auto& slider : data.sliders)
        {
            const uint  idx = slider_idx[ uint(slider.type) ]++;
            BEGIN_ENUM_CHECKS();
            switch ( slider.type )
            {
                case ESlider::Int :
                    info.intRange [idx][0]                          = slider.intRange[0];
                    info.intRange [idx][1]                          = slider.intRange[1];
                    info.intRange [idx][2]                          = slider.intRange[2];
                    info.intVecSize [idx]                           = slider.count;
                    info.names [idx][UIInteraction::IntSliderIdx]   = slider.name;
                    dyn_sliders.push_back( slider.dyn );
                    break;

                case ESlider::Float :
                    info.floatRange [idx][0]                        = slider.floatRange[0];
                    info.floatRange [idx][1]                        = slider.floatRange[1];
                    info.floatRange [idx][2]                        = slider.floatRange[2];
                    info.floatVecSize [idx]                         = slider.count;
                    info.names [idx][UIInteraction::FloatSliderIdx] = slider.name;
                    dyn_sliders.push_back( slider.dyn );
                    break;

                case ESlider::Color :
                case ESlider::_Count :
                    break;
            }
            END_ENUM_CHECKS();
        }

        info.passName = "Global";
        UIInteraction::Instance().AddSliders( renderer, RVRef(info) );

        renderer->SetSliders( RVRef(dyn_sliders) );
    }

/*
=================================================
    _RunScript*
=================================================
*/
    ScriptBasePass*  ScriptExe::_RunScript1 (const String &filePath, const ScriptCollectionPtr &collection) __Th___
    {
        return _RunScript2( filePath, Default, collection );
    }

    ScriptBasePass*  ScriptExe::_RunScript2 (const String &filePath, PassGroup::EFlags flags, const ScriptCollectionPtr &collection) __Th___
    {
        CHECK_THROW_MSG( collection );

        auto&       data     = _GetTempData();
        const auto  path     = Path{data.cfg.scriptDir} / filePath;

        CHECK_THROW_MSG( data.passGroup );
        CHECK_THROW_MSG( FileSystem::IsFile( path ),
            "script '"s << filePath << "' is not exists" );

        ScriptPassGroupPtr  pg {new ScriptPassGroup{ flags, data.renderer }};
        ScriptPassGroupPtr  prev = data.passGroup;
        data.passGroup = pg;

        data.currPath.push_back( FileSystem::ToAbsolute( path ));

        CHECK_THROW_MSG( s_scriptExe->_Run( path, collection ),
            "Failed to run script '"s << filePath << "'" );

        data.currPath.pop_back();
        data.passGroup = prev;
        data.passGroup->Add( pg );

        return pg.Detach();
    }

/*
=================================================
    _Run2
=================================================
*/
    bool  ScriptExe::_Run2 (const Path &filePath) __NE___
    {
        try
        {
            CHECK_ERR( _tempData->passGroup == null );

            ScriptPassGroupPtr  pg {new ScriptPassGroup{ PassGroup::EFlags::Unknown, _tempData->renderer }};
            _tempData->passGroup = pg;

            if ( not _Run( filePath, null ))
                return false;

            for (auto& script_pass : pg->GetPasses())
            {
                auto    pass = script_pass->ToPass();   // throw
                CHECK_ERR( pass );

                if ( auto cont = pass->GetController())
                    _tempData->renderer->SetController( RVRef(cont) );  // throw

                _tempData->renderer->AddPass( pass );   // throw
            }

            return true;
        }
        catch(...)
        {}
        return false;
    }

/*
=================================================
    _Run
=================================================
*/
    bool  ScriptExe::_Run (const Path &filePath, const ScriptCollectionPtr &collection) __NE___
    {
        CHECK_ERR( _engine );

        _tempData->passGroupDepth++;

        const String                ansi_path = ToString( filePath );
        ScriptEngine::ModuleSource  src;
        {
            FileRStream     file {filePath};

            if ( not file.IsOpen() )
                RETURN_ERR( "Failed to open script file: '"s << ansi_path << "'" );

            src.name = ToString( filePath.filename().replace_extension("") );

            if ( not file.Read( file.RemainingSize(), OUT src.script ))
                RETURN_ERR( "Failed to read script file: '"s << ansi_path << "'" );
        }

        src.dbgLocation     = SourceLoc{ ansi_path, 0 };
        src.usePreprocessor = true;

        ScriptModulePtr     module = _engine->CreateModule( {src}, {"SCRIPT"} );
        if ( not module )
            RETURN_ERR( "Failed to parse script file: '"s << ansi_path << "'" );

        if ( collection     or
             module->HasFunction< void (ScriptCollectionPtr) >( "ASmain" )) // callable script can be used too
        {
            auto    fn = _engine->CreateScript< void (ScriptCollectionPtr) >( "ASmain", module );
            if ( not fn )
                RETURN_ERR( "Failed to create script context for file: '"s << ansi_path << "'" );

            if ( not fn->Run( collection ))
                RETURN_ERR( "Failed to run script '"s << ansi_path << "'" );
        }
        else
        {
            auto    fn = _engine->CreateScript< void () >( "ASmain", module );
            if ( not fn )
                RETURN_ERR( "Failed to create script context for file: '"s << ansi_path << "'" );

            if ( not fn->Run() )
                RETURN_ERR( "Failed to run script '"s << ansi_path << "'" );
        }

        _tempData->passGroupDepth--;
        _tempData->passGroup = null;
        return true;
    }

/*
=================================================
    _ConvertAndLoad
=================================================
*/
    RTechInfo  ScriptExe::_ConvertAndLoad () __Th___
    {
        auto    mem3     = _ConvertAndLoad2();
        auto&   res_mngr = RenderTaskScheduler().GetResourceManager();

        PipelinePackDesc    desc;
        desc.stream         = mem3;

        GAutorelease    pack_id = res_mngr.LoadPipelinePack( desc );
        CHECK_THROW( pack_id );

        auto            rtech   = res_mngr.LoadRenderTech( pack_id, RenderTechName{"rtech"}, Default );
        CHECK_THROW( rtech );

        return RTechInfo{ pack_id.Release(), rtech };
    }
//-----------------------------------------------------------------------------



/*
=================================================
    _GetTempData
=================================================
*/
    ScriptExe::TempData&  ScriptExe::_GetTempData () __Th___
    {
        CHECK_THROW_MSG( s_scriptExe != null );
        CHECK_THROW_MSG( s_scriptExe->_tempData != null );
        return *s_scriptExe->_tempData;
    }

/*
=================================================
    _SurfaceSize
=================================================
*/
    ScriptDynamicDim*  ScriptExe::_SurfaceSize () __Th___
    {
        ScriptDynamicDimPtr result{new ScriptDynamicDim{ _GetTempData().cfg.dynSize }};
        return result.Detach();
    }

/*
=================================================
    _Present*
=================================================
*/
    void  ScriptExe::_Present1 (const ScriptImagePtr &rt) __Th___
    {
        return _Present6( rt, ImageLayer{}, MipmapLevel{}, Default );
    }

    void  ScriptExe::_Present2 (const ScriptImagePtr &rt, const MipmapLevel &mipmap) __Th___
    {
        return _Present6( rt, ImageLayer{}, mipmap, Default );
    }

    void  ScriptExe::_Present3 (const ScriptImagePtr &rt, const ImageLayer &layer) __Th___
    {
        return _Present6( rt, layer, MipmapLevel{}, Default );
    }

    void  ScriptExe::_Present4 (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap) __Th___
    {
        return _Present6( rt, layer, mipmap, Default );
    }

    void  ScriptExe::_Present5 (const ScriptImagePtr &rt, EColorSpace cs) __Th___
    {
        return _Present6( rt, ImageLayer{}, MipmapLevel{}, cs );
    }

    void  ScriptExe::_Present6 (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap, EColorSpace cs) __Th___
    {
        CHECK_THROW_MSG( rt );

        rt->AddUsage( EResourceUsage::Present );

        auto&   data = _GetTempData();
        CHECK_THROW_MSG( data.passGroupDepth == 1, "'Present()' must be used in main script" );
        CHECK_THROW_MSG( not data.hasPresent, "'Present()' must be used once" );

        data.hasPresent = true;
        data.renderer->SetSurfaceFormat( cs != Default ? ESurfaceFormat_Cast( rt->Description().format, cs ) : Default );

        data.passGroup->Add( ScriptBasePassPtr{ new ScriptPresent{ rt, layer, mipmap, data.cfg.dynSize }});
    }

/*
=================================================
    _GenMipmaps
=================================================
*/
    void  ScriptExe::_GenMipmaps (const ScriptImagePtr &rt) __Th___
    {
        CHECK_THROW_MSG( rt );
        rt->AddUsage( EResourceUsage::GenMipmaps );

        auto&   data = _GetTempData();
        CHECK_THROW_MSG( data.passGroup );

        data.passGroup->Add( ScriptBasePassPtr{ new ScriptGenMipmaps{ rt }});
    }

/*
=================================================
    _CopyImage
=================================================
*/
    void  ScriptExe::_CopyImage (const ScriptImagePtr &src, const ScriptImagePtr &dst) __Th___
    {
        CHECK_THROW_MSG( src and dst );
        src->AddUsage( EResourceUsage::Transfer );
        dst->AddUsage( EResourceUsage::Transfer );

        auto&   data = _GetTempData();
        CHECK_THROW_MSG( data.passGroup );

        data.passGroup->Add( ScriptBasePassPtr{ new ScriptCopyImage{ src, dst }});
    }

/*
=================================================
    _ClearImage*
=================================================
*/
    void  ScriptExe::_ClearImage1 (const ScriptImagePtr &image, const RGBA32f &value) __Th___
    {
        CHECK_THROW_MSG( image );
        image->AddUsage( EResourceUsage::Transfer );

        auto&   data = _GetTempData();
        CHECK_THROW_MSG( data.passGroup );

        data.passGroup->Add( ScriptBasePassPtr{ new ScriptClearImage{ image, value }});
    }

    void  ScriptExe::_ClearImage2 (const ScriptImagePtr &image, const RGBA32u &value) __Th___
    {
        CHECK_THROW_MSG( image );
        image->AddUsage( EResourceUsage::Transfer );

        auto&   data = _GetTempData();
        CHECK_THROW_MSG( data.passGroup );

        data.passGroup->Add( ScriptBasePassPtr{ new ScriptClearImage{ image, value }});
    }

    void  ScriptExe::_ClearImage3 (const ScriptImagePtr &image, const RGBA32i &value) __Th___
    {
        CHECK_THROW_MSG( image );
        image->AddUsage( EResourceUsage::Transfer );

        auto&   data = _GetTempData();
        CHECK_THROW_MSG( data.passGroup );

        data.passGroup->Add( ScriptBasePassPtr{ new ScriptClearImage{ image, value }});
    }

/*
=================================================
    _ClearBuffer
=================================================
*/
    void  ScriptExe::_ClearBuffer (const ScriptBufferPtr &buf, uint value) __Th___
    {
        CHECK_THROW_MSG( buf );

        auto&   data = _GetTempData();
        CHECK_THROW_MSG( data.passGroup );

        data.passGroup->Add( ScriptBasePassPtr{ new ScriptClearBuffer{ buf, value }});
    }

/*
=================================================
    _BuildRTGeometry*
=================================================
*/
    void  ScriptExe::_BuildRTGeometry (const ScriptRTGeometryPtr &geom) __Th___
    {
        CHECK_THROW_MSG( geom );

        auto&   data = _GetTempData();
        CHECK_THROW_MSG( data.passGroup );

        data.passGroup->Add( ScriptBasePassPtr{ new ScriptBuildRTGeometry{ geom, false }});
    }

    void  ScriptExe::_BuildRTGeometryIndirect (const ScriptRTGeometryPtr &geom) __Th___
    {
        CHECK_THROW_MSG( geom );
        CHECK_THROW_MSG( geom->HasIndirectBuffer() );

        if ( RenderTaskScheduler().GetFeatureSet().accelerationStructureIndirectBuild != EFeature::RequireTrue )
        {
            CHECK_THROW_MSG( geom->WithHistory() );
        }

        auto&   data = _GetTempData();
        CHECK_THROW_MSG( data.passGroup );

        data.passGroup->Add( ScriptBasePassPtr{ new ScriptBuildRTGeometry{ geom, true }});
    }

/*
=================================================
    _BuildRTScene*
=================================================
*/
    void  ScriptExe::_BuildRTScene (const ScriptRTScenePtr &scene) __Th___
    {
        CHECK_THROW_MSG( scene );

        auto&   data = _GetTempData();
        CHECK_THROW_MSG( data.passGroup );

        data.passGroup->Add( ScriptBasePassPtr{ new ScriptBuildRTScene{ scene, false }});
    }

    void  ScriptExe::_BuildRTSceneIndirect (const ScriptRTScenePtr &scene) __Th___
    {
        CHECK_THROW_MSG( scene );
        CHECK_THROW_MSG( scene->HasIndirectBuffer() );

        if ( RenderTaskScheduler().GetFeatureSet().accelerationStructureIndirectBuild != EFeature::RequireTrue )
        {
            CHECK_THROW_MSG( scene->WithHistory() );
        }

        auto&   data = _GetTempData();
        CHECK_THROW_MSG( data.passGroup );

        data.passGroup->Add( ScriptBasePassPtr{ new ScriptBuildRTScene{ scene, true }});
    }

/*
=================================================
    _DbgView*
=================================================
*/
    void  ScriptExe::_DbgView1 (const ScriptImagePtr &rt, DebugView::EFlags flags) __Th___
    {
        return _DbgView4( rt, ImageLayer{}, MipmapLevel{}, flags );
    }

    void  ScriptExe::_DbgView2 (const ScriptImagePtr &rt, const MipmapLevel &mipmap, DebugView::EFlags flags) __Th___
    {
        return _DbgView4( rt, ImageLayer{}, mipmap, flags );
    }

    void  ScriptExe::_DbgView3 (const ScriptImagePtr &rt, const ImageLayer &layer, DebugView::EFlags flags) __Th___
    {
        return _DbgView4( rt, layer, MipmapLevel{}, flags );
    }

    void  ScriptExe::_DbgView4 (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap, DebugView::EFlags flags) __Th___
    {
        CHECK_THROW_MSG( rt );

        auto&   data    = _GetTempData();
        auto    idx     = data.dbgViewCounter++;

        CHECK_THROW_MSG( idx < UIInteraction::MaxDebugViews );

        BEGIN_ENUM_CHECKS();
        switch ( flags )
        {
            case DebugView::EFlags::Copy :          rt->AddUsage( EResourceUsage::Transfer );       break;
            case DebugView::EFlags::NoCopy :        rt->AddUsage( EResourceUsage::Present );        break;
            case DebugView::EFlags::Histogram :     rt->AddUsage( EResourceUsage::Sampled );        break;
            case DebugView::EFlags::LinearDepth :   rt->AddUsage( EResourceUsage::Sampled );        break;
            case DebugView::EFlags::Stencil :       rt->AddUsage( EResourceUsage::DepthStencil );   break;
            case DebugView::EFlags::_Count :
            default :                               CHECK_THROW_MSG( false, "unsupported flags" );
        }
        END_ENUM_CHECKS();

        data.passGroup->Add( ScriptBasePassPtr{ new ScriptDbgView{ rt, layer, mipmap, flags, idx }});
    }

/*
=================================================
    _Slider
=================================================
*/
    template <typename D, typename T>
    void  ScriptExe::_Slider (const D &dyn, const String &name, const T &min, const T &max, const T &initial, ESlider type) __Th___
    {
        auto&   data = _GetTempData();

        CHECK_THROW_MSG( dyn and dyn->Get() );

        CHECK_THROW_MSG( data.uniqueSliderNames.insert( name ).second,
            "Slider '"s << name << "' is already exists" );

        uint    idx = data.sliderCounter[ uint(type) ]++;
        CHECK_THROW_MSG( idx < UIInteraction::MaxSlidersPerType,
            "Slider count "s << ToString(idx) << " must be less than " << ToString(UIInteraction::MaxSlidersPerType) );

        auto&   dst = data.sliders.emplace_back();

        dst.name    = name;
        dst.index   = idx;
        dst.count   = sizeof(T) / sizeof(int);
        dst.type    = type;
        dst.dyn     = dyn->Get();

        std::memcpy( OUT &dst.intRange[0], &min, sizeof(min) );
        std::memcpy( OUT &dst.intRange[1], &max, sizeof(max) );
        std::memcpy( OUT &dst.intRange[2], &initial, sizeof(initial) );
    }

/*
=================================================
    _SliderI*
=================================================
*/
    void ScriptExe:: _SliderI0 (const ScriptDynamicIntPtr &dyn, const String &name) __Th___
    {
        int min = 0, max = 1024;
        return _Slider( dyn, name, min, max, min, ESlider::Int );
    }

    void  ScriptExe::_SliderI1 (const ScriptDynamicIntPtr &dyn,const String &name, int min, int max) __Th___
    {
        return _Slider( dyn, name, min, max, min, ESlider::Int );
    }

    void  ScriptExe::_SliderI2 (const ScriptDynamicInt2Ptr &dyn,const String &name, const packed_int2 &min, const packed_int2 &max) __Th___
    {
        return _Slider( dyn, name, min, max, min, ESlider::Int );
    }

    void  ScriptExe::_SliderI3 (const ScriptDynamicInt3Ptr &dyn,const String &name, const packed_int3 &min, const packed_int3 &max) __Th___
    {
        return _Slider( dyn, name, min, max, min, ESlider::Int );
    }

    void  ScriptExe::_SliderI4 (const ScriptDynamicInt4Ptr &dyn,const String &name, const packed_int4 &min, const packed_int4 &max) __Th___
    {
        return _Slider( dyn, name, min, max, min, ESlider::Int );
    }

/*
=================================================
    _SliderU*
=================================================
*/
    void  ScriptExe::_SliderU0 (const ScriptDynamicUIntPtr &dyn, const String &name) __Th___
    {
        uint    min = 0, max = 1024;
        return _Slider( dyn, name, min, max, min, ESlider::Int );
    }

    void  ScriptExe::_SliderU1 (const ScriptDynamicUIntPtr &dyn,const String &name, uint min, uint max) __Th___
    {
        return _Slider( dyn, name, min, max, min, ESlider::Int );
    }

    void  ScriptExe::_SliderU2 (const ScriptDynamicUInt2Ptr &dyn,const String &name, const packed_uint2 &min, const packed_uint2 &max) __Th___
    {
        return _Slider( dyn, name, min, max, min, ESlider::Int );
    }

    void  ScriptExe::_SliderU3 (const ScriptDynamicUInt3Ptr &dyn,const String &name, const packed_uint3 &min, const packed_uint3 &max) __Th___
    {
        return _Slider( dyn, name, min, max, min, ESlider::Int );
    }

    void  ScriptExe::_SliderU4 (const ScriptDynamicUInt4Ptr &dyn,const String &name, const packed_uint4 &min, const packed_uint4 &max) __Th___
    {
        return _Slider( dyn, name, min, max, min, ESlider::Int );
    }

/*
=================================================
    _SliderF*
=================================================
*/
    void  ScriptExe::_SliderF0 (const ScriptDynamicFloatPtr &dyn, const String &name) __Th___
    {
        float min = 0.f, max = 1.f;
        return _Slider( dyn, name, min, max, min, ESlider::Float );
    }

    void  ScriptExe::_SliderF1 (const ScriptDynamicFloatPtr &dyn, const String &name, float min, float max) __Th___
    {
        return _Slider( dyn, name, min, max, min, ESlider::Float );
    }

    void  ScriptExe::_SliderF2 (const ScriptDynamicFloat2Ptr &dyn, const String &name, const packed_float2 &min, const packed_float2 &max) __Th___
    {
        return _Slider( dyn, name, min, max, min, ESlider::Float );
    }

    void  ScriptExe::_SliderF3 (const ScriptDynamicFloat3Ptr &dyn, const String &name, const packed_float3 &min, const packed_float3 &max) __Th___
    {
        return _Slider( dyn, name, min, max, min, ESlider::Float );
    }

    void  ScriptExe::_SliderF4 (const ScriptDynamicFloat4Ptr &dyn, const String &name, const packed_float4 &min, const packed_float4 &max) __Th___
    {
        return _Slider( dyn, name, min, max, min, ESlider::Float );
    }

/*
=================================================
    _Bind
=================================================
*/
    void  ScriptExe::_Bind (const ScriptEnginePtr &se, const Config &cfg) __Th___
    {
        using namespace AE::Graphics;

        CoreBindings::BindStdTypes( se );
        CoreBindings::BindScalarMath( se );
        CoreBindings::BindVectorMath( se );
        CoreBindings::BindRect( se );
        CoreBindings::BindMatrixMath( se );
        CoreBindings::BindColor( se );
        CoreBindings::BindArray( se );
        CoreBindings::BindLog( se );
        CoreBindings::BindRandom( se );
        CoreBindings::BindToString( se, true, true, true, true );

        GraphicsBindings::BindEnums( se );
        GraphicsBindings::BindTypes( se );

        {
            EnumBinder<EColorSpace> binder {se};
            binder.Create();
            binder.AddValue( "sRGB_nonlinear",          EColorSpace::sRGB_nonlinear );
            binder.AddValue( "BT709_nonlinear",         EColorSpace::BT709_nonlinear );
            binder.AddValue( "Extended_sRGB_linear",    EColorSpace::Extended_sRGB_linear );
            binder.AddValue( "HDR10_ST2084",            EColorSpace::HDR10_ST2084 );
            binder.AddValue( "BT2020_linear",           EColorSpace::BT2020_linear );
            STATIC_ASSERT( uint(EColorSpace::_Count) == 15 );
            STATIC_ASSERT( uint(ESurfaceFormat::_Count) == 10 );

            // not compatible with ESurfaceFormat
            //binder.AddValue( "Display_P3_nonlinear",      EColorSpace::Display_P3_nonlinear );
            //binder.AddValue( "Display_P3_linear",         EColorSpace::Display_P3_linear );
            //binder.AddValue( "DCI_P3_nonlinear",          EColorSpace::DCI_P3_nonlinear );
            //binder.AddValue( "BT709_linear",              EColorSpace::BT709_linear );
            //binder.AddValue( "DolbyVision",               EColorSpace::DolbyVision );
            //binder.AddValue( "HDR10_HLG",                 EColorSpace::HDR10_HLG );
            //binder.AddValue( "AdobeRGB_linear",           EColorSpace::AdobeRGB_linear );
            //binder.AddValue( "AdobeRGB_nonlinear",        EColorSpace::AdobeRGB_nonlinear );
            //binder.AddValue( "PassThrough",               EColorSpace::PassThrough );
            //binder.AddValue( "Extended_sRGB_nonlinear",   EColorSpace::Extended_sRGB_nonlinear );
        }

        _Bind_DbgViewFlags( se );
        _Bind_PassGroupFlags( se );
        ScriptDynamicDim::Bind( se );
        ScriptDynamicUInt::Bind( se );
        ScriptDynamicUInt2::Bind( se );
        ScriptDynamicUInt3::Bind( se );
        ScriptDynamicUInt4::Bind( se );
        ScriptDynamicInt::Bind( se );
        ScriptDynamicInt2::Bind( se );
        ScriptDynamicInt3::Bind( se );
        ScriptDynamicInt4::Bind( se );
        ScriptDynamicFloat::Bind( se );
        ScriptDynamicFloat2::Bind( se );
        ScriptDynamicFloat3::Bind( se );
        ScriptDynamicFloat4::Bind( se );
        ScriptDynamicULong::Bind( se );
        ScriptImage::Bind( se );
        ScriptVideoImage::Bind( se );
        ScriptBuffer::Bind( se );
        ScriptRTGeometry::Bind( se );
        ScriptRTScene::Bind( se );

        ScriptBaseController::Bind( se );
        ScriptControllerScaleBias::Bind( se );
        ScriptControllerTopDown::Bind( se );
        ScriptControllerOrbitalCamera::Bind( se );
        ScriptControllerFlightCamera::Bind( se );
        ScriptControllerFPVCamera::Bind( se );
        ScriptControllerFreeCamera::Bind( se );

        ScriptBasePass::Bind( se );
        ScriptGeomSource::Bind( se );
        ScriptSphericalCube::Bind( se );
        ScriptUniGeometry::Bind( se );
        ScriptModelGeometrySrc::Bind( se );

        ScriptCollection::Bind( se );

        // don't forget to update '_SaveCppStructs()'
        ScriptPostprocess::Bind( se );
        ScriptComputePass::Bind( se );
        ScriptRayTracingPass::Bind( se );
        ScriptSceneGraphicsPass::Bind( se );
        ScriptScene::Bind( se );

        se->AddFunction( &ScriptExe::_SurfaceSize,              "SurfaceSize",              {},     "Returns dynamic dimensions of the screen size."    );

        se->AddFunction( &ScriptExe::_Present1,                 "Present",                  {},     "Present image to the screen." );
        se->AddFunction( &ScriptExe::_Present2,                 "Present",                  {} );
        se->AddFunction( &ScriptExe::_Present3,                 "Present",                  {} );
        se->AddFunction( &ScriptExe::_Present4,                 "Present",                  {} );
        se->AddFunction( &ScriptExe::_Present5,                 "Present",                  {} );
        se->AddFunction( &ScriptExe::_Present6,                 "Present",                  {} );

        se->AddFunction( &ScriptExe::_DbgView1,                 "DbgView",                  {},     "Draw image in child window for debugging." );
        se->AddFunction( &ScriptExe::_DbgView2,                 "DbgView",                  {} );
        se->AddFunction( &ScriptExe::_DbgView3,                 "DbgView",                  {} );
        se->AddFunction( &ScriptExe::_DbgView4,                 "DbgView",                  {} );

        se->AddFunction( &ScriptExe::_GenMipmaps,               "GenMipmaps",               {},     "Pass which generates mipmaps for image." );
        se->AddFunction( &ScriptExe::_CopyImage,                "CopyImage",                {},     "Pass which copy image content to another image." );

        se->AddFunction( &ScriptExe::_ClearImage1,              "ClearImage",               {},     "Pass to clear float-color image." );
        se->AddFunction( &ScriptExe::_ClearImage2,              "ClearImage",               {},     "Pass to clear uint-color image." );
        se->AddFunction( &ScriptExe::_ClearImage3,              "ClearImage",               {},     "Pass to clear int-color image." );
        se->AddFunction( &ScriptExe::_ClearBuffer,              "ClearBuffer",              {},     "Pass to clear buffer." );

        se->AddFunction( &ScriptExe::_BuildRTGeometry,          "BuildRTGeometry",          {},     "Pass to build RTGeometry, executed every frame."           );
        se->AddFunction( &ScriptExe::_BuildRTGeometryIndirect,  "BuildRTGeometryIndirect",  {},     "Pass to indirect build RTGeometry, executed every frame."  );

        se->AddFunction( &ScriptExe::_BuildRTScene,             "BuildRTScene",             {},     "Pass to build RTScene, executed every frame."              );
        se->AddFunction( &ScriptExe::_BuildRTSceneIndirect,     "BuildRTSceneIndirect",     {},     "Pass to indirect build RTScene, executed every frame."     );

        se->AddFunction( &ScriptExe::_GetCube2,                 "GetCube",                  {"positions", "normals", "indices"} );
        se->AddFunction( &ScriptExe::_GetCube3,                 "GetCube",                  {"positions", "normals", "tangents", "bitangents", "texcoords2d", "indices"} );
        se->AddFunction( &ScriptExe::_GetCube4,                 "GetCube",                  {"positions", "normals", "tangents", "bitangents", "cubemapTexcoords", "indices"} );
        se->AddFunction( &ScriptExe::_GetGrid1,                 "GetGrid",                  {"size", "unorm2Positions", "indices"},                 "Returns (size * size) grid" );
        se->AddFunction( &ScriptExe::_GetGrid2,                 "GetGrid",                  {"size", "unorm3Positions", "indices"},                 "Returns (size * size) grid in XY space." );
        se->AddFunction( &ScriptExe::_GetSphere1,               "GetSphere",                {"lod", "positions", "indices"},                        "Returns spherical cube" );
        se->AddFunction( &ScriptExe::_GetSphere2,               "GetSphere",                {"lod", "positions", "cubemapTexcoords", "indices"},    "Returns spherical cube" );
        se->AddFunction( &ScriptExe::_GetSphere3,               "GetSphere",                {"lod", "positions", "normals", "tangents", "bitangents", "cubemapTexcoords", "indices"},   "Returns spherical cube with tangential projection for cubemap." );
        se->AddFunction( &ScriptExe::_GetSphere4,               "GetSphere",                {"lod", "positions", "normals", "tangents", "bitangents", "texcoords2d", "indices"},        "Returns spherical cube" );
        se->AddFunction( &ScriptExe::_GetCylinder1,             "GetCylinder",              {"segmentCount", "isInner", "positions", "texcoords", "indices"},           "Returns cylinder" );
        se->AddFunction( &ScriptExe::_GetCylinder2,             "GetCylinder",              {"segmentCount", "isInner", "positions", "normals", "tangents", "bitangents", "texcoords", "indices"},  "Returns cylinder" );

        se->AddFunction( &ScriptExe::_IndicesToPrimitives,      "IndicesToPrimitives",      {"indices", "primitives"},      "Helper function to convert array of indices to array of uint3 indices per triangle" );
        se->AddFunction( &ScriptExe::_GetFrustumPlanes,         "GetFrustumPlanes",         {"viewProj", "outPlanes"},      "Helper function to convert matrix to 6 planes of the frustum." );
        se->AddFunction( &ScriptExe::_MergeMesh,                "MergeMesh",                {"srcIndices", "srcVertexCount", "indicesToAdd"} );

        se->AddFunction( &ScriptExe::_RunScript1,               "RunScript",                {"filePath", "collection"},     "Run script, path to script must be added to 'res_editor_cfg.as' as 'SecondaryScriptDir()'" );
        se->AddFunction( &ScriptExe::_RunScript2,               "RunScript",                {"filePath", "flags", "collection"} );

        se->AddFunction( &ScriptExe::_SliderI0,                 "Slider",                   {"dyn", "name"} );
        se->AddFunction( &ScriptExe::_SliderI1,                 "Slider",                   {"dyn", "name", "min", "max"} );
        se->AddFunction( &ScriptExe::_SliderI2,                 "Slider",                   {"dyn", "name", "min", "max"} );
        se->AddFunction( &ScriptExe::_SliderI3,                 "Slider",                   {"dyn", "name", "min", "max"} );
        se->AddFunction( &ScriptExe::_SliderI4,                 "Slider",                   {"dyn", "name", "min", "max"},      "Add slider to UI." );

        se->AddFunction( &ScriptExe::_SliderU0,                 "Slider",                   {"dyn", "name"} );
        se->AddFunction( &ScriptExe::_SliderU1,                 "Slider",                   {"dyn", "name", "min", "max"} );
        se->AddFunction( &ScriptExe::_SliderU2,                 "Slider",                   {"dyn", "name", "min", "max"} );
        se->AddFunction( &ScriptExe::_SliderU3,                 "Slider",                   {"dyn", "name", "min", "max"} );
        se->AddFunction( &ScriptExe::_SliderU4,                 "Slider",                   {"dyn", "name", "min", "max"} );

        se->AddFunction( &ScriptExe::_SliderF0,                 "Slider",                   {"dyn", "name"} );
        se->AddFunction( &ScriptExe::_SliderF1,                 "Slider",                   {"dyn", "name", "min", "max"} );
        se->AddFunction( &ScriptExe::_SliderF2,                 "Slider",                   {"dyn", "name", "min", "max"} );
        se->AddFunction( &ScriptExe::_SliderF3,                 "Slider",                   {"dyn", "name", "min", "max"} );
        se->AddFunction( &ScriptExe::_SliderF4,                 "Slider",                   {"dyn", "name", "min", "max"} );

        // TODO:
        //  PresentVR( left, left_layer, left_mipmap,  right, right_layer, right_mipmap )

        if ( FileSystem::IsDirectory( cfg.scriptHeaderOutFolder ))
        {
            se->AddCppHeader( "", "#define SCRIPT\n\n", 0 );

            CHECK( se->SaveCppHeader( cfg.scriptHeaderOutFolder / "res_editor.as" ));
        }
    }

/*
=================================================
    _Bind_DbgViewFlags
=================================================
*/
    void  ScriptExe::_Bind_DbgViewFlags (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<DebugView::EFlags>   binder {se};
        binder.Create();
        binder.AddValue( "NoCopy",      DebugView::EFlags::NoCopy );
        binder.AddValue( "Copy",        DebugView::EFlags::Copy );
        binder.AddValue( "Histogram",   DebugView::EFlags::Histogram );
        binder.AddValue( "LinearDepth", DebugView::EFlags::LinearDepth );
        binder.AddValue( "Stencil",     DebugView::EFlags::Stencil );
        STATIC_ASSERT( uint(DebugView::EFlags::_Count) == 5 );
    }

/*
=================================================
    _Bind_PassGroupFlags
=================================================
*/
    void  ScriptExe::_Bind_PassGroupFlags (const ScriptEnginePtr &se) __Th___
    {
        EnumBinder<PassGroup::EFlags>   binder {se};
        binder.Create();
        binder.AddValue( "RunOnce",                 PassGroup::EFlags::RunOnce );
        binder.AddValue( "OnRequest",               PassGroup::EFlags::OnRequest );
        binder.AddValue( "RunOnce_AfterLoading",    PassGroup::EFlags::RunOnce_AfterLoading );
        STATIC_ASSERT( uint(PassGroup::EFlags::_Count) == 4 );
    }

/*
=================================================
    _Bind_Constants
=================================================
*/
    void  ScriptExe::_Bind_Constants (const ScriptEnginePtr &se) __Th___
    {
        se->AddConstProperty( _sampConsts->NearestClamp,                "Sampler_" + _sampConsts->NearestClamp );
        se->AddConstProperty( _sampConsts->NearestRepeat,               "Sampler_" + _sampConsts->NearestRepeat );
        se->AddConstProperty( _sampConsts->NearestMirrorRepeat,         "Sampler_" + _sampConsts->NearestMirrorRepeat );
        se->AddConstProperty( _sampConsts->LinearClamp,                 "Sampler_" + _sampConsts->LinearClamp );
        se->AddConstProperty( _sampConsts->LinearRepeat,                "Sampler_" + _sampConsts->LinearRepeat );
        se->AddConstProperty( _sampConsts->LinearMirrorRepeat,          "Sampler_" + _sampConsts->LinearMirrorRepeat );
        se->AddConstProperty( _sampConsts->LinearMipmapClamp,           "Sampler_" + _sampConsts->LinearMipmapClamp );
        se->AddConstProperty( _sampConsts->LinearMipmapRepeat,          "Sampler_" + _sampConsts->LinearMipmapRepeat );
        se->AddConstProperty( _sampConsts->LinearMipmapMirrorRepeat,    "Sampler_" + _sampConsts->LinearMipmapMirrorRepeat );
        se->AddConstProperty( _sampConsts->Anisotropy8Repeat,           "Sampler_" + _sampConsts->Anisotropy8Repeat );
        se->AddConstProperty( _sampConsts->Anisotropy8MirrorRepeat,     "Sampler_" + _sampConsts->Anisotropy8MirrorRepeat );
        se->AddConstProperty( _sampConsts->Anisotropy8Clamp,            "Sampler_" + _sampConsts->Anisotropy8Clamp );
        se->AddConstProperty( _sampConsts->Anisotropy16Repeat,          "Sampler_" + _sampConsts->Anisotropy16Repeat );
        se->AddConstProperty( _sampConsts->Anisotropy16MirrorRepeat,    "Sampler_" + _sampConsts->Anisotropy16MirrorRepeat );
        se->AddConstProperty( _sampConsts->Anisotropy16Clamp,           "Sampler_" + _sampConsts->Anisotropy16Clamp );

        STATIC_ASSERT( (sizeof(SamplerConsts) / sizeof(String)) == 15 );
    }

/*
=================================================
    _SaveCppStructs
=================================================
*/
    void  ScriptExe::_SaveCppStructs (const Path &fname) __Th___
    {
        ScriptBasePass::CppStructsFromShaders   data;

        _GetSharedShaderTypes( INOUT data );

        // don't forget to update '_Bind()'
        ScriptPostprocess::GetShaderTypes( INOUT data );
        ScriptComputePass::GetShaderTypes( INOUT data );
        ScriptRayTracingPass::GetShaderTypes( INOUT data );
        ScriptSceneGraphicsPass::GetShaderTypes( INOUT data );

        ScriptSphericalCube::GetShaderTypes( INOUT data );
        ScriptUniGeometry::GetShaderTypes( INOUT data );
        ScriptModelGeometrySrc::GetShaderTypes( INOUT data );

        if ( data.cpp.empty() )
            return;

        FileWStream     file {fname};
        CHECK_ERRV( file.IsOpen() );

        CHECK_ERRV( file.Write( data.cpp ));
    }

/*
=================================================
    _GetRenderer
=================================================
*/
    Renderer*  ScriptExe::_GetRenderer () __Th___
    {
        return _GetTempData().renderer.get();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    ConvertAndLoad
=================================================
*/
    RTechInfo  ScriptExe::ScriptPassApi::ConvertAndLoad (Function<void (ScriptEnginePtr)> fn) __Th___
    {
        CHECK_THROW( s_scriptExe != null );

        RTechInfo   result;
        s_scriptExe->_RunWithPipelineCompiler(
            [&] ()
            {
                ScriptBasePass::CppStructsFromShaders   data;
                ScriptSphericalCube::GetShaderTypes( INOUT data );
                ScriptUniGeometry::GetShaderTypes( INOUT data );
                ScriptModelGeometrySrc::GetShaderTypes( INOUT data );

                fn( s_scriptExe->_engine2 );
                result = s_scriptExe->_ConvertAndLoad();
            });
        return result;
    }

/*
=================================================
    AddPass
=================================================
*/
    void  ScriptExe::ScriptPassApi::AddPass (ScriptBasePassPtr pass) __Th___
    {
        CHECK_THROW( pass );

        auto&   data = _GetTempData();
        CHECK_THROW( data.passGroup );

        data.passGroup->Add( pass );
    }

/*
=================================================
    GetCurrentFile
=================================================
*/
    Path  ScriptExe::ScriptPassApi::GetCurrentFile () __Th___
    {
        auto&   data = _GetTempData();
        CHECK_THROW( not data.currPath.empty() );

        return data.currPath.back();
    }

    Path  ScriptExe::ScriptPassApi::ToAbsolute (const Path &path) __Th___
    {
        auto    curr = GetCurrentFile().parent_path();
        curr /= path;

        if ( FileSystem::IsFile( curr ))
            return curr;

        curr = FileSystem::CurrentPath();
        curr /= path;

        if ( FileSystem::IsFile( curr ))
            return curr;

        CHECK_THROW_MSG( false, "can't find file '"s << ToString(path) << "'");
    }

/*
=================================================
    ToShaderPath
=================================================
*/
    Path  ScriptExe::ScriptPassApi::ToShaderPath (const Path &path) __Th___
    {
        auto&   data = _GetTempData();

        // shader source in the same file
        if ( path.empty() )
        {
            CHECK_THROW( not data.currPath.empty() );
            return data.currPath.back();
        }

        for (const auto& p : data.cfg.shaderDirs)
        {
            Path    pp = p / path;

            if ( FileSystem::IsFile( pp ))
            {
                data.dependencies.push_back( pp );
                return pp;
            }
        }

        if ( FileSystem::IsFile( path ))
        {
            Path    pp = ToAbsolute( path );
            data.dependencies.push_back( pp );
            return RVRef(pp);
        }

        return {};
    }

/*
=================================================
    ToPipelinePath
=================================================
*/
    Path  ScriptExe::ScriptPassApi::ToPipelinePath (const Path &path) __Th___
    {
        CHECK_THROW( not path.empty() );

        auto&   data = _GetTempData();
        for (const auto& p : data.cfg.pipelineDirs)
        {
            Path    pp = p / path;

            if ( FileSystem::IsFile( pp ))
                return pp;
        }
        CHECK_THROW( false );
    }

/*
=================================================
    ToPipelineFolder
=================================================
*/
    Path  ScriptExe::ScriptPassApi::ToPipelineFolder (const Path &path) __Th___
    {
        CHECK_THROW( not path.empty() );

        auto&   data = _GetTempData();
        for (const auto& p : data.cfg.pipelineDirs)
        {
            Path    pp = p / path;

            if ( FileSystem::IsDirectory( pp ))
                return pp;
        }
        CHECK_THROW( false );
    }

/*
=================================================
    GetRenderer
=================================================
*/
    Renderer&  ScriptExe::ScriptPassApi::GetRenderer () __Th___
    {
        CHECK_THROW( s_scriptExe != null );
        auto*   result = s_scriptExe->_GetRenderer();
        CHECK_THROW( result != null );
        return *result;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    GetRenderer
=================================================
*/
    Renderer&  ScriptExe::ScriptResourceApi::GetRenderer () __Th___
    {
        CHECK_THROW( s_scriptExe != null );
        auto*   result = s_scriptExe->_GetRenderer();
        CHECK_THROW( result != null );
        return *result;
    }

/*
=================================================
    IsPassGroup
=================================================
*/
    bool  ScriptExe::ScriptResourceApi::IsPassGroup (const ScriptBasePassPtr &pass) __NE___
    {
        return DynCast<ScriptPassGroup>( pass.Get() ) != null;
    }

/*
=================================================
    ToAbsolute
=================================================
*/
    Path  ScriptExe::ScriptResourceApi::ToAbsolute (const String &inPath) __Th___
    {
        CHECK_THROW( s_scriptExe != null );

        auto&       cfg = s_scriptExe->_config;
        const usize cnt = Min( cfg.vfsPaths.size(), cfg.vfsPathPrefixes.size() );

        for (usize i = 0; i < cnt; ++i)
        {
            if ( StartsWith( inPath, cfg.vfsPathPrefixes[i] ))
            {
                Path    path = cfg.vfsPaths[i] / inPath.substr( cfg.vfsPathPrefixes[i].size() );
                if ( FileSystem::IsFile( path ))
                    return FileSystem::ToAbsolute( path );
            }
        }

        CHECK_THROW_MSG( false,
            "File '"s << inPath << "' is not exists" );
    }

} // AE::ResEditor
//-----------------------------------------------------------------------------



namespace AE::ResEditor
{

/*
=================================================
    _RunWithPipelineCompiler
=================================================
*/
    void  ScriptExe::_RunWithPipelineCompiler (Function<void ()> fn) __Th___
    {
        try
        {
            // init pipeline compiler
            ObjectStorage   obj_storage;
            PipelineStorage ppln_storage;
            {
                obj_storage.pplnStorage         = &ppln_storage;
                obj_storage.shaderFolders       = _GetTempData().cfg.shaderDirs;
                obj_storage.defaultFeatureSet   = "DefaultFS";

                obj_storage.spirvCompiler       = MakeUnique<SpirvCompiler>( _GetTempData().cfg.includeDirs );
                obj_storage.spirvCompiler->SetDefaultResourceLimits();

                ObjectStorage::SetInstance( &obj_storage );

                PipelineCompiler::ScriptConfig  cfg;
                cfg.SetTarget( ECompilationTarget::Vulkan );
                cfg.SetShaderVersion( EShaderVersion::SPIRV_1_5 );
                cfg.SetShaderOptions( EShaderOpt::Optimize );
                cfg.SetDefaultLayout( EStructLayout::Std140 );
                cfg.SetPipelineOptions( EPipelineOpt::Unknown );
                cfg.SetPreprocessor( EShaderPreprocessor::AEStyle );

                ScriptFeatureSetPtr fs {new ScriptFeatureSet{ "DefaultFS" }};
                fs->fs = RenderTaskScheduler().GetFeatureSet();
            }

            _LoadSamplers();                // throw
            _RegisterSharedShaderTypes();   // throw

            fn();

            ObjectStorage::SetInstance( null );
        }
        catch (...)
        {
            ObjectStorage::SetInstance( null );
            throw;
        }
    }

/*
=================================================
    _CompilePipeline
=================================================
*/
    bool  ScriptExe::_CompilePipeline (const Path &pplnPath)
    {
        CHECK_ERR( _engine2 );

        auto    obj_storage = ObjectStorage::Instance();
        CHECK_ERR( obj_storage != null );

        CHECK_ERR( obj_storage->CompilePipeline( _engine2, pplnPath ));
        return true;
    }

/*
=================================================
    _CompilePipelineFromSource
=================================================
*/
    bool  ScriptExe::_CompilePipelineFromSource (const Path &pplnPath, StringView source)
    {
        CHECK_ERR( _engine2 );

        auto    obj_storage = ObjectStorage::Instance();
        CHECK_ERR( obj_storage != null );

        CHECK_ERR( obj_storage->CompilePipelineFromSource( _engine2, pplnPath, source ));
        return true;
    }

/*
=================================================
    _ConvertAndLoad2
=================================================
*/
    RC<RStream>  ScriptExe::_ConvertAndLoad2 ()
    {
        auto    obj_storage = ObjectStorage::Instance();
        CHECK_THROW( obj_storage != null );

        CHECK_THROW( obj_storage->Build() );
        CHECK_THROW( obj_storage->BuildRenderTechniques() );

        auto    mem = MakeRC<MemWStream>();

        PipelinePackOffsets     offsets = {};
        CHECK_THROW( obj_storage->SavePack( *mem, true, OUT offsets ));

        auto    mem2 = MakeRC<MemWDataSource>( mem->ReleaseData() );
                mem  = null;

        CHECK_THROW( mem2->Write( Sizeof(PackOffsets_Name), offsets ));

        obj_storage->Clear();

        return MakeRC<MemRStream>( mem2->ReleaseData() );
    }

/*
=================================================
    _LoadSamplers
=================================================
*/
    void  ScriptExe::_LoadSamplers () __Th___
    {
        auto    obj_storage = ObjectStorage::Instance();
        CHECK_THROW( obj_storage != null );

        {
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->NearestClamp}};
            samp->SetFilter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
            samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->NearestRepeat}};
            samp->SetFilter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
            samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->NearestMirrorRepeat}};
            samp->SetFilter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
            samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->LinearClamp}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
            samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->LinearRepeat}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
            samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->LinearMirrorRepeat}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
            samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->LinearMipmapClamp}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->LinearMipmapRepeat}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->LinearMipmapMirrorRepeat}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->Anisotropy8Repeat}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
            samp->SetAnisotropy( 8.0f );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->Anisotropy8MirrorRepeat}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
            samp->SetAnisotropy( 8.0f );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->Anisotropy8Clamp}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
            samp->SetAnisotropy( 8.0f );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->Anisotropy16Repeat}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
            samp->SetAnisotropy( 16.0f );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->Anisotropy16MirrorRepeat}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
            samp->SetAnisotropy( 16.0f );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{_sampConsts->Anisotropy16Clamp}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
            samp->SetAnisotropy( 16.0f );
        }

        STATIC_ASSERT( (sizeof(SamplerConsts) / sizeof(String)) == 15 );
        CHECK_THROW( obj_storage->Build() );
    }

/*
=================================================
    _RegisterSharedShaderTypes
=================================================
*/
    void  ScriptExe::_RegisterSharedShaderTypes () __Th___
    {
        auto&   obj_storage = *ObjectStorage::Instance();

        if ( not obj_storage.structTypes.contains( "CameraData" ))
        {
            ShaderStructTypePtr st{ new ShaderStructType{"CameraData"}};
            st->Set( EStructLayout::Std140, R"#(
                    float4x4    viewProj;
                    float4x4    invViewProj;
                    float4x4    proj;
                    float4x4    view;
                    float3      pos;
                    float2      clipPlanes;
                    float4      frustum [6];
                )#");
        }

        if ( not obj_storage.structTypes.contains( "AccelStructInstance" ))
        {
            ShaderStructTypePtr st{ new ShaderStructType{"AccelStructInstance"}};
            st->Set( EStructLayout::Std430, R"#(
                    float3x4    transform;
                    uint        instanceCustomIndex24_mask8;
                    uint        instanceSBTOffset24_flags8;         // flags: gl::GeometryInstanceFlags
                    ulong       accelerationStructureReference;     // gl::DeviceAddress
                )#");
            CHECK( st->StaticSize() == 64_b );
        }

        if ( not obj_storage.structTypes.contains( "ASBuildIndirectCommand" ))
        {
            ShaderStructTypePtr st{ new ShaderStructType{"ASBuildIndirectCommand"}};
            st->Set( EStructLayout::Std430, R"#(
                    uint        primitiveCount;
                    uint        primitiveOffset;
                    uint        firstVertex;
                    uint        transformOffset;
                )#");
            CHECK( st->StaticSize() == 16_b );
        }
    }

/*
=================================================
    _GetSharedShaderTypes
=================================================
*/
    void  ScriptExe::_GetSharedShaderTypes (ScriptBasePass::CppStructsFromShaders &data) __Th___
    {
        _RegisterSharedShaderTypes();   // throw

        auto&   obj_storage = *ObjectStorage::Instance();

        {
            auto    it = obj_storage.structTypes.find( "CameraData" );
            CHECK_THROW( it != obj_storage.structTypes.end() );
            CHECK_THROW( it->second->ToCPP( INOUT data.cpp, INOUT data.uniqueTypes ));
        }
    }


} // AE::ResEditor
