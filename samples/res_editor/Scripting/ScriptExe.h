// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/DataSource/Stream.h"

// resources
#include "res_editor/Scripting/ScriptImage.h"
#include "res_editor/Scripting/ScriptVideoImage.h"
#include "res_editor/Scripting/ScriptBuffer.h"
#include "res_editor/Scripting/ScriptRTScene.h"
#include "res_editor/Scripting/ScriptDynamicVars.h"

// pass/view
#include "res_editor/Passes/IPass.h"
#include "res_editor/Passes/Present.h"
#include "res_editor/Passes/PassGroup.h"
#include "res_editor/Scripting/ScriptPostprocess.h"
#include "res_editor/Scripting/ScriptComputePass.h"
#include "res_editor/Scripting/ScriptScene.h"

// geometry source
#include "res_editor/Scripting/ScriptGeomSource.h"

// controller
#include "res_editor/Scripting/ScriptController.h"

#include "res_editor/Scripting/ScriptCollection.h"

#include "res_editor/Passes/Renderer.h"


namespace AE::PipelineCompiler
{
    struct ObjectStorage;
    class PipelineStorage;
}

namespace AE::ResEditor
{

    //
    // Script Executor
    //

    class ScriptExe final
    {
    // types
    public:
        class ScriptPassApi;
        class ScriptResourceApi;

        struct Config
        {
            Path            cppTypesFolder;
            Path            scriptHeaderOutFolder;
        };

        struct ScriptConfig
        {
            RC<DynamicDim>  dynSize;
            Array<Path>     shaderDirs;
            Array<Path>     includeDirs;
            Array<Path>     pipelineDirs;
            Path            scriptDir;
        };


    private:
        class ScriptPresent;
        class ScriptDbgView;
        class ScriptPassGroup;
        class ScriptGenMipmaps;
        class ScriptBuildRTGeometry;
        class ScriptBuildRTScene;

        using ScriptPassGroupPtr    = ScriptRC<ScriptPassGroup>;
        using DynSlider_t           = Renderer::DynSlider_t;

        struct Slider : ScriptBasePass::Slider
        {
            DynSlider_t     dyn;
        };

        using ESlider               = ScriptBasePass::ESlider;
        using Sliders_t             = Array< Slider >;
        using UniqueSliderNames_t   = FlatHashSet< String >;
        using SliderCounter_t       = StaticArray< uint, uint(ESlider::_Count) >;

        struct TempData
        {
            ScriptConfig        cfg;
            RC<Renderer>        renderer;
            ScriptPassGroupPtr  passGroup;
            Array< Path >       currPath;
            Array< Path >       dependencies;
            uint                dbgViewCounter  = 0;
            int                 passGroupDepth  = 0;

            Sliders_t           sliders;
            UniqueSliderNames_t uniqueSliderNames;
            SliderCounter_t     sliderCounter       {};
        };

        struct SamplerConsts;


    // variables
    public:
        SpinLock                _guard;

        ScriptEnginePtr         _engine;
        ScriptEnginePtr         _engine2;           // for pipeline compiler

        Unique<TempData>        _tempData;          // temporary during 'Run()'
        Unique<SamplerConsts>   _sampConsts;

        const Config            _config;


    // methods
    public:
        explicit ScriptExe (Config cfg);
        ~ScriptExe ();

        ND_ RC<Renderer>  Run (const Path &filePath, const ScriptConfig &cfg)                                   __NE___;


    private:
        ND_ bool  _CompilePipeline (const Path &pplnPath);
        ND_ bool  _CompilePipelineFromSource (const Path &pplnPath, StringView source);

        ND_ RTechInfo       _ConvertAndLoad ();
        ND_ RC<RStream>     _ConvertAndLoad2 ();
        ND_ Renderer*       _GetRenderer ()                                                                     __Th___;

            void  _LoadSamplers ()                                                                              __Th___;

        template <typename ...Args>
        ND_ bool  _Run (const Path &filePath, Args ...args)                                                     __NE___;
        ND_ bool  _Run2 (const Path &filePath)                                                                  __NE___;

            void  _RunWithPipelineCompiler (Function<void ()> fn)                                               __Th___;

        ND_ static TempData&  _GetTempData ()                                                                   __Th___;

        static void  _Bind (const ScriptEnginePtr &se, const Config &)                                          __Th___;
        static void  _SaveCppStructs (const Path &fname)                                                        __Th___;
        static void  _Bind_DbgViewFlags (const ScriptEnginePtr &se)                                             __Th___;
        static void  _Bind_PassGroupFlags (const ScriptEnginePtr &se)                                           __Th___;
               void  _Bind_Constants (const ScriptEnginePtr &se)                                                __Th___;

        static void  _RegisterSharedShaderTypes ()                                                              __Th___;
        static void  _GetSharedShaderTypes (ScriptBasePass::CppStructsFromShaders &)                            __Th___;

        static ScriptDynamicDim*  _SurfaceSize ()                                                               __Th___;

        static void  _Present1 (const ScriptImagePtr &rt)                                                       __Th___;
        static void  _Present2 (const ScriptImagePtr &rt, const MipmapLevel &mipmap)                            __Th___;
        static void  _Present3 (const ScriptImagePtr &rt, const ImageLayer &layer)                              __Th___;
        static void  _Present4 (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap)   __Th___;

        static void  _GenMipmaps (const ScriptImagePtr &rt)                                                     __Th___;
        static void  _BuildRTGeometry ()                                                                        __Th___;
        static void  _BuildRTScene ()                                                                           __Th___;

        static void  _DbgView1 (const ScriptImagePtr &rt, DebugView::EFlags flags)                                                          __Th___;
        static void  _DbgView2 (const ScriptImagePtr &rt, const MipmapLevel &mipmap, DebugView::EFlags flags)                               __Th___;
        static void  _DbgView3 (const ScriptImagePtr &rt, const ImageLayer &layer, DebugView::EFlags flags)                                 __Th___;
        static void  _DbgView4 (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap, DebugView::EFlags flags)      __Th___;

    //  static void  _SliderI0 (const ScriptDynamicIntPtr &dyn, const String &name)                                                         __Th___;
    //  static void  _SliderI1 (const ScriptDynamicIntPtr &dyn,const String &name, int min, int max)                                        __Th___;
    //  static void  _SliderI2 (const ScriptDynamicInt2Ptr &dyn,const String &name, const packed_int2 &min, const packed_int2 &max)         __Th___;
    //  static void  _SliderI3 (const ScriptDynamicInt3Ptr &dyn,const String &name, const packed_int3 &min, const packed_int3 &max)         __Th___;
        static void  _SliderI4 (const ScriptDynamicInt4Ptr &dyn,const String &name, const packed_int4 &min, const packed_int4 &max)         __Th___;

        static void  _SliderU0 (const ScriptDynamicUIntPtr &dyn, const String &name)                                                        __Th___;
        static void  _SliderU1 (const ScriptDynamicUIntPtr &dyn,const String &name, uint min, uint max)                                     __Th___;
    //  static void  _SliderU2 (const ScriptDynamicUInt2Ptr &dyn,const String &name, const packed_uint2 &min, const packed_uint2 &max)      __Th___;
        static void  _SliderU3 (const ScriptDynamicUInt3Ptr &dyn,const String &name, const packed_uint3 &min, const packed_uint3 &max)      __Th___;
    //  static void  _SliderU4 (const ScriptDynamicUInt4Ptr &dyn,const String &name, const packed_uint4 &min, const packed_uint4 &max)      __Th___;

        static void  _SliderF0 (const ScriptDynamicFloatPtr &dyn, const String &name)                                                       __Th___;
        static void  _SliderF1 (const ScriptDynamicFloatPtr &dyn, const String &name, float min, float max)                                 __Th___;
    //  static void  _SliderF2 (const ScriptDynamicFloat2Ptr &dyn, const String &name, const packed_float2 &min, const packed_float2 &max)  __Th___;
    //  static void  _SliderF3 (const ScriptDynamicFloat3Ptr &dyn, const String &name, const packed_float3 &min, const packed_float3 &max)  __Th___;
        static void  _SliderF4 (const ScriptDynamicFloat4Ptr &dyn, const String &name, const packed_float4 &min, const packed_float4 &max)  __Th___;

        template <typename D, typename T>
        static void  _Slider (const D &dyn, const String &name, const T &min, const T &max, ESlider type)                                   __Th___;

        static void  _AddSlidersToUIInteraction (TempData &, Renderer* renderer);

        ND_ static ScriptBasePass*  _RunScript1 (const String &filePath, const ScriptCollectionPtr &)               __Th___;
        ND_ static ScriptBasePass*  _RunScript2 (const String &filePath, uint flags, const ScriptCollectionPtr &)   __Th___;
    };


    //
    // Script Pass API
    //
    class ScriptExe::ScriptPassApi
    {
        friend class ScriptPostprocess;
        friend class ScriptComputePass;
        friend class ScriptSceneGraphicsPass;
    //  friend class ScriptSceneRayTracingPass;

            static RTechInfo    ConvertAndLoad (Function<void (ScriptEnginePtr)> fn)    __Th___;
            static void         AddPass (ScriptBasePassPtr)                             __Th___;

        ND_ static Path         GetCurrentFile ()                                       __Th___;
        ND_ static Path         ToAbsolute (const Path &)                               __Th___;
        ND_ static Path         ToShaderPath (const Path &)                             __Th___;

        ND_ static Path         ToPipelinePath (const Path &)                           __Th___;
        ND_ static Path         ToPipelineFolder (const Path &)                         __Th___;

        ND_ static Renderer&    GetRenderer ()                                          __Th___;
    };


    //
    // Script Resource API
    //
    class ScriptExe::ScriptResourceApi
    {
        friend class ScriptBuffer;
        friend class ScriptImage;
        friend class ScriptVideoImage;
        friend class ScriptSphericalCube;
        friend class ScriptUniGeometry;
        friend class ScriptRTGeometry;
        friend class ScriptRTScene;

        ND_ static Renderer&    GetRenderer ()                                          __Th___;
        ND_ static bool         IsPassGroup (const ScriptBasePassPtr &pass)             __NE___;
    };


} // AE::ResEditor


#include "graphics/Scripting/GraphicsBindings.h"

// resources
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptImage,                     "Image"             );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptVideoImage,                "VideoImage"        );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptBuffer,                    "Buffer"            );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptRTGeometry,                "RTGeometry"        );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptRTScene,                   "RTScene"           );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptDynamicDim,                "DynamicDim"        );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptDynamicUInt3,              "DynamicUInt3"      );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptDynamicInt4,               "DynamicInt4"       );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptDynamicFloat4,             "DynamicFloat4"     );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptDynamicUInt,               "DynamicUInt"       );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptDynamicULong,              "DynamicULong"      );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptDynamicFloat,              "DynamicFloat"      );
//AE_DECL_SCRIPT_OBJ_RC(AE::ResEditor::ScriptDynamicMatrix4x4,          "DynamicMat4x4"     );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptCollection,                "Collection"        );

AE_DECL_SCRIPT_OBJ(     AE::ResEditor::RTInstanceCustomIndex,           "RTInstanceCustomIndex" );
AE_DECL_SCRIPT_OBJ(     AE::ResEditor::RTInstanceMask,                  "RTInstanceMask"    );
AE_DECL_SCRIPT_OBJ(     AE::ResEditor::RTInstanceSBTOffset,             "RTInstanceSBTOffset" );

// pass/view
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptBasePass,                  "IPass"             );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptPostprocess,               "Postprocess"       );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptComputePass,               "ComputePass"       );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptScene,                     "Scene"             );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptSceneGraphicsPass,         "SceneGraphicsPass" );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptSceneRayTracingPass,       "SceneRayTracingPass");

// controller
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptBaseController,            "BaseController"    );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptController2D,              "Controller2D"      );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptControllerFlightCamera,    "FlightCamera"      );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptControllerFPVCamera,       "FPSCamera"         );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptControllerFreeCamera,      "FPVCamera"         );

// geometry
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptGeomSource,                "GeomSource"        );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptSphericalCube,             "SphericalCube"     );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptUniGeometry,               "UnifiedGeometry"   );

AE_DECL_SCRIPT_OBJ(     AE::ResEditor::ScriptUniGeometry::DrawCmd3,                         "UnifiedGeometry_Draw" );
AE_DECL_SCRIPT_OBJ(     AE::ResEditor::ScriptUniGeometry::DrawIndexedCmd3,                  "UnifiedGeometry_DrawIndexed" );
AE_DECL_SCRIPT_OBJ(     AE::ResEditor::ScriptUniGeometry::DrawIndirectCmd3,                 "UnifiedGeometry_DrawIndirect" );
AE_DECL_SCRIPT_OBJ(     AE::ResEditor::ScriptUniGeometry::DrawIndexedIndirectCmd3,          "UnifiedGeometry_DrawIndexedIndirect" );
AE_DECL_SCRIPT_OBJ(     AE::ResEditor::ScriptUniGeometry::DrawMeshTasksCmd3,                "UnifiedGeometry_DrawMeshTasks" );
AE_DECL_SCRIPT_OBJ(     AE::ResEditor::ScriptUniGeometry::DrawMeshTasksIndirectCmd3,        "UnifiedGeometry_DrawMeshTasksIndirect" );
AE_DECL_SCRIPT_OBJ(     AE::ResEditor::ScriptUniGeometry::DrawIndirectCountCmd3,            "UnifiedGeometry_DrawIndirectCount" );
AE_DECL_SCRIPT_OBJ(     AE::ResEditor::ScriptUniGeometry::DrawIndexedIndirectCountCmd3,     "UnifiedGeometry_DrawIndexedIndirectCount" );
AE_DECL_SCRIPT_OBJ(     AE::ResEditor::ScriptUniGeometry::DrawMeshTasksIndirectCountCmd3,   "UnifiedGeometry_DrawMeshTasksIndirectCount" );

AE_DECL_SCRIPT_TYPE(    AE::ResEditor::ScriptBasePass::EFlags,          "EPassFlags"        );
AE_DECL_SCRIPT_TYPE(    AE::ResEditor::ScriptPostprocess::EPostprocess, "EPostprocess"      );
AE_DECL_SCRIPT_TYPE(    AE::ResEditor::DebugView::EFlags,               "DbgViewFlags"      );
AE_DECL_SCRIPT_TYPE(    AE::ResEditor::PassGroup::EFlags,               "ScriptFlags"       );
AE_DECL_SCRIPT_TYPE(    AE::ResEditor::ScriptImage::ELoadOpFlags,       "ImageLoadOpFlags"  );