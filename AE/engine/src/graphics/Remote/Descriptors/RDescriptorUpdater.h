// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics/Remote/Resources/RDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Remote Graphics Descriptor Updater
	//

	class RDescriptorUpdater final : public IDescriptorUpdater
	{
	// types
	private:
		using Uniform_t		= PipelineCompiler::DescriptorSetLayoutDesc::Uniform;
		using DT			= EDescriptorType;


	// variables
	private:
		RResourceManager &				_resMngr;
		Ptr<const RDescriptorSetLayout>	_dsLayout;		// strong ref in '_descSetId'
		Strong< DescriptorSetID >		_descSetId;

		RC<ArrayWStream>				_memStream;
		Unique<Serializing::Serializer>	_ser;
		uint							_descCount		= 0;

		DRC_ONLY( RWDataRaceCheck		_drCheck; )


	// methods
	public:
		RDescriptorUpdater ()																													__NE___;
		~RDescriptorUpdater ()																													__NE_OV;

		bool  Set (DescriptorSetID descrSetId, EDescUpdateMode mode)																			__NE_OV;
		bool  Flush ()																															__NE_OV;

		bool  BindImage  (UniformName::Ref, ImageViewID image, uint elementIndex = 0)															__NE_OV;
		bool  BindImages (UniformName::Ref, ArrayView<ImageViewID> images, uint firstIndex = 0)													__NE_OV;
		uint  ImageCount (UniformName::Ref)																										C_NE_OV;

		bool  BindVideoImage (UniformName::Ref, VideoImageID image, uint elementIndex = 0)														__NE_OV;

		bool  BindTexture  (UniformName::Ref, ImageViewID image, SamplerName::Ref sampler, uint elementIndex = 0)								__NE_OV;
		bool  BindTextures (UniformName::Ref, ArrayView<ImageViewID> images, SamplerName::Ref sampler, uint firstIndex = 0)						__NE_OV;
		uint  TextureCount (UniformName::Ref)																									C_NE_OV;

		bool  BindSampler  (UniformName::Ref, SamplerName::Ref sampler, uint elementIndex = 0)													__NE_OV;
		bool  BindSamplers (UniformName::Ref, ArrayView<SamplerName> samplers, uint firstIndex = 0)												__NE_OV;
		uint  SamplerCount (UniformName::Ref)																									C_NE_OV;

		using IDescriptorUpdater::BindBuffer;
		using IDescriptorUpdater::BindBuffers;

		bool  BindBuffer  (UniformName::Ref, ShaderStructName::Ref typeName, BufferID buffer, Bytes offset, Bytes size, uint elementIndex = 0)	__NE_OV;
		bool  BindBuffer  (UniformName::Ref, ShaderStructName::Ref typeName, BufferID buffer, uint elementIndex = 0)							__NE_OV;
		bool  BindBuffers (UniformName::Ref, ShaderStructName::Ref typeName, ArrayView<BufferID> buffers, uint firstIndex = 0)					__NE_OV;
		uint  BufferCount (UniformName::Ref)																									C_NE_OV;

		ShaderStructName  GetBufferStructName (UniformName::Ref)																				C_NE_OV;

		bool  BindTexelBuffer  (UniformName::Ref, BufferViewID view, uint elementIndex = 0)														__NE_OV;
		bool  BindTexelBuffers (UniformName::Ref, ArrayView<BufferViewID> views, uint firstIndex = 0)											__NE_OV;
		uint  TexelBufferCount (UniformName::Ref)																								C_NE_OV;

		bool  BindRayTracingScene (UniformName::Ref, RTSceneID scene, uint elementIndex = 0)													__NE_OV;
		bool  BindRayTracingScenes (UniformName::Ref, ArrayView<RTSceneID> scenes, uint firstIndex = 0)											__NE_OV;
		uint  RayTracingSceneCount (UniformName::Ref)																							C_NE_OV;


	private:
		template <EDescriptorType DescType>
		ND_ const Uniform_t*  _FindUniform (UniformName::Ref)																					C_NE___;

		template <EDescriptorType DescType>
		ND_ uint  _GetArraySize (UniformName::Ref)																								C_NE___;

		ND_ bool  _AddCommand (const RemoteGraphics::Msg::DescUpd_Flush::BaseUpdCmd &)															__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
