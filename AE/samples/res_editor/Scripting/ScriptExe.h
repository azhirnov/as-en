// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

// resources
#include "res_editor/Scripting/ScriptImage.h"
#include "res_editor/Scripting/ScriptVideoImage.h"
#include "res_editor/Scripting/ScriptBuffer.h"
#include "res_editor/Scripting/ScriptRTScene.h"
#include "res_editor/Scripting/ScriptDynamicVars.h"

// pass/view
#include "res_editor/Passes/IPass.h"
#include "res_editor/Passes/OtherPasses.h"
#include "res_editor/Passes/PassGroup.h"
#include "res_editor/Scripting/ScriptPostprocess.h"
#include "res_editor/Scripting/ScriptComputePass.h"
#include "res_editor/Scripting/ScriptRayTracingPass.h"
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
            using VFSPaths_t    = ArrayView< Pair< Path, String >>;

            Path                cppTypesFolder;
            Path                scriptHeaderOutFolder;

            ArrayView<Path>     scriptIncludeDirs;
            ArrayView<Path>     pipelineIncludeDirs;

            // emulate VFS
            VFSPaths_t          vfsPaths;
        };

        struct ScriptConfig
        {
            RC<DynamicDim>  dynSize;
            Array<Path>     shaderDirs;
            Array<Path>     includeDirs;
            Array<Path>     pipelineDirs;
            Path            scriptDir;
            App::Monitor    monitor;
        };


    private:
        class ScriptPresent;
        class ScriptDbgView;
        class ScriptPassGroup;
        class ScriptGenMipmaps;
        class ScriptCopyImage;
        class ScriptClearImage;
        class ScriptClearBuffer;
        class ScriptBuildRTGeometry;
        class ScriptBuildRTScene;
        class ScriptExportImage;
        class ScriptExportBuffer;

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
            uint                dbgViewCounter      = 0;
            int                 passGroupDepth      = 0;

            bool                hasPresent          = false;

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

        Random                  _rand;


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

        ND_ bool  _Run (const Path &filePath, const ScriptCollectionPtr &collection)                            __NE___;
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
        static void  _Present5 (const ScriptImagePtr &rt, EColorSpace cs)                                       __Th___;
        static void  _Present6 (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap,
                                EColorSpace cs)                                                                 __Th___;

        static void  _GenMipmaps (const ScriptImagePtr &rt)                                                     __Th___;
        static void  _CopyImage (const ScriptImagePtr &src, const ScriptImagePtr &dst)                          __Th___;

        static void  _ClearImage1 (const ScriptImagePtr &image, const RGBA32f &value)                           __Th___;
        static void  _ClearImage2 (const ScriptImagePtr &image, const RGBA32u &value)                           __Th___;
        static void  _ClearImage3 (const ScriptImagePtr &image, const RGBA32i &value)                           __Th___;

        static void  _ClearBuffer1 (const ScriptBufferPtr &buf, uint value)                                     __Th___;
        static void  _ClearBuffer2 (const ScriptBufferPtr &buf, ulong offset, ulong size, uint value)           __Th___;

        static void  _ExportImage (const ScriptImagePtr &image, const String &prefix)                           __Th___;
        static void  _ExportBuffer (const ScriptBufferPtr &buffer, const String &prefix)                        __Th___;
        static void  _ExportGeometry (const ScriptGeomSourcePtr &geom, const String &prefix)                    __Th___;

        static void  _BuildRTGeometry (const ScriptRTGeometryPtr &)                                             __Th___;
        static void  _BuildRTGeometryIndirect (const ScriptRTGeometryPtr &)                                     __Th___;

        static void  _BuildRTScene (const ScriptRTScenePtr &)                                                   __Th___;
        static void  _BuildRTSceneIndirect (const ScriptRTScenePtr &)                                           __Th___;

        static void  _GetCube2 (OUT ScriptArray<packed_float3>  &positions,
                                OUT ScriptArray<packed_float3>  &normals,
                                OUT ScriptArray<uint>           &indices)                                       __Th___;
        static void  _GetCube3 (OUT ScriptArray<packed_float3>  &positions,
                                OUT ScriptArray<packed_float3>  &normals,
                                OUT ScriptArray<packed_float3>  &tangents,
                                OUT ScriptArray<packed_float3>  &bitangents,
                                OUT ScriptArray<packed_float2>  &texcoords,         // 2d
                                OUT ScriptArray<uint>           &indices)                                       __Th___;
        static void  _GetCube4 (OUT ScriptArray<packed_float3>  &positions,
                                OUT ScriptArray<packed_float3>  &normals,
                                OUT ScriptArray<packed_float3>  &tangents,
                                OUT ScriptArray<packed_float3>  &bitangents,
                                OUT ScriptArray<packed_float3>  &texcoords,         // cubemap
                                OUT ScriptArray<uint>           &indices)                                       __Th___;

        static void  _GetSphericalCube1 (uint                           lod,
                                         OUT ScriptArray<packed_float3> &positions,
                                         OUT ScriptArray<uint>          &indices)                               __Th___;

        static void  _GetSphere1 (uint                              lod,
                                  OUT ScriptArray<packed_float3>    &positions,
                                  OUT ScriptArray<uint>             &indices)                                   __Th___;
        static void  _GetSphere2 (uint                              lod,
                                  OUT ScriptArray<packed_float3>    &positions,
                                  OUT ScriptArray<packed_float3>    &texcoords,     // cubemap
                                  OUT ScriptArray<uint>             &indices)                                   __Th___;
        static void  _GetSphere3 (uint                              lod,
                                  OUT ScriptArray<packed_float3>    &positions,
                                  OUT ScriptArray<packed_float3>    &normals,
                                  OUT ScriptArray<packed_float3>    &tangents,
                                  OUT ScriptArray<packed_float3>    &bitangents,
                                  OUT ScriptArray<packed_float3>    &texcoords,     // cubemap
                                  OUT ScriptArray<uint>             &indices)                                   __Th___;
        static void  _GetSphere4 (uint                              lod,
                                  OUT ScriptArray<packed_float3>    &positions,
                                  OUT ScriptArray<packed_float3>    &normals,
                                  OUT ScriptArray<packed_float3>    &tangents,
                                  OUT ScriptArray<packed_float3>    &bitangents,
                                  OUT ScriptArray<packed_float2>    &texcoords,     // 2d
                                  OUT ScriptArray<uint>             &indices)                                   __Th___;

        static void  _GetGrid1 (uint                            size,
                                OUT ScriptArray<packed_float2>  &positions,         // unorm
                                OUT ScriptArray<uint>           &indices)                                       __Th___;
        static void  _GetGrid2 (uint                            size,
                                OUT ScriptArray<packed_float3>  &positions,         // unorm, XY space
                                OUT ScriptArray<uint>           &indices)                                       __Th___;

        static void  _GetCylinder1 (uint                            segmentCount,
                                    bool                            isInner,
                                    OUT ScriptArray<packed_float3>  &positions,
                                    OUT ScriptArray<packed_float2>  &texcoords,
                                    OUT ScriptArray<uint>           &indices)                                   __Th___;
        static void  _GetCylinder2 (uint                            segmentCount,
                                    bool                            isInner,
                                    OUT ScriptArray<packed_float3>  &positions,
                                    OUT ScriptArray<packed_float3>  &normals,
                                    OUT ScriptArray<packed_float3>  &tangents,
                                    OUT ScriptArray<packed_float3>  &bitangents,
                                    OUT ScriptArray<packed_float2>  &texcoords,
                                    OUT ScriptArray<uint>           &indices)                                   __Th___;

        static void  _IndicesToPrimitives (const ScriptArray<uint>          &indices,
                                           OUT ScriptArray<packed_uint3>    &primitives)                        __Th___;

        static void  _GetFrustumPlanes (const packed_float4x4           &viewProj,
                                        OUT ScriptArray<packed_float4>  &planes)                                __Th___;

        static void  _MergeMesh (INOUT ScriptArray<uint>    &srcIndices,
                                 uint                       srcVertexCount,
                                 const ScriptArray<uint>    &indicesToAdd)                                      __Th___;

        static void  _ExtrudeAndMerge (const ScriptArray<packed_float2>     &lineStrip,
                                        float                               height,
                                        INOUT ScriptArray<packed_float3>    &positions,
                                        INOUT ScriptArray<uint>             &indices)                           __Th___;

        static void  _TriangulateAndMerge1 (const ScriptArray<packed_float2>    &lineStrip,
                                            float                               yCoord,
                                            INOUT ScriptArray<packed_float3>    &positions,
                                            INOUT ScriptArray<uint>             &indices)                       __Th___;
        static void  _TriangulateAndMerge2 (const ScriptArray<packed_float2>    &vertices,
                                            const ScriptArray<uint>             &lineListIndices,
                                            float                               yCoord,
                                            INOUT ScriptArray<packed_float3>    &positions,
                                            INOUT ScriptArray<uint>             &indices)                       __Th___;
        static void  _ImplTriangulateAndMerge (ArrayView<float2>                tmp_vertices,
                                               ArrayView<uint>                  tmp_indices,
                                               float                            yCoord,
                                               INOUT ScriptArray<packed_float3> &positions,
                                               INOUT ScriptArray<uint>          &indices)                       __Th___;

        static void  _TriangulateExtrudeAndMerge1 (const ScriptArray<packed_float2> &lineStrip,
                                                   float                            height,
                                                   INOUT ScriptArray<packed_float3> &positions,
                                                   INOUT ScriptArray<uint>          &indices)                   __Th___;

        static void  _TriangulateExtrudeAndMerge2 (const ScriptArray<packed_float2> &vertices,
                                                   const ScriptArray<uint>          &lineListIndices,
                                                   float                            height,
                                                   INOUT ScriptArray<packed_float3> &positions,
                                                   INOUT ScriptArray<uint>          &indices)                   __Th___;
        static void  _ImplTriangulateExtrudeAndMerge (ArrayView<float2>                 tmp_vertices,
                                                      ArrayView<uint>                   tmp_indices,
                                                      ArrayView<uint>                   tmp_boundary,
                                                      const float                       height,
                                                      INOUT ScriptArray<packed_float3>  &positions,
                                                      INOUT ScriptArray<uint>           &indices)               __Th___;

        static void  _NormalizeSpectrum (INOUT ScriptArray<packed_float4> &)                                    __Th___;
        static void  _WhiteColorSpectrum3 (OUT ScriptArray<packed_float4> &)                                    __Th___;
        static void  _WhiteColorSpectrum7 (OUT ScriptArray<packed_float4> &, bool)                              __Th___;
        static void  _WhiteColorSpectrumStep50nm (OUT ScriptArray<packed_float4> &, bool)                       __Th___;
        static void  _WhiteColorSpectrumStep100nm (OUT ScriptArray<packed_float4> &, bool)                      __Th___;

        static packed_float3  _CM_CubeSC_Forward (const packed_float3 &);
        static packed_float3  _CM_IdentitySC_Forward (const packed_float3 &);
        static packed_float3  _CM_TangentialSC_Forward (const packed_float3 &);

        static void  _DbgView1 (const ScriptImagePtr &rt, DebugView::EFlags flags)                                                              __Th___;
        static void  _DbgView2 (const ScriptImagePtr &rt, const MipmapLevel &mipmap, DebugView::EFlags flags)                                   __Th___;
        static void  _DbgView3 (const ScriptImagePtr &rt, const ImageLayer &layer, DebugView::EFlags flags)                                     __Th___;
        static void  _DbgView4 (const ScriptImagePtr &rt, const ImageLayer &layer, const MipmapLevel &mipmap, DebugView::EFlags flags)          __Th___;

        static void  _SliderI0 (const ScriptDynamicIntPtr  &dyn, const String &name)                                                            __Th___;
        static void  _SliderI1 (const ScriptDynamicIntPtr  &dyn, const String &name, int min, int max)                                          __Th___;
        static void  _SliderI2 (const ScriptDynamicInt2Ptr &dyn, const String &name, const packed_int2 &min, const packed_int2 &max)            __Th___;
        static void  _SliderI3 (const ScriptDynamicInt3Ptr &dyn, const String &name, const packed_int3 &min, const packed_int3 &max)            __Th___;
        static void  _SliderI4 (const ScriptDynamicInt4Ptr &dyn, const String &name, const packed_int4 &min, const packed_int4 &max)            __Th___;

        static void  _SliderI1a (const ScriptDynamicIntPtr  &, const String &, int min, int max, int val)                                       __Th___;
        static void  _SliderI2a (const ScriptDynamicInt2Ptr &, const String &, const packed_int2 &, const packed_int2 &, const packed_int2 &)   __Th___;
        static void  _SliderI3a (const ScriptDynamicInt3Ptr &, const String &, const packed_int3 &, const packed_int3 &, const packed_int3 &)   __Th___;
        static void  _SliderI4a (const ScriptDynamicInt4Ptr &, const String &, const packed_int4 &, const packed_int4 &, const packed_int4 &)   __Th___;

        static void  _SliderU0 (const ScriptDynamicUIntPtr  &dyn, const String &name)                                                           __Th___;
        static void  _SliderU1 (const ScriptDynamicUIntPtr  &dyn, const String &name, uint min, uint max)                                       __Th___;
        static void  _SliderU2 (const ScriptDynamicUInt2Ptr &dyn, const String &name, const packed_uint2 &min, const packed_uint2 &max)         __Th___;
        static void  _SliderU3 (const ScriptDynamicUInt3Ptr &dyn, const String &name, const packed_uint3 &min, const packed_uint3 &max)         __Th___;
        static void  _SliderU4 (const ScriptDynamicUInt4Ptr &dyn, const String &name, const packed_uint4 &min, const packed_uint4 &max)         __Th___;

        static void  _SliderU1a (const ScriptDynamicUIntPtr &, const String&, uint min, uint max, uint val)                                     __Th___;
        static void  _SliderU2a (const ScriptDynamicUInt2Ptr&, const String&, const packed_uint2&, const packed_uint2&, const packed_uint2&)    __Th___;
        static void  _SliderU3a (const ScriptDynamicUInt3Ptr&, const String&, const packed_uint3&, const packed_uint3&, const packed_uint3&)    __Th___;
        static void  _SliderU4a (const ScriptDynamicUInt4Ptr&, const String&, const packed_uint4&, const packed_uint4&, const packed_uint4&)    __Th___;

        static void  _SliderF0 (const ScriptDynamicFloatPtr  &dyn, const String &name)                                                          __Th___;
        static void  _SliderF1 (const ScriptDynamicFloatPtr  &dyn, const String &name, float min, float max)                                    __Th___;
        static void  _SliderF2 (const ScriptDynamicFloat2Ptr &dyn, const String &name, const packed_float2 &min, const packed_float2 &max)      __Th___;
        static void  _SliderF3 (const ScriptDynamicFloat3Ptr &dyn, const String &name, const packed_float3 &min, const packed_float3 &max)      __Th___;
        static void  _SliderF4 (const ScriptDynamicFloat4Ptr &dyn, const String &name, const packed_float4 &min, const packed_float4 &max)      __Th___;

        static void  _SliderF1a (const ScriptDynamicFloatPtr &, const String&, float min, float max, float val)                                 __Th___;
        static void  _SliderF2a (const ScriptDynamicFloat2Ptr&, const String&, const packed_float2&, const packed_float2&, const packed_float2&)__Th___;
        static void  _SliderF3a (const ScriptDynamicFloat3Ptr&, const String&, const packed_float3&, const packed_float3&, const packed_float3&)__Th___;
        static void  _SliderF4a (const ScriptDynamicFloat4Ptr&, const String&, const packed_float4&, const packed_float4&, const packed_float4&)__Th___;

        template <typename D, typename T>
        static void  _Slider (const D &dyn, const String &name, const T &min, const T &max, const T &initial, ESlider type)                     __Th___;

        static void  _AddSlidersToUIInteraction (TempData &, Renderer* renderer)                                                                __NE___;

        ND_ static ScriptBasePass*  _RunScript1 (const String &filePath, const ScriptCollectionPtr &)                                           __Th___;
        ND_ static ScriptBasePass*  _RunScript2 (const String &filePath, PassGroup::EFlags flags, const ScriptCollectionPtr &)                  __Th___;
    };


    //
    // Script Pass API
    //
    class ScriptExe::ScriptPassApi
    {
        friend class ScriptPostprocess;
        friend class ScriptComputePass;
        friend class ScriptRayTracingPass;
        friend class ScriptSceneGraphicsPass;
        friend class ScriptSceneRayTracingPass;

            static RTechInfo    ConvertAndLoad (Function<void (ScriptEnginePtr)> fn)    __Th___;
            static void         WithPipelineCompiler (Function<void()> fn)              __Th___;
            static void         AddPass (ScriptBasePassPtr)                             __Th___;

        ND_ static Path         GetCurrentFile ()                                       __Th___;
        ND_ static Path         ToAbsolute (const Path &)                               __Th___;
        ND_ static Path         ToShaderPath (const Path &)                             __Th___;

        ND_ static Path         ToPipelinePath (const Path &)                           __Th___;
        ND_ static Path         ToPipelineFolder (const Path &)                         __Th___;

        ND_ static Renderer&    GetRenderer ()                                          __Th___;

        ND_ static ArrayView<Path>  GetPipelineIncludeDirs ()                           __NE___;

        ND_ static App::Monitor const&  GetMonitor ()                                   __Th___;
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
        friend class ScriptModelGeometrySrc;

        ND_ static Renderer&    GetRenderer ()                                          __Th___;
        ND_ static bool         IsPassGroup (const ScriptBasePassPtr &pass)             __NE___;

        ND_ static Path         ToAbsolute (const String &)                             __Th___;

    public:
        ND_ static Graphics::FeatureSet const&  GetFeatureSet ()                        __NE___;
    };


} // AE::ResEditor


#include "graphics/Scripting/GraphicsBindings.h"

AE_DECL_SCRIPT_OBJ(     AE::ResEditor::RTInstanceCustomIndex,           "RTInstanceCustomIndex" );
AE_DECL_SCRIPT_OBJ(     AE::ResEditor::RTInstanceMask,                  "RTInstanceMask"    );
AE_DECL_SCRIPT_OBJ(     AE::ResEditor::RTInstanceSBTOffset,             "RTInstanceSBTOffset" );
AE_DECL_SCRIPT_OBJ(     AE::ResEditor::RTInstanceTransform,             "RTInstanceTransform" );

// pass/view
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptBasePass,                  "IPass"             );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptPostprocess,               "Postprocess"       );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptComputePass,               "ComputePass"       );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptRayTracingPass,            "RayTracingPass"    );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptScene,                     "Scene"             );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptSceneGraphicsPass,         "SceneGraphicsPass" );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptSceneRayTracingPass,       "SceneRayTracingPass");

// controller
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptBaseController,            "BaseController"    );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptControllerScaleBias,       "ScaleBiasCamera"   );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptControllerTopDown,         "TopDownCamera"     );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptControllerOrbitalCamera,   "OrbitalCamera"     );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptControllerFlightCamera,    "FlightCamera"      );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptControllerFPVCamera,       "FPSCamera"         );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptControllerFreeCamera,      "FPVCamera"         );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptControllerRemoteCamera,    "RemoteCamera"      );

// geometry
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptGeomSource,                "GeomSource"        );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptSphericalCube,             "SphericalCube"     );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptUniGeometry,               "UnifiedGeometry"   );
AE_DECL_SCRIPT_OBJ_RC(  AE::ResEditor::ScriptModelGeometrySrc,          "Model"             );

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
AE_DECL_SCRIPT_TYPE(    AE::Graphics::EColorSpace,                      "EColorSpace"       );
AE_DECL_SCRIPT_TYPE(    AE::ResEditor::ERenderLayer,                    "ERenderLayer"      );
