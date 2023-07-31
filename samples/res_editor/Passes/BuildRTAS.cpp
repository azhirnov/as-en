// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Passes/BuildRTAS.h"
#include "res_editor/Resources/Buffer.h"
#include "res_editor/Resources/RTScene.h"

namespace AE::ResEditor
{

/*
=================================================
    constructor
=================================================
*/
    BuildRTGeometry::BuildRTGeometry (RC<RTGeometry>    dstGeometry,
                                      bool              indirect,
                                      StringView        dbgName) __Th___ :
        _dstGeometry{ RVRef(dstGeometry) },
        _dbgName{ dbgName }
    {
        auto&   feats = RenderTaskScheduler().GetDevice().GetProperties().accelerationStructureFeats;
        CHECK_THROW_MSG( feats.accelerationStructure,
            "AS build is not supported" );

        if ( indirect )
        {
            if ( feats.accelerationStructureIndirectBuild )
                _mode = RTGeometry::EBuildMode::Indirect;
            else
                _mode = RTGeometry::EBuildMode::IndirectEmulated;
        }
        else
            _mode = RTGeometry::EBuildMode::Direct;
    }

/*
=================================================
    Execute
=================================================
*/
    bool  BuildRTGeometry::Execute (SyncPassData &pd) __NE___
    {
        DirectCtx::ASBuild  ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName} };
        bool                result;

        result = _dstGeometry->Build( ctx, _mode );

        pd.cmdbuf = ctx.ReleaseCommandBuffer();
        return result;
    }

/*
=================================================
    Update
=================================================
*/
    bool  BuildRTGeometry::Update (TransferCtx_t &, const UpdatePassData &) __NE___
    {
        return true;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    BuildRTScene::BuildRTScene (RC<RTScene> dstScene,
                                bool        indirect,
                                StringView  dbgName) __Th___ :
        _dstScene{ RVRef(dstScene) },
        _dbgName{ dbgName }
    {
        auto&   feats = RenderTaskScheduler().GetDevice().GetProperties().accelerationStructureFeats;
        CHECK_THROW_MSG( feats.accelerationStructure,
            "AS build is not supported" );

        if ( indirect )
        {
            if ( feats.accelerationStructureIndirectBuild )
                _mode = RTGeometry::EBuildMode::Indirect;
            else
                _mode = RTGeometry::EBuildMode::IndirectEmulated;
        }
        else
            _mode = RTGeometry::EBuildMode::Direct;
    }

/*
=================================================
    Execute
=================================================
*/
    bool  BuildRTScene::Execute (SyncPassData &pd) __NE___
    {
        DirectCtx::ASBuild  ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName} };
        bool                result;

        result = _dstScene->Build( ctx, _mode );

        pd.cmdbuf = ctx.ReleaseCommandBuffer();
        return result;
    }

/*
=================================================
    Update
=================================================
*/
    bool  BuildRTScene::Update (TransferCtx_t &, const UpdatePassData &) __NE___
    {
        return true;
    }


} // AE::ResEditor
