// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "demo/Core/ISample.h"

namespace AE::Samples::Demo
{

	//
	// Canvas 2D Sample
	//

	class Canvas2DSample final : public ISample
	{
	// types
	public:
		class UploadTextureTask;
		class UploadAtlasTask;
		class UploadRasterFontTask;

		class DrawTask;
		class ProcessInputTask;


	// variables
	public:
		Unique<Canvas>				canvasPtr;
		RenderTechPipelinesPtr		rtech;
		GraphicsPipelineID			ppln1;			// UV only
		GraphicsPipelineID			ppln2_wire;		// color only, wireframe
		GraphicsPipelineID			ppln2;			// color only
		GraphicsPipelineID			ppln3;			// textured
		Strong<DescriptorSetID>		ppln3_ds1;
		Strong<DescriptorSetID>		ppln3_ds2;
		GraphicsPipelineID			ppln4;			// font
		Strong<DescriptorSetID>		ppln4_ds;
		GraphicsPipelineID			ppln5;			// SDF font
		Strong<DescriptorSetID>		ppln5_ds;

		const DescSetBinding		dsIndex		{0};

		Atomic<bool>				uploaded	{false};
		StrongImageAndViewID		tex;
		RC<StaticImageAtlas>		atlas;
		RC<RasterFont>				font;
		RC<RasterFont>				sdfFont;
		Strong<BufferID>			ublock;

		float2						cursorPos;
		bool						enter		= false;
		float						angle		= 0.f;
		const float					fontHeight	= 40.f;
		
		Profiler::ProfilerUI		profiler;


	// methods
	public:
		Canvas2DSample () {}
		~Canvas2DSample () override;

		// ISample //
		bool			Init (PipelinePackID pack) override;
		AsyncTask		Update (const ActionQueueReader &reader, ArrayView<AsyncTask> deps) override;
		AsyncTask		Draw (RenderGraph &rg, ArrayView<AsyncTask> deps) override;
		InputModeName	GetInputMode () const override { return InputModeName{"Canvas2D"}; }
	};

} // AE::Samples::Demo
