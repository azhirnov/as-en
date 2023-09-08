// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/OpenVR/OpenVRCommon.h"

#ifdef AE_ENABLE_OPENVR
# include "platform/Private/VRDeviceBase.h"
# include "platform/Private/VRSurface.h"
# include "platform/Private/ProjectionImpl.h"
# include "platform/OpenVR/InputActionsOpenVR.h"

namespace AE::App
{

    //
    // OpenVR Device
    //

    class OpenVRDevice final : public VRDeviceBase
    {
    // types
    public:
        static constexpr ControllerID   HMD_ID          = ControllerID::_VRFirst;
        static constexpr ControllerID   LeftHand_ID     = ControllerID(uint(ControllerID::_VRFirst) + 1);
        static constexpr ControllerID   RightHand_ID    = ControllerID(uint(ControllerID::_VRFirst) + 2);

    private:
        using IVRSystemPtr          = intptr_t;
        using IVRSystemTable        = VR_IVRSystem_FnTable *;
        using IVRCompositorTable    = VR_IVRCompositor_FnTable *;


        //
        // OpenVR Loader
        //
        struct OpenVRLoader
        {
        // types
            using VR_InitInternal_t                         = intptr_t (*) (EVRInitError *peError, EVRApplicationType eType);
            using VR_ShutdownInternal_t                     = void (*) ();
            using VR_IsHmdPresent_t                         = int (*) ();
            using VR_GetGenericInterface_t                  = intptr_t (*) (const char *pchInterfaceVersion, EVRInitError *peError);
            using VR_IsRuntimeInstalled_t                   = int (*) ();
            using VR_GetVRInitErrorAsSymbol_t               = const char* (*) (EVRInitError error);
            using VR_GetVRInitErrorAsEnglishDescription_t   = const char* (*) (EVRInitError error);

        // variables
            VR_InitInternal_t                           InitInternal;
            VR_ShutdownInternal_t                       ShutdownInternal;
            VR_IsHmdPresent_t                           IsHmdPresent;
            VR_GetGenericInterface_t                    GetGenericInterface;
            VR_IsRuntimeInstalled_t                     IsRuntimeInstalled;
            VR_GetVRInitErrorAsSymbol_t                 GetVRInitErrorAsSymbol;
            VR_GetVRInitErrorAsEnglishDescription_t     GetVRInitErrorAsEnglishDescription;
        };


        //
        // VR Render Surface
        //
        class VRRenderSurface final : public VRSurface
        {
        // types
        private:
            class SubmitImageTask;

        // variables
        private:
            OpenVRDevice &  _vrDev;

        // methods
        public:
            explicit VRRenderSurface (OpenVRDevice &vr)                                                                         __NE___: _vrDev{vr} {}

            // IOutputSurface //
            AsyncTask           Begin (CommandBatchPtr beginCmdBatch, CommandBatchPtr endCmdBatch, ArrayView<AsyncTask> deps)   __NE_OV;
            bool                GetTargets (OUT RenderTargets_t &targets)                                                       C_NE_OV;
            AsyncTask           End (ArrayView<AsyncTask> deps)                                                                 __NE_OV;

            bool                SetSurfaceMode (const SurfaceInfo &)                                                            __NE_OV { return false; }

            //AllImages_t       GetAllImages ()                                                                                 C_NE_OV;
            SurfaceFormats_t    GetSurfaceFormats ()                                                                            C_NE_OV { return Default; }
            PresentModes_t      GetPresentModes ()                                                                              C_NE_OV { return Default; }
            SurfaceInfo         GetSurfaceInfo ()                                                                               C_NE_OV { return Default; }
        };


        struct ControllerAxis
        {
            float2      value;
            bool        pressed = false;
        };
        using AxisStates_t  = StaticArray< ControllerAxis, k_unControllerStateAxisCount >;
        using Keys_t        = StaticArray< bool, EVRButtonId_k_EButton_Max >;
        using TimePoint_t   = std::chrono::high_resolution_clock::time_point;

        struct Controller
        {
            ControllerID    id;
            uint            lastPacket  = ~0u;
            Keys_t          keys;
            AxisStates_t    axis;
            TimePoint_t     lastUpdate;
        };
        using Controllers_t = FixedMap< /*tracked device index*/uint, Controller, 8 >;

        using Projections_t = StaticArray< ProjectionImpl, 4 >; // 2 eyes with double buffering


    // variables
    private:
        IVRSystemPtr            _hmd            = Zero;
        IVRSystemTable          _vrSystem       = null;
        IVRCompositorTable      _vrCompositor   = null;

        TrackedDevicePose_t     _trackedDevicePose [k_unMaxTrackedDeviceCount];

        Controllers_t           _controllers;

        InputActionsOpenVR      _input;
        VRRenderSurface         _surface;
        Projections_t           _projections;

        OpenVRLoader            _ovr;
        Library                 _openVRLib;


    // methods
    public:
        OpenVRDevice (VRDeviceListener, IInputActions* dst)                     __NE___;
        ~OpenVRDevice ()                                                        __NE___;

        ND_ bool  Create ()                                                     __NE___;
        ND_ bool  Update (Duration_t timeSinceStart)                            __NE___;


    // IVRDevice //
        bool  Setup (const Settings &)                                          __NE_OV;

        StringView          GetApiName ()                                       C_NE_OV { return "openvr"; }
        IInputActions&      InputActions ()                                     __NE_OV { return _input; }
        IOutputSurface&     GetSurface ()                                       __NE_OV { return _surface; }

        bool  CreateRenderSurface (const VRImageDesc &desc)                     __NE_OV;

        #ifdef AE_ENABLE_VULKAN
          ND_ VkPhysicalDevice  GetRequiredVkPhysicalDevice (VkInstance inst)   __NE___;
        #endif


    private:
            void  _Destroy ();
        ND_ bool  _LoadLib ();

        void  _ProcessHmdEvents (const VREvent_t &);
        void  _ProcessControllerEvents (INOUT Controller&, const VREvent_t &);
        void  _UpdateHMDMatrixPose ();
        void  _InitControllers ();
        void  _SetupCamera ();

        ND_ ControllerID  _GetControllerID (uint tdi) const;

        ND_ String  _GetTrackedDeviceString (TrackedDeviceIndex_t unDevice, TrackedDeviceProperty prop, TrackedPropertyError *peError = null) C_NE___;
    };


} // AE::App

#endif // AE_ENABLE_OPENVR
