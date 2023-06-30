// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/DataSource/FileStream.h"
#include "base/DataSource/MemStream.h"
#include "base/DataSource/MemDataSource.h"
#include "base/Algorithms/StringParser.h"

#include "scripting/Impl/ScriptFn.h"
#include "scripting/Impl/EnumBinder.h"

#include "res_editor/Scripting/ScriptExe.h"
#include "res_editor/Passes/Present.h"
#include "res_editor/Passes/BuildRTAS.h"

#include "res_editor/EditorUI.h"
#include "res_editor/Scripting/PassCommon.inl.h"

namespace AE::ResEditor
{       
namespace {
    ScriptExe*  s_scriptExe = null;
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
            ScriptBasePass{ Default }, rt{rt}, layer{layer}, mipmap{mipmap}, dynSize{dynSize} {}

        RC<IPass>  ToPass () C_Th_OV;
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

        return MakeRC<ResEditor::Present>( RVRef(src), "Present", dynSize );
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
            ScriptBasePass{Default}, rt{rt}, layer{layer}, mipmap{mipmap}, flags{flags}, index{idx} {}

        RC<IPass>  ToPass () C_Th_OV;
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
        ScriptGenMipmaps (const ScriptImagePtr &rt) : ScriptBasePass{Default}, rt{rt} {}

        RC<IPass>  ToPass () C_Th_OV;
    };

/*
=================================================
    ScriptGenMipmaps::ToPass
=================================================
*/
    RC<IPass>  ScriptExe::ScriptGenMipmaps::ToPass () C_Th___
    {
        return MakeRC<ResEditor::GenerateMipmaps>( rt->ToResource(), "GenMipmaps" );
    }
//-----------------------------------------------------------------------------



    //
    // Build Ray Tracing Geometry Pass
    //
    class ScriptExe::ScriptBuildRTGeometry final : public ScriptBasePass
    {
    private:
        ScriptRTGeometryPtr dstGeometry;
        ScriptBufferPtr     indirectBuffer;


    public:
        ScriptBuildRTGeometry () : ScriptBasePass{Default} {}

        RC<IPass>  ToPass () C_Th_OV;
    };

/*
=================================================
    ScriptBuildRTGeometry::ToPass
=================================================
*/
    RC<IPass>  ScriptExe::ScriptBuildRTGeometry::ToPass () C_Th___
    {
        return MakeRC<ResEditor::BuildRTGeometry>(
                    dstGeometry->ToResource(),
                    indirectBuffer->ToResource(),
                    "BuildRTGeometry" );
    }
//-----------------------------------------------------------------------------



    //
    // Build Ray Tracing Scene Pass
    //
    class ScriptExe::ScriptBuildRTScene final : public ScriptBasePass
    {
    private:
        ScriptRTScenePtr    dstScene;
        ScriptBufferPtr     indirectBuffer;


    public:
        ScriptBuildRTScene () : ScriptBasePass{Default} {}

        RC<IPass>  ToPass () C_Th_OV;
    };

/*
=================================================
    ScriptBuildRTScene::ToPass
=================================================
*/
    RC<IPass>  ScriptExe::ScriptBuildRTScene::ToPass () C_Th___
    {
        return MakeRC<ResEditor::BuildRTScene>(
                    dstScene->ToResource(),
                    indirectBuffer->ToResource(),
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
        mutable RC<IPass>           _result;

    public:
        ScriptPassGroup (uint flags) : ScriptBasePass{Default}, _flags{flags}   {}

        void  Add (ScriptBasePassPtr pass)                                      { _passes.push_back( RVRef(pass) ); }

        ND_ ArrayView<ScriptBasePassPtr>  GetPasses () const                    { return _passes; }

        RC<IPass>  ToPass () C_Th_OV;
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

        RC<PassGroup>   pg = MakeRC<PassGroup>( _flags );

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
                obj_storage.target          = ECompilationTarget::Vulkan;
                obj_storage.shaderVersion   = EShaderVersion::SPIRV_1_5;

                obj_storage.spirvCompiler   = MakeUnique<SpirvCompiler>( Array<Path>{} );
                obj_storage.spirvCompiler->SetDefaultResourceLimits();

                ObjectStorage::SetInstance( &obj_storage );
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
        _tempData->renderer = MakeRC<Renderer>();
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
            result->SetDependencies( RVRef(_tempData->dependencies) );

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
                    info.intRange[ idx * 2 + 0 ]                        = slider.intRange[0];
                    info.intRange[ idx * 2 + 1 ]                        = slider.intRange[1];
                    info.intVecSize[ idx ]                              = slider.count;
                    info.names[ UIInteraction::IntSlidersOffset + idx ] = slider.name;
                    dyn_sliders.push_back( slider.dyn );
                    break;

                case ESlider::Float :
                    info.floatRange[ idx * 2 + 0 ]                      = slider.floatRange[0];
                    info.floatRange[ idx * 2 + 1 ]                      = slider.floatRange[1];
                    info.floatVecSize[ idx ]                            = slider.count;
                    info.names[ UIInteraction::FloatSlidersOffset+idx ] = slider.name;
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
        return _RunScript2( filePath, 0, collection );
    }

    ScriptBasePass*  ScriptExe::_RunScript2 (const String &filePath, uint flags, const ScriptCollectionPtr &collection) __Th___
    {
        CHECK_THROW_MSG( collection );

        auto&       data     = _GetTempData();
        const auto  path     = Path{data.cfg.scriptDir} / filePath;

        CHECK_THROW_MSG( data.passGroup );
        CHECK_THROW_MSG( FileSystem::IsFile( path ),
            "script '"s << filePath << "' is not exists" );

        ScriptPassGroupPtr  pg {new ScriptPassGroup{ flags }};
        ScriptPassGroupPtr  prev = data.passGroup;
        data.passGroup = pg;

        data.currPath.push_back( FileSystem::ToAbsolute( path ));

        CHECK_THROW( s_scriptExe->_Run( path, collection ));

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

            ScriptPassGroupPtr  pg {new ScriptPassGroup{ 0 }};
            _tempData->passGroup = pg;

            CHECK_ERR( _Run( filePath ));

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
    template <typename ...Args>
    bool  ScriptExe::_Run (const Path &filePath, Args ...args) __NE___
    {
        CHECK_ERR( _engine );

        _tempData->passGroupDepth++;

        const String    ansi_path   = ToString( filePath );
        FileRStream     file        { filePath };

        if ( not file.IsOpen() )
            RETURN_ERR( "Failed to open script file: '"s << ansi_path << "'" );

        ScriptEngine::ModuleSource  src;
        src.name = ToString( filePath.filename().replace_extension("") );

        if ( not file.Read( file.RemainingSize(), OUT src.script ))
            RETURN_ERR( "Failed to read script file: '"s << ansi_path << "'" );

        src.dbgLocation     = SourceLoc{ ansi_path, 0 };
        src.usePreprocessor = true;

        ScriptModulePtr     module = _engine->CreateModule( {src}, {"SCRIPT"} );
        if ( not module )
            RETURN_ERR( "Failed to parse script file: '"s << ansi_path << "'" );

        auto    fn = _engine->CreateScript< void (Args ...) >( "ASmain", module );
        if ( not fn )
            RETURN_ERR( "Failed to create script context for file: '"s << ansi_path << "'" );

        if ( not fn->Run( FwdArg<Args>(args)... ))
            RETURN_ERR( "Failed to run script '"s << ansi_path << "'" );

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
        return _Present4( rt, ImageLayer{}, MipmapLevel{} );
    }

    void  ScriptExe::_Present2 (const ScriptImagePtr &rt, const MipmapLevel &mipmap) __Th___
    {
        return _Present4( rt, ImageLayer{}, mipmap );
    }

    void  ScriptExe::_Present3 (const ScriptImagePtr &rt, const ImageLayer &layer) __Th___
    {
        return _Present4( rt, layer, MipmapLevel{} );
    }

    void  ScriptExe::_Present4 (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap) __Th___
    {
        CHECK_THROW_MSG( rt );

        rt->AddUsage( EResourceUsage::Present );

        auto&   data = _GetTempData();
        CHECK_THROW_MSG( data.passGroupDepth == 1, "'Present()' must be used in main script" );

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
    _BuildRTGeometry
=================================================
*/
    void  ScriptExe::_BuildRTGeometry () __Th___
    {
        // TODO
    }

/*
=================================================
    _BuildRTScene
=================================================
*/
    void  ScriptExe::_BuildRTScene () __Th___
    {
        // TODO
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
            case DebugView::EFlags::Unknown :   break;
            case DebugView::EFlags::NoCopy :    rt->AddUsage( EResourceUsage::Present );    break;
            case DebugView::EFlags::Histogram : rt->AddUsage( EResourceUsage::Sampled );    break;
            case DebugView::EFlags::_Count :
            default :                           CHECK_THROW_MSG( false, "unsupported flags" );
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
    void  ScriptExe::_Slider (const D &dyn, const String &name, const T &min, const T &max, ESlider type) __Th___
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
    }

/*
=================================================
    _SliderI*
=================================================
*/
//  void ScriptExe:: _SliderI0 (const ScriptDynamicIntPtr &dyn, const String &name) __Th___;
//  void  ScriptExe::_SliderI1 (const ScriptDynamicIntPtr &dyn,const String &name, int min, int max) __Th___;
//  void  ScriptExe::_SliderI2 (const ScriptDynamicInt2Ptr &dyn,const String &name, const packed_int2 &min, const packed_int2 &max) __Th___;
//  void  ScriptExe::_SliderI3 (const ScriptDynamicInt3Ptr &dyn,const String &name, const packed_int3 &min, const packed_int3 &max) __Th___;

    void  ScriptExe::_SliderI4 (const ScriptDynamicInt4Ptr &dyn,const String &name, const packed_int4 &min, const packed_int4 &max) __Th___
    {
        return _Slider( dyn, name, min, max, ESlider::Int );
    }

/*
=================================================
    _SliderU*
=================================================
*/
    void  ScriptExe::_SliderU0 (const ScriptDynamicUIntPtr &dyn, const String &name) __Th___
    {
        int min = 0, max = 1024;
        return _Slider( dyn, name, min, max, ESlider::Int );
    }

    void  ScriptExe::_SliderU1 (const ScriptDynamicUIntPtr &dyn,const String &name, uint min, uint max) __Th___
    {
        return _Slider( dyn, name, min, max, ESlider::Int );
    }

    void  ScriptExe::_SliderU3 (const ScriptDynamicUInt3Ptr &dyn,const String &name, const packed_uint3 &min, const packed_uint3 &max) __Th___
    {
        return _Slider( dyn, name, min, max, ESlider::Int );
    }

//  void  ScriptExe::_SliderU2 (const ScriptDynamicUInt2Ptr &dyn,const String &name, const packed_uint2 &min, const packed_uint2 &max) __Th___;
//  void  ScriptExe::_SliderU4 (const ScriptDynamicUInt4Ptr &dyn,const String &name, const packed_uint4 &min, const packed_uint4 &max) __Th___;

/*
=================================================
    _SliderF*
=================================================
*/
    void  ScriptExe::_SliderF0 (const ScriptDynamicFloatPtr &dyn, const String &name) __Th___
    {
        float min = 0.f, max = 1.f;
        return _Slider( dyn, name, min, max, ESlider::Float );
    }

    void  ScriptExe::_SliderF1 (const ScriptDynamicFloatPtr &dyn, const String &name, float min, float max) __Th___
    {
        return _Slider( dyn, name, min, max, ESlider::Float );
    }

    void  ScriptExe::_SliderF4 (const ScriptDynamicFloat4Ptr &dyn, const String &name, const packed_float4 &min, const packed_float4 &max) __Th___
    {
        return _Slider( dyn, name, min, max, ESlider::Float );
    }

//  void  ScriptExe::_SliderF2 (const ScriptDynamicFloat2Ptr &dyn, const String &name, const packed_float2 &min, const packed_float2 &max) __Th___;
//  void  ScriptExe::_SliderF3 (const ScriptDynamicFloat3Ptr &dyn, const String &name, const packed_float3 &min, const packed_float3 &max) __Th___;

/*
=================================================
    _Bind
=================================================
*/
    void  ScriptExe::_Bind (const ScriptEnginePtr &se, const Config &cfg) __Th___
    {
        using namespace AE::Graphics;

        CoreBindings::BindScalarMath( se );
        CoreBindings::BindVectorMath( se );
        CoreBindings::BindMatrixMath( se );
        CoreBindings::BindRect( se );
        CoreBindings::BindColor( se );
        CoreBindings::BindArray( se );
        CoreBindings::BindLog( se );
        CoreBindings::BindRandom( se );
        CoreBindings::BindToString( se, true, true, true, true );

        GraphicsBindings::BindEnums( se );
        GraphicsBindings::BindTypes( se );

        _Bind_DbgViewFlags( se );
        _Bind_PassGroupFlags( se );
        ScriptDynamicDim::Bind( se );
        ScriptDynamicUInt3::Bind( se );
        ScriptDynamicInt4::Bind( se );
        ScriptDynamicFloat4::Bind( se );
        ScriptDynamicUInt::Bind( se );
        ScriptDynamicULong::Bind( se );
        ScriptDynamicFloat::Bind( se );
        ScriptImage::Bind( se );
        ScriptVideoImage::Bind( se );
        ScriptBuffer::Bind( se );
        ScriptRTGeometry::Bind( se );
        ScriptRTScene::Bind( se );

        ScriptBaseController::Bind( se );
        ScriptController2D::Bind( se );
        ScriptControllerFlightCamera::Bind( se );
        ScriptControllerFPVCamera::Bind( se );
        ScriptControllerFreeCamera::Bind( se );

        ScriptBasePass::Bind( se );
        ScriptGeomSource::Bind( se );
        ScriptSphericalCube::Bind( se );
        ScriptUniGeometry::Bind( se );

        ScriptCollection::Bind( se );

        // don't forget to update '_SaveCppStructs()'
        ScriptPostprocess::Bind( se );
        ScriptComputePass::Bind( se );
        ScriptSceneGraphicsPass::Bind( se );
    //  ScriptSceneRayTracingPass::Bind( se );
        ScriptScene::Bind( se );

        se->AddFunction( &ScriptExe::_SurfaceSize,      "SurfaceSize"   );

        se->AddFunction( &ScriptExe::_Present1,         "Present"       );
        se->AddFunction( &ScriptExe::_Present2,         "Present"       );
        se->AddFunction( &ScriptExe::_Present3,         "Present"       );
        se->AddFunction( &ScriptExe::_Present4,         "Present"       );

        se->AddFunction( &ScriptExe::_DbgView1,         "DbgView"       );
        se->AddFunction( &ScriptExe::_DbgView2,         "DbgView"       );
        se->AddFunction( &ScriptExe::_DbgView3,         "DbgView"       );
        se->AddFunction( &ScriptExe::_DbgView4,         "DbgView"       );

        se->AddFunction( &ScriptExe::_GenMipmaps,       "GenMipmaps"    );
        se->AddFunction( &ScriptExe::_BuildRTGeometry,  "BuildRTGeometry" );
        se->AddFunction( &ScriptExe::_BuildRTScene,     "BuildRTScene"  );
        se->AddFunction( &ScriptExe::_RunScript1,       "RunScript"     );
        se->AddFunction( &ScriptExe::_RunScript2,       "RunScript"     );

        //se->AddFunction( &ScriptExe::_SliderI0,       "Slider"        );
        //se->AddFunction( &ScriptExe::_SliderI1,       "Slider"        );
        //se->AddFunction( &ScriptExe::_SliderI2,       "Slider"        );
        //se->AddFunction( &ScriptExe::_SliderI3,       "Slider"        );
        se->AddFunction( &ScriptExe::_SliderI4,         "Slider"        );

        se->AddFunction( &ScriptExe::_SliderU0,         "Slider"        );
        se->AddFunction( &ScriptExe::_SliderU1,         "Slider"        );
        //se->AddFunction( &ScriptExe::_SliderU2,       "Slider"        );
        se->AddFunction( &ScriptExe::_SliderU3,         "Slider"        );
        //se->AddFunction( &ScriptExe::_SliderU4,       "Slider"        );

        se->AddFunction( &ScriptExe::_SliderF0,         "Slider"        );
        se->AddFunction( &ScriptExe::_SliderF1,         "Slider"        );
        //se->AddFunction( &ScriptExe::_SliderF2,       "Slider"        );
        //se->AddFunction( &ScriptExe::_SliderF3,       "Slider"        );
        se->AddFunction( &ScriptExe::_SliderF4,         "Slider"        );

        // TODO:
        //  PresentVR( left, left_layer, left_mipmap,  right, right_layer, right_mipmap )
        //  IsConsoleApp()  - UI or Console

        if ( FileSystem::IsDirectory( cfg.scriptHeaderOutFolder ))
        {
            se->AddCppHeader( "", "#define SCRIPT\n\n", 0 );

            CHECK_THROW( se->SaveCppHeader( cfg.scriptHeaderOutFolder / "res_editor" ));
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
        binder.AddValue( "None",        DebugView::EFlags::Unknown );
        binder.AddValue( "Histogram",   DebugView::EFlags::Histogram );
        STATIC_ASSERT( uint(DebugView::EFlags::_Count) == 3 );
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
        binder.AddValue( "RunOnce",     PassGroup::EFlags::RunOnce );
        binder.AddValue( "OnRequest",   PassGroup::EFlags::OnRequest );
        STATIC_ASSERT( uint(PassGroup::EFlags::All) == 3 );
    }

/*
=================================================
    _Bind_Constants
=================================================
*/
    void  ScriptExe::_Bind_Constants (const ScriptEnginePtr &se) __Th___
    {
        se->AddConstProperty( _sampConsts->NearestClamp,                "Sampler_NearestClamp" );
        se->AddConstProperty( _sampConsts->NearestRepeat,               "Sampler_NearestRepeat" );
        se->AddConstProperty( _sampConsts->NearestMirrorRepeat,         "Sampler_NearestMirrorRepeat" );
        se->AddConstProperty( _sampConsts->LinearClamp,                 "Sampler_LinearClamp" );
        se->AddConstProperty( _sampConsts->LinearRepeat,                "Sampler_LinearRepeat" );
        se->AddConstProperty( _sampConsts->LinearMirrorRepeat,          "Sampler_LinearMirrorRepeat" );
        se->AddConstProperty( _sampConsts->LinearMipmapClamp,           "Sampler_LinearMipmapClamp" );
        se->AddConstProperty( _sampConsts->LinearMipmapRepeat,          "Sampler_LinearMipmapRepeat" );
        se->AddConstProperty( _sampConsts->LinearMipmapMirrorRepeat,    "Sampler_LinearMipmapMirrorRepeat" );
        se->AddConstProperty( _sampConsts->Anisotropy8Repeat,           "Sampler_Anisotropy8Repeat" );
        se->AddConstProperty( _sampConsts->Anisotropy8MirrorRepeat,     "Sampler_Anisotropy8MirrorRepeat" );
        se->AddConstProperty( _sampConsts->Anisotropy8Clamp,            "Sampler_Anisotropy8Clamp" );
        se->AddConstProperty( _sampConsts->Anisotropy16Repeat,          "Sampler_Anisotropy16Repeat" );
        se->AddConstProperty( _sampConsts->Anisotropy16MirrorRepeat,    "Sampler_Anisotropy16MirrorRepeat" );
        se->AddConstProperty( _sampConsts->Anisotropy16Clamp,           "Sampler_Anisotropy16Clamp" );

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
        ScriptSceneGraphicsPass::GetShaderTypes( INOUT data );
    //  ScriptSceneRayTracingPass::GetShaderTypes( INOUT data );

        ScriptSphericalCube::GetShaderTypes( INOUT data );
        ScriptUniGeometry::GetShaderTypes( INOUT data );

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
        // init pipeline compiler
        ObjectStorage   obj_storage;
        PipelineStorage ppln_storage;
        {
            obj_storage.pplnStorage     = &ppln_storage;
            obj_storage.shaderFolders   = _GetTempData().cfg.shaderDirs;

            obj_storage.spirvCompiler   = MakeUnique<SpirvCompiler>( _GetTempData().cfg.includeDirs );
            obj_storage.spirvCompiler->SetDefaultResourceLimits();

            ObjectStorage::SetInstance( &obj_storage );

            PipelineCompiler::ScriptConfig  cfg;
            cfg.SetTarget( ECompilationTarget::Vulkan );
            cfg.SetShaderVersion( EShaderVersion::SPIRV_1_5 );
            cfg.SetShaderOptions( EShaderOpt::Optimize );
            cfg.SetDefaultLayout( EStructLayout::Std140 );
            cfg.SetPipelineOptions( EPipelineOpt::Unknown );
            cfg.SetPreprocessor( EShaderProprocessor::AEStyle );

            ScriptFeatureSetPtr fs {new ScriptFeatureSet{ "Default" }};
            fs->fs = RenderTaskScheduler().GetResourceManager().GetFeatureSet();
        }

        _LoadSamplers();                // throw
        _RegisterSharedShaderTypes();   // throw

        fn();

        ObjectStorage::SetInstance( null );
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
            ScriptSamplerPtr    samp{new ScriptSampler{"NearestClamp"}};
            samp->SetFilter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
            samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"NearestRepeat"}};
            samp->SetFilter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
            samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"NearestMirrorRepeat"}};
            samp->SetFilter( EFilter::Nearest, EFilter::Nearest, EMipmapFilter::Nearest );
            samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"LinearClamp"}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
            samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"LinearRepeat"}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
            samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"LinearMirrorRepeat"}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Nearest );
            samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"LinearMipmapClamp"}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"LinearMipmapRepeat"}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"LinearMipmapMirrorRepeat"}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"Anisotropy8Repeat"}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
            samp->SetAnisotropy( 8.0f );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"Anisotropy8MirrorRepeat"}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
            samp->SetAnisotropy( 8.0f );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"Anisotropy8Clamp"}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::ClampToEdge, EAddressMode::ClampToEdge, EAddressMode::ClampToEdge );
            samp->SetAnisotropy( 8.0f );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"Anisotropy16Repeat"}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::Repeat, EAddressMode::Repeat, EAddressMode::Repeat );
            samp->SetAnisotropy( 16.0f );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"Anisotropy16MirrorRepeat"}};
            samp->SetFilter( EFilter::Linear, EFilter::Linear, EMipmapFilter::Linear );
            samp->SetAddressMode( EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat, EAddressMode::MirrorRepeat );
            samp->SetAnisotropy( 16.0f );
        }{
            ScriptSamplerPtr    samp{new ScriptSampler{"Anisotropy16Clamp"}};
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
                    float4      frustum [6];
                )#");
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
