// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "RenderPassPack.h"
#include "graphics/Private/EnumUtils.h"
#include "graphics/Private/EnumToString.h"
#include "Packer/VulkanEnums.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics/Vulkan/VEnumCast.h"
# include "graphics/Vulkan/VQueue.h"
#endif

#ifdef AE_BUILD_PIPELINE_COMPILER
# include "ScriptObjects/ObjectStorage.h"
#endif

namespace AE::PipelineCompiler
{
#ifdef AE_BUILD_PIPELINE_COMPILER
namespace
{
	ND_ inline bool  IsValidInitialLayout (EResourceState state)
	{
		return not AnyEqual( state, EResourceState::Preserve );
	}

	ND_ inline bool  IsValidLayout (EResourceState state)
	{
		return not AnyEqual( state, EResourceState::Invalidate, EResourceState::Preserve );
	}
}
#endif

/*
=================================================
	operator ==
=================================================
*/
	bool  SubpassShaderIO::operator == (const SubpassShaderIO &rhs) C_NE___
	{
		return	colorAttachments	== rhs.colorAttachments	and
				inputAttachments	== rhs.inputAttachments;
	}

/*
=================================================
	CalcHash
=================================================
*/
	HashVal  SubpassShaderIO::CalcHash () C_NE___
	{
		return HashOf(colorAttachments) + HashOf(inputAttachments);
	}

/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  SubpassShaderIO::Serialize (Serializing::Serializer &ser) C_NE___
	{
		return ser( colorAttachments, inputAttachments );
	}

	bool  SubpassShaderIO::Deserialize (Serializing::Deserializer &des) __NE___
	{
		return des( OUT colorAttachments, OUT inputAttachments );
	}

/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  SerializableRenderPassInfo::Subpass::Serialize (Serializing::Serializer &ser) C_NE___
	{
		return ser( subpassIndex, hasDepth, hasStencil ) and SubpassShaderIO::Serialize( ser );
	}

	bool  SerializableRenderPassInfo::Subpass::Deserialize (Serializing::Deserializer &des) __NE___
	{
		return des( OUT subpassIndex, OUT hasDepth, OUT hasStencil ) and SubpassShaderIO::Deserialize( des );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Create
=================================================
*/
#ifdef AE_BUILD_PIPELINE_COMPILER
	bool  SerializableRenderPassInfo::Create (const CompatibleRenderPassDesc &compat)
	{
		CHECK_ERR( compat._name.IsDefined() );

		name = compat._name;

		for (auto& [att_name, att] : compat._attachments) {
			attachments.emplace( att_name, CheckCast<ubyte>(att->index) );
		}

		for (usize i = 0; i < compat._subpasses.size(); ++i)
		{
			const auto&	src_sp = compat._subpasses[i];

			Subpass	dst_sp;
			dst_sp.subpassIndex	= CheckCast<ubyte>(i);

			for (auto& [att_name, att] : compat._attachments)
			{
				auto	usage_it = att->usageMap.find( src_sp.name );
				if ( usage_it != att->usageMap.end() )
				{
					const auto&	input	= usage_it->second.input;
					const auto&	output	= usage_it->second.output;

					switch ( usage_it->second.type )
					{
						case EAttachment::Depth :
							dst_sp.hasDepth		= EPixelFormat_HasDepth( att->format );
							dst_sp.hasStencil	= EPixelFormat_HasStencil( att->format );
							CHECK_ERR( dst_sp.hasDepth or dst_sp.hasStencil );
							break;

						case EAttachment::ShadingRate :
						case EAttachment::Preserve :
						case EAttachment::Invalidate :
							break;

						default :
							CHECK_ERR( input.IsDefined() or output.IsDefined() );
					}

					if ( output.IsDefined() )
					{
						CHECK_ERR( output.index < GraphicsConfig::MaxColorAttachments );
						dst_sp.colorAttachments.resize( Max( output.index + 1, dst_sp.colorAttachments.size() ));

						CHECK_ERR( not dst_sp.colorAttachments.at<0>( output.index ).IsDefined() );
						dst_sp.colorAttachments.set( output.index, ShaderIOName::Optimized_t{output.name}, output.type );
					}

					if ( input.IsDefined() )
					{
						CHECK_ERR( input.index < GraphicsConfig::MaxColorAttachments );
						dst_sp.inputAttachments.resize( Max( input.index + 1, dst_sp.inputAttachments.size() ));

						CHECK_ERR( not dst_sp.inputAttachments.at<0>( input.index ).IsDefined() );
						dst_sp.inputAttachments.set( input.index, ShaderIOName::Optimized_t{input.name}, input.type );
					}
				}
			}

			subpasses.emplace( src_sp.name, RVRef(dst_sp) );
		}

		for (auto& feat : compat._features) {
			features.emplace( FeatureSetName::Optimized_t{feat->Name()} );
		}

		return true;
	}
#endif // AE_BUILD_PIPELINE_COMPILER

/*
=================================================
	ToString
=================================================
*/
#ifdef AE_TEST_PIPELINE_COMPILER
	String  SerializableRenderPassInfo::ToString (const HashToName &nameMap) const
	{
		String	str;

		str << "Name:  '" << nameMap( name ) << "'";

		{
			Array<Pair< AttachmentName::Optimized_t, uint >>		sorted;

			for (auto att : attachments) {
				sorted.emplace_back( att.first, att.second );
			}
			std::sort( sorted.begin(), sorted.end(), [](auto& lhs, auto& rhs) { return lhs.second < rhs.second; });

			str << "\nAttachments {";
			for (auto& att : sorted) {
				str << "\n  [" << Base::ToString(att.second) << "] '" << nameMap( att.first ) << "'";
			}
			str << "\n}";
		}
		{
			Array<Pair< SubpassName::Optimized_t, Subpass >>	sorted;

			for (auto sp : subpasses) {
				sorted.emplace_back( sp.first, sp.second );
			}
			std::sort( sorted.begin(), sorted.end(), [](auto& lhs, auto& rhs) { return lhs.second.subpassIndex < rhs.second.subpassIndex; });

			str << "\nSubpasses {";
			for (auto& sp : sorted)
			{
				str << "\n  [" << Base::ToString(sp.second.subpassIndex) << "] '" << nameMap( sp.first ) << "'";

				if ( not sp.second.colorAttachments.empty() )
				{
					str << "\n    colorAttachments = {";
					for (usize i = 0; i < sp.second.colorAttachments.size(); ++i)
					{
						const auto&	ca = sp.second.colorAttachments[i];
						str << "\n      [" << Base::ToString(i) << "] ";
						if ( ca.Get<0>().IsDefined() )
							str << Base::ToString(ca.Get<1>()) << ", '" << nameMap( ca.Get<0>() ) << "'";
					}
					str << "\n    }";
				}

				if ( not sp.second.inputAttachments.empty() )
				{
					str << "\n    inputAttachments = {";
					for (usize i = 0; i < sp.second.inputAttachments.size(); ++i)
					{
						const auto&	ia = sp.second.inputAttachments[i];
						str << "\n      [" << Base::ToString(i) << "] ";
						if ( ia.Get<0>().IsDefined() )
							str << Base::ToString(ia.Get<1>()) << ", '" << nameMap( ia.Get<0>() ) << "'";
					}
					str << "\n    }";
				}
			}
			str << "\n}";
		}

		if ( not features.empty() )
		{
			str << "\nFeatureSets = { ";
			for (auto feat : features) {
				str << "'" << nameMap( feat ) << "', ";
			}
			str.pop_back();
			str.pop_back();
			str << " }";
		}
		return str;
	}
#endif // AE_TEST_PIPELINE_COMPILER

/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableRenderPassInfo::Serialize (Serializing::Serializer &ser) C_NE___
	{
		return ser( name, attachments, subpasses, features );
	}

/*
=================================================
	Deserialize
=================================================
*/
	bool  SerializableRenderPassInfo::Deserialize (Serializing::Deserializer &des) __NE___
	{
		return des( OUT name, OUT attachments, OUT subpasses, OUT features );
	}
//-----------------------------------------------------------------------------



#ifdef AE_ENABLE_VULKAN
# ifdef AE_BUILD_PIPELINE_COMPILER

/*
=================================================
	_ConvertAttachments
=================================================
*/
	bool  SerializableVkRenderPass::_ConvertAttachments (const CompatibleRenderPassDesc &compat, const RenderPassSpec &spec)
	{
		CHECK_ERR( compat._attachments.size() <= GraphicsConfig::MaxAttachments );

		if ( compat._attachments.empty() )
			return true;

		const uint	count			= uint(compat._attachments.size());
		auto*		dst_attachments	= _allocator.Allocate<VkAttachmentDescription2>( count );
		CHECK_ERR( dst_attachments != null );

		_ci.attachmentCount	= count;
		_ci.pAttachments	= dst_attachments;

		for (auto& [name, rt] : compat._attachments)
		{
			CHECK_ERR( rt->index < count );

			const auto&	src			= *rt;
			auto&		dst			= dst_attachments[ rt->index ];
			auto&		dst_state	= _states[ rt->index ];

			dst					= {};
			dst.sType			= VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
			dst.pNext			= null;
			dst.flags			= 0;
			dst.format			= src.format == EPixelFormat::SwapchainColor ? VK_FORMAT_MAX_ENUM : VEnumCast( src.format );
			dst.samples			= VEnumCast( src.samples );
			dst.loadOp			= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			dst.storeOp			= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			dst.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
			dst.initialLayout	= VK_IMAGE_LAYOUT_GENERAL;
			dst.finalLayout		= VK_IMAGE_LAYOUT_GENERAL;

			auto	attach_spec = spec._attachments.find( name );
			if ( attach_spec != spec._attachments.end() )
			{
				dst.loadOp		= VEnumCast( attach_spec->second->loadOp );
				dst.storeOp		= VEnumCast( attach_spec->second->storeOp );

				auto&	layouts			= attach_spec->second->layouts;
				auto	initial_layout	= layouts.find( Subpass_ExternalIn );
				auto	final_layout	= layouts.find( Subpass_ExternalOut );

				if ( initial_layout != layouts.end() and IsValidInitialLayout( initial_layout->second.state ))
				{
					dst_state.initial = initial_layout->second.state;
				}
				else
				{
					// find first layout
					for (auto& sp : compat._subpasses)
					{
						auto	l_it = layouts.find( sp.name );
						if ( l_it != layouts.end() and IsValidInitialLayout( l_it->second.state ))
						{
							dst_state.initial = l_it->second.state;
							break;
						}
					}
				}

				if ( final_layout != layouts.end() and IsValidLayout( final_layout->second.state ))
				{
					dst_state.final = final_layout->second.state;
				}
				else
				{
					// find last layout
					for (auto& sp : Reverse(compat._subpasses))
					{
						auto	l_it = layouts.find( sp.name );
						if ( l_it != layouts.end() and IsValidLayout( l_it->second.state ))
						{
							dst_state.final = l_it->second.state;
							break;
						}
					}
				}

				if ( attach_spec->second->storeOp == EAttachmentStoreOp::Invalidate )
					dst_state.final |= EResourceState::Invalidate;

				dst.initialLayout	= EResourceState_ToSrcImageLayout( dst_state.initial );
				dst.finalLayout		= EResourceState_ToDstImageLayout( dst_state.final );
			}

			CHECK_ERR( dst.finalLayout != VK_IMAGE_LAYOUT_UNDEFINED );

			if ( src.format < EPixelFormat::_Count and EPixelFormat_HasStencil( src.format ))
			{
				dst.stencilLoadOp	= dst.loadOp;
				dst.stencilStoreOp	= dst.storeOp;
			}
		}

		return true;
	}

/*
=================================================
	_ConvertSubpasses
=================================================
*/
	bool  SerializableVkRenderPass::_ConvertSubpasses (const CompatibleRenderPassDesc &compat, const RenderPassSpec &spec)
	{
		CHECK_ERR( not compat._subpasses.empty() );
		CHECK_ERR( compat._subpasses.size() <= GraphicsConfig::MaxSubpasses );

		const uint	count			= uint(compat._subpasses.size());
		auto*		dst_subpasses	= _allocator.Allocate<VkSubpassDescription2>( count );
		CHECK_ERR( dst_subpasses != null );

		_ci.subpassCount	= count;
		_ci.pSubpasses		= dst_subpasses;

		for (usize i = 0; i < compat._subpasses.size(); ++i)
		{
			const auto&				sp		= compat._subpasses[i];
			const auto&				sp_name	= sp.name;
			VkSubpassDescription2&	dst		= dst_subpasses[i];
			void const * *			p_next	= &dst.pNext;

			dst						= {};
			dst.sType				= VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
			dst.pNext				= null;	// TODO: VkSubpassDescriptionDepthStencilResolve
			dst.flags				= 0;
			dst.pipelineBindPoint	= VK_PIPELINE_BIND_POINT_GRAPHICS;
			//dst.viewMask			= uint(src.viewMask.to_ulong());

			Array<VkAttachmentReference2>	color_attachments;
			Array<VkAttachmentReference2>	resolve_attachments;
			Array<VkAttachmentReference2>	input_attachments;
			Array<uint>						preserve_attachments;
			bool							has_resolve_attachments	= false;

			const auto	InitDefaultAttachmentRefs = [] (const int lastIdx, OUT Array<VkAttachmentReference2> &refs)
			{{
				for (int j = 0; j <= lastIdx; ++j)
				{
					auto&	ref		= refs.emplace_back();
					ref				= {};
					ref.sType		= VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
					ref.pNext		= null;
					ref.layout		= VK_IMAGE_LAYOUT_UNDEFINED;
					ref.aspectMask	= 0;
					ref.attachment	= VK_ATTACHMENT_UNUSED;
				}
			}};

			InitDefaultAttachmentRefs( IntLog2( sp.assignedColorAttachment.to_ulong() ), OUT color_attachments );
			InitDefaultAttachmentRefs( IntLog2( sp.assignedColorAttachment.to_ulong() ), OUT resolve_attachments );
			InitDefaultAttachmentRefs( IntLog2( sp.assignedInputAttachment.to_ulong() ), OUT input_attachments );
			CHECK_ERR( color_attachments.size() < GraphicsConfig::MaxAttachments );
			CHECK_ERR( input_attachments.size() < GraphicsConfig::MaxAttachments );

			const auto	InitAttachmentRef = [&sp_name, &spec] (AttachmentName::Ref name, uint idx, OUT VkAttachmentReference2 &ref, EPixelFormat fmt = Default)
			{{
				ref				= {};
				ref.sType		= VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
				ref.pNext		= null;
				ref.layout		= VK_IMAGE_LAYOUT_GENERAL;
				ref.aspectMask	= 0;	// only for input attachment
				ref.attachment	= idx;

				auto	attach_spec = spec._attachments.find( name );
				if ( attach_spec != spec._attachments.end() )
				{
					auto	attach_layout = attach_spec->second->layouts.find( sp_name );
					if ( attach_layout != attach_spec->second->layouts.end() )
						ref.layout = EResourceState_ToDstImageLayout( attach_layout->second.state );
				}

				if ( fmt != Default )
				{
					if ( EPixelFormat_IsColor( fmt ))
						ref.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					else
					{
						// TODO: validate by layout
						if ( EPixelFormat_HasDepth( fmt ))
							ref.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
						else
						if ( EPixelFormat_HasStencil( fmt ))
							ref.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
					}
				}
			}};

			for (auto& [name, rt] : compat._attachments)
			{
				auto	usage_it = rt->usageMap.find( sp_name );
				if ( usage_it != rt->usageMap.end() )
				{
					has_resolve_attachments |= (usage_it->second.type == EAttachment::ColorResolve);

					switch_enum( usage_it->second.type )
					{
						case EAttachment::Color :
						case EAttachment::ColorResolve :
						{
							CHECK_ERR( usage_it->second.output.index < color_attachments.size() );
							InitAttachmentRef( name, rt->index, OUT color_attachments[ usage_it->second.output.index ]);
							InitAttachmentRef( name, rt->index, OUT resolve_attachments[ usage_it->second.output.index ]);
							break;
						}
						case EAttachment::ReadWrite :
						{
							CHECK_ERR( usage_it->second.input.index < input_attachments.size() );
							InitAttachmentRef( name, rt->index, OUT input_attachments[ usage_it->second.input.index ], rt->format );
							if ( rt->IsColor() ) {
								CHECK_ERR( usage_it->second.output.index < color_attachments.size() );
								InitAttachmentRef( name, rt->index, OUT color_attachments[ usage_it->second.output.index ], rt->format );
							}else{
								CHECK_ERR( false );	// not supported yet
							}
							break;
						}
						case EAttachment::Input :
						{
							CHECK_ERR( usage_it->second.input.index < input_attachments.size() );
							InitAttachmentRef( name, rt->index, OUT input_attachments[ usage_it->second.input.index ], rt->format );
							break;
						}
						case EAttachment::Depth :
						{
							CHECK_ERR( dst.pDepthStencilAttachment == null );

							auto*	ds_ref = _allocator.Allocate<VkAttachmentReference2>( 1 );
							CHECK_ERR( ds_ref != null );
							InitAttachmentRef( name, rt->index, OUT *ds_ref );
							dst.pDepthStencilAttachment = ds_ref;
							break;
						}
						case EAttachment::Preserve :
						{
							CHECK_ERR( preserve_attachments.size() < GraphicsConfig::MaxAttachments );
							preserve_attachments.push_back( rt->index );
							break;
						}
						case EAttachment::ShadingRate :
						{
							auto*	sra		= _allocator.Allocate<VkFragmentShadingRateAttachmentInfoKHR>( 1 );
							auto*	sra_ref	= _allocator.Allocate<VkAttachmentReference2>( 1 );
							CHECK_ERR( sra != null and sra_ref != null );

							InitAttachmentRef( name, rt->index, OUT *sra_ref, rt->format );

							*p_next = sra;
							p_next	= &sra->pNext;

							sra->sType	= VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR;
							sra->pNext	= null;
							sra->pFragmentShadingRateAttachment = sra_ref;
							sra->shadingRateAttachmentTexelSize = { usage_it->second.texelSize.x, usage_it->second.texelSize.y };
							break;
						}
						case EAttachment::Invalidate :
						case EAttachment::Unknown :
						case EAttachment::_Count :	break;
					}
					switch_end
				}
			}

			if ( not color_attachments.empty() )
			{
				auto*	ptr = _allocator.Allocate<VkAttachmentReference2>( color_attachments.size() );
				CHECK_ERR( ptr != null );
				MemCopy( OUT ptr, color_attachments.data(), ArraySizeOf(color_attachments) );
				dst.colorAttachmentCount	= uint(color_attachments.size());
				dst.pColorAttachments		= ptr;
			}

			if ( has_resolve_attachments )
			{
				auto*	ptr = _allocator.Allocate<VkAttachmentReference2>( resolve_attachments.size() );
				CHECK_ERR( ptr != null );
				MemCopy( OUT ptr, resolve_attachments.data(), ArraySizeOf(resolve_attachments) );
				dst.pResolveAttachments = ptr;
				CHECK( dst.colorAttachmentCount == resolve_attachments.size() );
			}

			if ( not input_attachments.empty() )
			{
				auto*	ptr = _allocator.Allocate<VkAttachmentReference2>( input_attachments.size() );
				CHECK_ERR( ptr != null );
				MemCopy( OUT ptr, input_attachments.data(), ArraySizeOf(input_attachments) );
				dst.inputAttachmentCount	= uint(input_attachments.size());
				dst.pInputAttachments		= ptr;
			}

			if ( not preserve_attachments.empty() )
			{
				auto*	ptr = _allocator.Allocate<uint>( preserve_attachments.size() );
				CHECK_ERR( ptr != null );
				MemCopy( OUT ptr, preserve_attachments.data(), ArraySizeOf(preserve_attachments) );
				dst.preserveAttachmentCount	= uint(preserve_attachments.size());
				dst.pPreserveAttachments	= ptr;
			}

			p_next = null;
		}

		return true;
	}

/*
=================================================
	_ConvertDependencies
=================================================
*/
	bool  SerializableVkRenderPass::_ConvertDependencies (const CompatibleRenderPassDesc &compat, const RenderPassSpec &spec)
	{
		CHECK_ERR( not compat._subpasses.empty() );

		static constexpr auto	read_access_mask	= VPipelineScope::GetReadAccess( EPipelineScope::All );
		//static constexpr auto	write_access_mask	= VPipelineScope::GetWriteAccess( EPipelineScope::All );
		static constexpr auto	graphics_stages		= VPipelineScope::GetStages( EPipelineScope::Graphics );

		struct DepInfo
		{
			ulong					hash	= 0;
			VkSubpassDependency2	dep		= {};
			VkMemoryBarrier2		bar		= {};

			DepInfo () {}
			DepInfo (const DepInfo &) = default;

			DepInfo (uint srcIdx, uint dstIdx, uint srcSP, uint dstSP)
			{
				hash				= (ulong(srcIdx) << 32) | ulong(dstIdx);

				dep.sType			= VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2;
				dep.dependencyFlags	= VK_DEPENDENCY_BY_REGION_BIT;	// TODO: VK_DEPENDENCY_VIEW_LOCAL_BIT  // TODO: remove by_region for VK_SUBPASS_EXTERNAL ?
				dep.srcSubpass		= srcSP;
				dep.dstSubpass		= dstSP;

				bar.sType			= VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
			}

			DepInfo&	operator = (const DepInfo &) = default;

			ND_ uint	SrcSubpass ()	const	{ return uint(hash >> 32); }
			ND_ uint	DstSubpass ()	const	{ return uint(hash & 0xFFFF); }
		};
		HashMap<ulong, DepInfo>	dependencies;

		// create execution dependencies between all subpasses
		for (usize i = 1; i < compat._subpasses.size(); ++i)
		{
			DepInfo	dep{ uint(i), uint(i+1), uint(i-1), uint(i) };
			dependencies[dep.hash] = dep;
		}
		{
			DepInfo		initial{ 0, 1, VK_SUBPASS_EXTERNAL, 0 };
			dependencies[initial.hash] = initial;

			DepInfo		final{ uint(compat._subpasses.size()), uint(compat._subpasses.size()+1), uint(compat._subpasses.size()-1), VK_SUBPASS_EXTERNAL };
			dependencies[final.hash] = final;
		}

		const auto	MinimizeStages = [] (INOUT VkPipelineStageFlagBits2 &stages)
		{{
			CHECK( stages != 0 );

			if ( stages & VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT )
				stages = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

			if ( (stages & VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT) and AllBits( graphics_stages, stages ))
				stages = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
		}};


		for (auto& [att_name, rt] : spec._attachments)
		{
			Array<Tuple< EResourceState, uint >>	states;

			// initial layout
			{
				auto	layout_it = rt->layouts.find( Subpass_ExternalIn );
				if ( layout_it != rt->layouts.end() and IsValidLayout( layout_it->second.state ))
					states.emplace_back( layout_it->second.state, 0 );
			}

			for (usize i = 0; i < compat._subpasses.size(); ++i)
			{
				auto	layout_it = rt->layouts.find( compat._subpasses[i].name );
				if ( layout_it != rt->layouts.end() and IsValidLayout( layout_it->second.state ))
				{
					auto	state = layout_it->second.state;

					// add self dependency
					switch ( ToEResState( state )) {
						case _EResState::InputColorAttachment_RW :
						case _EResState::InputDepthStencilAttachment_RW :
							states.emplace_back( state, uint(i+1) );
							break;
					}

					states.emplace_back( state, uint(i+1) );
				}
			}

			// final layout
			{
				auto	layout_it = rt->layouts.find( Subpass_ExternalOut );
				if ( layout_it != rt->layouts.end() and IsValidLayout( layout_it->second.state ))
					states.emplace_back( layout_it->second.state, uint(compat._subpasses.size()+1) );
			}

			// add execution barriers
			for (auto& st : states)
			{
				const auto	stages = EResourceState_ToPipelineStages( st.Get<0>() );
				for (auto& [h, dep] : dependencies)
				{
					if ( dep.SrcSubpass() == st.Get<1>() )
						dep.bar.srcStageMask |= stages;

					if ( dep.DstSubpass() == st.Get<1>() )
						dep.bar.dstStageMask |= stages;
				}
			}

			// add memory access mask
			for (usize i = 1; i < states.size(); ++i)
			{
				const auto&		prev = states[i-1];
				const auto&		next = states[i];

				if ( prev.Get<0>() == next.Get<0>() )
					continue;

				const auto		prev_access	= EResourceState_ToAccessMask( prev.Get<0>() ) & ~read_access_mask;
				const auto		next_access	= EResourceState_ToAccessMask( next.Get<0>() ); // & ~prev_access;

				for (auto& [h, dep] : dependencies)
				{
					if ( dep.SrcSubpass() == prev.Get<1>() )
						dep.bar.srcAccessMask |= prev_access;

					if ( dep.DstSubpass() == next.Get<1>() )
						dep.bar.dstAccessMask |= next_access;
				}
			}
		}

		if ( not dependencies.empty() )
		{
			Array<DepInfo>	sorted;

			for (auto& [h, src] : dependencies)
			{
				if ( (src.bar.srcStageMask | src.bar.dstStageMask) == 0							or
					 (src.dep.srcSubpass == VK_SUBPASS_EXTERNAL and src.bar.srcStageMask == 0)	or
					 (src.dep.dstSubpass == VK_SUBPASS_EXTERNAL and src.bar.dstStageMask == 0) )
					continue;

				auto&	dep = sorted.emplace_back( src );
				MinimizeStages( INOUT dep.bar.srcStageMask );
				MinimizeStages( INOUT dep.bar.dstStageMask );
			}
			std::sort( sorted.begin(), sorted.end(), [](auto& lhs, auto& rhs) { return lhs.hash < rhs.hash; });

			if ( not sorted.empty() )
			{
				auto*	deps	= _allocator.Allocate<VkSubpassDependency2>( sorted.size() );
				auto*	bars	= _allocator.Allocate<VkMemoryBarrier2>( sorted.size() );
				CHECK_ERR( deps != null and bars != null );

				for (usize i = 0; i < sorted.size(); ++i)
				{
					MemCopy( OUT deps[i], sorted[i].dep );
					MemCopy( OUT bars[i], sorted[i].bar );

					deps[i].pNext = &bars[i];
				}

				_ci.dependencyCount	= uint(sorted.size());
				_ci.pDependencies	= deps;
			}
		}
		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  SerializableVkRenderPass::Create (const CompatibleRenderPassDesc &compat)
	{
		return _Create( compat, RenderPassSpec{}, false );
	}

	bool  SerializableVkRenderPass::Create (const CompatibleRenderPassDesc &compat, const RenderPassSpec &rp)
	{
		return _Create( compat, rp, true );
	}

/*
=================================================
	_Create
=================================================
*/
	bool  SerializableVkRenderPass::_Create (const CompatibleRenderPassDesc &compat, const RenderPassSpec &rp, bool withDeps)
	{
		_allocator.Discard();
		_ci = {};

		_name = rp._name;	// for compatible RP name is not defined

		_ci.sType	= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
		_ci.pNext	= null;		// TODO: VkRenderPassFragmentDensityMapCreateInfoEXT
		_ci.flags	= 0;		// unused

		CHECK_ERR( _ConvertAttachments( compat, rp ));
		CHECK_ERR( _ConvertSubpasses( compat, rp ));

		if ( withDeps )
			CHECK_ERR( _ConvertDependencies( compat, rp ));

		_ci.correlatedViewMaskCount	= 0;
		_ci.pCorrelatedViewMasks	= null;		// TODO

		_isCompatible = false;

		return true;
	}

/*
=================================================
	MakeCompatible
=================================================
*/
	bool  SerializableVkRenderPass::MakeCompatible (INOUT Array<SerializableVkRenderPass> &renderPasses)
	{
		CHECK_ERR( renderPasses.size() > 1 );

		const auto	GetRPName = [](const SerializableVkRenderPass &rp) -> String
		{{
			#if AE_OPTIMIZE_IDS
				Unused( rp );	return "";
			#else
				return "'"s << rp.Name().GetName() << "'";
			#endif
		}};

		// find max 'dependencyCount'
		uint	max_deps = 0;
		for (const auto& rp : renderPasses) {
			max_deps = Max( max_deps, rp->dependencyCount );
		}

		// 'dependencyCount' must be the same for all specializations
		for (usize i = 1; i < renderPasses.size(); ++i)
		{
			const auto&	rp = renderPasses[i];
			CHECK_ERR( not rp._isCompatible );
			CHECK_ERR_MSG( rp->dependencyCount == max_deps,
				"'dependencyCount' must be the same for all specializations, required ("s << Base::ToString(max_deps) <<
				") but in RP " << GetRPName(rp) << " it is (" << Base::ToString(rp->dependencyCount) << ")" );
		}

		if ( max_deps == 0 )
		{
			for (auto& rp : renderPasses) {
				rp._isCompatible = true;
			}
			return true;
		}

		// add dependencies to compatible RP
		auto&	compat_rp = renderPasses[0];
		{
			const auto&	src_deps	= renderPasses[1]->pDependencies;
			auto*		deps		= compat_rp._allocator.Allocate<VkSubpassDependency2>( max_deps );
			auto*		bars		= compat_rp._allocator.Allocate<VkMemoryBarrier2>( max_deps );
			CHECK_ERR( deps != null and bars != null );

			compat_rp._ci.dependencyCount	= max_deps;
			compat_rp._ci.pDependencies		= deps;

			for (uint i = 0; i < max_deps; ++i, ++deps, ++bars)
			{
				*deps = src_deps[i];

				CHECK_ERR( deps->pNext != null );
				deps->pNext = bars;

				*bars = *Cast<VkMemoryBarrier2>(src_deps[i].pNext);
			}
		}

		// validate dependencies and accumulate flags into compatible RP
		for (usize i = 1; i < renderPasses.size(); ++i)
		{
			const auto&	rp = renderPasses[i];

			for (uint j = 0; j < max_deps; ++j)
			{
				const auto&	src_dep	= rp->pDependencies[j];
				const auto&	src_bar	= *Cast<VkMemoryBarrier2>(src_dep.pNext);
				auto&		dst_dep	= const_cast<VkSubpassDependency2 &>(compat_rp->pDependencies[j]);
				auto&		dst_bar	= const_cast<VkMemoryBarrier2 &>(*Cast<VkMemoryBarrier2>(dst_dep.pNext));

				CHECK_ERR_MSG( src_dep.srcSubpass == dst_dep.srcSubpass,
					"'srcSubpass' must be same in CompatRP ("s << Base::ToString(dst_dep.srcSubpass) << ") and RP " <<
					GetRPName(rp) << " (" << Base::ToString(src_dep.srcSubpass) << ")" );

				CHECK_ERR_MSG( src_dep.dstSubpass == dst_dep.dstSubpass,
					"'dstSubpass' must be same in CompatRP ("s << Base::ToString(dst_dep.dstSubpass) << ") and RP " <<
					GetRPName(rp) << " (" << Base::ToString(src_dep.dstSubpass) << ")" );

				CHECK_ERR_MSG( src_dep.viewOffset == dst_dep.viewOffset,
					"'viewOffset' must be same in CompatRP ("s << Base::ToString(dst_dep.viewOffset) << ") and RP " <<
					GetRPName(rp) << " (" << Base::ToString(src_dep.viewOffset) << ")" );

				dst_dep.dependencyFlags	|= src_dep.dependencyFlags;
				dst_bar.srcStageMask	|= src_bar.srcStageMask;
				dst_bar.srcAccessMask	|= src_bar.srcAccessMask;
				dst_bar.dstStageMask	|= src_bar.dstStageMask;
				dst_bar.dstAccessMask	|= src_bar.dstAccessMask;
			}
		}

		// copy flags from compatible RP to specializations
		for (usize i = 1; i < renderPasses.size(); ++i)
		{
			auto&	rp = renderPasses[i];

			for (uint j = 0; j < max_deps; ++j)
			{
				const auto&	src_dep	= compat_rp->pDependencies[j];
				const auto&	src_bar	= *Cast<VkMemoryBarrier2>(src_dep.pNext);
				auto&		dst_dep	= const_cast<VkSubpassDependency2 &>(rp->pDependencies[j]);
				auto&		dst_bar	= const_cast<VkMemoryBarrier2 &>(*Cast<VkMemoryBarrier2>(dst_dep.pNext));

				dst_dep.dependencyFlags	|= src_dep.dependencyFlags;
				dst_bar.srcStageMask	|= src_bar.srcStageMask;
				dst_bar.srcAccessMask	|= src_bar.srcAccessMask;
				dst_bar.dstStageMask	|= src_bar.dstStageMask;
				dst_bar.dstAccessMask	|= src_bar.dstAccessMask;
			}
		}

		for (auto& rp : renderPasses) {
			rp._isCompatible = true;
		}
		return true;
	}

# endif // AE_BUILD_PIPELINE_COMPILER

/*
=================================================
	ToString
=================================================
*/
# ifdef AE_TEST_PIPELINE_COMPILER
#	include "vulkan_loader/vkenum_to_str.h"

	String  SerializableVkRenderPass::ToString (const HashToName &nameMap) const
	{
		ASSERT( _isCompatible );
		String	str;

		const auto	AttachmentRefsToString = [&str] (const String &prefix, const VkAttachmentReference2 *refs, uint count, bool indexed = true)
		{{
			ASSERT( indexed or count  );
			for (uint i = 0; i < count; ++i)
			{
				if ( i > 0 )
					str << "\n    --------";

				auto&	ref = refs[i];
				str //<< prefix << "[" << Base::ToString(i) << "].sType      = " << VkStructureTypeToString( ref.sType )
					<< prefix << (indexed ? "["s << Base::ToString(i) << "]" : "") << ".pNext     " << (ref.pNext == null ? " = null" : "!= null")
					<< prefix << (indexed ? "["s << Base::ToString(i) << "]" : "") << ".attachment = " << (ref.attachment == VK_ATTACHMENT_UNUSED ? "UNUSED"s : Base::ToString( ref.attachment ))
					<< prefix << (indexed ? "["s << Base::ToString(i) << "]" : "") << ".layout     = " << VkImageLayoutToString( ref.layout )
					<< prefix << (indexed ? "["s << Base::ToString(i) << "]" : "") << ".aspectMask = " << VkImageAspectFlagsToString( ref.aspectMask );
			}
		}};

		CHECK( _ci.pNext == null );

		str << "\nname               = " << (_name.IsDefined() ? ("'" + nameMap( _name ) + "'") : "--")		// for compatible RP name is not defined
			//<< "\nci.sType           = " << VkStructureTypeToString( _ci.sType )
			//<< "\nci.pNext          " << (_ci.pNext == null ? " = null" : "!= null")
			<< "\nci.flags           = " << VkRenderPassCreateFlagsToString( _ci.flags )
			<< "\nci.attachmentCount = " << Base::ToString( _ci.attachmentCount );

		if ( _ci.attachmentCount > 0 )
		{
			CHECK_ERR( _ci.pAttachments != null );
			str << "\nci.pAttachments    = {";
			for (uint i = 0; i < _ci.attachmentCount; ++i)
			{
				const auto&	att = _ci.pAttachments[i];
				CHECK( att.pNext == null );

				if ( i > 0 )
					str << "\n  --------";

				str //<< "\n  [" << Base::ToString(i) << "].sType          = " << VkStructureTypeToString( att.sType )
					//<< "\n  [" << Base::ToString(i) << "].pNext         " << (att.pNext == null ? " = null" : "!= null")
					<< "\n  [" << Base::ToString(i) << "].flags          = " << VkAttachmentDescriptionFlagsToString( att.flags )
					<< "\n  [" << Base::ToString(i) << "].format         = " << (att.format == VK_FORMAT_MAX_ENUM ? "SwapchainColor"s : VkFormatToString( att.format ))
					<< "\n  [" << Base::ToString(i) << "].samples        = " << VkSampleCountFlagBitsToString( att.samples )
					<< "\n  [" << Base::ToString(i) << "].loadOp         = " << VkAttachmentLoadOpToString( att.loadOp )
					<< "\n  [" << Base::ToString(i) << "].storeOp        = " << VkAttachmentStoreOpToString( att.storeOp )
					<< "\n  [" << Base::ToString(i) << "].stencilLoadOp  = " << VkAttachmentLoadOpToString( att.stencilLoadOp )
					<< "\n  [" << Base::ToString(i) << "].stencilStoreOp = " << VkAttachmentStoreOpToString( att.stencilStoreOp )
					<< "\n  [" << Base::ToString(i) << "].initialLayout  = " << VkImageLayoutToString( att.initialLayout ) << "  (" << Base::ToString( _states[i].initial ) << ")"
					<< "\n  [" << Base::ToString(i) << "].finalLayout    = " << VkImageLayoutToString( att.finalLayout   ) << "  (" << Base::ToString( _states[i].final   ) << ")";
			}
			str << "\n}";
		}

		str	<< "\nci.subpassCount = " << Base::ToString( _ci.subpassCount );
		if ( _ci.subpassCount > 0 )
		{
			CHECK_ERR( _ci.pSubpasses != null );
			str << "\nci.pSubpasses   = {";
			for (uint i = 0; i < _ci.subpassCount; ++i)
			{
				const auto&	sp = _ci.pSubpasses[i];

				if ( i > 0 )
					str << "\n  --------";

				str //<< "\n  [" << Base::ToString(i) << "].sType                = " << VkStructureTypeToString( sp.sType )
					//<< "\n  [" << Base::ToString(i) << "].pNext               " << (sp.pNext == null ? " = null" : "!= null")
					<< "\n  [" << Base::ToString(i) << "].flags                = " << VkSubpassDescriptionFlagsToString( sp.flags )
					<< "\n  [" << Base::ToString(i) << "].pipelineBindPoint    = " << VkPipelineBindPointToString( sp.pipelineBindPoint )
					<< "\n  [" << Base::ToString(i) << "].viewMask             = " << Base::ToString( sp.viewMask )
					<< "\n  [" << Base::ToString(i) << "].inputAttachmentCount = " << Base::ToString( sp.inputAttachmentCount );

				if ( sp.inputAttachmentCount > 0 )
				{
					CHECK_ERR( sp.pInputAttachments != null );
					str << "\n  ["s << Base::ToString(i) << "].pInputAttachments    = {";
					AttachmentRefsToString( "\n    ", sp.pInputAttachments, sp.inputAttachmentCount );
					str << "\n  }";
				}

				str << "\n  [" << Base::ToString(i) << "].colorAttachmentCount = " << Base::ToString( sp.colorAttachmentCount );
				if ( sp.colorAttachmentCount > 0 )
				{
					CHECK_ERR( sp.pColorAttachments != null );
					str << "\n  ["s << Base::ToString(i) << "].pColorAttachments    = {";
					AttachmentRefsToString( "\n    ", sp.pColorAttachments, sp.colorAttachmentCount );
					str << "\n  }";

					if ( sp.pResolveAttachments != null )
					{
						str << "\n  ["s << Base::ToString(i) << "].pResolveAttachments   = {";
						AttachmentRefsToString( "\n    ", sp.pResolveAttachments, sp.colorAttachmentCount );
						str << "\n  }";
					}
				}

				str << "\n  [" << Base::ToString(i) << "].pDepthStencilAttachment = {";
				if ( sp.pDepthStencilAttachment != null )
				{
					AttachmentRefsToString( "\n    ", sp.pDepthStencilAttachment, 1, false );
					str << "\n  }";
				}
				else
					str << "}";

				str << "\n  [" << Base::ToString(i) << "].preserveAttachmentCount = " << Base::ToString( sp.preserveAttachmentCount );
				if ( sp.preserveAttachmentCount > 0 )
				{
					CHECK_ERR( sp.pPreserveAttachments != null );
					str << "\n  ["s << Base::ToString(i) << "].pPreserveAttachments    = { ";
					for (uint j = 0; j < sp.preserveAttachmentCount; ++j)
					{
						if ( j > 0 ) str << ", ";
						str << Base::ToString( sp.pPreserveAttachments[j] );
					}
					str << " }";
				}

				for (auto* next = Cast<VkBaseInStructure>(sp.pNext); next != null; next = next->pNext)
				{
					switch ( next->sType )
					{
						case VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR :
						{
							auto&	sra = *Cast<VkFragmentShadingRateAttachmentInfoKHR>(next);
							str << "\n  ["s << Base::ToString(i) << "].fragmentShadingRateAttachment = { ";
							AttachmentRefsToString( "\n    ", sra.pFragmentShadingRateAttachment, 1, false );
							str << "\n    .shadingRateAttachmentTexelSize = {" << Base::ToString( sra.shadingRateAttachmentTexelSize.width )
								<< ", " << Base::ToString( sra.shadingRateAttachmentTexelSize.height ) << "}";
							str << "\n  }";
							break;
						}
						default :
							DBG_WARNING( "unsupported extension" );
					}
				}
			}
			str << "\n}";
		}

		str	<< "\nci.dependencyCount = " << Base::ToString( _ci.dependencyCount );
		if ( _ci.dependencyCount > 0 )
		{
			CHECK_ERR( _ci.pDependencies != null );
			str << "\nci.pDependencies   = {";
			for (uint i = 0; i < _ci.dependencyCount; ++i)
			{
				const auto&	dep = _ci.pDependencies[i];

				if ( i > 0 )
					str << "\n  --------";

				str //<< "\n  [" << Base::ToString(i) << "].sType           = " << VkStructureTypeToString( dep.sType )
					//<< "\n  [" << Base::ToString(i) << "].pNext          " << (dep.pNext == null ? " = null" : "!= null")
					<< "\n  [" << Base::ToString(i) << "].srcSubpass      = " << (dep.srcSubpass == VK_SUBPASS_EXTERNAL ? "VK_SUBPASS_EXTERNAL"s : Base::ToString( dep.srcSubpass ))
					<< "\n  [" << Base::ToString(i) << "].dstSubpass      = " << (dep.dstSubpass == VK_SUBPASS_EXTERNAL ? "VK_SUBPASS_EXTERNAL"s : Base::ToString( dep.dstSubpass ));

				CHECK_ERR( dep.srcStageMask == 0 and dep.dstStageMask == 0 and dep.srcAccessMask == 0 and dep.dstAccessMask == 0 );

				if ( dep.pNext != null )
				{
					const auto&	bar = *Cast<VkMemoryBarrier2>(dep.pNext);

					CHECK_ERR( bar.sType == VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 );
					CHECK_ERR( bar.pNext == null );
					str << "\n  [" << Base::ToString(i) << "].srcStageMask    = " << VkPipelineStageFlags2ToString( bar.srcStageMask )
						<< "\n  [" << Base::ToString(i) << "].srcAccessMask   = " << VkAccessFlags2ToString( bar.srcAccessMask )
						<< "\n  [" << Base::ToString(i) << "].dstStageMask    = " << VkPipelineStageFlags2ToString( bar.dstStageMask )
						<< "\n  [" << Base::ToString(i) << "].dstAccessMask   = " << VkAccessFlags2ToString( bar.dstAccessMask );
				}

				str << "\n  [" << Base::ToString(i) << "].dependencyFlags = " << VkDependencyFlagsToString( dep.dependencyFlags )
					<< "\n  [" << Base::ToString(i) << "].viewOffset      = " << Base::ToString( dep.viewOffset );
			}
			str << "\n}";
		}

		str << "\n=========================================================================\n\n";
		return str;
	}
# endif // AE_TEST_PIPELINE_COMPILER


/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableVkRenderPass::Serialize (Serializing::Serializer &ser) C_NE___
	{
		ASSERT( _isCompatible );

		bool	result = true;

		CHECK( _ci.sType == VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2 );
		CHECK( _ci.pNext == null );
		CHECK( _ci.flags == 0 );

		result &= ser( _name, _states );

		result &= ser( _ci.attachmentCount );
		for (uint i = 0; i < _ci.attachmentCount; ++i)
		{
			const auto&	att = _ci.pAttachments[i];

			CHECK( att.sType == VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2 );
			CHECK( att.pNext == null );
			CHECK( att.flags == 0 );

			result &= ser( att.format, att.samples, att.loadOp, att.storeOp,
						   att.stencilLoadOp, att.stencilStoreOp,
						   att.initialLayout, att.finalLayout );
		}

		const auto	SerAttachmentRef = [] (const VkAttachmentReference2 &ref, Serializing::Serializer &ser2) -> bool
		{{
			CHECK( ref.sType == VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2 );
			CHECK( ref.pNext == null );
			return ser2( ref.attachment, ref.layout, ref.aspectMask );
		}};

		const auto	SerFragShadingRateAtt = [&SerAttachmentRef] (const VkFragmentShadingRateAttachmentInfoKHR &sra, Serializing::Serializer &ser2) -> bool
		{{
			CHECK( sra.sType == VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR );
			CHECK( sra.pFragmentShadingRateAttachment != null );
			return	ser2( uint(sra.sType), sra.shadingRateAttachmentTexelSize.width, sra.shadingRateAttachmentTexelSize.height ) and
					SerAttachmentRef( *sra.pFragmentShadingRateAttachment, ser2 );
		}};

		result &= ser( _ci.subpassCount );
		for (uint i = 0; i < _ci.subpassCount; ++i)
		{
			const auto&	sp = _ci.pSubpasses[i];

			CHECK( sp.sType == VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2 );
			CHECK( sp.flags == 0 );
			CHECK( sp.pipelineBindPoint == VK_PIPELINE_BIND_POINT_GRAPHICS );
			CHECK( sp.viewMask == 0 );

			result &= ser( sp.inputAttachmentCount );
			for (uint j = 0; j < sp.inputAttachmentCount; ++j) {
				result &= SerAttachmentRef( sp.pInputAttachments[j], ser );
			}

			result &= ser( sp.colorAttachmentCount );
			for (uint j = 0; j < sp.colorAttachmentCount; ++j) {
				result &= SerAttachmentRef( sp.pColorAttachments[j], ser );
			}

			if ( sp.pResolveAttachments != null )
			{
				result &= ser( sp.colorAttachmentCount );
				for (uint j = 0; j < sp.colorAttachmentCount; ++j) {
					result &= SerAttachmentRef( sp.pResolveAttachments[j], ser );
				}
			}
			else
				result &= ser( 0u );

			result &= ser( sp.pDepthStencilAttachment != null );
			if ( sp.pDepthStencilAttachment != null ) {
				result &= SerAttachmentRef( *sp.pDepthStencilAttachment, ser );
			}

			result &= ser( sp.preserveAttachmentCount );
			for (uint j = 0; j < sp.preserveAttachmentCount; ++j) {
				result &= ser( sp.pPreserveAttachments[j] );
			}

			// serialize extensions
			{
				uint	count = 0;
				for (auto* next = Cast<VkBaseInStructure>(sp.pNext); next != null; next = next->pNext, ++count) {}
				result &= ser( count );
			}

			for (auto* next = Cast<VkBaseInStructure>(sp.pNext); next != null; next = next->pNext)
			{
				switch ( next->sType )
				{
					case VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR :
						result &= SerFragShadingRateAtt( *Cast<VkFragmentShadingRateAttachmentInfoKHR>(next), ser );
						break;

					default :
						RETURN_ERR( "unsupported extension" );
				}
			}
		}

		result &= ser( _ci.dependencyCount );
		for (uint i = 0; i < _ci.dependencyCount; ++i)
		{
			const auto&	dep = _ci.pDependencies[i];

			CHECK( dep.sType == VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2 );
			CHECK( dep.viewOffset == 0 );
			CHECK_ERR( dep.pNext != null );

			const auto&	bar = *Cast<VkMemoryBarrier2>(dep.pNext);
			CHECK_ERR( bar.sType == VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 );
			CHECK_ERR( bar.pNext == null );

			result &= ser( dep.srcSubpass, dep.dstSubpass, dep.dependencyFlags );
			result &= ser( bar.srcStageMask, bar.srcAccessMask, bar.dstStageMask, bar.dstAccessMask );
		}

		CHECK( _ci.correlatedViewMaskCount == 0 );
		CHECK( _ci.pCorrelatedViewMasks == null );

		return result;
	}

/*
=================================================
	Deserialize
=================================================
*/
	bool  SerializableVkRenderPass::Deserialize (Serializing::Deserializer &des) __NE___
	{
		return Deserialize( Default, des );
	}

	bool  SerializableVkRenderPass::Deserialize (EPixelFormat surfaceFormat, Serializing::Deserializer &des) __NE___
	{
		const VkFormat	vk_swfmt = (surfaceFormat != Default ? VEnumCast( surfaceFormat ) : VK_FORMAT_MAX_ENUM);
		bool			result	 = true;

		result &= des( OUT _name, OUT _states );

		_ci			= {};
		_ci.sType	= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2;
		_ci.pNext	= null;		// TODO: VkRenderPassFragmentDensityMapCreateInfoEXT
		_ci.flags	= 0;		// unused

		result &= des( OUT _ci.attachmentCount );
		CHECK_ERR( result and _ci.attachmentCount <= GraphicsConfig::MaxAttachments );

		if ( _ci.attachmentCount > 0 )
		{
			auto*	attachments = _allocator.Allocate<VkAttachmentDescription2>( _ci.attachmentCount );
			CHECK_ERR( attachments != null );

			for (uint i = 0; i < _ci.attachmentCount; ++i)
			{
				auto&	att = attachments[i];

				att			= {};
				att.sType	= VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
				att.pNext	= null;
				att.flags	= 0;

				result &= des( att.format, att.samples, att.loadOp, att.storeOp,
							   att.stencilLoadOp, att.stencilStoreOp,
							   att.initialLayout, att.finalLayout );

				if ( att.format == VK_FORMAT_MAX_ENUM )
					att.format = vk_swfmt;
			}
			_ci.pAttachments = attachments;
		}

		const auto	DesAttachmentRef = [] (OUT VkAttachmentReference2 &ref, Serializing::Deserializer &des2) -> bool
		{{
			ref			= {};
			ref.sType	= VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2;
			ref.pNext	= null;
			return des2( OUT ref.attachment, OUT ref.layout, OUT ref.aspectMask );
		}};

		const auto	DesFragShadingRateAtt = [&DesAttachmentRef, this] (VkBaseOutStructure** &pNext, Serializing::Deserializer &des2) -> bool
		{{
			auto*	sra	= _allocator.Allocate<VkFragmentShadingRateAttachmentInfoKHR>(1);
			auto*	ref	= _allocator.Allocate<VkAttachmentReference2>(1);
			CHECK_ERR( sra != null and ref != null );

			sra->sType							= VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR;
			sra->pNext							= null;
			sra->pFragmentShadingRateAttachment	= ref;

			*pNext = Cast<VkBaseOutStructure>(sra);
			pNext = &(*pNext)->pNext;

			return	des2( OUT sra->shadingRateAttachmentTexelSize.width, OUT sra->shadingRateAttachmentTexelSize.height ) and
					DesAttachmentRef( OUT *ref, des2 );
		}};

		result &= des( _ci.subpassCount );
		CHECK_ERR( result and _ci.subpassCount <= GraphicsConfig::MaxSubpasses );

		if ( _ci.subpassCount > 0 )
		{
			auto*	subpasses = _allocator.Allocate<VkSubpassDescription2>( _ci.subpassCount );
			CHECK_ERR( subpasses != null );

			for (uint i = 0; i < _ci.subpassCount; ++i)
			{
				auto&					sp		= subpasses[i];
				VkBaseOutStructure**	p_next	= BitCast<VkBaseOutStructure**>( &sp.pNext );

				sp						= {};
				sp.sType				= VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2;
				sp.pNext				= null;
				sp.flags				= 0;
				sp.pipelineBindPoint	= VK_PIPELINE_BIND_POINT_GRAPHICS;
				sp.viewMask				= 0;

				result &= des( OUT sp.inputAttachmentCount );
				CHECK_ERR( result and sp.inputAttachmentCount <= GraphicsConfig::MaxAttachments );
				if ( sp.inputAttachmentCount > 0 )
				{
					auto*	refs = _allocator.Allocate<VkAttachmentReference2>( sp.inputAttachmentCount );
					CHECK_ERR( refs != null );

					for (uint j = 0; j < sp.inputAttachmentCount; ++j) {
						result &= DesAttachmentRef( OUT refs[j], des );
					}
					sp.pInputAttachments = refs;
				}

				result &= des( OUT sp.colorAttachmentCount );
				CHECK_ERR( result and sp.colorAttachmentCount <= GraphicsConfig::MaxAttachments );
				if ( sp.colorAttachmentCount > 0 )
				{
					auto*	refs = _allocator.Allocate<VkAttachmentReference2>( sp.colorAttachmentCount );
					CHECK_ERR( refs != null );

					for (uint j = 0; j < sp.colorAttachmentCount; ++j) {
						result &= DesAttachmentRef( OUT refs[j], des );
					}
					sp.pColorAttachments = refs;
				}

				uint	resolve_attachment_count = 0;
				result &= des( OUT resolve_attachment_count );
				CHECK_ERR( result and resolve_attachment_count <= GraphicsConfig::MaxAttachments );
				if ( resolve_attachment_count > 0 )
				{
					auto*	refs = _allocator.Allocate<VkAttachmentReference2>( resolve_attachment_count );
					CHECK_ERR( refs != null );

					for (uint j = 0; j < resolve_attachment_count; ++j) {
						result &= DesAttachmentRef( OUT refs[j], des );
					}
					sp.pResolveAttachments = refs;
				}

				bool	 has_depth_stencil = false;
				result &= des( OUT has_depth_stencil );
				CHECK_ERR( result );
				if ( has_depth_stencil )
				{
					auto*	ds = _allocator.Allocate<VkAttachmentReference2>( 1 );
					CHECK_ERR( ds != null );

					result &= DesAttachmentRef( OUT *ds, des );
					sp.pDepthStencilAttachment = ds;
				}

				result &= des( OUT sp.preserveAttachmentCount );
				CHECK_ERR( result and sp.preserveAttachmentCount <= GraphicsConfig::MaxAttachments );
				if ( sp.preserveAttachmentCount > 0 )
				{
					auto*	refs = _allocator.Allocate<uint>( sp.preserveAttachmentCount );
					CHECK_ERR( refs != null );

					for (uint j = 0; j < sp.preserveAttachmentCount; ++j) {
						result &= des( OUT refs[j] );
					}
					sp.pPreserveAttachments = refs;
				}

				// deserialize extensions
				{
					uint	ext_count = 0;
					result &= des( OUT ext_count );

					for (uint j = 0; j < ext_count; ++j)
					{
						uint	type = 0;
						result &= des( OUT type );

						switch ( type )
						{
							case VK_STRUCTURE_TYPE_FRAGMENT_SHADING_RATE_ATTACHMENT_INFO_KHR :
								result &= DesFragShadingRateAtt( OUT p_next, des );
								break;

							default :
								RETURN_ERR( "unsupported extension" );
						}
					}
				}

				p_next = null;
			}
			_ci.pSubpasses = subpasses;
		}

		result &= des( OUT _ci.dependencyCount );
		CHECK_ERR( result and _ci.dependencyCount <= GraphicsConfig::MaxSubpassDeps );

		if ( _ci.dependencyCount > 0 )
		{
			auto*	dependencies = _allocator.Allocate<VkSubpassDependency2>( _ci.dependencyCount );
			auto*	barriers	 = _allocator.Allocate<VkMemoryBarrier2>( _ci.dependencyCount );
			CHECK_ERR( dependencies != null and barriers != null );

			for (uint i = 0; i < _ci.dependencyCount; ++i)
			{
				auto&	dep = dependencies[i];
				auto&	bar = barriers[i];

				dep				= {};
				dep.sType		= VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2;
				dep.pNext		= &bar;
				dep.viewOffset	= 0;
				result &= des( OUT dep.srcSubpass, OUT dep.dstSubpass, OUT dep.dependencyFlags );

				bar			= {};
				bar.sType	= VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
				bar.pNext	= null;
				result &= des( OUT bar.srcStageMask, OUT bar.srcAccessMask, OUT bar.dstStageMask, OUT bar.dstAccessMask );

				ASSERT( bar.srcStageMask != 0 );
				ASSERT( bar.dstStageMask != 0 );
			}
			_ci.pDependencies = dependencies;
		}

		_ci.correlatedViewMaskCount	= 0;
		_ci.pCorrelatedViewMasks	= null;		// TODO

		_isCompatible = true;

		return result;
	}

#endif // AE_ENABLE_VULKAN
//-----------------------------------------------------------------------------



#ifdef AE_BUILD_PIPELINE_COMPILER
/*
=================================================
	Create
=================================================
*/
	bool  SerializableMtlRenderPass::Create (const CompatibleRenderPassDesc &compat)
	{
		return Create( compat, RenderPassSpec{} );
	}

	bool  SerializableMtlRenderPass::Create (const CompatibleRenderPassDesc &compat, const RenderPassSpec &rp)
	{
		_name = rp._name;	// for compatible RP name is not defined

		CHECK_ERR( _ConvertAttachments( compat, rp ));
		CHECK_ERR( _ValidateSubpasses( compat, rp ));

		return true;
	}

/*
=================================================
	_ConvertAttachments
=================================================
*/
	bool  SerializableMtlRenderPass::_ConvertAttachments (const CompatibleRenderPassDesc &compat, const RenderPassSpec &spec)
	{
		CHECK_ERR( compat._attachments.size() <= GraphicsConfig::MaxAttachments );

		const uint	count = uint(compat._attachments.size());
		//_attCount = CheckCast<ubyte>( count );

		for (auto& [name, rt] : compat._attachments)
		{
			CHECK_ERR( rt->index < count );

			const auto&	src			= *rt;
			auto&		dst_att		= _mtlAtt[ src.index ];
			auto&		dst_state	= _states[ src.index ];

			dst_att.format = MEnumCast( src.format );

			if ( src.format != EPixelFormat::SwapchainColor )
			{
				static constexpr auto	mask = PixelFormatInfo::EType::DepthStencil;

				auto&	fmt_info = EPixelFormat_GetInfo( src.format );
				switch ( fmt_info.valueType & mask )
				{
					case PixelFormatInfo::EType::Depth :		dst_att.flags = MtlAttachmentFlags::Depth;		break;
					case PixelFormatInfo::EType::Stencil :		dst_att.flags = MtlAttachmentFlags::Stencil;	break;
					case PixelFormatInfo::EType::DepthStencil :	dst_att.flags = MtlAttachmentFlags::Depth | MtlAttachmentFlags::Stencil;	break;
					default :									dst_att.flags = MtlAttachmentFlags::Color;		break;
				}
			}
			else
			{
				dst_att.flags = MtlAttachmentFlags::Color;
			}

			auto	attach_spec = spec._attachments.find( name );
			if ( attach_spec != spec._attachments.end() )
			{
				dst_att.loadAction		= MEnumCast( attach_spec->second->loadOp );
				dst_att.storeAction		= MEnumCast( attach_spec->second->storeOp );
				//dst_att.storeActionOptions	// TODO

				auto&	layouts			= attach_spec->second->layouts;
				auto	initial_layout	= layouts.find( Subpass_ExternalIn );
				auto	final_layout	= layouts.find( Subpass_ExternalOut );

				if ( initial_layout != layouts.end() and IsValidInitialLayout( initial_layout->second.state ))
				{
					dst_state.initial = initial_layout->second.state;
				}
				else
				{
					// find first layout
					for (auto& sp : compat._subpasses)
					{
						auto	l_it = layouts.find( sp.name );
						if ( l_it != layouts.end() and IsValidInitialLayout( l_it->second.state ))
						{
							dst_state.initial = l_it->second.state;
							break;
						}
					}
				}

				if ( final_layout != layouts.end() and IsValidLayout( final_layout->second.state ))
				{
					dst_state.final = final_layout->second.state;
				}
				else
				{
					// find last layout
					for (auto& sp : Reverse(compat._subpasses))
					{
						auto	l_it = layouts.find( sp.name );
						if ( l_it != layouts.end() and IsValidLayout( l_it->second.state ))
						{
							dst_state.final = l_it->second.state;
							break;
						}
					}
				}

				if ( attach_spec->second->storeOp == EAttachmentStoreOp::Invalidate )
					dst_state.final |= EResourceState::Invalidate;
			}
		}

		return true;
	}

/*
=================================================
	_ValidateSubpasses
=================================================
*/
	bool  SerializableMtlRenderPass::_ValidateSubpasses (const CompatibleRenderPassDesc &compat, const RenderPassSpec &spec)
	{
		Unused( compat, spec );
		// TODO
		return true;
	}

#endif // AE_BUILD_PIPELINE_COMPILER

/*
=================================================
	ToString
=================================================
*/
#ifdef AE_TEST_PIPELINE_COMPILER
	String  SerializableMtlRenderPass::ToString (const HashToName &nameMap) const
	{
		String	str;
		str << "\nname              = " << (_name.IsDefined() ? ("'" + nameMap( _name ) + "'") : "--")		// for compatible RP name is not defined
			<< "\ncolorAttachments  = {";

		usize	col_count = 0;
		for (usize i = 0; i < _mtlAtt.size(); ++i)
		{
			const auto&	att = _mtlAtt[i];

			if ( not att.IsDefined() or NoBits( att.flags, MtlAttachmentFlags::Color ))
			{
				col_count = i;
				break;
			}

			if ( i > 0 )
				str << "\n  --------";

			str << "\n  [" << Base::ToString(i) << "].format         = " << Base::ToString( att.format )
				<< "\n  [" << Base::ToString(i) << "].loadAction     = " << Base::ToString( att.loadAction )
				<< "\n  [" << Base::ToString(i) << "].storeAction    = " << Base::ToString( att.storeAction )
				<< "\n  [" << Base::ToString(i) << "].storeActionOpt = " << Base::ToString( att.storeActionOpt );
				//<< "\n  [" << Base::ToString(i) << "].flags          = " << Base::ToString( att.flags );
		}
		str << "\n}";

		for (usize i = 0; i < _mtlAtt.size(); ++i)
		{
			const auto&	att = _mtlAtt[i];

			if ( AllBits( att.flags, MtlAttachmentFlags::Depth ))
			{
				str << "\ndepthAttachment = {"
					<< "\n  format         = " << Base::ToString( att.format )
					<< "\n  loadAction     = " << Base::ToString( att.loadAction )
					<< "\n  storeAction    = " << Base::ToString( att.storeAction )
					<< "\n  storeActionOpt = " << Base::ToString( att.storeActionOpt )
					//<< "\n  flags          = " << Base::ToString( att.flags )
					<< "\n  resolveFilter  = " << Base::ToString( _depthResolve )
					<< "\n}";

				CHECK( i <= col_count );
				break;
			}
		}

		for (usize i = 0; i < _mtlAtt.size(); ++i)
		{
			const auto&	att = _mtlAtt[i];

			if ( AllBits( att.flags, MtlAttachmentFlags::Stencil ))
			{
				str << "\nstencilAttachment = {"
					<< "\n  format         = " << Base::ToString( att.format )
					<< "\n  loadAction     = " << Base::ToString( att.loadAction )
					<< "\n  storeAction    = " << Base::ToString( att.storeAction )
					<< "\n  storeActionOpt = " << Base::ToString( att.storeActionOpt )
					//<< "\n  flags          = " << Base::ToString( att.flags )
					<< "\n  resolveFilter  = " << Base::ToString( _stencilResolve )
					<< "\n}";

				CHECK( i <= col_count );
				break;
			}
		}

		str << "\n=========================================================================\n\n";
		return str;
	}
#endif

/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableMtlRenderPass::Serialize (Serializing::Serializer &ser) C_NE___
	{
		bool	result = true;

		result &= ser( _name, _mtlAtt );
		result &= ser( _depthResolve, _stencilResolve );

		return result;
	}

/*
=================================================
	Deserialize
=================================================
*/
	bool  SerializableMtlRenderPass::Deserialize (Serializing::Deserializer &des) __NE___
	{
		return Deserialize( Default, des );
	}

	bool  SerializableMtlRenderPass::Deserialize (EPixelFormat surfaceFormat, Serializing::Deserializer &des) __NE___
	{
		const MtlPixelFormat	mtl_swfmt	= (surfaceFormat != Default ? MEnumCast( surfaceFormat ) : MtlPixelFormat::SwapchainColor);
		bool					result		= true;

		result &= des( OUT _name, OUT _mtlAtt );
		result &= des( OUT _depthResolve, OUT _stencilResolve );

		for (auto& att : _mtlAtt)
		{
			if ( att.format == MtlPixelFormat::SwapchainColor )
				att.format = mtl_swfmt;
		}

		return result;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
#ifdef AE_BUILD_PIPELINE_COMPILER
	bool  RenderPassPacker::Serialize (Serializing::Serializer &ser) __NE___
	{
		auto&		storage			= *ObjectStorage::Instance();
		const auto&	compatible_rp	= storage.compatibleRPs;
		CHECK_ERR( not compatible_rp.empty() );
		CHECK_ERR( compatible_rp.size() <= SerializableRenderPassInfo::MaxCompatCount );

		usize	rp_spec_count = 0;

		// serialize
		CHECK_ERR( ser( RenderPassPack_Name ));
		CHECK_ERR( ser( RenderPassPack_Version ));
		CHECK_ERR( ser( uint(compatible_rp.size()) ));

		for (auto& [crp_name, compat] : compatible_rp)
		{
			ASSERT( compat.UseCount() == 1 );

			CHECK_ERR( not compat->_specializations.empty() );
			rp_spec_count += compat->_specializations.size();

			SerializableRenderPassInfo	info;
			CHECK_ERR( info.Create( *compat ));
			CHECK_ERR( info.Serialize( ser ));

			switch_enum( storage.target )
			{
			  #ifdef AE_ENABLE_VULKAN
				case ECompilationTarget::Vulkan :
				{
					CHECK_ERR( not compat->_specializations.empty() );
					CHECK_ERR( compat->_specializations.size() <= SerializableRenderPassInfo::MaxSpecCount );

					CHECK_ERR( ser( RenderPassPack_VkRpBlock, uint(compat->_specializations.size()) ));

					Array<SerializableVkRenderPass>	vk_rpasses;
					NOTHROW_ERR( vk_rpasses.reserve( compat->_specializations.size() + 1 ));

					{
						SerializableVkRenderPass&	vk_rp = vk_rpasses.emplace_back();
						CHECK_ERR( vk_rp.Create( *compat ));
					}
					for (auto& [rp_name, rp] : compat->_specializations)
					{
						SerializableVkRenderPass&	vk_rp = vk_rpasses.emplace_back();
						CHECK_ERR( vk_rp.Create( *compat, *rp ));
					}

					CHECK_ERR( SerializableVkRenderPass::MakeCompatible( INOUT vk_rpasses ));
					for (auto& rp : vk_rpasses) {
						CHECK_ERR( rp.Serialize( ser ));
					}
					break;
				}
			  #endif

				case ECompilationTarget::Metal_iOS :
				case ECompilationTarget::Metal_Mac :
				{
					CHECK_ERR( not compat->_specializations.empty() );
					CHECK_ERR( compat->_specializations.size() <= SerializableRenderPassInfo::MaxSpecCount );

					CHECK_ERR( ser( RenderPassPack_MtlRpBlock, uint(compat->_specializations.size()) ));
					{
						SerializableMtlRenderPass	mtl_rp;
						CHECK_ERR( mtl_rp.Create( *compat ));
						CHECK_ERR( mtl_rp.Serialize( ser ));
					}
					for (auto& [rp_name, rp] : compat->_specializations)
					{
						SerializableMtlRenderPass	mtl_rp;
						CHECK_ERR( mtl_rp.Create( *compat, *rp ));
						CHECK_ERR( mtl_rp.Serialize( ser ));
					}
					break;
				}

				case ECompilationTarget::Unknown :
				case ECompilationTarget::_Count :
			  #ifndef AE_ENABLE_VULKAN
				case ECompilationTarget::Vulkan :
			  #endif
				default :
					RETURN_ERR( "unknown compilation target" );
			}
			switch_end
		}

		Unused( rp_spec_count );
		AE_LOG_DBG( "Serialized compatible render passes: "s << ToString(compatible_rp.size()) <<
					", unique render passes: " << ToString(rp_spec_count) );
		return true;
	}

#endif

} // AE::PipelineCompiler
