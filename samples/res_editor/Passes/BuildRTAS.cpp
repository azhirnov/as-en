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
	BuildRTGeometry::BuildRTGeometry (RC<RTGeometry>	dstGeometry,
									  RC<Buffer>		indirectBuffer,
									  StringView		dbgName) :
		_dstGeometry{ RVRef(dstGeometry) },
		_indirectBuffer{ indirectBuffer },
		_dbgName{ dbgName }
	{}

/*
=================================================
	Execute
=================================================
*/
	bool  BuildRTGeometry::Execute (SyncPassData &pd) __NE___
	{
		DirectCtx::ASBuild	ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName} };
		
		//ctx.BuildIndirect( _indirectBuffer->GetBufferId(), 0_b );
		

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return true;
	}
	
/*
=================================================
	Update
=================================================
*/
	bool  BuildRTGeometry::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __NE___
	{
		return true;
	}
//-----------------------------------------------------------------------------


	
/*
=================================================
	constructor
=================================================
*/
	BuildRTScene::BuildRTScene (RC<RTScene>	dstScene,
								RC<Buffer>	indirectBuffer,
								StringView	dbgName) :
		_dstScene{ RVRef(dstScene) },
		_indirectBuffer{ indirectBuffer },
		_dbgName{ dbgName }
	{}

/*
=================================================
	Execute
=================================================
*/
	bool  BuildRTScene::Execute (SyncPassData &pd) __NE___
	{
		DirectCtx::ASBuild	ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName} };
		
		//ctx.BuildIndirect( _indirectBuffer->GetBufferId(), 0_b );
		

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return true;
	}
	
/*
=================================================
	Update
=================================================
*/
	bool  BuildRTScene::Update (TransferCtx_t &ctx, const UpdatePassData &pd) __NE___
	{
		return true;
	}


} // AE::ResEditor
