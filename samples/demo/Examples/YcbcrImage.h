// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Core/ISample.h"

namespace AE::Samples::Demo
{

	//
	// Ycbcr Image Sample
	//

	class YcbcrImageSample final : public ISample
	{
	// types
	public:
		class DrawTask;
		class ProcessInputTask;


	// variables
	public:
		RenderTechPipelinesPtr		rtech;
		GraphicsPipelineID			ppln;
		Strong<DescriptorSetID>		descSet;
		
		Strong<VideoImageID>		ycbcrImg;
		ImageID						ycbcrImageId;

		GfxMemAllocatorPtr			gfxAlloc;

		static constexpr uint		dim [2]		= {64, 64};
		const DescSetBinding		dsIndex		{0};


	// methods
	public:
		YcbcrImageSample () {}
		~YcbcrImageSample () override;

		// ISample //
		bool			Init (PipelinePackID pack)											override;
		AsyncTask		Update (const ActionQueueReader &reader, ArrayView<AsyncTask> deps)	override;
		AsyncTask		Draw (RenderGraph &rg, ArrayView<AsyncTask> deps)					override;
		InputModeName	GetInputMode ()														const override { return InputModeName{"YcbcrImage"}; }
	};

} // AE::Samples::Demo
