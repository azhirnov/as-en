// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"
#include "res_editor/Core/RenderGraph.h"
#include "res_editor/Dynamic/DynamicDimension.h"

struct ImGuiContext;

namespace AE::ResEditor
{
    class ResEditorCore;


    //
    // UI/Input/Renderer Interaction
    //

    class UIInteraction
    {
    // types
    public:
        static constexpr uint   MaxDebugViews       = 7;
        static constexpr uint   MaxSlidersPerType   = 4;

        static constexpr uint   IntSliderIdx        = 0;
        static constexpr uint   FloatSliderIdx      = 1;
        static constexpr uint   ColorSelectorIdx    = 2;

        struct PerPassMutableSlidersImpl
        {
            StaticArray< int4,    MaxSlidersPerType >       intSliders      {};
            StaticArray< float4,  MaxSlidersPerType >       floatSliders    {};
            StaticArray< RGBA32f, MaxSlidersPerType >       colors          {};
        };
        using PerPassMutableSliders = Synchronized< RWSpinLock, PerPassMutableSlidersImpl >;

        struct PerPassSlidersInfo
        {
            String                                                      passName;
            StaticArray< StaticArray< String, 3>, MaxSlidersPerType >   names           {};     // int, float, color
            StaticArray< StaticArray< int4, 3 >,  MaxSlidersPerType >   intRange        {};     // min, max, initial
            StaticArray< ubyte,                   MaxSlidersPerType >   intVecSize      {};
            StaticArray< StaticArray< float4, 3>, MaxSlidersPerType >   floatRange      {};     // min, max, initial
            StaticArray< ubyte,                   MaxSlidersPerType >   floatVecSize    {};
            StaticArray< RGBA32f,                 MaxSlidersPerType >   colors          {};     // initial
        };
        using AllSliders_t = Array< Tuple< PerPassMutableSliders*, const PerPassSlidersInfo* >>;

        using EDebugMode        = IPass::EDebugMode;
        using EDebugModeBits    = EnumBitSet< EDebugMode >;

        struct PassDebugInfo
        {
            String              name;
            const void*         pass    = null;
            EDebugModeBits      modes;
            EShaderStages       stages  = Default;
        };

    private:
        using DbgImageView_t    = StaticArray< AtomicRC<Image>, MaxDebugViews >;
        using SliderMap_t       = HashMap< const void*, Tuple< PerPassMutableSliders, PerPassSlidersInfo >>;
        using PassDbgMap_t      = HashMap< const void*, PassDebugInfo >;
        using AllPassDbgInfo_t  = Array< PassDebugInfo const *>;

        struct Debugger
        {
            void const*         target  = null;
            EDebugMode          mode    = Default;
            EShaderStages       stage   = Default;
            float2              coord;
        };

        struct SelectedPixel
        {
            FrameUID            frame;
            uint2               pos;
            uint2               pendingPos;
            RGBA32f             color;      // TODO: colors [8]  ???
        };

        struct Graphics
        {
            EPixelFormat        colorFormat     = Default;
            EColorSpace         colorSpace      = Default;
            EPresentMode        presentMode     = Default;
            RC<DynamicDim>      dynSize;
            RC<DynamicUInt>     filterMode;
            uint                colorModeIdx    = UMax;
            uint                presentModeIdx  = UMax;
        };

        struct Capture
        {
            bool                video           = false;
            bool                screenshot      = false;
            EImageFormat        imageFormat     = EImageFormat::DDS;
            float               bitrate         = 50.0f;    // Mbit/s
            EVideoFormat        videoFormat     = EVideoFormat::YUV420P;
            EVideoCodec         videoCodec      = EVideoCodec::H265;
            EVideoColorPreset   colorPreset     = EVideoColorPreset::Unspecified;
        };


    // variables
    private:
        DbgImageView_t                              _dbgView;
        Synchronized< SharedMutex, SliderMap_t >    _sliderMap;
        Synchronized< SharedMutex, PassDbgMap_t >   _passDbgMap;

    public:
        Synchronized< RWSpinLock, Capture >         capture;
        Synchronized< RWSpinLock, Debugger >        debugger;
        Synchronized< RWSpinLock, SelectedPixel >   selectedPixel;
        Synchronized< RWSpinLock, Graphics >        graphics;       // swapchain, VR, ...


    // methods
    public:
        UIInteraction ();

            void  RemovePass (const void* uid);

            void  AddPassDbgInfo (const IPass* uid, EDebugModeBits modes, EShaderStages stages);
        ND_ auto  GetAllPassDbgInfo () const -> AllPassDbgInfo_t;

            void  AddSliders (const void* uid, PerPassSlidersInfo info);
        ND_ auto  GetSliders (const void* uid) const -> Ptr<const PerPassMutableSliders>;
        ND_ auto  GetAllSliders () -> AllSliders_t;

            void        SetDbgView (usize idx, RC<Image> img);
            void        ResetDbgView (usize idx);
        ND_ RC<Image>   GetDbgView (usize idx);

        ND_ RC<DynamicDim>  GetDynamicSize ()       { return graphics->dynSize; }
        ND_ RC<DynamicUInt> GetFilterMode ()        { return graphics->filterMode; }

        ND_ static UIInteraction&  Instance ();
    };



    //
    // Editor UI
    //

    class EditorUI
    {
    // types
    private:
        class UploadTask;
        class DrawTask;

        using TimePoint_t       = std::chrono::high_resolution_clock::time_point;
        using PerFrameDescSet_t = StaticArray< Strong<DescriptorSetID>, GraphicsConfig::MaxFrames >;
        using DbgViewData_t     = StaticArray< float4, UIInteraction::MaxDebugViews >;

        struct ScriptFolder
        {
            String                          name;
            usize                           baseId  = UMax;     // ID for selection
            Array< Unique<ScriptFolder> >   folders;
            Array< String >                 scripts;
        };

        struct ImGuiData
        {
            ImGuiContext*       ctx                 = null;
            float2              mousePos;
            float2              mouseWheel;
            bool                mouseLBDown         = false;
            bool                showDbgViews        = true;
            bool                showUI              = true;
            bool                runShaderDebugger   = false;
            bool                reloadScript        = false;
            bool                showHelp            = false;
            uint                activeTab           = UMax;
            usize               nodeClicked         = UMax;
            String              scriptName;
            DbgViewData_t       dbgViewData;
            uint                dbgPassIdx          = UMax;
            uint                dbgModeIdx          = UMax;
            uint                dbgStageIdx         = UMax;
            String              helpText;
            ESurfaceFormat      defaultSurfFormat   = Default;
            ESurfaceFormat      reqSurfFormat       = Default;
        };
        using ImGuiDataSync = Synchronized< RWSpinLock, ImGuiData >;

        struct GraphicsData
        {
            Ptr<IOutputSurface>                 output;
            IOutputSurface::SurfaceFormats_t    surfaceFormats;
            IOutputSurface::PresentModes_t      presentModes;
        };
        using GraphicsDataSync = Synchronized< RWSpinLock, GraphicsData >;

        struct ScriptDirData
        {
            TimePoint_t         lastCheck;
            const seconds       interval    {10};
            const uint          maxDepth    {3};
            const Path          root;
            ScriptFolder        rootInfo;

            explicit ScriptDirData (Path path) : root{RVRef(path)} {}
        };

        struct PipelineSet
        {
            RenderTechPassName      pass;
            GraphicsPipelineID      blend;
            GraphicsPipelineID      opaque;
        };
        using PipelineMap_t = FixedMap< EPixelFormat, PipelineSet, 8 >;


    // variables
    private:
        Atomic<bool>                _initialized        {false};
        Atomic<bool>                _uploaded           {false};
        Atomic<bool>                _compiling          {false};
        Atomic<bool>                _pauseRendering     {false};
        Atomic<bool>                _requestCapture     {false};

        struct {
            Atomic<ubyte>               current         {0};
            EWindowMode                 windowedMode    = EWindowMode::Resizable;
            EWindowMode                 fullscreenMode  = EWindowMode::FullscreenWindow;
        }                           _windowMode;

        struct {
            RenderTechPipelinesPtr      rtech;
            PipelineMap_t               pplns;
            PerFrameDescSet_t           descSets;

            DescSetBinding              dsIndex;
            PushConstantIndex           pc1Index;
            PushConstantIndex           pc2Index;

            Strong<ImageID>             fontImg;
            Strong<ImageViewID>         fontView;
        }                           _res;               // read-only after '_InitUI()'

        ImGuiDataSync               _imgui;
        GraphicsDataSync            _graphics;

        Profiler::ProfilerUI        _profiler;
        ResEditorCore &             _core;

        ScriptDirData               _scriptDir;


    // methods
    public:
        EditorUI (ResEditorCore &core, Path scriptPath);
        ~EditorUI ();

        ND_ bool        Init (IOutputSurface &, EWindowMode);
        ND_ AsyncTask   Draw (ArrayView<AsyncTask> deps);
            void        ProcessInput (ActionQueueReader reader, OUT bool &switchMode);
            void        SetHelpText (String txt);
            void        SetSurfaceFormat (ESurfaceFormat fmt);

        ND_ bool        IsInitialized ()        const   { return _initialized.load(); }
        ND_ bool        IsCaptureRequested ()           { return _requestCapture.exchange( false ); }

        // '_Count' - mode is not changed
        // Other valid value - new window mode.
        ND_ auto        GetNewWindowMode ()         -> Optional<EWindowMode>;


    private:
        ND_ bool  _LoadPipelinePack ();
        ND_ bool  _InitSurface (IOutputSurface &);

        ND_ bool  _InitUI (PipelinePackID pack);

        static void  _CheckScriptDir (INOUT ScriptDirData &);
        static void  _RecursiveCheckScriptDir (INOUT ScriptFolder &dst, INOUT usize &nodeID, const Path &dir, uint depth, uint maxDepth);
    };

} // AE::ResEditor
