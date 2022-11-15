// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "graphics/Public/DescriptorSet.h"
# include "graphics/Metal/MCommon.h"
# include "PipelineCompilerImpl.h"

namespace AE::Graphics
{

	//
	// Metal Descriptor Set Layout
	//

	class MDescriptorSetLayout final
	{
	// types
	public:
		using Uniform_t		= PipelineCompiler::DescriptorSetLayoutDesc::Uniform;
		using Uniforms_t	= Tuple< uint, UniformName::Optimized_t const*, Uniform_t const*, Bytes16u* >;

		struct CreateInfo
		{
			Uniforms_t				uniforms;
			EDescSetUsage			usage;
			StringView				dbgName;
		};

	private:
		using Image				= PipelineCompiler::DescriptorSetLayoutDesc::Image;
		using Buffer			= PipelineCompiler::DescriptorSetLayoutDesc::Buffer;
		using TexelBuffer		= PipelineCompiler::DescriptorSetLayoutDesc::TexelBuffer;
		using Sampler			= PipelineCompiler::DescriptorSetLayoutDesc::Sampler;
		using ImmutableSampler	= PipelineCompiler::DescriptorSetLayoutDesc::ImmutableSampler;
		using ImageWithSampler	= PipelineCompiler::DescriptorSetLayoutDesc::ImageWithSampler;
		using SubpassInput		= PipelineCompiler::DescriptorSetLayoutDesc::SubpassInput;
		using RayTracingScene	= PipelineCompiler::DescriptorSetLayoutDesc::RayTracingScene;

		struct DescriptorBinding
		{
			//Array< VkDescriptorSetLayoutBinding >		desc;
			//Array< VkDescriptorUpdateTemplateEntry >	entries;
			Bytes										tmplEntryOffset;
			uint										dbCount			= 0;
			bool										allowUpdateTmpl	= false;
		};

		// uniforms are sorted by types, this array map desc type to uniform offset to speedup search
		using UniformOffsets_t = StaticArray< ushort, 6 >;


	// variables
	private:
		MetalArrayOfArgumentDescriptor	_descArray;
		Bytes16u::SizeAndAlign			_argbufSizeAlign;

		EDescSetUsage					_usage			= Default;

		UniformOffsets_t				_unOffsets;
		Uniforms_t						_uniforms;		// allocated by pipeline pack linear allocator

		DEBUG_ONLY(	DebugName_t			_debugName;	)
		DRC_ONLY(	RWDataRaceCheck		_drCheck;	)


	// methods
	public:
		MDescriptorSetLayout () {}
		~MDescriptorSetLayout ();

		ND_ bool  Create (const MDevice &dev, StringView dbgName);
		ND_ bool  Create (const MDevice &dev, const CreateInfo &ci);
			void  Destroy (MResourceManager &);

		template <EDescriptorType DescType>
		ND_ Uniforms_t  GetUniformRange () const;

		ND_ MetalArgumentEncoderRC		CreateArgumentEncoder ()		const;

		ND_ Bytes						GetArgumentBufferSize ()		const	{ DRC_SHAREDLOCK( _drCheck );  return Bytes{_argbufSizeAlign.size}; }
		ND_ SizeAndAlign			GetArgumentBufferSizeAlign ()	const	{ DRC_SHAREDLOCK( _drCheck );  return SizeAndAlign{_argbufSizeAlign}; }
		ND_ Uniforms_t const&			GetUniforms ()					const	{ DRC_SHAREDLOCK( _drCheck );  return _uniforms; }
		
		DEBUG_ONLY(  ND_ StringView		GetDebugName ()					const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


		ND_ static SizeAndAlign  CalcBufferSize (uint bufferCount, uint imageCount, uint samplerCount);

	private:
		//bool  _AddUniform (const Uniform_t &un, ArrayView<VkSampler> samplerStorage, OUT Bytes16u* offset, INOUT DescriptorBinding &binding) const;
		//void  _AddBuffer (const Buffer &buf, INOUT VkDescriptorSetLayoutBinding &bind, INOUT DescriptorBinding &binding) const;
		void  _UpdateUniformOffsets ();
	};
	

/*
=================================================
	GetUniformRange
=================================================
*/
	template <EDescriptorType DescType>
	MDescriptorSetLayout::Uniforms_t  MDescriptorSetLayout::GetUniformRange () const
	{
		const bool	has_upd_tmpl = _uniforms.Get<3>() != null;

		BEGIN_ENUM_CHECKS();
		switch ( DescType )
		{
			case EDescriptorType::UniformBuffer :
			case EDescriptorType::StorageBuffer :
				return Uniforms_t( _unOffsets[1] - _unOffsets[0],
								   _uniforms.Get<1>() + _unOffsets[0],
								   _uniforms.Get<2>() + _unOffsets[0],
								   has_upd_tmpl ? _uniforms.Get<3>() + _unOffsets[0] : null );
		
			case EDescriptorType::UniformTexelBuffer :
			case EDescriptorType::StorageTexelBuffer :
				return Uniforms_t( _unOffsets[2] - _unOffsets[1],
								   _uniforms.Get<1>() + _unOffsets[1],
								   _uniforms.Get<2>() + _unOffsets[1],
								   has_upd_tmpl ? _uniforms.Get<3>() + _unOffsets[1] : null );
		
			case EDescriptorType::StorageImage :
			case EDescriptorType::SampledImage :
			case EDescriptorType::CombinedImage :
			case EDescriptorType::CombinedImage_ImmutableSampler :
			case EDescriptorType::SubpassInput :
				return Uniforms_t( _unOffsets[3] - _unOffsets[2],
								   _uniforms.Get<1>() + _unOffsets[2],
								   _uniforms.Get<2>() + _unOffsets[2],
								   has_upd_tmpl ? _uniforms.Get<3>() + _unOffsets[2] : null );

			case EDescriptorType::Sampler :
				return Uniforms_t( _unOffsets[4] - _unOffsets[3],
								   _uniforms.Get<1>() + _unOffsets[3],
								   _uniforms.Get<2>() + _unOffsets[3],
								   has_upd_tmpl ? _uniforms.Get<3>() + _unOffsets[3] : null );

			case EDescriptorType::RayTracingScene :
				return Uniforms_t( _unOffsets[5] - _unOffsets[4],
								   _uniforms.Get<1>() + _unOffsets[4],
								   _uniforms.Get<2>() + _unOffsets[4],
								   has_upd_tmpl ? _uniforms.Get<3>() + _unOffsets[4] : null );

			case EDescriptorType::ImmutableSampler :
			case EDescriptorType::_Count :
			case EDescriptorType::Unknown :
			default :
				return Uniforms_t{};
		}
		END_ENUM_CHECKS();
	}

} // AE::Graphics

#endif // AE_ENABLE_METAL
