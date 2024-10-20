// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TestDevice.h"

/*
=================================================
	CompileShaders
=================================================
*/
static bool CompileShaders (TestDevice &vulkan, OUT VkShaderModule &vertShader, OUT VkShaderModule &fragShader)
{
	// create vertex shader
	{
		static const char	vert_shader_source[] = R"#(
const vec2	g_Positions[] = {
	{-1.0f, -1.0f}, {-1.0f, 2.0f}, {2.0f, -1.0f},	// primitive 0 - must hit
	{-1.0f,  2.0f},									// primitive 1 - miss
	{-2.0f,  0.0f}									// primitive 2 - must hit
};

layout(location = 0) out vec4  out_Position;

layout(location=2) out VertOutput {
	vec2	out_Texcoord;
	vec4	out_Color;
};

void main()
{
	out_Position = vec4( g_Positions[gl_VertexIndex], float(gl_VertexIndex) * 0.01f, 1.0f );
	gl_Position = out_Position;
	out_Texcoord = g_Positions[gl_VertexIndex].xy * 0.5f + 0.5f;
	out_Color = mix(vec4(1.0, 0.3, 0.0, 0.8), vec4(0.6, 0.9, 0.1, 1.0), float(gl_VertexIndex) / float(g_Positions.length()));
})#";

		CHECK_ERR( vulkan.Compile( OUT vertShader, {vert_shader_source}, EShLangVertex ));
	}

	// create fragment shader
	{
		static const char	frag_shader_source[] = R"#(
layout(location = 0) out vec4  out_Color;

layout(location = 0) in vec4  in_Position;

layout(location=2) in VertOutput {
	vec2	in_Texcoord;
	vec4	in_Color;
};

float Fn2 (int i, float x)
{
	return sin( x ) * float(i);
}

float Fn1 (const int i, in vec2 k)
{
	float f = 0.0f;
	for (int j = 0; j < 10; ++j)
	{
		f += cos( 1.5432f * float(j) ) * Fn2( j, k[j&1] );

		if ( j+i == 12 )
			return f + 12.0f;
	}
	return f;
}

void main ()
{
	float c = Fn1( 3, in_Texcoord.xy + in_Position.yx );
	out_Color = vec4(c);
})#";

		CHECK_ERR( vulkan.Compile( OUT fragShader, {frag_shader_source}, EShLangFragment, ETraceMode::Performance, 0 ));
	}
	return true;
}

/*
=================================================
	ShaderPerf_Test1
=================================================
*/
extern bool ShaderPerf_Test1 (TestDevice& vulkan)
{
	ON_DESTROY( [&vulkan]() { vulkan.FreeTempHandles(); });

	// create renderpass and framebuffer
	uint			width = 16, height = 16;
	VkRenderPass	render_pass;
	VkImage			color_target;
	VkFramebuffer	framebuffer;
	CHECK_ERR( vulkan.CreateRenderTarget( VK_FORMAT_R8G8B8A8_UNORM, width, height, 0,
										  OUT render_pass, OUT color_target, OUT framebuffer ));


	// create pipeline
	VkShaderModule	vert_shader, frag_shader;
	CHECK_ERR( CompileShaders( vulkan, OUT vert_shader, OUT frag_shader ));

	VkDescriptorSetLayout	ds_layout;
	VkDescriptorSet			desc_set;
	CHECK_ERR( vulkan.CreateDebugDescriptorSet( VK_SHADER_STAGE_FRAGMENT_BIT, OUT ds_layout, OUT desc_set ));

	VkPipelineLayout	ppln_layout;
	VkPipeline			pipeline;
	CHECK_ERR( vulkan.CreateGraphicsPipelineVar1( vert_shader, frag_shader, ds_layout, render_pass, OUT ppln_layout, OUT pipeline ));


	// build command buffer
	VkCommandBufferBeginInfo	begin = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, null, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, null };
	VK_CHECK_ERR( vulkan.vkBeginCommandBuffer( vulkan.cmdBuffer, &begin ));

	// image layout undefined -> color_attachment
	{
		VkImageMemoryBarrier	barrier = {};
		barrier.sType			= VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.srcAccessMask	= 0;
		barrier.dstAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.oldLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrier.image			= color_target;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange	= {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

		vulkan.vkCmdPipelineBarrier( vulkan.cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
									 0, null, 0, null, 1, &barrier);
	}

	// setup storage buffer
	{
		const uint	data[] = { width/2, height/2 };		// selected pixel

		vulkan.vkCmdFillBuffer( vulkan.cmdBuffer, vulkan.debugOutputBuf, sizeof(data), VK_WHOLE_SIZE, 0 );
		vulkan.vkCmdUpdateBuffer( vulkan.cmdBuffer, vulkan.debugOutputBuf, 0, sizeof(data), data );
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

		vulkan.vkCmdPipelineBarrier( vulkan.cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
									 0, null, 1, &barrier, 0, null);
	}

	// begin render pass
	{
		VkClearValue			clear_value = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};
		VkRenderPassBeginInfo	begin_rp	= {};
		begin_rp.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		begin_rp.framebuffer		= framebuffer;
		begin_rp.renderPass			= render_pass;
		begin_rp.renderArea			= { {0,0}, {width, height} };
		begin_rp.clearValueCount	= 1;
		begin_rp.pClearValues		= &clear_value;

		vulkan.vkCmdBeginRenderPass( vulkan.cmdBuffer, &begin_rp, VK_SUBPASS_CONTENTS_INLINE );
	}

	vulkan.vkCmdBindPipeline( vulkan.cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline );
	vulkan.vkCmdBindDescriptorSets( vulkan.cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, ppln_layout, 0, 1, &desc_set, 0, null );

	// set dynamic states
	{
		VkViewport	viewport = {};
		viewport.x			= 0.0f;
		viewport.y			= 0.0f;
		viewport.width		= float(width);
		viewport.height		= float(height);
		viewport.minDepth	= 0.0f;
		viewport.maxDepth	= 1.0f;
		vulkan.vkCmdSetViewport( vulkan.cmdBuffer, 0, 1, &viewport );

		VkRect2D	scissor_rect = { {0,0}, {width, height} };
		vulkan.vkCmdSetScissor( vulkan.cmdBuffer, 0, 1, &scissor_rect );
	}

	vulkan.vkCmdDraw( vulkan.cmdBuffer, 5, 1, 0, 0 );

	vulkan.vkCmdEndRenderPass( vulkan.cmdBuffer );

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

		vulkan.vkCmdPipelineBarrier( vulkan.cmdBuffer, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
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

	CHECK_ERR( vulkan.TestPerformanceOutput(
					{frag_shader}, {
					"float Fn2 (int i, float x)"s,
					"float Fn1 (const int i, in vec2 k)"s,
					"void main ()"s
					}));

	TEST_PASSED();
	return true;
}
