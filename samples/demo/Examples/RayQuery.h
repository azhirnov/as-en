// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Core/ISample.h"

namespace AE::Samples::Demo
{

	//
	// Ray Query Sample
	//

	class RayQuerySample final : public ISample
	{
	// types
	public:
		class UploadTask;
		class RayTracingTask;


	// variables
	public:
		Atomic<bool>				uploaded	{false};

		RenderTechPipelinesPtr		rqPipelines;

		Strong<ImageID>				rtImage;
		Strong<ImageViewID>			rtView;
		const uint2					rtSize		{800, 600};
		
		Strong<BufferID>			vb;
		Strong<BufferID>			ib;
		Strong<BufferID>			instances;
		Strong<BufferID>			scratch;

		Strong<RTGeometryID>		rtGeom;
		Strong<RTSceneID>			rtScene;

		ComputePipelineID			ppln;
		Strong<DescriptorSetID>		descSet;
		const DescSetBinding		dsIndex		{0};
		
		RC<GfxLinearMemAllocator>	gfxAlloc;


	// methods
	public:
		RayQuerySample () {}
		~RayQuerySample () override;
		
		// ISample //
		bool			Init (PipelinePackID pack) override;
		AsyncTask		Draw (RenderGraph &rg, ArrayView<AsyncTask> deps) override;
		InputModeName	GetInputMode () const override { return Default; } //InputModeName{"RayQuery"}; }
	};


} // AE::Samples::Demo
