// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Resources/MDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Metal Descriptor Updater
	//

	class MDescriptorUpdater final : public IDescriptorUpdater
	{
	// types
	private:
		using Uniform_t	= PipelineCompiler::DescriptorSetLayoutDesc::Uniform;
		using DT		= EDescriptorType;


	// variables
	private:
		MResourceManager &				_resMngr;
		Ptr<const MDescriptorSetLayout>	_dsLayout;	// strong ref in '_descrSet'
		MetalArgumentEncoderRC			_encoder;

		EDescUpdateMode					_mode		= Default;
		
		Strong< DescriptorSetID >		_descSetId;

		DRC_ONLY( RWDataRaceCheck		_drCheck; )


	// methods
	public:
		MDescriptorUpdater ();
		~MDescriptorUpdater () override;
		
		bool  Set (DescriptorSetID descrSetId, EDescUpdateMode mode) override;
		bool  Flush () override;

		bool  BindImage  (const UniformName &name, ImageViewID image, uint elementIndex = 0) override;
		bool  BindImages (const UniformName &name, ArrayView<ImageViewID> images, uint firstIndex = 0) override;

		bool  BindTexture  (const UniformName &name, ImageViewID image, const SamplerName &sampler, uint elementIndex = 0) override;
		bool  BindTextures (const UniformName &name, ArrayView<ImageViewID> images, const SamplerName &sampler, uint firstIndex = 0) override;

		bool  BindSampler  (const UniformName &name, const SamplerName &sampler, uint elementIndex = 0) override;
		bool  BindSamplers (const UniformName &name, ArrayView<SamplerName> samplers, uint firstIndex = 0) override;

		bool  BindBuffer  (const UniformName &name, BufferID buffer, uint elementIndex = 0) override;
		bool  BindBuffer  (const UniformName &name, BufferID buffer, Bytes offset, Bytes size, uint elementIndex = 0) override;
		bool  BindBuffers (const UniformName &name, ArrayView<BufferID> buffers, uint firstIndex = 0) override;

		bool  BindTexelBuffer  (const UniformName &name, BufferViewID view, uint elementIndex = 0) override;
		bool  BindTexelBuffers (const UniformName &name, ArrayView<BufferViewID> views, uint firstIndex = 0) override;

		bool  BindRayTracingScene (const UniformName &name, RTSceneID scene, uint elementIndex = 0) override;
		bool  BindRayTracingScenes (const UniformName &name, ArrayView<RTSceneID> scenes, uint firstIndex = 0) override;

		
	private:
		bool  _Set (DescriptorSetID descrSetId, EDescUpdateMode mode);
		void  _Reset ();
		bool  _Flush ();

		template <EDescriptorType DescType>
		ND_ Tuple< const Uniform_t*, const Bytes16u* >  _FindUniform (const UniformName &name) const;
	};


} // AE::Graphics

#endif // AE_ENABLE_METAL
