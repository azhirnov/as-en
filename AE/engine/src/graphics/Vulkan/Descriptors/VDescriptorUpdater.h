// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/Resources/VDescriptorSetLayout.h"

namespace AE::Graphics
{

	//
	// Vulkan Descriptor Updater
	//

	class VDescriptorUpdater final : public IDescriptorUpdater
	{
	// types
	private:
		using Uniform_t		= PipelineCompiler::DescriptorSetLayoutDesc::Uniform;
		using DT			= EDescriptorType;
		using Allocator_t	= LinearAllocator< UntypedAllocator, 4, false >;

		struct UpdateDescriptorSetsData
		{
			VkWriteDescriptorSet*	descriptors;
			uint					index;
			uint					count;
		};

		struct UpdateWithTemplateData
		{
			void *		tmplData;
			Bytes		tmplDataSize;
		};

		static constexpr uint	_MaxDescSetCount = 1024;

		static constexpr usize	_UpdTmplAlign = Max( Max( alignof(VkDescriptorImageInfo), alignof(VkDescriptorBufferInfo) ),
													 Max( alignof(VkBufferView), alignof(VkAccelerationStructureKHR) ));


	// variables
	private:
		VResourceManager &				_resMngr;
		Ptr<const VDescriptorSetLayout>	_dsLayout;		// strong ref in '_descSetId'
		VkDescriptorSet					_dsHandle		= Default;

		EDescUpdateMode					_mode			= Default;
		union {
			UpdateDescriptorSetsData	_updDesc		{};
			UpdateWithTemplateData		_updDescTempl;
		};
		Allocator_t						_allocator;
		Strong< DescriptorSetID >		_descSetId;

		DRC_ONLY( RWDataRaceCheck		_drCheck; )


	// methods
	public:
		VDescriptorUpdater ()																													__NE___;
		~VDescriptorUpdater ()																													__NE_OV;

		bool  Set (DescriptorSetID descrSetId, EDescUpdateMode mode)																			__NE_OV;
		bool  Flush ()																															__NE_OV;


		bool  BindImage  (UniformName::Ref, VkImageView image, uint elementIndex = 0)															__NE___;
		bool  BindImages (UniformName::Ref, ArrayView<VkImageView> images, uint firstIndex = 0)													__NE___;

		bool  BindImage  (UniformName::Ref, ImageViewID image, uint elementIndex = 0)															__NE_OV;
		bool  BindImages (UniformName::Ref, ArrayView<ImageViewID> images, uint firstIndex = 0)													__NE_OV;

		bool  BindVideoImage (UniformName::Ref, VideoImageID image, uint elementIndex = 0)														__NE_OV;

		uint  ImageCount (UniformName::Ref)																										C_NE_OV;


		bool  BindTexture  (UniformName::Ref, VkImageView image, VkSampler sampler, uint elementIndex = 0)										__NE___;
		bool  BindTextures (UniformName::Ref, ArrayView<VkImageView> images, VkSampler sampler, uint firstIndex = 0)							__NE___;

		bool  BindTexture  (UniformName::Ref, ImageViewID image, SamplerName::Ref sampler, uint elementIndex = 0)								__NE_OV;
		bool  BindTextures (UniformName::Ref, ArrayView<ImageViewID> images, SamplerName::Ref sampler, uint firstIndex = 0)						__NE_OV;

		uint  TextureCount (UniformName::Ref)																									C_NE_OV;


		bool  BindSampler  (UniformName::Ref, VkSampler sampler, uint elementIndex = 0)															__NE___;
		bool  BindSamplers (UniformName::Ref, ArrayView<VkSampler> samplers, uint firstIndex = 0)												__NE___;

		bool  BindSampler  (UniformName::Ref, SamplerName::Ref sampler, uint elementIndex = 0)													__NE_OV;
		bool  BindSamplers (UniformName::Ref, ArrayView<SamplerName> samplers, uint firstIndex = 0)												__NE_OV;

		uint  SamplerCount (UniformName::Ref)																									C_NE_OV;


		using IDescriptorUpdater::BindBuffer;
		using IDescriptorUpdater::BindBuffers;

		bool  BindBuffer  (UniformName::Ref, ShaderStructName::Ref typeName, VkBuffer buffer, Bytes offset, Bytes size, uint elementIndex = 0)	__NE___;
		bool  BindBuffer  (UniformName::Ref, ShaderStructName::Ref typeName, BufferID buffer, Bytes offset, Bytes size, uint elementIndex = 0)	__NE_OV;

		bool  BindBuffer  (UniformName::Ref, ShaderStructName::Ref typeName, VkBuffer buffer, uint elementIndex = 0)							__NE___;
		bool  BindBuffers (UniformName::Ref, ShaderStructName::Ref typeName, ArrayView<VkBuffer> buffers, uint firstIndex = 0)					__NE___;

		bool  BindBuffer  (UniformName::Ref, ShaderStructName::Ref typeName, BufferID buffer, uint elementIndex = 0)							__NE_OV;
		bool  BindBuffers (UniformName::Ref, ShaderStructName::Ref typeName, ArrayView<BufferID> buffers, uint firstIndex = 0)					__NE_OV;

		uint  BufferCount (UniformName::Ref)																									C_NE_OV;

		ShaderStructName  GetBufferStructName (UniformName::Ref)																				C_NE_OV;


		bool  BindTexelBuffer  (UniformName::Ref, VkBufferView view, uint elementIndex = 0)														__NE___;
		bool  BindTexelBuffers (UniformName::Ref, ArrayView<VkBufferView> views, uint firstIndex = 0)											__NE___;

		bool  BindTexelBuffer  (UniformName::Ref, BufferViewID view, uint elementIndex = 0)														__NE_OV;
		bool  BindTexelBuffers (UniformName::Ref, ArrayView<BufferViewID> views, uint firstIndex = 0)											__NE_OV;

		uint  TexelBufferCount (UniformName::Ref)																								C_NE_OV;


		bool  BindRayTracingScene (UniformName::Ref, VkAccelerationStructureKHR scene, uint elementIndex = 0)									__NE___;
		bool  BindRayTracingScenes (UniformName::Ref, ArrayView<VkAccelerationStructureKHR> scenes, uint firstIndex = 0)						__NE___;

		bool  BindRayTracingScene (UniformName::Ref, RTSceneID scene, uint elementIndex = 0)													__NE_OV;
		bool  BindRayTracingScenes (UniformName::Ref, ArrayView<RTSceneID> scenes, uint firstIndex = 0)											__NE_OV;

		uint  RayTracingSceneCount (UniformName::Ref)																							C_NE_OV;


	private:
		bool  _Set (DescriptorSetID descrSetId, EDescUpdateMode mode)																			__NE___;
		void  _Reset ()																															__NE___;
		bool  _Flush ()																															__NE___;

		ND_ bool  _UseUpdateTemplate ()																											C_NE___	{ return _mode == EDescUpdateMode::UpdateTemplate; }

		template <EDescriptorType DescType>
		ND_ Tuple< const Uniform_t*, const Bytes16u* >  _FindUniform (UniformName::Ref)															C_NE___;

		template <EDescriptorType DescType>
		ND_ uint  _GetArraySize (UniformName::Ref)																								C_NE___;

		template <typename T>
		bool  _BindImages (UniformName::Ref, ArrayView<T> images, uint firstIndex)																__NE___;

		template <typename T1, typename T2>
		bool  _BindTextures (UniformName::Ref, ArrayView<T1> images, const T2 &sampler, uint firstIndex)										__NE___;

		template <typename T>
		bool  _BindSamplers (UniformName::Ref, ArrayView<T> samplers, uint firstIndex)															__NE___;

		template <typename T>
		bool  _BindBuffers (UniformName::Ref, ShaderStructName::Ref typeName, ArrayView<T> buffers, uint firstIndex)							__NE___;

		template <typename T>
		bool  _BindBuffer (UniformName::Ref, ShaderStructName::Ref typeName, T buffer, Bytes offset, Bytes size, uint elementIndex)				__NE___;

		template <typename T>
		bool  _BindTexelBuffers (UniformName::Ref, ArrayView<T> views, uint firstIndex)															__NE___;

		template <typename T>
		bool  _BindRayTracingScenes (UniformName::Ref, ArrayView<T> scenes, uint firstIndex)													__NE___;
	};


} // AE::Graphics

#endif // AE_ENABLE_VULKAN
