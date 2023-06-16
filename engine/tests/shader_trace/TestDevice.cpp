// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "TestDevice.h"
#include "base/Algorithms/StringParser.h"
#include "base/DataSource/MemStream.h"
#include "base/DataSource/FileStream.h"
#include "serializing/ObjectFactory.h"
#include "res_loaders/DDS/DDSSaver.h"

#ifdef AE_COMPILER_MSVC
#	pragma warning (push, 0)
#	pragma warning (disable: 4005)
#	pragma warning (disable: 4668)
#endif
#ifdef AE_COMPILER_CLANG
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wdouble-promotion"
#endif
#ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wundef"
#	pragma GCC diagnostic ignored "-Wdouble-promotion"
#endif

// glslang includes
#include "glslang/MachineIndependent/localintermediate.h"
#include "glslang/Include/intermediate.h"
#include "glslang/SPIRV/doc.h"
#include "glslang/SPIRV/disassemble.h"
#include "glslang/SPIRV/GlslangToSpv.h"
#include "glslang/SPIRV/GLSL.std.450.h"

// spirv cross
#ifdef AE_ENABLE_SPIRV_CROSS
#	include "spirv_cross/spirv_cross.hpp"
#	include "spirv_cross/spirv_glsl.hpp"
#endif

#ifdef ENABLE_OPT
#	include "spirv-tools/optimizer.hpp"
#	include "spirv-tools/libspirv.h"
#endif

#ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
#endif
#ifdef AE_COMPILER_CLANG
#	pragma clang diagnostic pop
#endif
#ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic pop
#endif

// resources are removed in new version of glslang, so define it locally.
#include "ResourceLimits.h"

namespace AE::PipelineCompiler
{

// Warning:
// Before testing on new GPU set 'UpdateReferences' to 'true', run tests,
// using git compare new references with origin, only float values may differ slightly.
// Then set 'UpdateReferences' to 'false' and run tests again.
// All tests must pass.
static const bool	UpdateReferences = true;


/*
=================================================
	Create
=================================================
*/
bool  TestDevice::Create ()
{
	VulkanDeviceFn_Init( _vulkan );

	glslang::InitializeProcess();
	_tempBuf.reserve( 1024 );
	
	VDeviceInitializer::InstanceCreateInfo	inst_ci;
	inst_ci.appName			= "GLSLTrace";
	inst_ci.instanceLayers	= _vulkan.GetRecomendedInstanceLayers();

	CHECK_ERR( _vulkan.CreateInstance( inst_ci ));
	
	_vulkan.CreateDebugCallback( DefaultDebugMessageSeverity,
								 [] (const VDeviceInitializer::DebugReport &rep) { AE_LOG_SE(rep.message);  CHECK(not rep.isError); });

	CHECK_ERR( _vulkan.ChooseHighPerformanceDevice() );
	CHECK_ERR( _vulkan.CreateDefaultQueue() );
	CHECK_ERR( _vulkan.CreateLogicalDevice() );

	CHECK_ERR( _CreateResources() );
	return true;
}

/*
=================================================
	Destroy
=================================================
*/
void  TestDevice::Destroy ()
{
	_DestroyResources();
	_vulkan.DestroyLogicalDevice();
	_vulkan.DestroyInstance();

	for (auto& [module, trace] : _debuggableShaders) {
		delete trace;
	}
	_debuggableShaders.clear();

	glslang::FinalizeProcess();
}

/*
=================================================
	_CreateResources
=================================================
*/
bool  TestDevice::_CreateResources ()
{
	// create command pool
	{
		VkCommandPoolCreateInfo		info = {};
		info.sType				= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		info.flags				= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		info.queueFamilyIndex	= GetQueueFamily();

		VK_CHECK_ERR( vkCreateCommandPool( GetVkDevice(), &info, null, OUT &cmdPool ));

		VkCommandBufferAllocateInfo		alloc = {};
		alloc.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc.commandPool			= cmdPool;
		alloc.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc.commandBufferCount	= 1;

		VK_CHECK_ERR( vkAllocateCommandBuffers( GetVkDevice(), &alloc, OUT &cmdBuffer ));
	}

	// create descriptor pool
	{
		VkDescriptorPoolSize	sizes[] = {
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
			{ VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 100 }
		};
		
		if ( GetRayTracingFeats().rayTracingPipeline == VK_FALSE )
		{
			// if ray-tracing is not supported then change descriptor type for something else
			sizes[3].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		}

		VkDescriptorPoolCreateInfo		info = {};
		info.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		info.maxSets		= 100;
		info.poolSizeCount	= uint(CountOf( sizes ));
		info.pPoolSizes		= sizes;

		VK_CHECK_ERR( vkCreateDescriptorPool( GetVkDevice(), &info, null, OUT &descPool ));
	}
	
	// debug output buffer
	{
		debugOutputSize = Min( debugOutputSize, GetDeviceProps().limits.maxStorageBufferRange );
		AE_LOGI( "Shader debug output storage buffer size: "s << ToString(debugOutputSize) );

		VkBufferCreateInfo	info = {};
		info.sType			= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.flags			= 0;
		info.size			= debugOutputSize;
		info.usage			= VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		info.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK_ERR( vkCreateBuffer( GetVkDevice(), &info, null, OUT &debugOutputBuf ));
		
		VkMemoryRequirements	mem_req;
		vkGetBufferMemoryRequirements( GetVkDevice(), debugOutputBuf, OUT &mem_req );
		
		// allocate GetVkDevice() local memory
		VkMemoryAllocateInfo	alloc_info = {};
		alloc_info.sType			= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize	= mem_req.size;
		CHECK_ERR( GetMemoryTypeIndex( mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, OUT alloc_info.memoryTypeIndex ));

		VK_CHECK_ERR( vkAllocateMemory( GetVkDevice(), &alloc_info, null, OUT &debugOutputMem ));
		VK_CHECK_ERR( vkBindBufferMemory( GetVkDevice(), debugOutputBuf, debugOutputMem, 0 ));
	}

	// debug output read back buffer
	{
		VkBufferCreateInfo	info = {};
		info.sType			= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.flags			= 0;
		info.size			= VkDeviceSize(debugOutputSize);
		info.usage			= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		info.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK_ERR( vkCreateBuffer( GetVkDevice(), &info, null, OUT &readBackBuf ));
		
		VkMemoryRequirements	mem_req;
		vkGetBufferMemoryRequirements( GetVkDevice(), readBackBuf, OUT &mem_req );
		
		// allocate host visible memory
		VkMemoryAllocateInfo	alloc_info = {};
		alloc_info.sType			= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize	= mem_req.size;
		CHECK_ERR( GetMemoryTypeIndex( mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
									   OUT alloc_info.memoryTypeIndex ));

		VK_CHECK_ERR( vkAllocateMemory( GetVkDevice(), &alloc_info, null, OUT &readBackMem ));
		VK_CHECK_ERR( vkMapMemory( GetVkDevice(), readBackMem, 0, info.size, 0, OUT &readBackPtr ));

		VK_CHECK_ERR( vkBindBufferMemory( GetVkDevice(), readBackBuf, readBackMem, 0 ));
	}
	return true;
}

/*
=================================================
	_DestroyResources
=================================================
*/
void  TestDevice::_DestroyResources ()
{
	if ( cmdPool )
	{
		vkDestroyCommandPool( GetVkDevice(), cmdPool, null );
		cmdPool		= Default;
		cmdBuffer	= Default;
	}

	if ( descPool )
	{
		vkDestroyDescriptorPool( GetVkDevice(), descPool, null );
		descPool = Default;
	}

	if ( debugOutputBuf )
	{
		vkDestroyBuffer( GetVkDevice(), debugOutputBuf, null );
		debugOutputBuf = Default;
	}

	if ( readBackBuf )
	{
		vkDestroyBuffer( GetVkDevice(), readBackBuf, null );
		readBackBuf = Default;
	}

	if ( debugOutputMem )
	{
		vkFreeMemory( GetVkDevice(), debugOutputMem, null );
		debugOutputMem = Default;
	}

	if ( readBackMem )
	{
		vkFreeMemory( GetVkDevice(), readBackMem, null );
		readBackMem = Default;
	}
}

/*
=================================================
	GetMemoryTypeIndex
=================================================
*/
bool  TestDevice::GetMemoryTypeIndex (uint memoryTypeBits, VkMemoryPropertyFlags flags, OUT uint &memoryTypeIndex) const
{
	memoryTypeIndex = ~0u;
	
	auto&	mem_props = _vulkan.GetProperties().memoryProperties;

	for (uint i = 0; i < mem_props.memoryTypeCount; ++i)
	{
		const auto&		mem_type = mem_props.memoryTypes[i];

		if ( ((memoryTypeBits >> i) & 1) == 1			and
			 (mem_type.propertyFlags & flags) == flags )
		{
			memoryTypeIndex = i;
			return true;
		}
	}
	return false;
}

/*
=================================================
	Compile
=================================================
*/
bool  TestDevice::Compile  (OUT VkShaderModule &		shaderModule,
							Array<const char *>			source,
							EShLanguage					shaderType,
							ETraceMode					mode,
							uint						dbgBufferSetIndex)
{
	const bool				debuggable	= dbgBufferSetIndex != ~0u;
	Unique<ShaderTrace>		debug_info	{ debuggable ? new ShaderTrace{} : null };
	const String			header		= "#version 460 core\n"
										  "#extension GL_ARB_separate_shader_objects : require\n"
										  "#extension GL_ARB_shading_language_420pack : require\n";
	
	source.insert( source.begin(), header.data() );
	
	EShTargetLanguageVersion	spv_version = EShTargetSpv_1_3;
	if ( shaderType >= EShLangRayGen )
		spv_version = EShTargetSpv_1_4;

	if ( not _Compile( OUT _tempBuf, OUT debug_info.get(), dbgBufferSetIndex, source, shaderType, mode, spv_version ))
		return false;

	VkShaderModuleCreateInfo	info = {};
	info.sType		= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.flags		= 0;
	info.pCode		= _tempBuf.data();
	info.codeSize	= sizeof(_tempBuf[0]) * _tempBuf.size();

	VK_CHECK_ERR( vkCreateShaderModule( GetVkDevice(), &info, null, OUT &shaderModule ));
	tempHandles.emplace_back( EHandleType::Shader, ulong(shaderModule) );

	if ( debuggable )
	{
		// test serialization
		#if 0
		{
			auto	wfile = MakeRC<MemWStream>();
			{
				Serializing::Serializer	ser{ wfile };
				CHECK_ERR( debug_info->Serialize( ser ));
			}
		
			Unique<ShaderTrace>	debug_info2	{new ShaderTrace{}};

			auto	rfile = MakeRC<MemRefRStream>( wfile->GetData() );
			{
				Serializing::Deserializer	des{ rfile };
				CHECK_ERR( debug_info2->Deserialize( des ));
			}

			CHECK( *debug_info == *debug_info2 );

			debug_info = RVRef(debug_info2);
		}
		#endif

		_debuggableShaders.insert_or_assign( shaderModule, debug_info.release() );
	}
	return true;
}

/*
=================================================
	_Compile
=================================================
*/
bool  TestDevice::_Compile (OUT Array<uint>&			spirvData,
							OUT ShaderTrace*			dbgInfo,
							uint						dbgBufferSetIndex,
							Array<const char *>			source,
							EShLanguage					shaderType,
							ETraceMode					mode,
							EShTargetLanguageVersion	spvVersion)
{
	EShMessages				messages		= EShMsgDefault;
	TProgram				program;
	TShader					shader			{ shaderType };
	EshTargetClientVersion	client_version	= EShTargetVulkan_1_1;
	TBuiltInResource		builtin_res		= DefaultTBuiltInResource;

	shader.setStrings( source.data(), int(source.size()) );
	shader.setEntryPoint( "main" );
	shader.setEnvInput( EShSourceGlsl, shaderType, EShClientVulkan, 110 );
	shader.setEnvClient( EShClientVulkan, client_version );
	shader.setEnvTarget( EshTargetSpv, spvVersion );

	if ( not shader.parse( &builtin_res, 460, ECoreProfile, false, true, messages ))
	{
		AE_LOGI( shader.getInfoLog() );
		return false;
	}
		
	program.addShader( &shader );

	if ( not program.link( messages ))
	{
		AE_LOGI( program.getInfoLog() );
		return false;
	}

	TIntermediate*	intermediate = program.getIntermediate( shader.getStage() );
	if ( not intermediate )
		return false;

	if ( dbgInfo )
	{
		BEGIN_ENUM_CHECKS();
		switch ( mode )
		{
			case ETraceMode::DebugTrace :
				CHECK_ERR( dbgInfo->InsertTraceRecording( INOUT *intermediate, dbgBufferSetIndex ));
				break;

			case ETraceMode::Performance :
				CHECK_ERR( dbgInfo->InsertFunctionProfiler( INOUT *intermediate, dbgBufferSetIndex,
															GetShaderClockFeats().shaderSubgroupClock,
															GetShaderClockFeats().shaderDeviceClock ));
				break;
				
			case ETraceMode::TimeMap :
				CHECK_ERR( GetShaderClockFeats().shaderDeviceClock );
				CHECK_ERR( dbgInfo->InsertShaderClockHeatmap( INOUT *intermediate, dbgBufferSetIndex ));
				break;

			case ETraceMode::None :
			default :
				RETURN_ERR( "unknown shader trace mode" );
		}
		END_ENUM_CHECKS();
	
		for (auto* src : source) {
			dbgInfo->AddSource( StringView{src} );
		}
	}

	SpvOptions				spv_options;
	spv::SpvBuildLogger		logger;

	spv_options.generateDebugInfo	= false;
	spv_options.disableOptimizer	= true;
	spv_options.optimizeSize		= false;
	spv_options.validate			= true;
		
	spirvData.clear();
	GlslangToSpv( *intermediate, OUT spirvData, &logger, &spv_options );

	//AE_LOGI( logger.getAllMessages() );
	CHECK_ERR( spirvData.size() );
	
	// for debugging
	#if 0 //def AE_ENABLE_SPIRV_CROSS
	//if ( logger.getAllMessages().size() )
	{
		spirv_cross::CompilerGLSL			compiler {spirvData.data(), spirvData.size()};
		spirv_cross::CompilerGLSL::Options	opt = {};

		opt.version						= 460;
		opt.es							= false;
		opt.vulkan_semantics			= true;
		opt.separate_shader_objects		= true;
		opt.enable_420pack_extension	= true;

		opt.vertex.fixup_clipspace		= false;
		opt.vertex.flip_vert_y			= false;
		opt.vertex.support_nonzero_base_instance = true;

		opt.fragment.default_float_precision	= spirv_cross::CompilerGLSL::Options::Precision::Highp;
		opt.fragment.default_int_precision		= spirv_cross::CompilerGLSL::Options::Precision::Highp;

		compiler.set_common_options( opt );

		String	glsl_src = compiler.compile();	// throw
		AE_LOGI( glsl_src );
	}
	#endif

	// disassembly
	#if 0 //defined(ENABLE_OPT) 
	{
		spv_context	ctx = spvContextCreate( SPV_ENV_VULKAN_1_1 );
		CHECK_ERR( ctx != null );

		spv_text		text		= null;
		spv_diagnostic	diagnostic	= null;

		if ( spvBinaryToText( ctx, spirvData.data(), spirvData.size(), 0, &text, &diagnostic ) == SPV_SUCCESS )
		{
			AE_LOGI( String{ text->str, text->length });
		}
		
		spvTextDestroy( text );
		spvDiagnosticDestroy( diagnostic );
		spvContextDestroy( ctx );
	}
	#endif
	return true;
}

/*
=================================================
	_GetDebugOutput
=================================================
*/
bool  TestDevice::_GetDebugOutput (VkShaderModule shaderModule, const void *ptr, VkDeviceSize maxSize, OUT Array<String> &result) const
{
	auto	iter = _debuggableShaders.find( shaderModule );
	CHECK_ERR( iter != _debuggableShaders.end() );

	return iter->second->ParseShaderTrace( ptr, Bytes{maxSize}, ShaderTrace::ELogFormat::Text, OUT result );
}

/*
=================================================
	CreateDebugDescSetLayout
=================================================
*/
bool  TestDevice::CreateDebugDescriptorSet (VkShaderStageFlags stages, OUT VkDescriptorSetLayout &dsLayout, OUT VkDescriptorSet &descSet)
{
	// create layout
	{
		VkDescriptorSetLayoutBinding	binding = {};
		binding.binding			= 0;
		binding.descriptorType	= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		binding.descriptorCount	= 1;
		binding.stageFlags		= stages;

		VkDescriptorSetLayoutCreateInfo		info = {};
		info.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		info.bindingCount	= 1;
		info.pBindings		= &binding;

		VK_CHECK_ERR( vkCreateDescriptorSetLayout( GetVkDevice(), &info, null, OUT &dsLayout ));
		tempHandles.emplace_back( EHandleType::DescriptorSetLayout, ulong(dsLayout) );
	}
	
	// allocate descriptor set
	{
		VkDescriptorSetAllocateInfo		info = {};
		info.sType				= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		info.descriptorPool		= descPool;
		info.descriptorSetCount	= 1;
		info.pSetLayouts		= &dsLayout;

		VK_CHECK_ERR( vkAllocateDescriptorSets( GetVkDevice(), &info, OUT &descSet ));
	}

	// update descriptor set
	{
		VkDescriptorBufferInfo	buffer	= {};
		buffer.buffer	= debugOutputBuf;
		buffer.range	= VK_WHOLE_SIZE;
		
		VkWriteDescriptorSet	write	= {};
		write.sType				= VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet			= descSet;
		write.dstBinding		= 0;
		write.descriptorCount	= 1;
		write.descriptorType	= VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		write.pBufferInfo		= &buffer;
		
		vkUpdateDescriptorSets( GetVkDevice(), 1, &write, 0, null );
	}
	return true;
}

/*
=================================================
	CreateStorageImage
=================================================
*/
bool  TestDevice::CreateStorageImage (VkFormat format, uint width, uint height, VkImageUsageFlags imageUsage,
									  OUT VkImage &outImage, OUT VkImageView &outView)
{
	VkImageCreateInfo	image_ci = {};
	image_ci.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_ci.flags			= 0;
	image_ci.imageType		= VK_IMAGE_TYPE_2D;
	image_ci.format			= format;
	image_ci.extent			= { width, height, 1 };
	image_ci.mipLevels		= 1;
	image_ci.arrayLayers	= 1;
	image_ci.samples		= VK_SAMPLE_COUNT_1_BIT;
	image_ci.tiling			= VK_IMAGE_TILING_OPTIMAL;
	image_ci.usage			= imageUsage | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	image_ci.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
	image_ci.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;

	VK_CHECK_ERR( vkCreateImage( GetVkDevice(), &image_ci, null, OUT &outImage ));
	tempHandles.emplace_back( EHandleType::Image, ulong(outImage) );

	VkMemoryRequirements	mem_req;
	vkGetImageMemoryRequirements( GetVkDevice(), outImage, OUT &mem_req );
		
	// allocate GetVkDevice() local memory
	VkMemoryAllocateInfo	alloc_info = {};
	alloc_info.sType			= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloc_info.allocationSize	= mem_req.size;
	CHECK_ERR( GetMemoryTypeIndex( mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, OUT alloc_info.memoryTypeIndex ));
		
	VkDeviceMemory	image_mem;
	VK_CHECK_ERR( vkAllocateMemory( GetVkDevice(), &alloc_info, null, OUT &image_mem ));
	tempHandles.emplace_back( EHandleType::Memory, ulong(image_mem) );

	VK_CHECK_ERR( vkBindImageMemory( GetVkDevice(), outImage, image_mem, 0 ));
	
	VkImageViewCreateInfo	view_ci = {};
	view_ci.sType				= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_ci.flags				= 0;
	view_ci.image				= outImage;
	view_ci.viewType			= VK_IMAGE_VIEW_TYPE_2D;
	view_ci.format				= format;
	view_ci.components			= { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
	view_ci.subresourceRange	= { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	VK_CHECK_ERR( vkCreateImageView( GetVkDevice(), &view_ci, null, OUT &outView ));
	tempHandles.emplace_back( EHandleType::ImageView, ulong(outView) );
	
	return true;
}

/*
=================================================
	CreateRenderTarget
=================================================
*/
bool  TestDevice::CreateRenderTarget (VkFormat colorFormat, uint width, uint height, VkImageUsageFlags imageUsage,
									  OUT VkRenderPass &outRenderPass, OUT VkImage &outImage,
									  OUT VkFramebuffer &outFramebuffer)
{
	// create image
	{
		VkImageCreateInfo	info = {};
		info.sType			= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.flags			= 0;
		info.imageType		= VK_IMAGE_TYPE_2D;
		info.format			= colorFormat;
		info.extent			= { width, height, 1 };
		info.mipLevels		= 1;
		info.arrayLayers	= 1;
		info.samples		= VK_SAMPLE_COUNT_1_BIT;
		info.tiling			= VK_IMAGE_TILING_OPTIMAL;
		info.usage			= imageUsage | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		info.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
		info.initialLayout	= VK_IMAGE_LAYOUT_UNDEFINED;

		VK_CHECK_ERR( vkCreateImage( GetVkDevice(), &info, null, OUT &outImage ));
		tempHandles.emplace_back( EHandleType::Image, ulong(outImage) );

		VkMemoryRequirements	mem_req;
		vkGetImageMemoryRequirements( GetVkDevice(), outImage, OUT &mem_req );
		
		// allocate GetVkDevice() local memory
		VkMemoryAllocateInfo	alloc_info = {};
		alloc_info.sType			= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize	= mem_req.size;
		CHECK_ERR( GetMemoryTypeIndex( mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, OUT alloc_info.memoryTypeIndex ));

		VkDeviceMemory	image_mem;
		VK_CHECK_ERR( vkAllocateMemory( GetVkDevice(), &alloc_info, null, OUT &image_mem ));
		tempHandles.emplace_back( EHandleType::Memory, ulong(image_mem) );

		VK_CHECK_ERR( vkBindImageMemory( GetVkDevice(), outImage, image_mem, 0 ));
	}

	// create image view
	VkImageView		view = Default;
	{
		VkImageViewCreateInfo	info = {};
		info.sType				= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		info.flags				= 0;
		info.image				= outImage;
		info.viewType			= VK_IMAGE_VIEW_TYPE_2D;
		info.format				= colorFormat;
		info.components			= { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };
		info.subresourceRange	= { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

		VK_CHECK_ERR( vkCreateImageView( GetVkDevice(), &info, null, OUT &view ));
		tempHandles.emplace_back( EHandleType::ImageView, ulong(view) );
	}

	// create renderpass
	{
		// setup attachment
		VkAttachmentDescription		attachments[1] = {};

		attachments[0].format			= colorFormat;
		attachments[0].samples			= VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout	= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments[0].finalLayout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


		// setup subpasses
		VkSubpassDescription	subpasses[1]		= {};
		VkAttachmentReference	attachment_ref[1]	= {};

		attachment_ref[0] = { 0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };

		subpasses[0].pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpasses[0].colorAttachmentCount	= 1;
		subpasses[0].pColorAttachments		= &attachment_ref[0];


		// setup dependencies
		VkSubpassDependency		dependencies[2] = {};

		dependencies[0].srcSubpass		= VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass		= 0;
		dependencies[0].srcStageMask	= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask	= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask	= 0;
		dependencies[0].dstAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags	= VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass		= 0;
		dependencies[1].dstSubpass		= VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask	= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask	= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask	= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask	= 0;
		dependencies[1].dependencyFlags	= VK_DEPENDENCY_BY_REGION_BIT;


		// setup create info
		VkRenderPassCreateInfo	info = {};
		info.sType				= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.flags				= 0;
		info.attachmentCount	= uint(CountOf( attachments ));
		info.pAttachments		= attachments;
		info.subpassCount		= uint(CountOf( subpasses ));
		info.pSubpasses			= subpasses;
		info.dependencyCount	= uint(CountOf( dependencies ));
		info.pDependencies		= dependencies;

		VK_CHECK_ERR( vkCreateRenderPass( GetVkDevice(), &info, null, OUT &outRenderPass ));
		tempHandles.emplace_back( EHandleType::RenderPass, ulong(outRenderPass) );
	}
	
	// create framebuffer
	{
		VkFramebufferCreateInfo		info = {};
		info.sType				= VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.flags				= 0;
		info.renderPass			= outRenderPass;
		info.attachmentCount	= 1;
		info.pAttachments		= &view;
		info.width				= width;
		info.height				= height;
		info.layers				= 1;

		VK_CHECK_ERR( vkCreateFramebuffer( GetVkDevice(), &info, null, OUT &outFramebuffer ));
		tempHandles.emplace_back( EHandleType::Framebuffer, ulong(outFramebuffer) );
	}
	return true;
}

/*
=================================================
	CreateGraphicsPipelineVar1
=================================================
*/
bool  TestDevice::CreateGraphicsPipelineVar1 (VkShaderModule vertShader, VkShaderModule fragShader,
											  VkDescriptorSetLayout dsLayout, VkRenderPass renderPass,
											  OUT VkPipelineLayout &outPipelineLayout, OUT VkPipeline &outPipeline)
{
	// create pipeline layout
	{
		VkPipelineLayoutCreateInfo	info = {};
		info.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.setLayoutCount			= 1;
		info.pSetLayouts			= &dsLayout;
		info.pushConstantRangeCount	= 0;
		info.pPushConstantRanges	= null;

		VK_CHECK_ERR( vkCreatePipelineLayout( GetVkDevice(), &info, null, OUT &outPipelineLayout ));
		tempHandles.emplace_back( EHandleType::PipelineLayout, ulong(outPipelineLayout) );
	}

	VkPipelineShaderStageCreateInfo			stages[2] = {};
	stages[0].sType		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[0].stage		= VK_SHADER_STAGE_VERTEX_BIT;
	stages[0].module	= vertShader;
	stages[0].pName		= "main";
	stages[1].sType		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[1].stage		= VK_SHADER_STAGE_FRAGMENT_BIT;
	stages[1].module	= fragShader;
	stages[1].pName		= "main";

	VkPipelineVertexInputStateCreateInfo	vertex_input = {};
	vertex_input.sType		= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	
	VkPipelineInputAssemblyStateCreateInfo	input_assembly = {};
	input_assembly.sType	= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology	= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

	VkPipelineViewportStateCreateInfo		viewport = {};
	viewport.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport.viewportCount	= 1;
	viewport.scissorCount	= 1;

	VkPipelineRasterizationStateCreateInfo	rasterization = {};
	rasterization.sType			= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization.polygonMode	= VK_POLYGON_MODE_FILL;
	rasterization.cullMode		= VK_CULL_MODE_NONE;
	rasterization.frontFace		= VK_FRONT_FACE_COUNTER_CLOCKWISE;

	VkPipelineMultisampleStateCreateInfo	multisample = {};
	multisample.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo	depth_stencil = {};
	depth_stencil.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil.depthTestEnable		= VK_FALSE;
	depth_stencil.depthWriteEnable		= VK_FALSE;
	depth_stencil.depthCompareOp		= VK_COMPARE_OP_LESS_OR_EQUAL;
	depth_stencil.depthBoundsTestEnable	= VK_FALSE;
	depth_stencil.stencilTestEnable		= VK_FALSE;

	VkPipelineColorBlendAttachmentState		blend_attachment = {};
	blend_attachment.blendEnable		= VK_FALSE;
	blend_attachment.colorWriteMask		= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo		blend_state = {};
	blend_state.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blend_state.logicOpEnable		= VK_FALSE;
	blend_state.attachmentCount		= 1;
	blend_state.pAttachments		= &blend_attachment;

	VkDynamicState							dynamic_states[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo		dynamic_state = {};
	dynamic_state.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount	= uint(CountOf( dynamic_states ));
	dynamic_state.pDynamicStates	= dynamic_states;

	// create pipeline
	VkGraphicsPipelineCreateInfo	info = {};
	info.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info.stageCount				= uint(CountOf( stages ));
	info.pStages				= stages;
	info.pViewportState			= &viewport;
	info.pVertexInputState		= &vertex_input;
	info.pInputAssemblyState	= &input_assembly;
	info.pRasterizationState	= &rasterization;
	info.pMultisampleState		= &multisample;
	info.pDepthStencilState		= &depth_stencil;
	info.pColorBlendState		= &blend_state;
	info.pDynamicState			= &dynamic_state;
	info.layout					= outPipelineLayout;
	info.renderPass				= renderPass;
	info.subpass				= 0;

	VK_CHECK_ERR( vkCreateGraphicsPipelines( GetVkDevice(), Default, 1, &info, null, OUT &outPipeline ));
	tempHandles.emplace_back( EHandleType::Pipeline, ulong(outPipeline) );

	return true;
}

/*
=================================================
	CreateMeshPipelineVar1
=================================================
*/
bool  TestDevice::CreateGraphicsPipelineVar2 (VkShaderModule vertShader, VkShaderModule tessContShader, VkShaderModule tessEvalShader,
											  VkShaderModule fragShader, VkDescriptorSetLayout dsLayout, VkRenderPass renderPass, uint patchSize,
											  OUT VkPipelineLayout &outPipelineLayout, OUT VkPipeline &outPipeline)
{
	// create pipeline layout
	{
		VkPipelineLayoutCreateInfo	info = {};
		info.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.setLayoutCount			= 1;
		info.pSetLayouts			= &dsLayout;
		info.pushConstantRangeCount	= 0;
		info.pPushConstantRanges	= null;

		VK_CHECK_ERR( vkCreatePipelineLayout( GetVkDevice(), &info, null, OUT &outPipelineLayout ));
		tempHandles.emplace_back( EHandleType::PipelineLayout, ulong(outPipelineLayout) );
	}

	VkPipelineShaderStageCreateInfo			stages[4] = {};
	stages[0].sType		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[0].stage		= VK_SHADER_STAGE_VERTEX_BIT;
	stages[0].module	= vertShader;
	stages[0].pName		= "main";
	stages[1].sType		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[1].stage		= VK_SHADER_STAGE_FRAGMENT_BIT;
	stages[1].module	= fragShader;
	stages[1].pName		= "main";
	stages[2].sType		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[2].stage		= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	stages[2].module	= tessContShader;
	stages[2].pName		= "main";
	stages[3].sType		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[3].stage		= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	stages[3].module	= tessEvalShader;
	stages[3].pName		= "main";

	VkPipelineVertexInputStateCreateInfo	vertex_input = {};
	vertex_input.sType		= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	
	VkPipelineInputAssemblyStateCreateInfo	input_assembly = {};
	input_assembly.sType	= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology	= VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

	VkPipelineViewportStateCreateInfo		viewport = {};
	viewport.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport.viewportCount	= 1;
	viewport.scissorCount	= 1;

	VkPipelineRasterizationStateCreateInfo	rasterization = {};
	rasterization.sType			= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization.polygonMode	= VK_POLYGON_MODE_FILL;
	rasterization.cullMode		= VK_CULL_MODE_NONE;
	rasterization.frontFace		= VK_FRONT_FACE_COUNTER_CLOCKWISE;

	VkPipelineMultisampleStateCreateInfo	multisample = {};
	multisample.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo	depth_stencil = {};
	depth_stencil.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil.depthTestEnable		= VK_FALSE;
	depth_stencil.depthWriteEnable		= VK_FALSE;
	depth_stencil.depthCompareOp		= VK_COMPARE_OP_LESS_OR_EQUAL;
	depth_stencil.depthBoundsTestEnable	= VK_FALSE;
	depth_stencil.stencilTestEnable		= VK_FALSE;

	VkPipelineColorBlendAttachmentState		blend_attachment = {};
	blend_attachment.blendEnable		= VK_FALSE;
	blend_attachment.colorWriteMask		= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo		blend_state = {};
	blend_state.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blend_state.logicOpEnable		= VK_FALSE;
	blend_state.attachmentCount		= 1;
	blend_state.pAttachments		= &blend_attachment;

	VkDynamicState							dynamic_states[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo		dynamic_state = {};
	dynamic_state.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount	= uint(CountOf( dynamic_states ));
	dynamic_state.pDynamicStates	= dynamic_states;

	VkPipelineTessellationStateCreateInfo	tess_state = {};
	tess_state.sType				= VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	tess_state.patchControlPoints	= patchSize;

	// create pipeline
	VkGraphicsPipelineCreateInfo	info = {};
	info.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info.stageCount				= uint(CountOf( stages ));
	info.pStages				= stages;
	info.pViewportState			= &viewport;
	info.pVertexInputState		= &vertex_input;
	info.pInputAssemblyState	= &input_assembly;
	info.pTessellationState		= &tess_state;
	info.pRasterizationState	= &rasterization;
	info.pMultisampleState		= &multisample;
	info.pDepthStencilState		= &depth_stencil;
	info.pColorBlendState		= &blend_state;
	info.pDynamicState			= &dynamic_state;
	info.layout					= outPipelineLayout;
	info.renderPass				= renderPass;
	info.subpass				= 0;

	VK_CHECK_ERR( vkCreateGraphicsPipelines( GetVkDevice(), Default, 1, &info, null, OUT &outPipeline ));
	tempHandles.emplace_back( EHandleType::Pipeline, ulong(outPipeline) );

	return true;
}

/*
=================================================
	CreateMeshPipelineVar1
=================================================
*/
bool  TestDevice::CreateMeshPipelineVar1 (VkShaderModule meshShader, VkShaderModule fragShader,
										  VkDescriptorSetLayout dsLayout, VkRenderPass renderPass,
										  OUT VkPipelineLayout &outPipelineLayout, OUT VkPipeline &outPipeline)
{
	// create pipeline layout
	{
		VkPipelineLayoutCreateInfo	info = {};
		info.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info.setLayoutCount			= 1;
		info.pSetLayouts			= &dsLayout;
		info.pushConstantRangeCount	= 0;
		info.pPushConstantRanges	= null;

		VK_CHECK_ERR( vkCreatePipelineLayout( GetVkDevice(), &info, null, OUT &outPipelineLayout ));
		tempHandles.emplace_back( EHandleType::PipelineLayout, ulong(outPipelineLayout) );
	}

	VkPipelineShaderStageCreateInfo			stages[2] = {};
	stages[0].sType		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[0].stage		= VK_SHADER_STAGE_MESH_BIT_EXT;
	stages[0].module	= meshShader;
	stages[0].pName		= "main";
	stages[1].sType		= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[1].stage		= VK_SHADER_STAGE_FRAGMENT_BIT;
	stages[1].module	= fragShader;
	stages[1].pName		= "main";

	VkPipelineVertexInputStateCreateInfo	vertex_input = {};
	vertex_input.sType		= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	
	VkPipelineInputAssemblyStateCreateInfo	input_assembly = {};
	input_assembly.sType	= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly.topology	= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineViewportStateCreateInfo		viewport = {};
	viewport.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport.viewportCount	= 1;
	viewport.scissorCount	= 1;

	VkPipelineRasterizationStateCreateInfo	rasterization = {};
	rasterization.sType			= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterization.polygonMode	= VK_POLYGON_MODE_FILL;
	rasterization.cullMode		= VK_CULL_MODE_NONE;
	rasterization.frontFace		= VK_FRONT_FACE_COUNTER_CLOCKWISE;

	VkPipelineMultisampleStateCreateInfo	multisample = {};
	multisample.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;

	VkPipelineDepthStencilStateCreateInfo	depth_stencil = {};
	depth_stencil.sType					= VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil.depthTestEnable		= VK_FALSE;
	depth_stencil.depthWriteEnable		= VK_FALSE;
	depth_stencil.depthCompareOp		= VK_COMPARE_OP_LESS_OR_EQUAL;
	depth_stencil.depthBoundsTestEnable	= VK_FALSE;
	depth_stencil.stencilTestEnable		= VK_FALSE;

	VkPipelineColorBlendAttachmentState		blend_attachment = {};
	blend_attachment.blendEnable		= VK_FALSE;
	blend_attachment.colorWriteMask		= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo		blend_state = {};
	blend_state.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blend_state.logicOpEnable		= VK_FALSE;
	blend_state.attachmentCount		= 1;
	blend_state.pAttachments		= &blend_attachment;

	VkDynamicState							dynamic_states[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
	VkPipelineDynamicStateCreateInfo		dynamic_state = {};
	dynamic_state.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamic_state.dynamicStateCount	= uint(CountOf( dynamic_states ));
	dynamic_state.pDynamicStates	= dynamic_states;

	// create pipeline
	VkGraphicsPipelineCreateInfo	info = {};
	info.sType					= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	info.stageCount				= uint(CountOf( stages ));
	info.pStages				= stages;
	info.pViewportState			= &viewport;
	info.pVertexInputState		= &vertex_input;		// ignored in mesh shader
	info.pInputAssemblyState	= &input_assembly;
	info.pRasterizationState	= &rasterization;
	info.pMultisampleState		= &multisample;
	info.pDepthStencilState		= &depth_stencil;
	info.pColorBlendState		= &blend_state;
	info.pDynamicState			= &dynamic_state;
	info.layout					= outPipelineLayout;
	info.renderPass				= renderPass;
	info.subpass				= 0;

	VK_CHECK_ERR( vkCreateGraphicsPipelines( GetVkDevice(), Default, 1, &info, null, OUT &outPipeline ));
	tempHandles.emplace_back( EHandleType::Pipeline, ulong(outPipeline) );

	return true;
}

/*
=================================================
	CreateRayTracingScene
=================================================
*/
bool  TestDevice::CreateRayTracingScene (VkPipeline rtPipeline, uint numGroups, OUT RTData &outRTData)
{
	struct MemInfo
	{
		VkDeviceSize			totalSize		= 0;
		uint					memTypeBits		= 0;
		VkMemoryPropertyFlags	memProperty		= 0;
	};

	struct ResourceInit
	{
		using BindMemCallbacks_t	= Array< Function<bool ()> >;
		using DrawCallbacks_t		= Array< Function<void (VkCommandBuffer)> >;

		MemInfo					dev;
		BindMemCallbacks_t		onBind;
		DrawCallbacks_t			onUpdate;
	};

	static const float4		vertices[] = {
		{ 0.25f, 0.25f, 0.0f, 0.0f },
		{ 0.75f, 0.25f, 0.0f, 0.0f },
		{ 0.50f, 0.75f, 0.0f, 0.0f }
	};
	static const uint		indices[] = {
		0, 1, 2
	};
	static const uint		instance_count = 1;

	VkBuffer		vertex_buffer			= Default;
	VkDeviceAddress	vertex_buffer_addr		= Default;
	VkBuffer		index_buffer			= Default;
	VkDeviceAddress	index_buffer_addr		= Default;
	VkBuffer		instance_buffer			= Default;
	VkDeviceAddress	instance_buffer_addr	= Default;
	VkBuffer		scratch_buffer			= Default;
	VkDeviceAddress	scratch_buffer_addr		= Default;
	VkBuffer		blas_buffer				= Default;
	VkDeviceAddress	blas_addr				= Default;
	VkBuffer		tlas_buffer				= Default;
	VkDeviceAddress	tlas_addr				= Default;
	VkDeviceMemory	dev_memory				= Default;

	ResourceInit	res;
	res.dev.memProperty = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	// create vertex buffer
	{
		VkBufferCreateInfo	info = {};
		info.sType			= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.flags			= 0;
		info.size			= sizeof(vertices);
		info.usage			= VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
							  VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
		info.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK_ERR( vkCreateBuffer( GetVkDevice(), &info, null, OUT &vertex_buffer ));
		
		VkBufferMemoryRequirementsInfo2	mem_info	= {};
		VkMemoryRequirements2			mem_req		= {};

		mem_req.sType	= VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		mem_info.sType	= VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
		mem_info.buffer	= vertex_buffer;

		vkGetBufferMemoryRequirements2( GetVkDevice(), &mem_info, OUT &mem_req );
		
		VkDeviceSize	offset	 = AlignUp( res.dev.totalSize, mem_req.memoryRequirements.alignment );
		res.dev.totalSize		 = offset + mem_req.memoryRequirements.size;
		res.dev.memTypeBits		|= mem_req.memoryRequirements.memoryTypeBits;

		res.onBind.push_back( [this, &dev_memory, &vertex_buffer_addr, vertex_buffer, offset] () -> bool
		{
			VK_CHECK_ERR( vkBindBufferMemory( GetVkDevice(), vertex_buffer, dev_memory, offset ));

			VkBufferDeviceAddressInfoKHR	buf_info = {};
			buf_info.sType		= VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
			buf_info.buffer		= vertex_buffer;

			vertex_buffer_addr = vkGetBufferDeviceAddressKHR( GetVkDevice(), &buf_info );
			return true;
		});

		res.onUpdate.push_back( [this, vertex_buffer] (VkCommandBuffer cmdbuf)
		{
			vkCmdUpdateBuffer( cmdbuf, vertex_buffer, 0, sizeof(vertices), vertices );
		});
	}

	// create index buffer
	{
		VkBufferCreateInfo	info = {};
		info.sType			= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.flags			= 0;
		info.size			= sizeof(indices);
		info.usage			= VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
							  VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
		info.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK_ERR( vkCreateBuffer( GetVkDevice(), &info, null, OUT &index_buffer ));
		
		VkBufferMemoryRequirementsInfo2	mem_info	= {};
		VkMemoryRequirements2			mem_req		= {};

		mem_req.sType	= VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		mem_info.sType	= VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
		mem_info.buffer	= index_buffer;

		vkGetBufferMemoryRequirements2( GetVkDevice(), &mem_info, OUT &mem_req );
		
		VkDeviceSize	offset	 = AlignUp( res.dev.totalSize, mem_req.memoryRequirements.alignment );
		res.dev.totalSize		 = offset + mem_req.memoryRequirements.size;
		res.dev.memTypeBits		|= mem_req.memoryRequirements.memoryTypeBits;

		res.onBind.push_back( [this, &dev_memory, &index_buffer_addr, index_buffer, offset] () -> bool
		{
			//std::memcpy( ptr + Bytes{offset}, indices, sizeof(indices) );		// TODO: wtf?
			VK_CHECK_ERR( vkBindBufferMemory( GetVkDevice(), index_buffer, dev_memory, offset ));
			
			VkBufferDeviceAddressInfoKHR	buf_info = {};
			buf_info.sType		= VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
			buf_info.buffer		= index_buffer;

			index_buffer_addr = vkGetBufferDeviceAddressKHR( GetVkDevice(), &buf_info );
			return true;
		});
		
		res.onUpdate.push_back( [this, index_buffer] (VkCommandBuffer cmdbuf)
		{
			vkCmdUpdateBuffer( cmdbuf, index_buffer, 0, sizeof(indices), indices );
		});
	}

	// create bottom level acceleration structure
	VkAccelerationStructureBuildSizesInfoKHR	blas_size_info = {};
	{
		VkAccelerationStructureGeometryKHR		geometry[1] = {};
		geometry[0].sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
		geometry[0].geometryType	= VK_GEOMETRY_TYPE_TRIANGLES_KHR;
		geometry[0].flags			= 0;
		geometry[0].geometry.triangles.sType		= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
		geometry[0].geometry.triangles.vertexFormat	= VK_FORMAT_R32G32B32_SFLOAT;
		geometry[0].geometry.triangles.maxVertex	= uint(CountOf( vertices ));
		geometry[0].geometry.triangles.vertexStride	= sizeof(vertices[0]);
		geometry[0].geometry.triangles.indexType	= VK_INDEX_TYPE_UINT32;
		
		// get size of acceleration structure and scratch buffer
		{
			blas_size_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
		
			VkAccelerationStructureBuildGeometryInfoKHR		build_info	= {};
			build_info.sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
			build_info.flags			= 0;
			build_info.type				= VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
			build_info.pGeometries		= geometry;
			build_info.geometryCount	= uint(CountOf( geometry ));
		
			const uint	max_primitives[1] = { uint(CountOf( indices )) / 3 };

			vkGetAccelerationStructureBuildSizesKHR( GetVkDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &build_info, max_primitives, OUT &blas_size_info );
		}

		// create buffer for acceleration structure
		{
			VkBufferCreateInfo	info = {};
			info.sType			= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			info.flags			= 0;
			info.size			= blas_size_info.accelerationStructureSize;
			info.usage			= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
			info.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
			
			VK_CHECK_ERR( vkCreateBuffer( GetVkDevice(), &info, null, OUT &blas_buffer ));
			tempHandles.emplace_back( EHandleType::Buffer, ulong(blas_buffer) );

			VkBufferMemoryRequirementsInfo2	mem_info	= {};
			VkMemoryRequirements2			mem_req		= {};

			mem_req.sType	= VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
			mem_info.sType	= VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
			mem_info.buffer	= blas_buffer;

			vkGetBufferMemoryRequirements2( GetVkDevice(), &mem_info, OUT &mem_req );
		
			VkDeviceSize	offset	 = AlignUp( res.dev.totalSize, mem_req.memoryRequirements.alignment );
			res.dev.totalSize		 = offset + mem_req.memoryRequirements.size;
			res.dev.memTypeBits		|= mem_req.memoryRequirements.memoryTypeBits;

			res.onBind.push_back( [this, &dev_memory, blas_buffer, offset] () -> bool
			{
				VK_CHECK_ERR( vkBindBufferMemory( GetVkDevice(), blas_buffer, dev_memory, offset ));
				return true;
			});
		}

		// create acceleration structure
		{
			VkAccelerationStructureCreateInfoKHR	accel_struct_ci = {};
			accel_struct_ci.sType		= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
			accel_struct_ci.createFlags	= 0;
			accel_struct_ci.buffer		= blas_buffer;
			accel_struct_ci.offset		= 0;
			accel_struct_ci.size		= blas_size_info.accelerationStructureSize;
			accel_struct_ci.type		= VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
			
			res.onBind.push_back( [this, accel_struct_ci, &outRTData, &blas_addr] () -> bool
			{
				VK_CHECK_ERR( vkCreateAccelerationStructureKHR( GetVkDevice(), &accel_struct_ci, null, OUT &outRTData.bottomLevelAS ));
				tempHandles.emplace_back( EHandleType::AccStruct, ulong(outRTData.bottomLevelAS) );

				VkAccelerationStructureDeviceAddressInfoKHR	addr_info = {};
				addr_info.sType					= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
				addr_info.accelerationStructure	= outRTData.bottomLevelAS;

				blas_addr = vkGetAccelerationStructureDeviceAddressKHR( GetVkDevice(), &addr_info );
				CHECK_ERR( blas_addr != 0 );
				return true;
			});
		}
		
		res.onUpdate.push_back( [&] (VkCommandBuffer cmd)
		{
			geometry[0].geometry.triangles.vertexData.deviceAddress	= vertex_buffer_addr;
			geometry[0].geometry.triangles.indexData.deviceAddress	= index_buffer_addr;

			VkAccelerationStructureBuildGeometryInfoKHR	info = {};
			info.sType						= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
			info.type						= VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
			info.mode						= VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
			info.srcAccelerationStructure	= Default;
			info.dstAccelerationStructure	= outRTData.bottomLevelAS;
			info.geometryCount				= uint(CountOf( geometry ));
			info.pGeometries				= geometry;
			info.scratchData.deviceAddress	= scratch_buffer_addr;

			VkAccelerationStructureBuildRangeInfoKHR		range		= {};
			VkAccelerationStructureBuildRangeInfoKHR const*	range_ptr	= &range;

			range.primitiveCount = uint(CountOf( indices )) / 3;

			vkCmdBuildAccelerationStructuresKHR( cmd, 1, &info, &range_ptr );
		});
	}
	
	// create instance buffer
	{
		VkBufferCreateInfo	info = {};
		info.sType			= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.flags			= 0;
		info.size			= sizeof(VkAccelerationStructureInstanceKHR);
		info.usage			= VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT |
							  VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
		info.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK_ERR( vkCreateBuffer( GetVkDevice(), &info, null, OUT &instance_buffer ));
		
		VkBufferMemoryRequirementsInfo2	mem_info	= {};
		VkMemoryRequirements2			mem_req		= {};

		mem_req.sType	= VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		mem_info.sType	= VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
		mem_info.buffer	= instance_buffer;

		vkGetBufferMemoryRequirements2( GetVkDevice(), &mem_info, OUT &mem_req );
		
		VkDeviceSize	offset	 = AlignUp( res.dev.totalSize, mem_req.memoryRequirements.alignment );
		res.dev.totalSize		 = offset + mem_req.memoryRequirements.size;
		res.dev.memTypeBits		|= mem_req.memoryRequirements.memoryTypeBits;

		res.onBind.push_back( [this, &dev_memory, &instance_buffer_addr, instance_buffer, offset] () -> bool
		{
			VK_CHECK_ERR( vkBindBufferMemory( GetVkDevice(), instance_buffer, dev_memory, offset ));
			
			VkBufferDeviceAddressInfoKHR	buf_info = {};
			buf_info.sType		= VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
			buf_info.buffer		= instance_buffer;

			instance_buffer_addr = vkGetBufferDeviceAddressKHR( GetVkDevice(), &buf_info );
			CHECK_ERR( instance_buffer_addr != 0 );
			return true;
		});
		
		res.onUpdate.push_back( [this, instance_buffer, &blas_addr] (VkCommandBuffer cmdbuf)
		{
			VkAccelerationStructureInstanceKHR	instance [instance_count] = {};
			instance[0].transform.matrix[0][0]		= 1.0f;
			instance[0].transform.matrix[1][1]		= 1.0f;
			instance[0].transform.matrix[2][2]		= 1.0f;
			instance[0].mask						= 0xFF;
			instance[0].flags						= 0;
			instance[0].instanceCustomIndex						= 0;
			instance[0].instanceShaderBindingTableRecordOffset	= 0;
			instance[0].accelerationStructureReference			= blas_addr;

			vkCmdUpdateBuffer( cmdbuf, instance_buffer, 0, sizeof(instance), instance );
		});
	}

	// create top level acceleration structure
	VkAccelerationStructureBuildSizesInfoKHR	tlas_size_info = {};
	{
		// get size of acceleration structure and scratch buffer
		{
			tlas_size_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

			VkAccelerationStructureGeometryKHR	instance_geom = {};
			instance_geom.sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
			instance_geom.geometryType	= VK_GEOMETRY_TYPE_INSTANCES_KHR;
			instance_geom.geometry.instances.sType				= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
			instance_geom.geometry.instances.arrayOfPointers	= VK_FALSE;

			VkAccelerationStructureBuildGeometryInfoKHR		build_info	= {};
			build_info.sType			= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
			build_info.flags			= 0;
			build_info.type				= VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
			build_info.pGeometries		= &instance_geom;
			build_info.geometryCount	= 1;

			vkGetAccelerationStructureBuildSizesKHR( GetVkDevice(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR, &build_info, &instance_count, OUT &tlas_size_info );
		}
		
		// create buffer for acceleration structure
		{
			VkBufferCreateInfo	info = {};
			info.sType			= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			info.flags			= 0;
			info.size			= tlas_size_info.accelerationStructureSize;
			info.usage			= VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
			info.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
			
			VK_CHECK_ERR( vkCreateBuffer( GetVkDevice(), &info, null, OUT &tlas_buffer ));
			tempHandles.emplace_back( EHandleType::Buffer, ulong(tlas_buffer) );

			VkBufferMemoryRequirementsInfo2	mem_info	= {};
			VkMemoryRequirements2			mem_req		= {};

			mem_req.sType	= VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
			mem_info.sType	= VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
			mem_info.buffer	= tlas_buffer;

			vkGetBufferMemoryRequirements2( GetVkDevice(), &mem_info, OUT &mem_req );
		
			VkDeviceSize	offset	 = AlignUp( res.dev.totalSize, mem_req.memoryRequirements.alignment );
			res.dev.totalSize		 = offset + mem_req.memoryRequirements.size;
			res.dev.memTypeBits		|= mem_req.memoryRequirements.memoryTypeBits;

			res.onBind.push_back( [this, &dev_memory, tlas_buffer, offset] () -> bool
			{
				VK_CHECK_ERR( vkBindBufferMemory( GetVkDevice(), tlas_buffer, dev_memory, offset ));
				return true;
			});
		}
		
		// create acceleration structure
		{
			VkAccelerationStructureCreateInfoKHR	accel_struct_ci = {};
			accel_struct_ci.sType		= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
			accel_struct_ci.createFlags	= 0;
			accel_struct_ci.buffer		= tlas_buffer;
			accel_struct_ci.offset		= 0;
			accel_struct_ci.size		= tlas_size_info.accelerationStructureSize;
			accel_struct_ci.type		= VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
			
			res.onBind.push_back( [this, accel_struct_ci, &outRTData, &tlas_addr] () -> bool
			{
				VK_CHECK_ERR( vkCreateAccelerationStructureKHR( GetVkDevice(), &accel_struct_ci, null, OUT &outRTData.topLevelAS ));
				tempHandles.emplace_back( EHandleType::AccStruct, ulong(outRTData.topLevelAS) );

				VkAccelerationStructureDeviceAddressInfoKHR	addr_info = {};
				addr_info.sType					= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
				addr_info.accelerationStructure	= outRTData.topLevelAS;

				tlas_addr = vkGetAccelerationStructureDeviceAddressKHR( GetVkDevice(), &addr_info );
				return true;
			});
		}
		
		res.onUpdate.push_back( [&] (VkCommandBuffer cmd)
		{
			VkAccelerationStructureGeometryKHR	geometry	= {};
			geometry.sType									= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
			geometry.flags									= 0;
			geometry.geometryType							= VK_GEOMETRY_TYPE_INSTANCES_KHR;
			geometry.geometry.instances.sType				= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
			geometry.geometry.instances.pNext				= null;
			geometry.geometry.instances.arrayOfPointers		= VK_FALSE;
			geometry.geometry.instances.data.deviceAddress	= instance_buffer_addr;

			VkAccelerationStructureBuildGeometryInfoKHR		info = {};
			info.sType						= VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
			info.type						= VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
			info.mode						= VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
			info.srcAccelerationStructure	= Default;
			info.dstAccelerationStructure	= outRTData.topLevelAS;
			info.geometryCount				= 1;
			info.pGeometries				= &geometry;
			info.scratchData.deviceAddress	= scratch_buffer_addr;

			VkAccelerationStructureBuildRangeInfoKHR		range		= {};
			VkAccelerationStructureBuildRangeInfoKHR const*	range_ptr	= &range;

			range.primitiveCount = 1;

			vkCmdBuildAccelerationStructuresKHR( cmd, 1, &info, &range_ptr );
		});
	}
	
	// create scratch buffer
	{
		VkBufferCreateInfo	info = {};
		info.sType			= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.flags			= 0;
		info.size			= Max( blas_size_info.buildScratchSize, tlas_size_info.buildScratchSize );
		info.usage			= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR;
		info.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK_ERR( vkCreateBuffer( GetVkDevice(), &info, null, OUT &scratch_buffer ));
		
		VkBufferMemoryRequirementsInfo2	mem_info	= {};
		VkMemoryRequirements2			mem_req		= {};

		mem_req.sType	= VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		mem_info.sType	= VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
		mem_info.buffer	= scratch_buffer;

		vkGetBufferMemoryRequirements2( GetVkDevice(), &mem_info, OUT &mem_req );
		
		VkDeviceSize	offset	 = AlignUp( res.dev.totalSize, mem_req.memoryRequirements.alignment );
		res.dev.totalSize		 = offset + mem_req.memoryRequirements.size;
		res.dev.memTypeBits		|= mem_req.memoryRequirements.memoryTypeBits;

		res.onBind.push_back( [this, &dev_memory, &scratch_buffer_addr, scratch_buffer, offset] () -> bool
		{
			VK_CHECK_ERR( vkBindBufferMemory( GetVkDevice(), scratch_buffer, dev_memory, offset ));
			
			VkBufferDeviceAddressInfoKHR	buf_info = {};
			buf_info.sType		= VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
			buf_info.buffer		= scratch_buffer;

			scratch_buffer_addr = vkGetBufferDeviceAddressKHR( GetVkDevice(), &buf_info );
			CHECK_ERR( scratch_buffer_addr != 0 );
			return true;
		});
	}

	// create shader binding table
	{
		const VkDeviceSize	alignment = Max( GetRayTracingProps().shaderGroupHandleSize, GetRayTracingProps().shaderGroupBaseAlignment );

		outRTData.shaderGroupSize	= GetRayTracingProps().shaderGroupHandleSize;
		outRTData.shaderGroupAlign	= alignment;

		VkBufferCreateInfo	info = {};
		info.sType			= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		info.flags			= 0;
		info.size			= numGroups * alignment;
		info.usage			= VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR;
		info.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK_ERR( vkCreateBuffer( GetVkDevice(), &info, null, OUT &outRTData.shaderBindingTable ));
		tempHandles.emplace_back( EHandleType::Buffer, ulong(outRTData.shaderBindingTable) );
		
		VkBufferMemoryRequirementsInfo2	mem_info	= {};
		VkMemoryRequirements2			mem_req		= {};

		mem_req.sType	= VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		mem_info.sType	= VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
		mem_info.buffer	= outRTData.shaderBindingTable;

		vkGetBufferMemoryRequirements2( GetVkDevice(), &mem_info, OUT &mem_req );
		
		VkDeviceSize	offset	 = AlignUp( res.dev.totalSize, mem_req.memoryRequirements.alignment );
		res.dev.totalSize		 = offset + mem_req.memoryRequirements.size;
		res.dev.memTypeBits		|= mem_req.memoryRequirements.memoryTypeBits;

		res.onBind.push_back( [this, &outRTData, &dev_memory, offset] () -> bool
		{
			VK_CHECK_ERR( vkBindBufferMemory( GetVkDevice(), outRTData.shaderBindingTable, dev_memory, offset ));
			
			VkBufferDeviceAddressInfoKHR	buf_info = {};
			buf_info.sType		= VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO_KHR;
			buf_info.buffer		= outRTData.shaderBindingTable;

			outRTData.sbtAddress = vkGetBufferDeviceAddressKHR( GetVkDevice(), &buf_info );
			CHECK_ERR( outRTData.sbtAddress != 0 );
			return true;
		});

		res.onUpdate.push_back( [this, sbt = outRTData.shaderBindingTable, rtPipeline, numGroups, alignment, size = info.size] (VkCommandBuffer cmd)
		{
			Array<ulong>	handles;  handles.resize( size / sizeof(ulong) );

			for (uint i = 0; i < numGroups; ++i) {
				VK_CHECK( vkGetRayTracingShaderGroupHandlesKHR( GetVkDevice(), rtPipeline, i, 1, GetRayTracingProps().shaderGroupHandleSize, OUT handles.data() + Bytes{alignment * i} ));
			}	
			vkCmdUpdateBuffer( cmd, sbt, 0, handles.size() * sizeof(handles[0]), handles.data() );
		});
	}
	
	// allocate GetVkDevice() local memory
	{
		VkMemoryAllocateInfo		mem_alloc	= {};
		VkMemoryAllocateFlagsInfo	mem_flag	= {};

		mem_flag.sType	= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
		mem_flag.flags	= VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;

		mem_alloc.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mem_alloc.pNext				= &mem_flag;
		mem_alloc.allocationSize	= res.dev.totalSize;

		CHECK_ERR( GetMemoryTypeIndex( res.dev.memTypeBits, res.dev.memProperty, OUT mem_alloc.memoryTypeIndex ));

		VK_CHECK_ERR( vkAllocateMemory( GetVkDevice(), &mem_alloc, null, OUT &dev_memory ));
		tempHandles.emplace_back( EHandleType::Memory, ulong(dev_memory) );
	}

	// bind resources
	for (auto& bind : res.onBind) {
		CHECK_ERR( bind() );
	}

	// update resources
	{
		VkCommandBufferBeginInfo	begin_info = {};
		begin_info.sType	= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags	= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_CHECK_ERR( vkBeginCommandBuffer( cmdBuffer, &begin_info ));

		for (auto& cb : res.onUpdate)
		{
			VkMemoryBarrier		barrier = {};
			barrier.sType			= VK_STRUCTURE_TYPE_MEMORY_BARRIER;
			barrier.srcAccessMask	= VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_HOST_READ_BIT;
			barrier.dstAccessMask	= VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_HOST_READ_BIT;
			
			vkCmdPipelineBarrier( cmdBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
								  0, 1, &barrier, 0, null, 0, null );

			cb( cmdBuffer );
		}

		VK_CHECK_ERR( vkEndCommandBuffer( cmdBuffer ));

		VkSubmitInfo		submit_info = {};
		submit_info.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount	= 1;
		submit_info.pCommandBuffers		= &cmdBuffer;

		VK_CHECK_ERR( vkQueueSubmit( GetVkQueue(), 1, &submit_info, Default ));
	}
	VK_CHECK_ERR( vkQueueWaitIdle( GetVkQueue() ));
	
	vkDestroyBuffer( GetVkDevice(), vertex_buffer, null );
	vkDestroyBuffer( GetVkDevice(), index_buffer, null );
	vkDestroyBuffer( GetVkDevice(), instance_buffer, null );
	vkDestroyBuffer( GetVkDevice(), scratch_buffer, null );

	return true;
}

/*
=================================================
	TestDebugTraceOutput
=================================================
*/
bool  TestDevice::TestDebugTraceOutput (Array<VkShaderModule> modules, String referenceFile)
{
	CHECK_ERR( referenceFile.size() );
	CHECK_ERR( modules.size() );

	String			merged;
	Array<String>	debug_output;

	for (auto& module : modules)
	{
		Array<String>	temp;
		CHECK_ERR( _GetDebugOutput( module, readBackPtr, debugOutputSize, OUT temp ));
		CHECK( temp.size() );
		debug_output.insert( debug_output.end(), temp.begin(), temp.end() );
	}

	std::sort( debug_output.begin(), debug_output.end() );

	for (auto& str : debug_output) {
		(merged += str) += "//---------------------------\n\n";
	}

	if ( UpdateReferences )
	{
		FileWStream		file {Path{DATA_PATH} /= referenceFile};
		CHECK_ERR( file.IsOpen() );
		CHECK_ERR( file.Write( merged ));
		return true;
	}

	String	file_data;
	{
		FileRStream		file {Path{DATA_PATH} /= referenceFile};
		CHECK_ERR( file.IsOpen() );
		CHECK_ERR( file.Read( file.RemainingSize(), OUT file_data ));
	}
	
	CHECK_ERR( StringParser::CompareLineByLine( file_data, merged,
					[referenceFile] (uint lline, StringView lstr, uint rline, StringView rstr) {
						AE_LOGE( "in: "s << referenceFile << "\n\n"
									<< "line mismatch:" << "\n(" << ToString( lline ) << "): " << lstr
									<< "\n(" << ToString( rline ) << "): " << rstr );
					},
					[referenceFile] () { AE_LOGE( "in: "s << referenceFile << "\n\n" << "sizes of dumps are not equal!" ); }
			 ));
	return true;
}

/*
=================================================
	TestPerformanceOutput
=================================================
*/
bool  TestDevice::TestPerformanceOutput (Array<VkShaderModule> modules, Array<String> fnNames)
{
	CHECK_ERR( fnNames.size() );
	CHECK_ERR( modules.size() );

	for (auto& module : modules)
	{
		Array<String>	temp;
		CHECK_ERR( _GetDebugOutput( module, readBackPtr, debugOutputSize, OUT temp ));
		CHECK_ERR( temp.size() );

		for (auto& output : temp)
		{
			for (auto& fn : fnNames)
			{
				usize	pos = output.find( fn );
				CHECK_ERR( pos != String::npos );
			}
		}
	}

	return true;
}

/*
=================================================
	CheckTimeMap
=================================================
*/
bool  TestDevice::CheckTimeMap (Array<VkShaderModule> modules, float emptyPxFactor)
{
	CHECK_ERR( modules.size() );

	for (auto& module : modules)
	{
		auto	iter = _debuggableShaders.find( module );
		CHECK_ERR( iter != _debuggableShaders.end() );
	}

	uint const*		ptr		= static_cast<uint const *>( readBackPtr );
	uint			width	= *(ptr + 2);
	uint			height	= *(ptr + 3);
	float const*	pixels	= static_cast<float const *>( readBackPtr ) + 4;
	uint			count	= 0;

	double			min_time	= 1.0e+300;
	double			max_time	= 0.0;
	double			mean_time	= 0.0;

	for (uint y = 0; y < height; ++y)
	for (uint x = 0; x < width; ++x)
	{
		double	dt = double(*(pixels + (x + y * width)));
		min_time   = Min( min_time, dt );
		max_time   = Max( max_time, dt );
		mean_time += dt;
		count     += (dt > 0.0);
	}

	mean_time /= double(width * height);

	float	time_factor		= float((mean_time - min_time) / (max_time - min_time));
	CHECK_ERR( time_factor > 0.0f );

	float	empty_px_factor	= float(count) / float(width * height);
	CHECK_ERR( empty_px_factor >= emptyPxFactor );

	return true;
}

/*
=================================================
	FreeTempHandles
=================================================
*/
void  TestDevice::FreeTempHandles ()
{
	for (auto& [type, hnd] : tempHandles)
	{
		BEGIN_ENUM_CHECKS();
		switch ( type )
		{
			case EHandleType::Memory :
				vkFreeMemory( GetVkDevice(), VkDeviceMemory(hnd), null );
				break;
			case EHandleType::Buffer :
				vkDestroyBuffer( GetVkDevice(), VkBuffer(hnd), null );
				break;
			case EHandleType::Image :
				vkDestroyImage( GetVkDevice(), VkImage(hnd), null );
				break;
			case EHandleType::ImageView :
				vkDestroyImageView( GetVkDevice(), VkImageView(hnd), null );
				break;
			case EHandleType::Pipeline :
				vkDestroyPipeline( GetVkDevice(), VkPipeline(hnd), null );
				break;
			case EHandleType::PipelineLayout :
				vkDestroyPipelineLayout( GetVkDevice(), VkPipelineLayout(hnd), null );
				break;
			case EHandleType::Shader :
				vkDestroyShaderModule( GetVkDevice(), VkShaderModule(hnd), null );
				break;
			case EHandleType::DescriptorSetLayout :
				vkDestroyDescriptorSetLayout( GetVkDevice(), VkDescriptorSetLayout(hnd), null );
				break;
			case EHandleType::RenderPass :
				vkDestroyRenderPass( GetVkDevice(), VkRenderPass(hnd), null );
				break;
			case EHandleType::Framebuffer :
				vkDestroyFramebuffer( GetVkDevice(), VkFramebuffer(hnd), null );
				break;
			case EHandleType::AccStruct :
				vkDestroyAccelerationStructureKHR( GetVkDevice(), VkAccelerationStructureKHR(hnd), null );
				break;
			default :
				CHECK_MSG( false, "unknown handle type" );
		}
		END_ENUM_CHECKS();
	}
	tempHandles.clear();
}

/*
=================================================
	SaveImage
=================================================
*/
bool  TestDevice::SaveImage (VkImage image, VkImageLayout layout, uint width, uint height, const Path &filename)
{
	VkBuffer		readback_buf;
	VkDeviceMemory	readback_mem;

	VkBufferCreateInfo	buffer_ci = {};
	buffer_ci.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_ci.size		= width * height * 4;
	buffer_ci.usage		= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		
	VK_CHECK_ERR( vkCreateBuffer( GetVkDevice(), &buffer_ci, null, OUT &readback_buf ));
	tempHandles.emplace_back( EHandleType::Buffer, ulong(readback_buf) );

	VkMemoryRequirements	mem_req = {};
	vkGetBufferMemoryRequirements( GetVkDevice(), readback_buf, OUT &mem_req );

	VkMemoryAllocateInfo	mem_alloc_info = {};
	mem_alloc_info.sType			= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mem_alloc_info.allocationSize	= mem_req.size;
	CHECK_ERR( GetMemoryTypeIndex( mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, OUT mem_alloc_info.memoryTypeIndex ));
		
	VK_CHECK_ERR( vkAllocateMemory( GetVkDevice(), &mem_alloc_info, null, OUT &readback_mem ));
	tempHandles.emplace_back( EHandleType::Memory, ulong(readback_mem) );

	VK_CHECK_ERR( vkBindBufferMemory( GetVkDevice(), readback_buf, readback_mem, 0));


	VkCommandBufferBeginInfo	begin_info = {};
	begin_info.sType	= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags	= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	VK_CHECK_ERR( vkBeginCommandBuffer( cmdBuffer, &begin_info ));
	
	{
		VkImageMemoryBarrier barrier {};
		barrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.oldLayout           = layout;
		barrier.newLayout           = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask		= VK_ACCESS_MEMORY_WRITE_BIT;
		barrier.dstAccessMask		= VK_ACCESS_TRANSFER_READ_BIT;
		barrier.image               = image;
		barrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
			
		vkCmdPipelineBarrier( cmdBuffer,
							  VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0,
							  0, null, 0, null, 1, &barrier );
	}
	{
		VkBufferImageCopy	region = {};
		region.bufferOffset		= 0;
		region.bufferRowLength	= width;
		region.bufferImageHeight= height;
		region.imageSubresource	= { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		region.imageOffset		= { 0, 0, 0 };
		region.imageExtent		= { width, height, 1 };

		vkCmdCopyImageToBuffer( cmdBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, readback_buf, 1, &region );
	}

	VK_CHECK_ERR( vkEndCommandBuffer( cmdBuffer ));

	VkSubmitInfo		submit_info = {};
	submit_info.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount	= 1;
	submit_info.pCommandBuffers		= &cmdBuffer;

	VK_CHECK_ERR( vkQueueSubmit( GetVkQueue(), 1, &submit_info, Default ));
	VK_CHECK_ERR( vkQueueWaitIdle( GetVkQueue() ));
	

	void*	mapped_ptr;
	VK_CHECK_ERR( vkMapMemory( GetVkDevice(), readback_mem, 0, width*height*4, 0, OUT &mapped_ptr ));

	ResLoader::IntermImage::Mipmaps_t	mm;
	mm.resize( 1 );
	mm[0].resize( 1 );
		
	auto&	lvl = mm[0][0];
	lvl.dimension	= {width, height, 1};
	lvl.format		= EPixelFormat::RGBA8_UNorm;
	lvl.rowPitch	= Bytes{width*4};
	lvl.slicePitch	= lvl.rowPitch * height;
	
	Unused( lvl.SetPixelDataRef( mapped_ptr, lvl.slicePitch ));

	ResLoader::IntermImage	img{ RVRef(mm), EImage_2D };
	FileWStream				file {filename};

	CHECK_ERR( file.IsOpen() );
	
	ResLoader::DDSSaver	saver;
	CHECK_ERR( saver.SaveImage( file, img, Default ));

	return true;
}


} // AE::PipelineCompiler
