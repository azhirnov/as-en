// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "pch/GraphicsRHI.h"
using namespace AE;
using namespace AE::Base;


class Executor
{
// types
public:
	using ByteBuffer	= MutableArrayView<ubyte>;

// variables
protected:
	Graphics::VDeviceInitializer	vulkan;
private:
	Graphics::BufferID									_bufC;


// methods
public:
	Executor () : vulkan{True{}} {}

	bool  Initialize ();
	void  Deinitialize ();

	bool  Run (StringView source,
			   ByteBuffer inputA, ByteBuffer inputB,
			   ByteBuffer inputC, ByteBuffer output,
			   uint elementSize = sizeof(half));

	bool  SupportsCoopMatrix () const;
	bool  SupportsCoopVector () const;
	bool  SupportsCoopVecTraining () const;
	bool  SupportsIntDotProduct () const;

	Graphics::VDevice const&  GetDevice()	const { return vulkan; }

private:
	void  _Compile (StringView source, uint elementSize,
					OUT Graphics::GAutorelease<Graphics::PipelinePackID> &packId,
					OUT Graphics::RenderTechPipelinesPtr &rtech) __Th___;

	bool  _RunPipe (ByteBuffer inputA, ByteBuffer inputB,
					ByteBuffer inputC, ByteBuffer output,
					Graphics::RenderTechPipelinesPtr rtech);
};

#include "../tests/shared/UnitTest_Shared.h"
