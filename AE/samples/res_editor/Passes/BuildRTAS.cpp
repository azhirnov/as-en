// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Passes/BuildRTAS.h"
#include "Resources/Buffer.h"
#include "Resources/RTScene.h"

namespace AE::ResEditor
{

/*
=================================================
	constructor
=================================================
*/
	BuildRTGeometry::BuildRTGeometry (RC<RTGeometry>	dstGeometry,
									  bool				indirect,
									  StringView		dbgName) __Th___ :
		IPass{ dbgName },
		_dstGeometry{ RVRef(dstGeometry) }
	{
		CHECK_THROW( _dstGeometry );

		auto&	fs = GraphicsScheduler().GetFeatureSet();
		CHECK_THROW_MSG( fs.accelerationStructure() == FeatureSet::EFeature::RequireTrue,
			"AS build is not supported" );

		if ( indirect )
		{
			if ( fs.accelerationStructureIndirectBuild == FeatureSet::EFeature::RequireTrue )
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
	bool  BuildRTGeometry::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		DirectCtx::ASBuild	ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName} };
		bool				result;

		_BeginTimeQuery( ctx );

		result = _dstGeometry->Build( ctx, _mode );

		_EndTimeQuery( ctx );

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return result;
	}

/*
=================================================
	Update
=================================================
*/
	bool  BuildRTGeometry::Update (TransferCtx_t &ctx, const UpdatePassData &) __Th___
	{
		_ReadTimeQuery( ctx.GetFrameId() );
		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	BuildRTScene::BuildRTScene (RC<RTScene>	dstScene,
								bool		indirect,
								StringView	dbgName) __Th___ :
		IPass{ dbgName },
		_dstScene{ RVRef(dstScene) }
	{
		CHECK_THROW( _dstScene );

		auto&	fs = GraphicsScheduler().GetFeatureSet();
		CHECK_THROW_MSG( fs.accelerationStructure() == FeatureSet::EFeature::RequireTrue,
			"AS build is not supported" );

		if ( indirect )
		{
			if ( fs.accelerationStructureIndirectBuild == FeatureSet::EFeature::RequireTrue )
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
	bool  BuildRTScene::Execute (SyncPassData &pd) __Th___
	{
		if_unlikely( not _IsEnabled() )
			return true;

		DirectCtx::ASBuild	ctx{ pd.rtask, RVRef(pd.cmdbuf), DebugLabel{_dbgName} };
		bool				result;

		_BeginTimeQuery( ctx );

		result = _dstScene->Build( ctx, _mode );

		_EndTimeQuery( ctx );

		pd.cmdbuf = ctx.ReleaseCommandBuffer();
		return result;
	}

/*
=================================================
	Update
=================================================
*/
	bool  BuildRTScene::Update (TransferCtx_t &ctx, const UpdatePassData &) __Th___
	{
		_ReadTimeQuery( ctx.GetFrameId() );
		return true;
	}


} // AE::ResEditor
