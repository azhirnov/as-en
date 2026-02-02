// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TestDevice.h"

/*
=================================================
	CompileShaders
=================================================
*/
static bool CompileShaders (TestDevice &vulkan, OUT VkShaderModule &compShader)
{
	// create compute shader
	{
		static const char	comp_shader_source[] = R"#(
//#extension GL_KHR_shader_subgroup_basic : require

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

//layout(std430, binding=0) coherent buffer dbg_ShaderTraceStorage
//{
//	uint	data[];
//} dbg_ShaderTrace;

void dbg_Assert (uint bitIndex)
{
//	if ( subgroupElect() )
//	{
//		uint  arr_idx = bitIndex / 32;
//		uint  bit = 1u << (bitIndex & 31);
//		atomicOr( dbg_ShaderTrace.data[arr_idx], bit );
//	}
}

void OtherFunction() {}

#define ASSERT( _expr_ )	\
	if ( !(_expr_) ){		\
		dbg_Assert(0);		\
	}

void main()
{
	OtherFunction();
	ASSERT( gl_GlobalInvocationID.x != 0 ); // 0
	ASSERT( false ); // 1
	ASSERT( true );  // 2
	ASSERT( false ); // 3
	ASSERT( false ); // 4
	ASSERT( false ); // 5
	ASSERT( true );  // 6
	ASSERT( false ); // 7
	ASSERT( false ); // 8
	ASSERT( false ); // 9
	ASSERT( true );  // 10
	ASSERT( false ); // 11
	ASSERT( false ); // 12
	ASSERT( false ); // 13
	ASSERT( false ); // 14
	ASSERT( false ); // 15
	ASSERT( true );  // 16
	ASSERT( false ); // 17
	ASSERT( false ); // 18
	ASSERT( false ); // 19
	ASSERT( false ); // 20
	ASSERT( true );  // 21
	ASSERT( false ); // 22
	ASSERT( false ); // 23
	ASSERT( false ); // 24
	ASSERT( false ); // 25
	ASSERT( true );  // 26
	ASSERT( false ); // 27
	ASSERT( false ); // 28
	ASSERT( false ); // 29
	ASSERT( true );  // 30
	ASSERT( false ); // 31
	ASSERT( false ); // 32
	ASSERT( true );  // 33
	ASSERT( false ); // 34
}
)#";
		CHECK_ERR( vulkan.Compile( OUT compShader, {comp_shader_source}, EShLangCompute, ETraceMode::Asserts, 0 ));
	}
	return true;
}

/*
=================================================
	CreatePipeline
=================================================
*/
static bool CreatePipeline (TestDevice &vulkan, VkShaderModule compShader, Array<VkDescriptorSetLayout> dsLayouts,
							OUT VkPipelineLayout &outPipelineLayout, OUT VkPipeline &outPipeline)
{
	// create pipeline layout
	{
		VkPipelineLayoutCreateInfo	info = {};
		info.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.setLayoutCount			= uint(dsLayouts.size());
		info.pSetLayouts			= dsLayouts.data();
		info.pushConstantRangeCount	= 0;
		info.pPushConstantRanges	= null;

		VK_CHECK_ERR( vulkan.vkCreatePipelineLayout( vulkan.GetVkDevice(), &info, null, OUT &outPipelineLayout ));
		vulkan.tempHandles.emplace_back( TestDevice::EHandleType::PipelineLayout, ulong(outPipelineLayout) );
	}

	VkComputePipelineCreateInfo		info = {};
	info.sType			= VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	info.stage.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	info.stage.stage	= VK_SHADER_STAGE_COMPUTE_BIT;
	info.stage.module	= compShader;
	info.stage.pName	= "main";
	info.layout			= outPipelineLayout;

	VK_CHECK_ERR( vulkan.vkCreateComputePipelines( vulkan.GetVkDevice(), Default, 1, &info, null, OUT &outPipeline ));
	vulkan.tempHandles.emplace_back( TestDevice::EHandleType::Pipeline, ulong(outPipeline) );

	return true;
}

/*
=================================================
	ShaderAssert_Test1
=================================================
*/
extern bool ShaderAssert_Test1 (TestDevice& vulkan)
{
	ON_DESTROY( [&vulkan]() { vulkan.FreeTempHandles(); });

	// create pipeline
	VkShaderModule	comp_shader;
	CHECK_ERR( CompileShaders( vulkan, OUT comp_shader ));

	VkDescriptorSetLayout	ds2_layout;
	VkDescriptorSet			desc_set2;
	CHECK_ERR( vulkan.CreateDebugDescriptorSet( VK_SHADER_STAGE_COMPUTE_BIT, OUT ds2_layout, OUT desc_set2 ));

	VkPipelineLayout	ppln_layout;
	VkPipeline			pipeline;
	CHECK_ERR( CreatePipeline( vulkan, comp_shader, {ds2_layout}, OUT ppln_layout, OUT pipeline ));


	// build command buffer
	VkCommandBufferBeginInfo	begin = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, null, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, null };
	VK_CHECK_ERR( vulkan.vkBeginCommandBuffer( vulkan.cmdBuffer, &begin ));

	// setup storage buffer
	{
		vulkan.vkCmdFillBuffer( vulkan.cmdBuffer, vulkan.debugOutputBuf, 0, VK_WHOLE_SIZE, 0 );
	}

	// debug output storage read/write after write
	{
		VkBufferMemoryBarrier	barrier = {};
		barrier.sType			= VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.srcAccessMask	= VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask	= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		barrier.buffer			= vulkan.debugOutputBuf;
		barrier.offset			= 0;
		barrier.size			= VK_WHOLE_SIZE;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		vulkan.vkCmdPipelineBarrier( vulkan.cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 0,
									 0, null, 1, &barrier, 0, null);
	}

	// dispatch
	{
		vulkan.vkCmdBindPipeline( vulkan.cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline );
		vulkan.vkCmdBindDescriptorSets( vulkan.cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ppln_layout, 0, 1, &desc_set2, 0, null );

		vulkan.vkCmdDispatch( vulkan.cmdBuffer, 16, 16, 1 );
	}

	// debug output storage read after write
	{
		VkBufferMemoryBarrier	barrier = {};
		barrier.sType			= VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.srcAccessMask	= VK_ACCESS_SHADER_WRITE_BIT;
		barrier.dstAccessMask	= VK_ACCESS_TRANSFER_READ_BIT;
		barrier.buffer			= vulkan.debugOutputBuf;
		barrier.offset			= 0;
		barrier.size			= VK_WHOLE_SIZE;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		vulkan.vkCmdPipelineBarrier( vulkan.cmdBuffer, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
									 0, null, 1, &barrier, 0, null);
	}

	// copy shader debug output into host visible memory
	{
		VkBufferCopy	region = {};
		region.srcOffset	= 0;
		region.dstOffset	= 0;
		region.size			= vulkan.debugOutputSize;

		vulkan.vkCmdCopyBuffer( vulkan.cmdBuffer, vulkan.debugOutputBuf, vulkan.readBackBuf, 1, &region );
	}

	VK_CHECK_ERR( vulkan.vkEndCommandBuffer( vulkan.cmdBuffer ));

	// submit commands and wait
	{
		VkSubmitInfo	submit = {};
		submit.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit.commandBufferCount	= 1;
		submit.pCommandBuffers		= &vulkan.cmdBuffer;

		VK_CHECK_ERR( vulkan.vkQueueSubmit( vulkan.GetVkQueue(), 1, &submit, Default ));
		VK_CHECK_ERR( vulkan.vkQueueWaitIdle( vulkan.GetVkQueue() ));
	}

	CHECK_ERR( vulkan.TestDebugTraceOutput( {comp_shader}, "ShaderAssert_Test1.txt", ETraceMode::Asserts ));

	TEST_PASSED();
	return true;
}
