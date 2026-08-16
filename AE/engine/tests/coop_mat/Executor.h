// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "pch/GraphicsRHI.h"
using namespace AE;
using namespace AE::Base;


class Executor
{
// types
public:
	using ByteBuffer	= MutableArrayView<ubyte>;

	struct WGConfig
	{
		uint	subgroupCount	= 1;	// per WG
		uint2	wgCount			{1u};

		WGConfig () {}
	};


// variables
protected:
	Graphics::VDeviceInitializer	_vulkan;
private:
	Graphics::BufferID				_bufC;


// methods
public:
	Executor () : _vulkan{True{}} {}

	bool  Initialize ();
	void  Deinitialize ();

	bool  Run (StringView source,
			   ByteBuffer inputA, ByteBuffer inputB,
			   ByteBuffer inputC, ByteBuffer output,
			   uint elementSize, const WGConfig &wgCfg = {});

	bool  SupportsCoopMatrix () const;
	bool  SupportsCoopVector () const;
	bool  SupportsCoopVecTraining () const;
	bool  SupportsIntDotProduct () const;

	Graphics::VDevice const&	GetDevice()		const { return _vulkan; }

private:
	void  _Compile (StringView source, uint elementSize, uint wgSubgroupCount,
					OUT Graphics::GAutorelease<Graphics::PipelinePackID> &packId,
					OUT Graphics::RenderTechPipelinesPtr &rtech) __Th___;

	bool  _RunPipe (ByteBuffer inputA, ByteBuffer inputB,
					ByteBuffer inputC, ByteBuffer output,
					Graphics::RenderTechPipelinesPtr rtech,
					uint2 wgCount);
};

#include "../tests/shared/UnitTest_Shared.h"
