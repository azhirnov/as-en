// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TestDevice.h"

/*
=================================================
	CompileShaders
=================================================
*/
static bool CompileShaders (TestDevice &vulkan, OUT VkShaderModule &vertShader, OUT VkShaderModule &tessContShader, OUT VkShaderModule &tessEvalShader, OUT VkShaderModule &fragShader)
{
	// create vertex shader
	{
		static const char	vert_shader_source[] = R"#(
const vec2	g_Positions[] = {
	{-1.0f, -1.0f}, {-1.0f, 2.0f}, {2.0f, -1.0f}
};

layout(location=0) out float  out_Level;

void main()
{
	gl_Position = vec4( g_Positions[gl_VertexIndex], float(gl_VertexIndex) * 0.01f, 1.0f );
	out_Level = float(gl_VertexIndex) * 2.5f;
})#";

		CHECK_ERR( vulkan.Compile( OUT vertShader, {vert_shader_source}, EShLangVertex ));
	}

	// create tessellation control shader
	{
		static const char	cont_shader_source[] = R"#(
layout(vertices = 3) out;

layout(location=0) in  float  in_Level[];
layout(location=0) out float  out_Level[];

void dbg_EnableTraceRecording (bool b) {}

void main ()
{
#	define I	gl_InvocationID

	dbg_EnableTraceRecording( I == 2 && gl_PrimitiveID == 0 );

	if ( I == 0 ) {
		gl_TessLevelInner[0] = max( max(in_Level[0], in_Level[1]), in_Level[2] );
		gl_TessLevelOuter[0] = max( in_Level[1], in_Level[2] );
		gl_TessLevelOuter[1] = max( in_Level[0], in_Level[1] );
		gl_TessLevelOuter[2] = max( in_Level[0], in_Level[2] );
	}
	gl_out[I].gl_Position = gl_in[I].gl_Position;
	out_Level[I] = in_Level[I];
})#";

		CHECK_ERR( vulkan.Compile( OUT tessContShader, {cont_shader_source}, EShLangTessControl, ETraceMode::DebugTrace, 0 ));
	}

	// create fragment shader
	{
		static const char	eval_shader_source[] = R"#(
layout(triangles, equal_spacing, ccw) in;

layout(location=0) in  float  in_Level[];
layout(location=0) out vec4   out_Color;

#define Interpolate( _arr, _field ) \
	( gl_TessCoord.x * _arr[0] _field + \
	  gl_TessCoord.y * _arr[1] _field + \
	  gl_TessCoord.z * _arr[2] _field )

void main ()
{
	vec4	pos = Interpolate( gl_in, .gl_Position );
	gl_Position = pos;
	out_Color   = vec4( in_Level[0], in_Level[1], in_Level[2], 1.0f );
})#";

		CHECK_ERR( vulkan.Compile( OUT tessEvalShader, {eval_shader_source}, EShLangTessEvaluation ));
	}

	// create fragment shader
	{
		static const char	frag_shader_source[] = R"#(
layout(location = 0) in  vec4  in_Color;
layout(location = 0) out vec4  out_Color;

void main ()
{
	out_Color = in_Color;
})#";

		CHECK_ERR( vulkan.Compile( OUT fragShader, {frag_shader_source}, EShLangFragment ));
	}
	return true;
}

/*
=================================================
	ShaderTrace_Test7
=================================================
*/
extern bool ShaderTrace_Test7 (TestDevice& vulkan)
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
	VkShaderModule	vert_shader, cont_shader, eval_shader, frag_shader;
	CHECK_ERR( CompileShaders( vulkan, OUT vert_shader, OUT cont_shader, OUT eval_shader, OUT frag_shader ));

	VkDescriptorSetLayout	ds_layout;
	VkDescriptorSet			desc_set;
	CHECK_ERR( vulkan.CreateDebugDescriptorSet( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, OUT ds_layout, OUT desc_set ));

	VkPipelineLayout	ppln_layout;
	VkPipeline			pipeline;
	CHECK_ERR( vulkan.CreateGraphicsPipelineVar2( vert_shader, cont_shader, eval_shader, frag_shader, ds_layout, render_pass, 3, OUT ppln_layout, OUT pipeline ));


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

		vulkan.vkCmdPipelineBarrier( vulkan.cmdBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, 0,
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

	vulkan.vkCmdDraw( vulkan.cmdBuffer, 3, 1, 0, 0 );

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

		vulkan.vkCmdPipelineBarrier( vulkan.cmdBuffer, VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
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

	CHECK_ERR( vulkan.TestDebugTraceOutput( {cont_shader}, "ShaderTrace_Test7.txt" ));

	TEST_PASSED();
	return true;
}
