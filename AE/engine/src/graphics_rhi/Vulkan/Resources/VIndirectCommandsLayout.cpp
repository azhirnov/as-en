// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Private/ResourceValidation.h"
# include "graphics_rhi/Vulkan/Resources/VIndirectCommandsLayout.h"
# include "graphics_rhi/Vulkan/VEnumCast.h"
# include "graphics_rhi/Vulkan/VResourceManager.h"

namespace AE::Graphics
{

/*
=================================================
	destructor
=================================================
*/
	VIndirectCommandsLayout::~VIndirectCommandsLayout () __NE___
	{
		ASSERT( _handle == Default );
	}

/*
=================================================
	Create
=================================================
*/
	bool  VIndirectCommandsLayout::Create (ResourceManager &resMngr, const IndirectCommandsLayoutDesc &desc, StringView dbgName) __NE___
	{
		CHECK_ERR( _handle == Default );
		CHECK_ERR( not desc.tokens.empty() );
		CHECK_ERR( desc.stages != Default );
		CHECK_ERR( desc.sequenceStride > 0 );

		using ETokenType	= IndirectCommandsLayoutDesc::ETokenType;
		using VTokenList	= TypeList< VkIndirectCommandsExecutionSetTokenEXT, VkIndirectCommandsPushConstantTokenEXT,
										VkIndirectCommandsIndexBufferTokenEXT, VkIndirectCommandsVertexBufferTokenEXT >;

		constexpr Bytes	max_token_size	= Bytes{VTokenList::ForEach_Max< TypeListUtils::GetTypeSize >()};
		constexpr Bytes	max_token_align	= Bytes{VTokenList::ForEach_Max< TypeListUtils::GetTypeAlign >()};
		constexpr Bytes	max_align		= Max( max_token_align, AlignOf<VkIndirectCommandsLayoutTokenEXT> );

		const Bytes		max_size		= SizeOf<VkIndirectCommandsLayoutTokenEXT> * desc.tokens.size() +
										  max_token_size * desc.tokens.size();

		auto&	dev			= resMngr.GetDevice();
		auto&	icb_props	= dev.GetDeviceProperties().icb;
		auto&	limits		= dev.GetVProperties().properties.limits;
		auto&	fs			= resMngr.GetFeatureSet();
		Unused( fs, icb_props );

		DynUntypedStorage	storage;
		CHECK_ERR( storage.Alloc( SizeAndAlign{max_size, max_align}, null ));

		MemWriter	writer		{storage.Ptr(), storage.Size()};
		auto*		vk_tokens	= writer.ReserveArray<VkIndirectCommandsLayoutTokenEXT>( desc.tokens.size() );
		CHECK_ERR( vk_tokens != null );

		StaticArray<uint, uint(ETokenType::_Count)>	token_count = {};
		Bytes										prev_offset;
		VPipelineLayout const*						ppln_layout = resMngr.GetResource( desc.pipelineLayout, False{"don't inc ref"}, True{"quiet"} );

		GRES_CHECK( desc.tokens.size() <= icb_props.maxIndirectCommandsTokenCount );

		for (usize i : IndicesOnly(desc.tokens))
		{
			auto&	src = desc.tokens[i];
			auto&	dst = vk_tokens[i];
			dst.sType	= VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_TOKEN_EXT;
			dst.pNext	= null;
			dst.data	= {};
			dst.offset	= uint{src.offset};

			GRES_CHECK( src.offset >= prev_offset );
			GRES_CHECK( src.offset <= icb_props.maxIndirectCommandsTokenOffset );
			prev_offset = src.offset;

			switch_enum( src.type )
			{
				case ETokenType::ExecutionSet :
				{
					auto&	es = writer.Emplace<VkIndirectCommandsExecutionSetTokenEXT>();

					es.type			= VK_INDIRECT_EXECUTION_SET_INFO_TYPE_PIPELINES_EXT;
					es.shaderStages	= VEnumCast( src.data.executionSet.stages );

					dst.type				= VK_INDIRECT_COMMANDS_TOKEN_TYPE_EXECUTION_SET_EXT;
					dst.data.pExecutionSet	= &es;

					GRES_CHECK_MSG( i == 0, "execution set token must be the first token in array" );
					break;
				}
				case ETokenType::PushConstant :
				{
					auto&	pc = writer.Emplace<VkIndirectCommandsPushConstantTokenEXT>();

					pc.updateRange.stageFlags	= VEnumCast( src.data.pushConstant.stages );
					pc.updateRange.offset		= uint{src.data.pushConstant.offset};
					pc.updateRange.size			= uint{src.data.pushConstant.size};

					dst.type				= VK_INDIRECT_COMMANDS_TOKEN_TYPE_PUSH_CONSTANT_EXT;
					dst.data.pPushConstant	= &pc;

					GRES_CHECK( ppln_layout != null );
					GRES_CHECK( pc.updateRange.size <= limits.maxPushConstantsSize );
					break;
				}
				case ETokenType::SequenceIndex :
				{
					auto&	pc = writer.Emplace<VkIndirectCommandsPushConstantTokenEXT>();

					pc.updateRange.stageFlags	= VEnumCast( src.data.sequenceIndex.stages );
					pc.updateRange.offset		= uint{src.data.sequenceIndex.offset};
					pc.updateRange.size			= 4;

					dst.type				= VK_INDIRECT_COMMANDS_TOKEN_TYPE_SEQUENCE_INDEX_EXT;
					dst.data.pPushConstant	= &pc;

					GRES_CHECK( ppln_layout != null );
					break;
				}
				case ETokenType::IndexBuffer :
				{
					auto&	ib	= writer.Emplace<VkIndirectCommandsIndexBufferTokenEXT >();

					ib.mode = VEnumCast( src.data.indexBuffer.mode );

					dst.type				= VK_INDIRECT_COMMANDS_TOKEN_TYPE_INDEX_BUFFER_EXT;
					dst.data.pIndexBuffer	= &ib;
					break;
				}
				case ETokenType::VertexBuffer :
				{
					auto&	vb	= writer.Emplace<VkIndirectCommandsVertexBufferTokenEXT >();

					vb.vertexBindingUnit	= src.data.vertexBuffer.vertexBindingUnit;

					dst.type				= VK_INDIRECT_COMMANDS_TOKEN_TYPE_VERTEX_BUFFER_EXT;
					dst.data.pVertexBuffer	= &vb;
					break;
				}

				// 'data' is not used
				case ETokenType::Draw :					dst.type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_EXT;					break;
				case ETokenType::DrawCount :			dst.type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_COUNT_EXT;				break;
				case ETokenType::DrawIndexed :			dst.type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_INDEXED_EXT;			break;
				case ETokenType::DrawIndexedCount :		dst.type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_INDEXED_COUNT_EXT;		break;
				case ETokenType::DrawMeshTasks :		dst.type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_MESH_TASKS_EXT;			break;
				case ETokenType::DrawMeshTasksCount :	dst.type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_DRAW_MESH_TASKS_COUNT_EXT;	break;
				case ETokenType::Dispatch :				dst.type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_DISPATCH_EXT;				break;
				case ETokenType::TraceRays :			dst.type = VK_INDIRECT_COMMANDS_TOKEN_TYPE_TRACE_RAYS2_EXT;				break;

				case ETokenType::Unknown :
				case ETokenType::_Count :
				default :
					RETURN_ERR( "unknown token type" );
			}
			switch_end

			// validation
			if ( src.type >= ETokenType::Draw and src.type <= ETokenType::TraceRays )
			{
				GRES_CHECK_MSG( i == desc.tokens.size()-1, "action command token must be the last token in array" );

				if ( src.type >= ETokenType::Draw and src.type <= ETokenType::DrawMeshTasksCount )
				{
					GRES_CHECK( AnyBits( desc.stages, EShaderStages::GraphicsPipeStages ));
					GRES_CHECK( NoBits( desc.stages, ~EShaderStages::GraphicsPipeStages ));
				}

				if ( AnyEqual( src.type, ETokenType::DrawMeshTasks, ETokenType::DrawMeshTasksCount ))
				{
					GRES_CHECK( AllBits( desc.stages, EShaderStages::Mesh | EShaderStages::Fragment ));
					GRES_CHECK( NoBits( desc.stages, ~EShaderStages::MeshPipeStages ));
				}

				if ( src.type == ETokenType::Dispatch )
					GRES_CHECK( desc.stages == EShaderStages::Compute );

				if ( src.type == ETokenType::TraceRays )
				{
					GRES_CHECK( AnyBits( desc.stages, EShaderStages::AllRayTracing ));
					GRES_CHECK( NoBits( desc.stages, ~EShaderStages::AllRayTracing ));
				}
			}

			if ( AnyEqual( src.type, ETokenType::DrawCount, ETokenType::DrawIndexedCount, ETokenType::DrawMeshTasksCount ))
				GRES_CHECK( fs.deviceGeneratedCommandsMultiDrawIndirectCount == FeatureSet::EFeature::RequireTrue );

			if ( AnyEqual( src.type, ETokenType::DrawMeshTasks, ETokenType::DrawMeshTasksCount ))
				GRES_CHECK( fs.meshShader == FeatureSet::EFeature::RequireTrue );

			if ( src.type == ETokenType::TraceRays )
				GRES_CHECK( fs.rayTracingPipeline == FeatureSet::EFeature::RequireTrue );

			++token_count[uint(src.type)];
		}

		// validation
		{
			GRES_CHECK( token_count[uint(ETokenType::ExecutionSet)] <= 1 );
			GRES_CHECK( token_count[uint(ETokenType::SequenceIndex)] <= 1 );
			GRES_CHECK( token_count[uint(ETokenType::IndexBuffer)] <= 1 );
		}

		VkIndirectCommandsLayoutCreateInfoEXT	info = {};
		info.sType			= VK_STRUCTURE_TYPE_INDIRECT_COMMANDS_LAYOUT_CREATE_INFO_EXT;
		info.flags			= VEnumCast( desc.usage );
		info.shaderStages	= VEnumCast( desc.stages );
		info.indirectStride	= uint{desc.sequenceStride};
		info.pipelineLayout	= ppln_layout ? ppln_layout->Handle() : Default;
		info.tokenCount		= uint(desc.tokens.size());
		info.pTokens		= vk_tokens;

		VK_CHECK_ERR( dev.vkCreateIndirectCommandsLayoutEXT( dev.GetVkDevice(), &info, null, OUT &_handle ));

		_usage	= desc.usage;
		_stages	= desc.stages;

		dev.SetObjectName( _handle, dbgName, VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_EXT );

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  VIndirectCommandsLayout::Destroy (ResourceManager &resMngr) __NE___
	{
		auto&	dev = resMngr.GetDevice();

		dev.vkDestroyIndirectCommandsLayoutEXT( dev.GetVkDevice(), _handle, null );

		_handle	= Default;
		_usage	= Default;
		_stages	= Default;

		GFX_DBG_ONLY( _debugName.clear() );
	}

} // AE::Graphics
#endif // AE_ENABLE_VULKAN
