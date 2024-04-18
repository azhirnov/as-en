// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "PipelinePack.h"
#include "base/Algorithms/StringUtils.h"
#include "base/DataSource/MemStream.h"
#include "serializing/Basic/Serializer.h"
#include "serializing/Basic/ObjectFactory.h"
#include "graphics/Private/EnumUtils.h"

#ifdef AE_ENABLE_GLSL_TRACE
# include "ShaderTrace.h"
#else
# include "ShaderTraceDummy.h"
#endif

using namespace AE::Base;

template <>
struct std::hash< AE::PipelineCompiler::SerializableRayTracingPipeline::GeneralShader > {
	size_t  operator () (const AE::PipelineCompiler::SerializableRayTracingPipeline::GeneralShader &x) const {
		return size_t(HashOf(x.name) + HashOf(x.shader));
	}
};

template <>
struct std::hash< AE::PipelineCompiler::SerializableRayTracingPipeline::TriangleHitGroup > {
	size_t  operator () (const AE::PipelineCompiler::SerializableRayTracingPipeline::TriangleHitGroup &x) const {
		return size_t(HashOf(x.name) + HashOf(x.closestHit) + HashOf(x.anyHit));
	}
};

template <>
struct std::hash< AE::PipelineCompiler::SerializableRayTracingPipeline::ProceduralHitGroup > {
	size_t  operator () (const AE::PipelineCompiler::SerializableRayTracingPipeline::ProceduralHitGroup &x) const {
		return size_t(HashOf(x.name) + HashOf(x.intersection) + HashOf(x.closestHit) + HashOf(x.anyHit));
	}
};


template <>
struct std::hash< AE::Graphics::GraphicsPipelineDesc::VertexInput > {
	size_t  operator () (const AE::Graphics::GraphicsPipelineDesc::VertexInput &x) const {
		return size_t(HashOf(x.type) + HashOf(x.offset) + HashOf(x.index) + HashOf(x.bufferBinding));
	}
};

template <>
struct std::hash< AE::Graphics::GraphicsPipelineDesc::VertexBuffer > {
	size_t  operator () (const AE::Graphics::GraphicsPipelineDesc::VertexBuffer &x) const {
		return size_t(HashOf(x.name) + HashOf(x.typeName) + HashOf(x.rate) + HashOf(x.index) + HashOf(x.stride) + HashOf(x.divisor));
	}
};

template <>
struct std::hash< AE::PipelineCompiler::SerializableRTShaderBindingTable::BindingInfo > {
	size_t  operator () (const AE::PipelineCompiler::SerializableRTShaderBindingTable::BindingInfo &x) const {
		return size_t(x.CalcHash());
	}
};
//-----------------------------------------------------------------------------


namespace AE::Graphics
{
/*
=================================================
	operator ==
=================================================
*/
	bool  operator == (const GraphicsPipelineDesc::VertexInput &lhs, const GraphicsPipelineDesc::VertexInput &rhs)
	{
		return	(lhs.type			== rhs.type)	and
				(lhs.offset			== rhs.offset)	and
				(lhs.index			== rhs.index)	and
				(lhs.bufferBinding	== rhs.bufferBinding);
	}

	bool  operator == (const GraphicsPipelineDesc::VertexBuffer &lhs, const GraphicsPipelineDesc::VertexBuffer &rhs)
	{
		return	(lhs.name		== rhs.name)		and
				(lhs.typeName	== rhs.typeName)	and
				(lhs.rate		== rhs.rate)		and
				(lhs.index		== rhs.index)		and
				(lhs.stride		== rhs.stride)		and
				(lhs.divisor	== rhs.divisor);
	}

} // AE::Graphics
//-----------------------------------------------------------------------------


namespace AE::PipelineCompiler
{
/*
=================================================
	Serialize
=================================================
*/
	bool  DescriptorSetLayoutDesc::Serialize (Serializing::Serializer& ser) C_NE___
	{
		CHECK_ERR( uniforms.size() <= MaxUniforms );
		CHECK_ERR( samplerStorage.size() <= MaxSamplers );

		UniformOffsets_t	offsets	= {};
		bool				result	= true;

		for (usize i = 0; i < uniforms.size(); ++i)
		{
			auto&		[un_name, un]	= uniforms[i];
			const uint	idx				= EDescriptorType_ToIndex( un.type )+1;
			CHECK_ERR( idx < offsets.size() );

			ushort	max_idx;
			CHECK_ERR( CheckCast( OUT max_idx, i+1 ));

			offsets[idx] = Max( offsets[idx], max_idx );
		}

		for (usize i = 1; i < offsets.size(); ++i) {
			offsets[i] = Max( offsets[i], offsets[i-1] );
		}
		CHECK_ERR( uniforms.size() == offsets.back() );

		result &= ser( name, usage, stages );
		result &= ser( features );
		result &= ser( offsets, uint(samplerStorage.size()) );

		for (auto& id : samplerStorage)
		{
			CHECK_ERR( id.IsDefined() );
			result &= ser( id );
		}

		for (auto& [un_name, un] : uniforms)
		{
			CHECK_ERR( un_name.IsDefined() );
			result &= ser( un_name );
		}

		const auto	Serialize_Image = [&ser] (const DescriptorSetLayoutDesc::Image &img)
		{{
			return ser( img.state, img.type, img.format, img.subpassInputIdx, img.samplerOffsetInStorage );
		}};

		for (usize i = 0; i < uniforms.size(); ++i)
		{
			auto&		[un_name, un]	= uniforms[i];
			const uint	idx				= EDescriptorType_ToIndex( un.type );
			const uint	min_offset		= offsets[idx];
			const uint	max_offset		= offsets[idx+1];

			CHECK_ERR( i >= min_offset );
			CHECK_ERR( i <  max_offset );

			result &= ser( un.type, un.stages, un.binding.vkIndex, un.binding.mtlIndex, un.arraySize );

			switch_enum( un.type )
			{
				case EDescriptorType::UniformBuffer :
				case EDescriptorType::StorageBuffer :
					result &= ser( un.buffer.state, un.buffer.dynamicOffsetIndex, un.buffer.staticSize, un.buffer.arrayStride, un.buffer.typeName );
					break;

				case EDescriptorType::UniformTexelBuffer :
				case EDescriptorType::StorageTexelBuffer :
					result &= ser( un.texelBuffer.state, un.texelBuffer.type );
					break;

				case EDescriptorType::StorageImage :
				case EDescriptorType::SampledImage :
				case EDescriptorType::CombinedImage :
				{
					CHECK_ERR( un.image.subpassInputIdx == UMax );
					CHECK_ERR( un.image.samplerOffsetInStorage == UMax );
					result &= Serialize_Image( un.image );
					break;
				}
				case EDescriptorType::SubpassInput :
				{
					CHECK_ERR( un.image.subpassInputIdx != UMax );
					CHECK_ERR( un.image.samplerOffsetInStorage == UMax );
					result &= Serialize_Image( un.image );
					break;
				}
				case EDescriptorType::CombinedImage_ImmutableSampler :
				{
					CHECK_ERR( un.image.subpassInputIdx == UMax );
					CHECK_ERR( un.image.samplerOffsetInStorage + un.arraySize <= samplerStorage.size() );
					result &= Serialize_Image( un.image );
					break;
				}
				case EDescriptorType::Sampler :
					break;

				case EDescriptorType::ImmutableSampler :
					CHECK_ERR( un.immutableSampler.offsetInStorage + un.arraySize <= samplerStorage.size() );
					result &= ser( un.immutableSampler.offsetInStorage );
					break;

				case EDescriptorType::RayTracingScene :
					break;

				case EDescriptorType::Unknown :
				case EDescriptorType::_Count :
				default :
					RETURN_ERR( "unknown descriptor type" );
			}
			switch_end
		}
		return result;
	}

/*
=================================================
	Merge
=================================================
*/
	bool  DescriptorSetLayoutDesc::Merge (const DescriptorSetLayoutDesc &other)
	{
		auto	src = other.uniforms.begin();
		auto	dst = uniforms.begin();

		for (; dst != uniforms.end() and src != other.uniforms.end();)
		{
			// see 'SortUniforms()'
			if ( dst->second.type == src->second.type ?
				 dst->first > src->first				:
				 dst->second.type > src->second.type )
			{
				++src;
				continue;
			}

			if ( dst->second.type == src->second.type ?
				 dst->first < src->first				:
				 dst->second.type < src->second.type )
			{
				++dst;
				continue;
			}

			if ( dst->first == src->first )
			{
				auto&	dst_un	= dst->second;
				auto&	src_un	= src->second;

				CHECK_ERR( dst_un.binding	== src_un.binding );
				CHECK_ERR( dst_un.type		== src_un.type );
				CHECK_ERR( dst_un.arraySize	== src_un.arraySize );	// TODO: runtime-sized arrays

				dst_un.stages |= src_un.stages;

				switch_enum( dst_un.type )
				{
					case EDescriptorType::UniformBuffer :
					case EDescriptorType::StorageBuffer :
					{
						bool	dst_has_dyn_idx = (dst_un.buffer.dynamicOffsetIndex != UMax);
						bool	src_has_dyn_idx = (src_un.buffer.dynamicOffsetIndex != UMax);

						CHECK_ERR( dst_has_dyn_idx				== src_has_dyn_idx			 );
						CHECK_ERR( dst_un.buffer.staticSize		== src_un.buffer.staticSize	 );
						CHECK_ERR( dst_un.buffer.arrayStride	== src_un.buffer.arrayStride );
						CHECK_ERR( dst_un.buffer.typeName		== src_un.buffer.typeName	 );
						CHECK_ERR( AllBits( dst_un.buffer.state, src_un.buffer.state, ~EResourceState::AllShaders ));

						dst_un.buffer.state |= (src_un.buffer.state & EResourceState::AllShaders);
						break;
					}
					case EDescriptorType::UniformTexelBuffer :
					case EDescriptorType::StorageTexelBuffer :
					{
						CHECK_ERR( AllBits( dst_un.texelBuffer.state, src_un.texelBuffer.state, ~EResourceState::AllShaders ));

						dst_un.texelBuffer.state |= (src_un.texelBuffer.state & EResourceState::AllShaders);
						break;
					}
					case EDescriptorType::StorageImage :
					case EDescriptorType::SampledImage :
					case EDescriptorType::CombinedImage :
					{
						CHECK_ERR( AllBits( dst_un.image.state, src_un.image.state, ~EResourceState::AllShaders ));
						CHECK_ERR( dst_un.image.type == src_un.image.type );
						CHECK_ERR( src_un.image.samplerOffsetInStorage == UMax and dst_un.image.samplerOffsetInStorage == UMax );
						CHECK_ERR( src_un.image.subpassInputIdx == UMax and dst_un.image.subpassInputIdx == UMax );

						dst_un.image.state |= (src_un.image.state & EResourceState::AllShaders);
						break;
					}
					case EDescriptorType::CombinedImage_ImmutableSampler :
					{
						CHECK_ERR( dst_un.image.samplerOffsetInStorage + dst_un.arraySize <= samplerStorage.size() );
						CHECK_ERR( src_un.image.samplerOffsetInStorage + src_un.arraySize <= other.samplerStorage.size() );

						CHECK_ERR( AllBits( dst_un.image.state, src_un.image.state, ~EResourceState::AllShaders ));
						CHECK_ERR( dst_un.image.type == src_un.image.type );

						dst_un.image.state |= (src_un.image.state & EResourceState::AllShaders);

						// compare samplers
						for (usize i = 0; i < dst_un.arraySize; ++i)
						{
							auto&	dst_samp = samplerStorage[ dst_un.image.samplerOffsetInStorage + i ];
							auto&	src_samp = other.samplerStorage[ src_un.image.samplerOffsetInStorage + i ];
							CHECK_ERR( dst_samp == src_samp );
						}
						break;
					}
					case EDescriptorType::SubpassInput :
					{
						CHECK_ERR( AllBits( dst_un.image.state, src_un.image.state, ~EResourceState::AllShaders ));
						CHECK_ERR( dst_un.image.type == src_un.image.type );
						CHECK_ERR( src_un.image.samplerOffsetInStorage == UMax and dst_un.image.samplerOffsetInStorage == UMax );
						CHECK_ERR( src_un.image.subpassInputIdx == dst_un.image.subpassInputIdx );

						dst_un.image.state |= (src_un.image.state & EResourceState::AllShaders);
						break;
					}
					case EDescriptorType::Sampler :
						break;
					case EDescriptorType::ImmutableSampler :
					{
						for (usize i = 0; i < dst_un.arraySize; ++i)
						{
							auto&	dst_samp = samplerStorage[ dst_un.immutableSampler.offsetInStorage + i ];
							auto&	src_samp = other.samplerStorage[ src_un.immutableSampler.offsetInStorage + i ];
							CHECK_ERR( dst_samp == src_samp );
						}
						break;
					}
					case EDescriptorType::RayTracingScene :
						break;

					case EDescriptorType::Unknown :
					case EDescriptorType::_Count :
					default :
						RETURN_ERR( "unknown descriptor type" );
				}
				switch_end

				++src;
				++dst;
			}
		}

		for (; src != other.uniforms.end(); ++src)
		{
			auto&	dst_un = uniforms.emplace_back( *src ).second;
			auto&	src_un = src->second;

			if ( src_un.type == EDescriptorType::CombinedImage_ImmutableSampler )
			{
				dst_un.image.samplerOffsetInStorage = CheckCast<SamplerIdx_t>(samplerStorage.size());

				for (usize i = 0; i < src_un.arraySize; ++i) {
					samplerStorage.push_back( other.samplerStorage[ src_un.image.samplerOffsetInStorage + i ]);
				}
			}

			if ( src_un.type == EDescriptorType::ImmutableSampler )
			{
				dst_un.immutableSampler.offsetInStorage = CheckCast<SamplerIdx_t>(samplerStorage.size());

				for (usize i = 0; i < src_un.arraySize; ++i) {
					samplerStorage.push_back( other.samplerStorage[ src_un.immutableSampler.offsetInStorage + i ]);
				}
			}
		}

		CHECK_ERR( usage == other.usage );
		stages |= other.stages;

		return true;
	}

/*
=================================================
	SortUniforms
=================================================
*/
	void  DescriptorSetLayoutDesc::SortUniforms ()
	{
		std::sort(	uniforms.begin(), uniforms.end(),
					[](auto& lhs, auto& rhs)
					{
						const uint	li = EDescriptorType_ToIndex( lhs.second.type );
						const uint	ri = EDescriptorType_ToIndex( rhs.second.type );
						return li != ri ?	li			< ri		:
											lhs.first	< rhs.first;
					});
	}

/*
=================================================
	IsCompatible
=================================================
*/
	bool  DescriptorSetLayoutDesc::IsCompatible (const DescriptorSetLayoutDesc &fromRefl,
												 const DescriptorSetLayoutDesc &fromScript)
	{
		//CHECK_ERR( fromRefl.uniforms.size() == fromScript.uniforms.size() );
		CHECK( fromRefl.samplerStorage.empty() );

		const auto	ImageEqual = [] (const Image &lhs, const Image &rhs) -> bool
		{{
			CHECK_ERR(	EImageType_IsCompatible( lhs.type, rhs.type ));
			CHECK_ERR(	lhs.format	== rhs.format	or
						lhs.format	== Default		or
						rhs.format	== Default );
			return true;
		}};

		auto	l_it	= fromRefl.uniforms.begin();
		auto	r_it	= fromScript.uniforms.begin();

		for (; l_it != fromRefl.uniforms.end() and r_it != fromScript.uniforms.end();)
		{
			if ( l_it->first != r_it->first )
			{
				const uint	li = EDescriptorType_ToIndex( l_it->second.type );
				const uint	ri = EDescriptorType_ToIndex( r_it->second.type );

				if ( li < ri )						++l_it;		else
				if ( li > ri )						++r_it;		else
				if ( l_it->first < r_it->first )	++l_it;		else
				if ( l_it->first > r_it->first )	++r_it;
				continue;
			}

			auto&	l_un	= l_it->second;
			auto&	r_un	= r_it->second;

			CHECK_ERR( l_it->first		== r_it->first		);
			CHECK_ERR( l_un.arraySize	== r_un.arraySize	);
			CHECK_ERR( AllBits( r_un.stages, l_un.stages ));
			CHECK_ERR( l_un.binding.vkIndex == r_un.binding.vkIndex );

			switch_enum( l_un.type )
			{
				case EDescriptorType::UniformBuffer :
				case EDescriptorType::StorageBuffer :
				{
					CHECK_ERR( l_un.type == r_un.type );
					//CHECK_ERR( l_un.buffer.state				== r_un.buffer.state				);
					//CHECK_ERR( l_un.buffer.dynamicOffsetIndex	== r_un.buffer.dynamicOffsetIndex	);	// shader reflection doesn't has dynamic offset
					CHECK_ERR( l_un.buffer.staticSize			== r_un.buffer.staticSize			);
					CHECK_ERR( l_un.buffer.arrayStride			== r_un.buffer.arrayStride			);
					CHECK_ERR( l_un.buffer.typeName				== r_un.buffer.typeName				);
					break;
				}

				case EDescriptorType::UniformTexelBuffer :
				case EDescriptorType::StorageTexelBuffer :
				{
					CHECK_ERR( l_un.type == r_un.type );
					//CHECK_ERR( l_un.texelBuffer.state == r_un.texelBuffer.state );
					break;
				}

				case EDescriptorType::StorageImage :
				case EDescriptorType::SampledImage :
				case EDescriptorType::SubpassInput :
				{
					CHECK_ERR( l_un.type == r_un.type );
					if ( not ImageEqual( l_un.image, r_un.image ))
						return false;
					break;
				}

				case EDescriptorType::CombinedImage :
				case EDescriptorType::CombinedImage_ImmutableSampler :
				{
					CHECK_ERR( AnyEqual( l_un.type, EDescriptorType::CombinedImage, EDescriptorType::CombinedImage_ImmutableSampler ) or
							   AnyEqual( r_un.type, EDescriptorType::CombinedImage, EDescriptorType::CombinedImage_ImmutableSampler ));

					if ( not ImageEqual( l_un.image, r_un.image ))
						return false;
					break;
				}

				case EDescriptorType::Sampler :
				case EDescriptorType::ImmutableSampler :
					CHECK_ERR( AnyEqual( l_un.type, EDescriptorType::Sampler, EDescriptorType::ImmutableSampler ) or
							   AnyEqual( r_un.type, EDescriptorType::Sampler, EDescriptorType::ImmutableSampler ));
					break;

				case EDescriptorType::RayTracingScene :
					CHECK_ERR( l_un.type == r_un.type );
					break;

				case EDescriptorType::Unknown :
				case EDescriptorType::_Count :
				default :
					RETURN_ERR( "unknown descriptor type" );
			}
			switch_end

			++l_it;
			++r_it;
		}
		return true;
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  DescriptorSetLayoutDesc::operator == (const DescriptorSetLayoutDesc &rhs) const
	{
		if ( uniforms.size()		!= rhs.uniforms.size()			or
			 samplerStorage.size()	!= rhs.samplerStorage.size()	or
			 usage					!= rhs.usage					or
			 stages					!= rhs.stages					or
			 features				!= rhs.features					)
		{
			return false;
		}

		const auto	BufferEqual = [] (const Buffer &lhs2, const Buffer &rhs2)
		{{
			return	(lhs2.state				== rhs2.state)				and
					(lhs2.dynamicOffsetIndex== rhs2.dynamicOffsetIndex)	and
					(lhs2.staticSize		== rhs2.staticSize)			and
					(lhs2.arrayStride		== rhs2.arrayStride)		and
					(lhs2.typeName			== rhs2.typeName);
		}};

		const auto	ImageEqual  = [] (const Image &lhs2, const Image &rhs2)
		{{
			// ignore samplerOffsetInStorage
			return	(lhs2.state				== rhs2.state)			and
					(lhs2.type				== rhs2.type)			and
					(lhs2.format			== rhs2.format)			and
					(lhs2.subpassInputIdx	== rhs2.subpassInputIdx);
		}};

		for (usize i = 0; i < uniforms.size(); ++i)
		{
			auto&	l_pair	= uniforms[i];
			auto&	r_pair	= rhs.uniforms[i];
			auto&	l_un	= l_pair.second;
			auto&	r_un	= r_pair.second;

			if ( (l_pair.first	 != r_pair.first)	or
				 (l_un.type		 != r_un.type)		or
				 (l_un.stages	 != r_un.stages)	or
				 (l_un.binding	 != r_un.binding)	or
				 (l_un.arraySize != r_un.arraySize) )
			{
				return false;
			}

			switch_enum( l_un.type )
			{
				case EDescriptorType::UniformBuffer :
				case EDescriptorType::StorageBuffer :
				{
					if ( not BufferEqual( l_un.buffer, r_un.buffer ))
						return false;
					break;
				}

				case EDescriptorType::UniformTexelBuffer :
				case EDescriptorType::StorageTexelBuffer :
				{
					if ( l_un.texelBuffer.state	!= r_un.texelBuffer.state	or
						 l_un.texelBuffer.type	!= r_un.texelBuffer.type	)
						return false;
					break;
				}

				case EDescriptorType::StorageImage :
				case EDescriptorType::SampledImage :
				case EDescriptorType::CombinedImage :
				case EDescriptorType::SubpassInput :
				{
					if ( not ImageEqual( l_un.image, r_un.image ))
						return false;
					break;
				}

				case EDescriptorType::CombinedImage_ImmutableSampler :
				{
					if ( not ImageEqual( l_un.image, r_un.image ))
						return false;

					CHECK_ERR( l_un.image.samplerOffsetInStorage + l_un.arraySize <= samplerStorage.size() );
					CHECK_ERR( r_un.image.samplerOffsetInStorage + r_un.arraySize <= rhs.samplerStorage.size() );

					for (usize j = 0; j < l_un.arraySize; ++j) {
						if ( samplerStorage[ l_un.image.samplerOffsetInStorage + j ] != rhs.samplerStorage[ r_un.image.samplerOffsetInStorage + j ] )
							return false;
					}
					break;
				}

				case EDescriptorType::Sampler :
					break;

				case EDescriptorType::ImmutableSampler :
				{
					CHECK_ERR( l_un.immutableSampler.offsetInStorage + l_un.arraySize <= samplerStorage.size() );
					CHECK_ERR( r_un.immutableSampler.offsetInStorage + r_un.arraySize <= rhs.samplerStorage.size() );

					for (usize j = 0; j < l_un.arraySize; ++j) {
						if ( samplerStorage[ l_un.immutableSampler.offsetInStorage + j ] != rhs.samplerStorage[ r_un.immutableSampler.offsetInStorage + j ] )
							return false;
					}
					break;
				}

				case EDescriptorType::RayTracingScene :
					break;

				case EDescriptorType::Unknown :
				case EDescriptorType::_Count :
				default :
					RETURN_ERR( "unknown descriptor type" );
			}
			switch_end
		}
		return true;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  DescriptorSetLayoutDesc::CalcHash () const
	{
		const auto	ImageHash = [] (const Image &img)
		{{
			return HashOf( img.state ) + HashOf( img.type ) + HashOf( img.format );
		}};

		HashVal	res = HashOf( usage ) + HashOf( stages ) + HashOf( features ) + HashOf( uniforms.size() );

		for (auto& [un_name, un] : uniforms)
		{
			res << HashOf( un_name ) << HashOf( un.type ) << HashOf( un.stages )
				<< HashOf( un.binding.vkIndex ) << HashOf( un.binding.mtlIndex ) << HashOf( un.arraySize );

			switch_enum( un.type )
			{
				case EDescriptorType::UniformBuffer :
				case EDescriptorType::StorageBuffer :
					res << HashOf( un.buffer.state ) << HashOf( un.buffer.dynamicOffsetIndex ) << HashOf( un.buffer.staticSize )
						<< HashOf( un.buffer.arrayStride ) << HashOf( un.buffer.typeName );
					break;

				case EDescriptorType::UniformTexelBuffer :
				case EDescriptorType::StorageTexelBuffer :
					res << HashOf( un.texelBuffer.state );
					break;

				case EDescriptorType::StorageImage :
				case EDescriptorType::SampledImage :
				case EDescriptorType::CombinedImage :
					res << ImageHash( un.image );
					break;

				case EDescriptorType::SubpassInput :
					res << ImageHash( un.image ) << HashOf( un.image.subpassInputIdx );
					break;

				case EDescriptorType::CombinedImage_ImmutableSampler :
					CHECK_ERR( un.image.samplerOffsetInStorage + un.arraySize <= samplerStorage.size() );
					res << ImageHash( un.image );

					for (usize i = 0; i < un.arraySize; ++i) {
						res << HashOf( samplerStorage[ un.image.samplerOffsetInStorage + i ] );
					}
					break;

				case EDescriptorType::Sampler :
					break;

				case EDescriptorType::ImmutableSampler :
					CHECK_ERR( un.immutableSampler.offsetInStorage + un.arraySize <= samplerStorage.size() );

					for (usize i = 0; i < un.arraySize; ++i) {
						res << HashOf( samplerStorage[ un.immutableSampler.offsetInStorage + i ] );
					}
					break;

				case EDescriptorType::RayTracingScene :
					break;

				case EDescriptorType::Unknown :
				case EDescriptorType::_Count :
				default :
					RETURN_ERR( "unknown descriptor type" );
			}
			switch_end
		}

		return res;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	operator ==
=================================================
*/
	bool  PushConstants::PushConst::operator == (const PushConst &rhs) const
	{
		CHECK_ERR( stage		 == rhs.stage );
		CHECK_ERR( typeName		 == rhs.typeName );
	//	CHECK_ERR( metalBufferId == rhs.metalBufferId );	// TODO
		CHECK_ERR( vulkanOffset	 == rhs.vulkanOffset );
		CHECK_ERR( size			 == rhs.size );
		return true;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  PushConstants::PushConst::CalcHash () const
	{
		return HashOf(stage) + HashOf(metalBufferId) + HashOf(vulkanOffset) + HashOf(size);
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Merge
=================================================
*/
	bool  PushConstants::Merge (const PushConstants &other)
	{
		for (auto src : other.items)
		{
			auto	dst = items.find( src.first );

			if ( dst != items.end() ){
				CHECK_ERR( dst->second == src.second );
			}else{
				CHECK( items.insert( src ).second );
			}
		}
		return true;
	}

/*
=================================================
	Serialize
=================================================
*/
	bool  PushConstants::Serialize (Serializing::Serializer& ser) C_NE___
	{
		bool	result = true;

		result &= ser( uint(items.size()) );
		for (auto [name, pc] : items)
		{
			CHECK_ERR( name.IsDefined() );
			result &= ser( name, pc.typeName, pc.stage, pc.metalBufferId, pc.vulkanOffset, pc.size );
		}

		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	operator ==
=================================================
*/
	bool  PipelineLayoutDesc::DescSetLayout::operator == (const DescSetLayout &rhs) const
	{
		return	(vkIndex	== rhs.vkIndex)		&
				(mtlIndex	== rhs.mtlIndex)	&
				(uid		== rhs.uid);
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  PipelineLayoutDesc::DescSetLayout::CalcHash () const
	{
		return HashOf( vkIndex ) + mtlIndex.CalcHash() + HashOf( uid );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	operator ==
=================================================
*/
	bool  PipelineLayoutDesc::operator == (const PipelineLayoutDesc &rhs) const
	{
		return	descrSets		== rhs.descrSets	and
				pushConstants	== rhs.pushConstants;

	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  PipelineLayoutDesc::CalcHash () const
	{
		return HashOf(descrSets) + pushConstants.CalcHash();
	}

/*
=================================================
	Serialize
=================================================
*/
	bool  PipelineLayoutDesc::Serialize (Serializing::Serializer& ser) C_NE___
	{
		return ser( descrSets, pushConstants );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableRenderState::Serialize (Serializing::Serializer& ser) C_NE___
	{
		return ser( rs );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableDepthStencilState::Serialize (Serializing::Serializer& ser) C_NE___
	{
		return ser( ds );
	}
//-----------------------------------------------------------------------------



namespace {
/*
=================================================
	Serialize_BasePipelineDesc
=================================================
*/
	ND_ static bool  Serialize_BasePipelineDesc (Serializing::Serializer& ser, const BasePipelineDesc &base)
	{
		return ser( OUT base.specialization, OUT base.options, OUT base.dynamicState );
	}

/*
=================================================
	BasePipelineDesc_Hash
=================================================
*/
	ND_ static HashVal  BasePipelineDesc_Hash (const BasePipelineDesc &desc)
	{
		return HashOf(desc.specialization) + HashOf(desc.dynamicState) + HashOf(desc.options);
	}

/*
=================================================
	BasePipelineDesc_Compare
=================================================
*/
	ND_ static bool  BasePipelineDesc_Compare (const BasePipelineDesc &lhs, const BasePipelineDesc &rhs)
	{
		return	(lhs.specialization	== rhs.specialization)	&
				(lhs.options		== rhs.options)			&
				(lhs.dynamicState	== rhs.dynamicState);
	}

} // namespace


/*
=================================================
	operator ==
=================================================
*/
	bool  SerializableGraphicsPipeline::VertexAttrib::operator == (const VertexAttrib &rhs) const
	{
		return	(type	== rhs.type)	&
				(index	== rhs.index);
	}

/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableGraphicsPipeline::Serialize (Serializing::Serializer& ser) C_NE___
	{
		bool	result = true;
		result &= ser( features, layout, shaders, supportedTopology );
		result &= ser( vertexAttribs );
		result &= ser( patchControlPoints, earlyFragmentTests );
		return result;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  SerializableGraphicsPipeline::CalcHash () C_NE___
	{
		HashVal	res;
		res << HashOf( features );
		res << HashOf( layout );
		res << HashOf( shaders );
		res << HashOf( supportedTopology );
		res << HashOf( vertexAttribs );
		res << HashOf( patchControlPoints );
		res << HashOf( earlyFragmentTests );
		return res;
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  SerializableGraphicsPipeline::operator == (const SerializableGraphicsPipeline &rhs) C_NE___
	{
		return	features			== rhs.features				and
				layout				== rhs.layout				and
				shaders				== rhs.shaders				and
				supportedTopology	== rhs.supportedTopology	and
				vertexAttribs		== rhs.vertexAttribs		and
				patchControlPoints	== rhs.patchControlPoints	and
				earlyFragmentTests	== rhs.earlyFragmentTests;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableMeshPipeline::Serialize (Serializing::Serializer& ser) C_NE___
	{
		bool	result = true;
		result &= ser( features, layout, shaders, outputTopology, maxVertices, maxIndices );
		result &= ser( taskDefaultLocalSize, taskLocalSizeSpec, meshDefaultLocalSize, meshLocalSizeSpec, earlyFragmentTests );
		return result;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  SerializableMeshPipeline::CalcHash () C_NE___
	{
		HashVal	res;
		res << HashOf( features );
		res << HashOf( layout );
		res << HashOf( shaders );
		res << HashOf( outputTopology );
		res << HashOf( maxVertices );
		res << HashOf( maxIndices );
		res << HashOf( taskDefaultLocalSize );
		res << HashOf( taskLocalSizeSpec );
		res << HashOf( meshDefaultLocalSize );
		res << HashOf( meshLocalSizeSpec );
		res << HashOf( earlyFragmentTests );
		return res;
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  SerializableMeshPipeline::operator == (const SerializableMeshPipeline &rhs) C_NE___
	{
		return	features					== rhs.features					and
				layout						== rhs.layout					and
				shaders						== rhs.shaders					and
				outputTopology				== rhs.outputTopology			and
				maxVertices					== rhs.maxVertices				and
				maxIndices					== rhs.maxIndices				and
				All( taskDefaultLocalSize	== rhs.taskDefaultLocalSize )	and
				All( taskLocalSizeSpec		== rhs.taskLocalSizeSpec )		and
				All( meshDefaultLocalSize	== rhs.meshDefaultLocalSize )	and
				All( meshLocalSizeSpec		== rhs.meshLocalSizeSpec )		and
				earlyFragmentTests			== rhs.earlyFragmentTests;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableComputePipeline::Serialize (Serializing::Serializer& ser) C_NE___
	{
		return ser( features, layout, shader, defaultLocalSize, localSizeSpec );
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  SerializableComputePipeline::CalcHash () C_NE___
	{
		HashVal	res;
		res << HashOf( features );
		res << HashOf( layout );
		res << HashOf( shader );
		res << HashOf( defaultLocalSize );
		res << HashOf( localSizeSpec );
		return res;
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  SerializableComputePipeline::operator == (const SerializableComputePipeline &rhs) C_NE___
	{
		return	features				== rhs.features				and
				layout					== rhs.layout				and
				shader					== rhs.shader				and
				All( localSizeSpec		== rhs.localSizeSpec )		and
				All( defaultLocalSize	== rhs.defaultLocalSize );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableTilePipeline::Serialize (Serializing::Serializer& ser) C_NE___
	{
		return ser( features, layout, shader, defaultLocalSize, localSizeSpec );
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  SerializableTilePipeline::CalcHash () C_NE___
	{
		HashVal	res;
		res << HashOf( features );
		res << HashOf( layout );
		res << HashOf( shader );
		res << HashOf( defaultLocalSize );
		res << HashOf( localSizeSpec );
		return res;
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  SerializableTilePipeline::operator == (const SerializableTilePipeline &rhs) C_NE___
	{
		return	features				== rhs.features				and
				layout					== rhs.layout				and
				shader					== rhs.shader				and
				All( defaultLocalSize	== rhs.defaultLocalSize )	and
				All( localSizeSpec		== rhs.localSizeSpec );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableRayTracingPipeline::Serialize (Serializing::Serializer& ser) C_NE___
	{
		CHECK_ERR( shaderArr.size() <= MaxShaders );
		CHECK_ERR( (generalShaders.size() + triangleGroups.size() + proceduralGroups.size()) <= MaxGroups );

		bool	result = true;
		result &= ser( features, layout, shaderArr );
		result &= ser( generalShaders, triangleGroups, proceduralGroups );
		return result;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  SerializableRayTracingPipeline::CalcHash () C_NE___
	{
		HashVal	res;
		res << HashOf( features );
		res << HashOf( layout );
		res << HashOf( shaderArr );
		res << HashOf( generalShaders );
		res << HashOf( triangleGroups );
		res << HashOf( proceduralGroups );
		return res;
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  SerializableRayTracingPipeline::operator == (const SerializableRayTracingPipeline &rhs) C_NE___
	{
		return	features			== rhs.features			and
				layout				== rhs.layout			and
				shaderArr			== rhs.shaderArr		and
				generalShaders		== rhs.generalShaders	and
				triangleGroups		== rhs.triangleGroups	and
				proceduralGroups	== rhs.proceduralGroups;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableGraphicsPipelineSpec::Serialize (Serializing::Serializer &ser) C_NE___
	{
		CHECK_ERR( desc.renderPass.IsDefined() );
		CHECK_ERR( desc.subpass.IsDefined() );

		bool	result = true;
		result &= ser( templUID, rStateUID, dsStateUID );
		result &= Serialize_BasePipelineDesc( ser, desc );
		// skip renderStatePtr
		result &= ser( desc.renderPass, desc.subpass );
		result &= ser( desc.vertexInput, desc.vertexBuffers, desc.viewportCount );
		return result;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  SerializableGraphicsPipelineSpec::CalcHash () C_NE___
	{
		HashVal	res;
		res << HashOf( uint(templUID) );
		res << HashOf( uint(rStateUID) );
		res << HashOf( uint(dsStateUID) );
		res << BasePipelineDesc_Hash( desc );
		res << HashOf( desc.renderPass );
		res << HashOf( desc.subpass );
		res << HashOf( desc.vertexInput );
		res << HashOf( desc.vertexBuffers );
		res << HashOf( desc.viewportCount );
		return res;
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  SerializableGraphicsPipelineSpec::operator == (const SerializableGraphicsPipelineSpec &rhs) C_NE___
	{
		return	templUID			== rhs.templUID				and
				rStateUID			== rhs.rStateUID			and
				dsStateUID			== rhs.dsStateUID			and
				BasePipelineDesc_Compare( desc, rhs.desc )		and
				desc.renderPass		== rhs.desc.renderPass		and
				desc.subpass		== rhs.desc.subpass			and
				desc.vertexInput	== rhs.desc.vertexInput		and
				desc.vertexBuffers	== rhs.desc.vertexBuffers	and
				desc.viewportCount	== rhs.desc.viewportCount;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableMeshPipelineSpec::Serialize (Serializing::Serializer &ser) C_NE___
	{
		CHECK_ERR( desc.renderPass.IsDefined() );
		CHECK_ERR( desc.subpass.IsDefined() );

		bool	result = true;
		result &= ser( templUID, rStateUID, dsStateUID );
		result &= Serialize_BasePipelineDesc( ser, desc );
		// skip renderStatePtr
		result &= ser( desc.renderPass, desc.subpass, desc.viewportCount );
		result &= ser( desc.taskLocalSize, desc.meshLocalSize );
		return result;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  SerializableMeshPipelineSpec::CalcHash () C_NE___
	{
		return	HashOf( uint(templUID) ) + HashOf( uint(rStateUID) ) + HashOf( uint(dsStateUID) ) +
				BasePipelineDesc_Hash( desc ) + HashOf( desc.renderPass ) + HashOf( desc.subpass ) + HashOf( desc.viewportCount ) +
				HashOf( desc.taskLocalSize ) + HashOf( desc.meshLocalSize );
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  SerializableMeshPipelineSpec::operator == (const SerializableMeshPipelineSpec &rhs) C_NE___
	{
		return	templUID				== rhs.templUID				and
				rStateUID				== rhs.rStateUID			and
				dsStateUID				== rhs.dsStateUID			and
				BasePipelineDesc_Compare( desc, rhs.desc )			and
				desc.renderPass			== rhs.desc.renderPass		and
				desc.subpass			== rhs.desc.subpass			and
				desc.viewportCount		== rhs.desc.viewportCount	and
				All( desc.taskLocalSize	== rhs.desc.taskLocalSize )	and
				All( desc.meshLocalSize	== rhs.desc.meshLocalSize );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableComputePipelineSpec::Serialize (Serializing::Serializer &ser) C_NE___
	{
		bool	result = true;
		result &= ser( templUID );
		result &= Serialize_BasePipelineDesc( ser, desc );
		result &= ser( desc.localSize );
		return result;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  SerializableComputePipelineSpec::CalcHash () C_NE___
	{
		return HashOf( uint(templUID) ) + BasePipelineDesc_Hash( desc ) + HashOf( desc.localSize );
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  SerializableComputePipelineSpec::operator == (const SerializableComputePipelineSpec &rhs) C_NE___
	{
		return	templUID			== rhs.templUID			and
				BasePipelineDesc_Compare( desc, rhs.desc )	and
				All( desc.localSize	== rhs.desc.localSize );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableTilePipelineSpec::Serialize (Serializing::Serializer &ser) C_NE___
	{
		bool	result = true;
		result &= ser( templUID );
		result &= Serialize_BasePipelineDesc( ser, desc );
		result &= ser( desc.renderPass, desc.subpass, desc.localSize );
		return result;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  SerializableTilePipelineSpec::CalcHash () C_NE___
	{
		return	HashOf( uint(templUID) ) +
				BasePipelineDesc_Hash( desc ) + HashOf( desc.renderPass ) + HashOf( desc.subpass ) + HashOf( desc.localSize );
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  SerializableTilePipelineSpec::operator == (const SerializableTilePipelineSpec &rhs) C_NE___
	{
		return	templUID			== rhs.templUID			and
				BasePipelineDesc_Compare( desc, rhs.desc )	and
				desc.renderPass		== rhs.desc.renderPass	and
				desc.subpass		== rhs.desc.subpass		and
				All( desc.localSize	== rhs.desc.localSize );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableRayTracingPipelineSpec::Serialize (Serializing::Serializer &ser) C_NE___
	{
		bool	result = true;
		result &= ser( templUID );
		result &= Serialize_BasePipelineDesc( ser, desc );
		result &= ser( desc.maxRecursionDepth, desc.maxPipelineRayPayloadSize, desc.maxPipelineRayHitAttributeSize );
		return result;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  SerializableRayTracingPipelineSpec::CalcHash () C_NE___
	{
		return	HashOf( uint(templUID) ) + BasePipelineDesc_Hash( desc ) +
				HashOf( desc.maxRecursionDepth ) + HashOf( desc.maxPipelineRayPayloadSize ) +
				HashOf( desc.maxPipelineRayHitAttributeSize );
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  SerializableRayTracingPipelineSpec::operator == (const SerializableRayTracingPipelineSpec &rhs) C_NE___
	{
		return	templUID							== rhs.templUID								and
				BasePipelineDesc_Compare( desc, rhs.desc )										and
				desc.maxRecursionDepth				== rhs.desc.maxRecursionDepth				and
				desc.maxPipelineRayPayloadSize		== rhs.desc.maxPipelineRayPayloadSize		and
				desc.maxPipelineRayHitAttributeSize	== rhs.desc.maxPipelineRayHitAttributeSize;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableRenderTechnique::Pass::Serialize (Serializing::Serializer &ser) C_NE___
	{
		CHECK_ERR( name.IsDefined() );
		return ser( name, dsLayout, renderPass, subpass );
	}

	bool  SerializableRenderTechnique::Serialize (Serializing::Serializer &ser) C_NE___
	{
		CHECK_ERR( name.IsDefined() );

		CHECK_ERR( not passes.empty() );
		CHECK_ERR( passes.size() <= MaxPassCount );

		CHECK_ERR( not pipelines.empty() );
		CHECK_ERR( pipelines.size() <= MaxPipelineCount );

		return ser( name, features, passes, pipelines, rtSBTs );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	operator ==
=================================================
*/
	bool  SerializableRTShaderBindingTable::operator == (const SerializableRTShaderBindingTable &rhs) C_NE___
	{
		return	pplnName		== rhs.pplnName		and
				raygen			== rhs.raygen		and
				miss			== rhs.miss			and
				hit				== rhs.hit			and
				callable		== rhs.callable		and
				numRayTypes		== rhs.numRayTypes;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  SerializableRTShaderBindingTable::CalcHash () C_NE___
	{
		return HashOf(pplnName) + HashOf(raygen) + HashOf(miss) + HashOf(hit) + HashOf(callable) + HashOf(numRayTypes);
	}

/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableRTShaderBindingTable::Serialize (Serializing::Serializer &ser) C_NE___
	{
		return ser( pplnName, raygen, miss, hit, callable, numRayTypes );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ShaderBytecode::ShaderBytecode (SpirvBytecode_t inCode, const SpecConstants_t &inSpec) :
		code{RVRef(inCode)},
		typeIdx{CheckCast<ubyte>(code.index())}
	{
		_CopySpecConst( inSpec );
		dataSize += ArraySizeOf( *UnionGet<SpirvBytecode_t>( this->code ));
		CHECK( dataSize <= MaxBytecodeSize );
	}

	ShaderBytecode::ShaderBytecode (MetalBytecode_t inCode, const SpecConstants_t &inSpec) :
		code{RVRef(inCode)},
		typeIdx{CheckCast<ubyte>(code.index())}
	{
		_CopySpecConst( inSpec );
		dataSize += ArraySizeOf( *UnionGet<MetalBytecode_t>( this->code ));
		CHECK( dataSize <= MaxBytecodeSize );
	}

	ShaderBytecode::ShaderBytecode (SpirvWithTrace inCode, const SpecConstants_t &inSpec) :
		code{RVRef(inCode)},
		typeIdx{CheckCast<ubyte>(code.index())}
	{
		_CopySpecConst( inSpec );

		const auto&	dbg_spv = *UnionGet<SpirvWithTrace>( this->code );

		auto	file = MakeRC<MemWStream>();
		{
			Serializing::Serializer	ser{ file };
			CHECK( dbg_spv.trace->Serialize( ser ));
		}
		dataSize += file->Position();
		dataSize += ArraySizeOf( dbg_spv.bytecode );

		CHECK( dataSize <= MaxBytecodeSize );
	}

/*
=================================================
	_CopySpecConst
=================================================
*/
	void  ShaderBytecode::_CopySpecConst (const SpecConstants_t &inSpec)
	{
		for (auto [name, sc] : inSpec) {
			CHECK( spec.emplace( name, sc ).second );
		}
		dataSize += ((/*name*/4_b + /*value*/4_b) * this->spec.size()) + /*spec size*/4_b;
	}

/*
=================================================
	Serialize
=================================================
*/
	bool  ShaderBytecode::Serialize (Serializing::Serializer &ser) C_NE___
	{
		CHECK_ERR( dataSize <= MaxBytecodeSize );
		return ser( CheckCast<uint>(offset), CheckCast<uint>(dataSize), typeIdx );
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  ShaderBytecode::CalcHash () C_NE___
	{
		return HashOf( code ) + HashOf( spec );
	}

/*
=================================================
	operator ==
=================================================
*/
	bool  ShaderBytecode::operator == (const ShaderBytecode &rhs) C_NE___
	{
		return (code == rhs.code) and (spec == rhs.spec);
	}

/*
=================================================
	WriteData
=================================================
*/
	bool  ShaderBytecode::WriteData (WStream &stream) C_NE___
	{
		bool		result	= true;
		const Bytes	start	= stream.Position();

		Visit( code,
			[&result]			(const NullUnion &)				{ result = false; },
			[&result, &stream]	(const SpirvBytecode_t &spirv)	{ result = stream.Write( ArrayView<uint>{ spirv }); },
			[&result, &stream]	(const MetalBytecode_t &mtbc)	{ result = stream.Write( ArrayView<ubyte>{ mtbc }); },
			[&result, &stream]	(const SpirvWithTrace &dbgSpirv)
			{
				{
					auto	file = MakeRC<MemWStream>();
					{
						Serializing::Serializer	ser{ file };
						result &= dbgSpirv.trace->Serialize( ser );
					}
					result &= file->Store( stream );
				}
				result &= stream.Write( ArrayView<uint>{ dbgSpirv.bytecode });
			}
		);
		CHECK_ERR( result );

		for (auto [name, val] : spec)
		{
			CHECK_ERR( name.IsDefined() );

			uint	h = uint(name.GetHash32());
			result &= stream.Write( h );
			result &= stream.Write( val );
		}
		result &= stream.Write( CheckCast<uint>(spec.size()) );
		CHECK_ERR( result );

		const Bytes	data_size = stream.Position() - start;
		CHECK_ERR( data_size == dataSize );

		return true;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	PipelineStorage::PipelineStorage ()
	{
		const usize	def_size = 128;

		_dsLayouts.reserve( def_size );
		_dsLayoutMap.reserve( def_size );

		_pplnLayouts.reserve( def_size );
		_pplnLayoutMap.reserve( def_size );

		_gpipelineTempl.reserve( def_size );
		_gpipelineTemplMap.reserve( def_size );

		_gpipelineSpec.reserve( def_size );
		_gpipelineSpecMap.reserve( def_size );

		_mpipelineTempl.reserve( def_size );
		_mpipelineTemplMap.reserve( def_size );

		_mpipelineSpec.reserve( def_size );
		_mpipelineSpecMap.reserve( def_size );

		_cpipelineTempl.reserve( def_size );
		_cpipelineTemplMap.reserve( def_size );

		_cpipelineSpec.reserve( def_size );
		_cpipelineSpecMap.reserve( def_size );

		_rtpipelineTempl.reserve( def_size );
		_rtpipelineTemplMap.reserve( def_size );

		_rtpipelineSpec.reserve( def_size );
		_rtpipelineSpecMap.reserve( def_size );

		_spirvShaders.reserve( def_size );
		_spirvShaderMap.reserve( def_size );

		_metaliOSShaders.reserve( def_size );
		_metaliOSShaderMap.reserve( def_size );

		_metalMacShaders.reserve( def_size );
		_metalMacShaderMap.reserve( def_size );
	}

/*
=================================================
	UpdateBufferDynamicOffsets
=================================================
*/
	static void  UpdateBufferDynamicOffsets (INOUT DescriptorSetLayoutDesc &desc)
	{
		Array< DescriptorSetLayoutDesc::Uniform * >	sorted;

		for (auto& un : desc.uniforms)
		{
			switch ( un.second.type )
			{
				case EDescriptorType::UniformBuffer :
				case EDescriptorType::StorageBuffer :
					if ( un.second.buffer.HasDynamicOffset() )
						sorted.push_back( &un.second );
					break;
			}
		}

		// sort by Vulkan binding indices
		std::sort( sorted.begin(), sorted.end(), [](auto& lhs, auto& rhs) { return lhs->binding.vkIndex < rhs->binding.vkIndex; });

		uint	index = 0;
		for (auto* un : sorted)
		{
			un->buffer.dynamicOffsetIndex = CheckCast<ushort>( index++ );
		}
	}

/*
=================================================
	_Add
=================================================
*/
	template <typename UID, typename T, typename ArrType, typename MapType>
	UID  PipelineStorage::_Add (T desc, ArrType &arr, MapType &map)
	{
		const usize	hash	= usize(desc.CalcHash());
		auto		iter	= map.find( hash );

		for (; iter != map.end() and iter->first == hash; ++iter)
		{
			auto&	lhs = arr[ usize(iter->second) ];

			if ( lhs == desc )
				return iter->second;
		}

		const auto	uid = UID(arr.size());

		map.emplace( hash, uid );
		arr.push_back( RVRef(desc) );

		return uid;
	}

/*
=================================================
	AddDescriptorSetLayout
=================================================
*/
	DescrSetUID  PipelineStorage::AddDescriptorSetLayout (DescriptorSetLayoutDesc desc)
	{
		desc.SortUniforms();
		UpdateBufferDynamicOffsets( INOUT desc );

		return _Add<DescrSetUID>( RVRef(desc), _dsLayouts, _dsLayoutMap );
	}

/*
=================================================
	GetDescriptorSetLayout
=================================================
*/
	DescriptorSetLayoutDesc const*  PipelineStorage::GetDescriptorSetLayout (DescrSetUID uid) const
	{
		CHECK_ERR( uint(uid) < _dsLayouts.size() );
		return &_dsLayouts[ uint(uid) ];
	}

/*
=================================================
	AddPipelineLayout
=================================================
*/
	PipelineLayoutUID  PipelineStorage::AddPipelineLayout (PipelineLayoutDesc desc)
	{
		return _Add<PipelineLayoutUID>( RVRef(desc), _pplnLayouts, _pplnLayoutMap );
	}

/*
=================================================
	AddRenderState
=================================================
*/
	RenderStateUID  PipelineStorage::AddRenderState (SerializableRenderState desc)
	{
		return _Add<RenderStateUID>( RVRef(desc), _renderStates, _renderStateMap );
	}

/*
=================================================
	AddDepthStencilState
=================================================
*/
	DepthStencilStateUID  PipelineStorage::AddDepthStencilState (SerializableDepthStencilState desc)
	{
		return _Add<DepthStencilStateUID>( RVRef(desc), _dsStates, _dsStateMap );
	}

/*
=================================================
	_AddPipelineTmpl
=================================================
*/
	template <typename PplnType, typename PplnArr, typename PplnMap>
	Pair<PipelineTemplUID, bool>  PipelineStorage::_AddPipelineTmpl (PipelineTmplName::Ref name, PplnType desc, PplnArr& templArr, PplnMap& templMap, PipelineTemplUID uidType)
	{
		CHECK_ERR( name.IsDefined() );

		const usize	hash	= usize(desc.CalcHash());
		auto		iter	= templMap.find( hash );

		for (; iter != templMap.end() and iter->first == hash; ++iter)
		{
			auto&	lhs = templArr[ usize(iter->second) & usize(~PipelineTemplUID::_Mask) ];

			if ( lhs == desc )
			{
				bool	is_unique = _pipelineTemplMap.insert_or_assign( name, iter->second ).second;
				return MakePair( iter->second, is_unique );
			}
		}

		const auto	uid = PipelineTemplUID(templArr.size() | uint(uidType));

		templMap.emplace( hash, uid );
		templArr.push_back( RVRef(desc) );

		bool	is_unique = _pipelineTemplMap.insert_or_assign( name, uid ).second;
		return MakePair( uid, is_unique );
	}

/*
=================================================
	AddPipeline
=================================================
*/
	Pair<PipelineTemplUID, bool>  PipelineStorage::AddPipeline (PipelineTmplName::Ref name, SerializableGraphicsPipeline desc)
	{
		return _AddPipelineTmpl( name, RVRef(desc), _gpipelineTempl, _gpipelineTemplMap, PipelineTemplUID::Graphics );
	}

	Pair<PipelineTemplUID, bool>  PipelineStorage::AddPipeline (PipelineTmplName::Ref name, SerializableMeshPipeline desc)
	{
		return _AddPipelineTmpl( name, RVRef(desc), _mpipelineTempl, _mpipelineTemplMap, PipelineTemplUID::Mesh );
	}

	Pair<PipelineTemplUID, bool>  PipelineStorage::AddPipeline (PipelineTmplName::Ref name, SerializableComputePipeline desc)
	{
		return _AddPipelineTmpl( name, RVRef(desc), _cpipelineTempl, _cpipelineTemplMap, PipelineTemplUID::Compute );
	}

	Pair<PipelineTemplUID, bool>  PipelineStorage::AddPipeline (PipelineTmplName::Ref name, SerializableTilePipeline desc)
	{
		return _AddPipelineTmpl( name, RVRef(desc), _tpipelineTempl, _tpipelineTemplMap, PipelineTemplUID::Tile );
	}

	Pair<PipelineTemplUID, bool>  PipelineStorage::AddPipeline (PipelineTmplName::Ref name, SerializableRayTracingPipeline desc)
	{
		return _AddPipelineTmpl( name, RVRef(desc), _rtpipelineTempl, _rtpipelineTemplMap, PipelineTemplUID::RayTracing );
	}

/*
=================================================
	_AddPipelineSpec
=================================================
*/
	template <typename PplnType, typename PplnArr, typename PplnMap>
	PipelineSpecUID  PipelineStorage::_AddPipelineSpec (PipelineName::Ref name, PplnType desc, PplnArr& specArr, PplnMap& pplnMap, PipelineSpecUID uidType)
	{
		CHECK_ERR( name.IsDefined() );
		CHECK_ERR( AllBits( PipelineSpecUID(desc.templUID), uidType ));

		const usize	hash	= usize(desc.CalcHash());
		auto		iter	= pplnMap.find( hash );

		for (; iter != pplnMap.end() and iter->first == hash; ++iter)
		{
			auto&	lhs = specArr[ usize(iter->second) & usize(~PipelineSpecUID::_Mask) ];

			if ( lhs == desc )
				return iter->second;
		}

		const auto	uid = PipelineSpecUID(specArr.size() | uint(uidType));

		pplnMap.emplace( hash, uid );
		specArr.push_back( RVRef(desc) );

		return uid;
	}

/*
=================================================
	AddPipeline
=================================================
*/
	PipelineSpecUID  PipelineStorage::AddPipeline (PipelineName::Ref name, SerializableGraphicsPipelineSpec desc)
	{
		return _AddPipelineSpec( name, RVRef(desc), _gpipelineSpec, _gpipelineSpecMap, PipelineSpecUID::Graphics );
	}

	PipelineSpecUID  PipelineStorage::AddPipeline (PipelineName::Ref name, SerializableMeshPipelineSpec desc)
	{
		return _AddPipelineSpec( name, RVRef(desc), _mpipelineSpec, _mpipelineSpecMap, PipelineSpecUID::Mesh );
	}

	PipelineSpecUID  PipelineStorage::AddPipeline (PipelineName::Ref name, SerializableComputePipelineSpec desc)
	{
		return _AddPipelineSpec( name, RVRef(desc), _cpipelineSpec, _cpipelineSpecMap, PipelineSpecUID::Compute );
	}

	PipelineSpecUID  PipelineStorage::AddPipeline (PipelineName::Ref name, SerializableTilePipelineSpec desc)
	{
		return _AddPipelineSpec( name, RVRef(desc), _tpipelineSpec, _tpipelineSpecMap, PipelineSpecUID::Tile );
	}

	PipelineSpecUID  PipelineStorage::AddPipeline (PipelineName::Ref name, SerializableRayTracingPipelineSpec desc)
	{
		return _AddPipelineSpec( name, RVRef(desc), _rtpipelineSpec, _rtpipelineSpecMap, PipelineSpecUID::RayTracing );
	}

/*
=================================================
	AddRenderTech
=================================================
*/
	RenderTechUID  PipelineStorage::AddRenderTech (SerializableRenderTechnique desc)
	{
		auto	uid = RenderTechUID(_rtech.size());
		_rtech.push_back( RVRef(desc) );
		return uid;
	}

/*
=================================================
	AddSBT
=================================================
*/
	RTShaderBindingUID  PipelineStorage::AddSBT (SerializableRTShaderBindingTable desc)
	{
		auto	uid = RTShaderBindingUID(_shaderBindingTables.size());
		_shaderBindingTables.push_back( RVRef(desc) );
		return uid;
	}

/*
=================================================
	AddSpirvShader
=================================================
*/
	template <typename T>
	ShaderUID  PipelineStorage::_AddSpvShader (T spirv, const SpecConstants_t &spec)
	{
		ShaderBytecode	code	{ RVRef(spirv), spec };
		const usize		hash	= usize(code.CalcHash());
		auto			iter	= _spirvShaderMap.find( hash );

		for (; iter != _spirvShaderMap.end() and iter->first == hash; ++iter)
		{
			const uint	idx = uint(iter->second) & ~uint(ShaderUID::_Mask);
			CHECK_ERR( idx < _spirvShaders.size() );

			auto&	lhs = _spirvShaders[idx];
			if ( lhs == code )
				return iter->second;
		}

		const auto	uid = ShaderUID(_spirvShaders.size() | uint(ShaderUID::SPIRV));

		_spirvShaderMap.emplace( hash, uid );
		_spirvShaders.push_back( RVRef(code) );

		return uid;
	}

	ShaderUID  PipelineStorage::AddSpirvShader (SpirvBytecode_t spirv, const SpecConstants_t &spec)
	{
		return _AddSpvShader( RVRef(spirv), spec );
	}

	ShaderUID  PipelineStorage::AddSpirvShader (SpirvWithTrace dbgSpirv, const SpecConstants_t &spec)
	{
		return _AddSpvShader( RVRef(dbgSpirv), spec );
	}

/*
=================================================
	_AddMslShader
=================================================
*/
	template <typename ArrType, typename MapType>
	ShaderUID  PipelineStorage::_AddMslShader (MetalBytecode_t msl, const SpecConstants_t &spec, ShaderUID mask, ArrType &arr, MapType &mapType)
	{
		CHECK_ERR(	AllBits( mask, ShaderUID::Metal_iOS, ShaderUID::_Mask ) or
					AllBits( mask, ShaderUID::Metal_Mac, ShaderUID::_Mask ));

		ShaderBytecode	code	{ RVRef(msl), spec };
		const usize		hash	= usize(code.CalcHash());
		auto			iter	= mapType.find( hash );

		for (; iter != mapType.end() and iter->first == hash; ++iter)
		{
			const uint	idx = uint(iter->second) & ~uint(ShaderUID::_Mask);
			CHECK_ERR( idx < arr.size() );

			auto&	lhs = arr[idx];
			if ( lhs == code )
				return iter->second;
		}

		const auto	uid = ShaderUID(arr.size() | uint(mask));

		mapType.emplace( hash, uid );
		arr.push_back( RVRef(code) );

		return uid;
	}

	ShaderUID  PipelineStorage::AddMsliOSShader (MetalBytecode_t msl, const SpecConstants_t &spec)
	{
		return _AddMslShader( RVRef(msl), spec, ShaderUID::Metal_iOS, _metaliOSShaders, _metaliOSShaderMap );
	}

	ShaderUID  PipelineStorage::AddMslMacShader (MetalBytecode_t msl, const SpecConstants_t &spec)
	{
		return _AddMslShader( RVRef(msl), spec, ShaderUID::Metal_Mac, _metalMacShaders, _metalMacShaderMap );
	}

/*
=================================================
	SerializePipelines
=================================================
*/
	bool  PipelineStorage::SerializePipelines (Serializing::Serializer& ser) const
	{
		#define LOG( ... )	AE_LOG_DBG( __VA_ARGS__ )

		CHECK_ERR( ser( PipelinePack_Name ));
		CHECK_ERR( ser( PipelinePack_Version ));

		if ( not _rtech.empty() )
		{
			CHECK_ERR( _rtech.size() <= MaxRenTechCount );
			CHECK_ERR( ser( uint(EMarker::RenderTechniques), _rtech ));
			LOG( "Serialized render techniques: "s << ToString(_rtech.size()) );
		}

		if ( not _renderStates.empty() )
		{
			CHECK_ERR( _renderStates.size() <= MaxStateCount );
			CHECK_ERR( ser( uint(EMarker::RenderStates), _renderStates ));
			LOG( "Serialized render states: "s << ToString(_renderStates.size()) );
		}

		if ( not _dsStates.empty() )
		{
			CHECK_ERR( _dsStates.size() <= MaxStateCount );
			CHECK_ERR( ser( uint(EMarker::DepthStencilStates), _dsStates ));
			LOG( "Serialized depth stencil states: "s << ToString(_dsStates.size()) );
		}

		if ( not _dsLayouts.empty() )
		{
			CHECK_ERR( _dsLayouts.size() <= MaxDSLayoutCount );
			CHECK_ERR( ser( uint(EMarker::DescrSetLayouts), _dsLayouts ));
			LOG( "Serialized descriptor set layouts: "s << ToString(_dsLayouts.size()) );
		}

		if ( not _pplnLayouts.empty() )
		{
			CHECK_ERR( _pplnLayouts.size() <= MaxPplnLayoutCount );
			CHECK_ERR( ser( uint(EMarker::PipelineLayouts), _pplnLayouts ));
			LOG( "Serialized pipeline layouts: "s << ToString(_pplnLayouts.size()) );
		}

		if ( not _gpipelineTempl.empty() )
		{
			CHECK_ERR( _gpipelineTempl.size() <= MaxPipelineCount );
			CHECK_ERR( ser( uint(EMarker::GraphicsPipelineTempl), _gpipelineTempl ));
			LOG( "Serialized graphics pipelines: "s << ToString(_gpipelineTempl.size()) );
		}

		if ( not _mpipelineTempl.empty() )
		{
			CHECK_ERR( _mpipelineTempl.size() <= MaxPipelineCount );
			CHECK_ERR( ser( uint(EMarker::MeshPipelineTempl), _mpipelineTempl ));
			LOG( "Serialized mesh pipelines: "s << ToString(_mpipelineTempl.size()) );
		}

		if ( not _cpipelineTempl.empty() )
		{
			CHECK_ERR( _cpipelineTempl.size() <= MaxPipelineCount );
			CHECK_ERR( ser( uint(EMarker::ComputePipelineTempl), _cpipelineTempl ));
			LOG( "Serialized compute pipelines: "s << ToString(_cpipelineTempl.size()) );
		}

		if ( not _rtpipelineTempl.empty() )
		{
			CHECK_ERR( _rtpipelineTempl.size() <= MaxPipelineCount );
			CHECK_ERR( ser( uint(EMarker::RayTracingPipelineTempl), _rtpipelineTempl ));
			LOG( "Serialized ray tracing pipelines: "s << ToString(_rtpipelineTempl.size()) );
		}

		if ( not _gpipelineSpec.empty() )
		{
			CHECK_ERR( _gpipelineSpec.size() <= MaxPipelineCount );
			CHECK_ERR( ser( uint(EMarker::GraphicsPipelineSpec), _gpipelineSpec ));
			LOG( "Serialized graphics pipeline specs: "s << ToString(_gpipelineSpec.size()) );
		}

		if ( not _mpipelineSpec.empty() )
		{
			CHECK_ERR( _mpipelineSpec.size() <= MaxPipelineCount );
			CHECK_ERR( ser( uint(EMarker::MeshPipelineSpec), _mpipelineSpec ));
			LOG( "Serialized mesh pipeline specs: "s << ToString(_mpipelineSpec.size()) );
		}

		if ( not _cpipelineSpec.empty() )
		{
			CHECK_ERR( _cpipelineSpec.size() <= MaxPipelineCount );
			CHECK_ERR( ser( uint(EMarker::ComputePipelineSpec), _cpipelineSpec ));
			LOG( "Serialized compute pipeline specs: "s << ToString(_cpipelineSpec.size()) );
		}

		if ( not _rtpipelineSpec.empty() )
		{
			CHECK_ERR( _rtpipelineSpec.size() <= MaxPipelineCount );
			CHECK_ERR( ser( uint(EMarker::RayTracingPipelineSpec), _rtpipelineSpec ));
			LOG( "Serialized ray tracing pipeline specs: "s << ToString(_rtpipelineSpec.size()) );
		}

		if ( not _pipelineTemplMap.empty() )
		{
			CHECK_ERR( _pipelineTemplMap.size() <= MaxPipelineNameCount );

			Array<Pair< PipelineTmplName, PipelineTemplUID >>	ppln_names;
			ppln_names.reserve( _pipelineTemplMap.size() );

			for (auto& item : _pipelineTemplMap) {
				ppln_names.push_back( item );
			}

			std::sort( ppln_names.begin(), ppln_names.end(), [](auto& lhs, auto& rhs) { return lhs.first < rhs.first; });

			CHECK_ERR( ser( uint(EMarker::PipelineTemplNames), ppln_names ));
		}

		if ( not _shaderBindingTables.empty() )
		{
			CHECK_ERR( _shaderBindingTables.size() <= MaxSBTCount );
			CHECK_ERR( ser( uint(EMarker::RTShaderBindingTable), _shaderBindingTables ));
			LOG( "Serialized shader binding tables: "s << ToString(_shaderBindingTables.size()) );
		}

		Bytes	shader_data_size;

		// spirv
		if ( not _spirvShaders.empty() )
		{
			CHECK_ERR( _spirvShaders.size() <= MaxShaderCount );
			CHECK_ERR( ser( uint(EMarker::SpirvShaders), uint(_spirvShaders.size()) ));

			Bytes	shader_offset;
			for (auto& code :_spirvShaders)
			{
				code.offset		= shader_offset + shader_data_size;
				shader_offset	+= code.GetDataSize();

				CHECK_ERR( ser( code ));
			}
			LOG( "Serialized SPIRV shaders: "s << ToString(_spirvShaders.size()) << ", data size: " << ToString(shader_offset) );
			shader_data_size += shader_offset;
		}

		// metal ios
		if ( not _metaliOSShaders.empty() )
		{
			CHECK_ERR( _metaliOSShaders.size() <= MaxShaderCount );
			CHECK_ERR( ser( uint(EMarker::MetaliOSShaders), uint(_metaliOSShaders.size()) ));

			Bytes	shader_offset;
			for (auto& code :_metaliOSShaders)
			{
				code.offset		= shader_offset + shader_data_size;
				shader_offset	+= code.GetDataSize();

				CHECK_ERR( ser( code ));
			}
			LOG( "Serialized Metal iOS shaders: "s << ToString(_metaliOSShaders.size()) << ", data size: " << ToString(shader_offset) );
			shader_data_size += shader_offset;
		}

		// metal mac
		if ( not _metalMacShaders.empty() )
		{
			CHECK_ERR( _metalMacShaders.size() <= MaxShaderCount );
			CHECK_ERR( ser( uint(EMarker::MetalMacShaders), uint(_metalMacShaders.size()) ));

			Bytes	shader_offset;
			for (auto& code :_metalMacShaders)
			{
				code.offset		= shader_offset + shader_data_size;
				shader_offset	+= code.GetDataSize();

				CHECK_ERR( ser( code ));
			}
			LOG( "Serialized Metal Mac shaders: "s << ToString(_metalMacShaders.size()) << ", data size: " << ToString(shader_offset) );
			shader_data_size += shader_offset;
		}

		return true;
	}

/*
=================================================
	WriteShaders
=================================================
*/
	bool  PipelineStorage::WriteShaders (WStream &stream) const
	{
		CHECK_ERR( stream.Write( ShaderPack_Name ));
		CHECK_ERR( stream.Write( ShaderPack_Version ));

		Bytes	shader_offset;

		const auto	WriteShader = [&stream, &shader_offset] (auto& code) -> bool
		{{
			Bytes	prev = stream.Position();

			CHECK_ERR( code.WriteData( stream ));

			Bytes	size = stream.Position() - prev;
			CHECK_ERR( size == code.GetDataSize() );

			CHECK_ERR( shader_offset == code.offset );
			shader_offset += size;
			return true;
		}};

		// spirv
		for (auto& code :_spirvShaders)
		{
			CHECK( code.IsSpirv() or code.IsSpirvWithTrace() );
			CHECK_ERR( WriteShader( code ));
		}

		// metal ios
		for (auto& code : _metaliOSShaders)
		{
			CHECK( code.IsMetalBytecode() );
			CHECK_ERR( WriteShader( code ));
		}

		// metal mac
		for (auto& code : _metalMacShaders)
		{
			CHECK( code.IsMetalBytecode() );
			CHECK_ERR( WriteShader( code ));
		}

		return true;
	}

/*
=================================================
	Empty
=================================================
*/
	bool  PipelineStorage::Empty () const
	{
		return	(_renderStates.size()		| _dsStates.size()			|
				 _dsLayouts.size()			| _pplnLayouts.size()		|
				 _pipelineTemplMap.size()	| _rtech.size()) == 0;
	}

/*
=================================================
	CalcAllocationSize
=================================================
*/
	Bytes  PipelineStorage::CalcAllocationSize (Bytes alignBytes) const
	{
		const ulong		align			= ulong(alignBytes);
		const usize		id_size			= 4;	// sizeof(ID<>)
		const usize		name_size		= 4;	// sizeof(***Name::Optimized_t)
		const usize		vk_shmod_size	= 128;	// sizeof(VPipelinePack::ShaderModule)
		const usize		mtl_shmod_size	= 128;	// sizeof(MPipelinePack::ShaderModule)
		ulong			size			= 0;

		size = AlignUp( size + RenderStateCount()		* sizeof(SerializableRenderState),			align );
		size = AlignUp( size + DepthStencilStateCount()	* sizeof(SerializableDepthStencilState),	align );

		size = AlignUp( size + DSLayoutCount()			* /*DescriptorSetLayoutID*/id_size,			align );
		size = AlignUp( size + PipelineLayoutCount()	* /*(V/M)PipelineLayoutID*/id_size,			align );

		size = AlignUp( size + GraphicsPplnTemplCount()	* sizeof(SerializableGraphicsPipeline),		align );
		size = AlignUp( size + ComputePplnTemplCount()	* sizeof(SerializableComputePipeline),		align );
		size = AlignUp( size + MeshPplnTemplCount()		* sizeof(SerializableMeshPipeline),			align );
		size = AlignUp( size + TilePplnTemplCount()		* sizeof(SerializableTilePipeline),			align );

		size = AlignUp( size + GraphicsPplnSpecCount()	* sizeof(SerializableGraphicsPipelineSpec),	align );
		size = AlignUp( size + ComputePplnSpecCount()	* sizeof(SerializableComputePipelineSpec),	align );
		size = AlignUp( size + MeshPplnSpecCount()		* sizeof(SerializableMeshPipelineSpec),		align );
		size = AlignUp( size + TilePplnSpecCount()		* sizeof(SerializableTilePipelineSpec),		align );

		size = AlignUp( size + PplnTemplNameCount()		* (name_size + sizeof(PipelineTemplUID)),	align );
		size = AlignUp( size + SpirvShaderCount()		* vk_shmod_size,							align );
		size = AlignUp( size + MetaliOSShaderCount()	* mtl_shmod_size,							align );
		size = AlignUp( size + MetalMacShaderCount()	* mtl_shmod_size,							align );

		return Bytes{size};
	}


} // AE::PipelineCompiler
