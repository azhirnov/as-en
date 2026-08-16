// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Core/ISample.h"

namespace AE::Samples::Demo
{

	//
	// Camera 3D Sample
	//

	class Camera3DSample final : public ISample
	{
	// types
	private:
		using Camera				= TFPVCamera<float>;
		using CubeRenderer			= GeometryTools::CubeRenderer;
		using SphericalCubeRenderer	= GeometryTools::SphericalCubeRenderer;


	// variables
	public:
		RC<ResourceUploadManager>	uploadMngr;
		RenderTechPipelinesPtr		rtech;
		GfxMemAllocatorPtr			gfxAlloc;

		GraphicsPipelineID			ppln;
		Strong<DescriptorSetID>		descSet;
		const DescSetBinding		dsIndex		{0};

		CubeRenderer				cube1;
		SphericalCubeRenderer		cube2;
		Strong<BufferID>			uniformBuf;

		RC<LoadableImage>			cubeMap;

		StrongImageAndViewID		depthBuf;

		Camera						camera;

		const uint					lod			= 9;
		const bool					use_cube1	= false;


	// methods
	public:
		Camera3DSample ()																	__NE___	{}
		~Camera3DSample ()																	__NE_OV;

		// ISample //
		bool			Init (PipelinePackID, IApplicationTS)								__NE_OV;
		AsyncTask		Update (const ActionQueueReader &reader, ArrayView<AsyncTask> deps)	__NE_OV;
		AsyncTask		Draw (RenderGraph &rg, ArrayView<AsyncTask> deps)					__NE_OV;
		InputModeName	GetInputMode ()														C_NE_OV;

	private:
		static AsyncCoro   _ProcessInputTask (RC<Camera3DSample>, ActionQueueReader)		__NE___;
		static RenderCoro  _DrawTask (RC<Camera3DSample>, IOutputSurface &)					__NE___;
	};


} // AE::Samples::Demo
