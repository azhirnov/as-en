// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Core/ISample.h"

namespace AE::Samples::Demo
{

	//
	// Ray Tracing Sample
	//

	class RayTracingSample final : public ISample
	{
	// types
	public:
		class UploadTask;
		class RayTracingTask;


	// variables
	public:
		Atomic<bool>				uploaded	{false};

		RenderTechPipelinesPtr		rtPipelines;

		Strong<ImageID>				rtImage;
		Strong<ImageViewID>			rtView;
		const uint2					rtSize		{800, 600};
		
		Strong<BufferID>			vb;
		Strong<BufferID>			ib;
		Strong<BufferID>			instances;
		Strong<BufferID>			scratch;

		Strong<RTGeometryID>		rtGeom;
		Strong<RTSceneID>			rtScene;
		RTShaderBindingID			sbt;

		RayTracingPipelineID		ppln;
		Strong<DescriptorSetID>		descSet;
		const DescSetBinding		dsIndex		{0};
		
		GfxMemAllocatorPtr			gfxAlloc;


	// methods
	public:
		RayTracingSample () {}
		~RayTracingSample () override;
		
		// ISample //
		bool			Init (PipelinePackID pack)							override;
		AsyncTask		Draw (RenderGraph &rg, ArrayView<AsyncTask> deps)	override;
		InputModeName	GetInputMode ()										const override { return Default; } //InputModeName{"RayTracing"}; }
	};


} // AE::Samples::Demo
