// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Passes/IPass.h"
#include "res_editor/Resources/IResource.h"

namespace AE::ResEditor
{

    //
    // Build Ray Tracing Geometry
    //

    class BuildRTGeometry final : public IPass
    {
    // variables
    private:
        RC<RTGeometry>      _dstGeometry;
        RC<Buffer>          _indirectBuffer;
        Strong<BufferID>    _scratchBuffer;     // mutable

        const String        _dbgName;


    // methods
    public:
        BuildRTGeometry (RC<RTGeometry> dstGeometry,
                         RC<Buffer>     indirectBuffer,
                         StringView     dbgName);
        ~BuildRTGeometry ()                                                 {}

    // IPass //
        EPassType       GetType ()                                          C_NE_OV { return EPassType::Sync | EPassType::Update; }
        RC<IController> GetController ()                                    C_NE_OV { return null; }
        StringView      GetName ()                                          C_NE_OV { return _dbgName; }
        bool            Execute (SyncPassData &)                            __NE_OV;
        bool            Update (TransferCtx_t &, const UpdatePassData &)    __NE_OV;
    };



    //
    // Build Ray Tracing Scene
    //

    class BuildRTScene final : public IPass
    {
    // variables
    private:
        RC<RTScene>         _dstScene;
        RC<Buffer>          _indirectBuffer;
        Strong<BufferID>    _scratchBuffer;     // mutable

        const String        _dbgName;


    // methods
    public:
        BuildRTScene (RC<RTScene>   dstScene,
                      RC<Buffer>    indirectBuffer,
                      StringView    dbgName);
        ~BuildRTScene ()                                                    {}

    // IPass //
        EPassType       GetType ()                                          C_NE_OV { return EPassType::Sync | EPassType::Update; }
        RC<IController> GetController ()                                    C_NE_OV { return null; }
        StringView      GetName ()                                          C_NE_OV { return _dbgName; }
        bool            Execute (SyncPassData &)                            __NE_OV;
        bool            Update (TransferCtx_t &, const UpdatePassData &)    __NE_OV;
    };


} // AE::ResEditor
