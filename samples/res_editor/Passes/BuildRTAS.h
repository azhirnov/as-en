// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"
#include "res_editor/Resources/RTScene.h"

namespace AE::ResEditor
{

    //
    // Build Ray Tracing Geometry
    //

    class BuildRTGeometry final : public IPass
    {
    // variables
    private:
        RC<RTGeometry>          _dstGeometry;
        RTGeometry::EBuildMode  _mode;
        const String            _dbgName;


    // methods
    public:
        BuildRTGeometry (RC<RTGeometry> dstGeometry,
                         bool           indirect,
                         StringView     dbgName)                            __Th___;
        ~BuildRTGeometry ()                                                 {}

    // IPass //
        EPassType       GetType ()                                          C_NE_OV { return EPassType::Sync | EPassType::Update; }
        RC<IController> GetController ()                                    C_NE_OV { return null; }
        StringView      GetName ()                                          C_NE_OV { return _dbgName; }
        bool            Execute (SyncPassData &)                            __NE_OV;
        bool            Update (TransferCtx_t &, const UpdatePassData &)    __NE_OV;
        void            GetResourcesToResize (INOUT Array<RC<IResource>> &) __NE_OV {}
    };



    //
    // Build Ray Tracing Scene
    //

    class BuildRTScene final : public IPass
    {
    // variables
    private:
        RC<RTScene>             _dstScene;
        RTScene::EBuildMode     _mode;
        const String            _dbgName;


    // methods
    public:
        BuildRTScene (RC<RTScene>   dstScene,
                      bool          indirect,
                      StringView    dbgName)                                __Th___;
        ~BuildRTScene ()                                                    {}

    // IPass //
        EPassType       GetType ()                                          C_NE_OV { return EPassType::Sync | EPassType::Update; }
        RC<IController> GetController ()                                    C_NE_OV { return null; }
        StringView      GetName ()                                          C_NE_OV { return _dbgName; }
        bool            Execute (SyncPassData &)                            __NE_OV;
        bool            Update (TransferCtx_t &, const UpdatePassData &)    __NE_OV;
        void            GetResourcesToResize (INOUT Array<RC<IResource>> &) __NE_OV {}
    };


} // AE::ResEditor
