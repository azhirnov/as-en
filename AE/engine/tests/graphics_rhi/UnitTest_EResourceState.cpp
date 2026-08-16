// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

#ifdef AE_ENABLE_VULKAN
# include "graphics_rhi/Vulkan/VEnumCast.h"
#endif

namespace
{
	static void  EResourceState_Test1 ()
	{
		TEST( EResourceState_RequireShaderStage( EResourceState::ShaderUniform ));
		TEST( not EResourceState_RequireShaderStage( EResourceState::Host_Read ));

		TEST( not EResourceState_IsReadOnly( EResourceState::ShaderStorage_Write ));
		TEST( EResourceState_IsReadOnly( EResourceState::ShaderStorage_Read ));
		TEST( EResourceState_HasReadAccess( EResourceState::ShaderStorage_ReadWrite ));
		TEST( EResourceState_HasWriteAccess( EResourceState::ShaderStorage_ReadWrite ));
		TEST( not EResourceState_HasWriteAccess( EResourceState::ShaderStorage_Read ));

		//TEST( EResourceState_IsDepthReadOnly( EResourceState::DepthStencilTest ));
		//TEST( not EResourceState_IsDepthReadOnly( EResourceState::DepthStencilAttachment_RW ));
		//TEST( EResourceState_IsStencilReadOnly( EResourceState::DepthStencilTest ));
		//TEST( not EResourceState_IsStencilReadOnly( EResourceState::DepthTest_StencilRW ));
	}


#ifdef AE_ENABLE_VULKAN
#	include "vulkan_loader/vkenum_to_str.h"

	static void  EResourceState_Test2 ()
	{
		const auto	Check = [] (EResourceState state, VkPipelineStageFlagBits2 allStages, VkAccessFlagBits2 allAccesses)
		{{
			VkPipelineStageFlagBits2	stage;
			VkAccessFlagBits2			access;
			VkImageLayout				layout;
			EResourceState_ToDstStageAccessLayout( state, OUT stage, OUT access, OUT layout );

			if ( auto diff = (stage & ~allStages);  diff != 0 )
				TEST_MSG( false, "'stage' must not contain stages: "s << VkPipelineStageFlags2ToString( diff ));

			if ( auto diff = (access & ~allAccesses);  diff != 0 )
				TEST_MSG( false, "'access' must not contain access masks: "s << VkAccessFlags2ToString( diff ));
		}};

		// Host
		/*{
			const auto				scope		= EPipelineScope::Host;
			const auto				all_stages	= VPipelineScope::GetStages( scope );
			const auto				all_access	= VPipelineScope::GetAccess( scope );
			const EResourceState	states[]	= { EResourceState::Host_Read };

			for (auto state : states)
			{
				VkPipelineStageFlagBits2	stage;
				VkAccessFlagBits2			access;
				VkImageLayout				layout;
				EResourceState_ToDstStageAccessLayout( state, OUT stage, OUT access, OUT layout );

				TEST( (stage  & ~all_stages) == 0 );
				TEST( (access & ~all_access) == 0 );
			}
		}*/

		// Graphics
		{
			const auto				scope		= EPipelineScope::Graphics;
			const auto				all_stages	= VPipelineScope::GetStages( scope );
			const auto				all_access	= VPipelineScope::GetAccess( scope );
			const auto				st1_stages	= EResourceState::AllGraphicsShaders;
			const auto				st2_stages	= EResourceState::FragmentShader;
			const EResourceState	states1 []	= { EResourceState::ShaderStorage_Read, EResourceState::ShaderStorage_Write, EResourceState::ShaderStorage_RW,
													EResourceState::ShaderUniform, EResourceState::ShaderSample, EResourceState::ShaderRTAS
												  };
			const EResourceState	states2 []	= { EResourceState::InputColorAttachment, EResourceState::InputColorAttachment_RW,
													EResourceState::InputDepthStencilAttachment,
													EResourceState::InputDepthStencilAttachment_RW	| EResourceState::DSTestAfterFS,
													EResourceState::DepthStencilTest_ShaderSample	| EResourceState::DSTestBeforeFS,
													EResourceState::DepthTest_DepthSample_StencilRW	| EResourceState::DSTestBeforeFS
												  };
			const EResourceState	states3 []	= { EResourceState::ColorAttachment, EResourceState::ColorAttachment_Blend,
													EResourceState::DepthRead_StencilReadWrite		| EResourceState::DSTestAfterFS,
													EResourceState::DepthReadWrite_StencilRead		| EResourceState::DSTestAfterFS,
													EResourceState::DepthStencilAttachment_Read		| EResourceState::DSTestBeforeFS,
													EResourceState::DepthStencilAttachment_Write	| EResourceState::DSTestBeforeFS,
													EResourceState::DepthStencilAttachment_RW		| EResourceState::DSTestBeforeFS,
													EResourceState::IndirectBuffer, EResourceState::IndexBuffer, EResourceState::VertexBuffer,
													EResourceState::ShadingRateImage, EResourceState::FragmentDensityMap
												  };

			for (auto state : states1){
				Check( state | st1_stages, all_stages, all_access );
			}
			for (auto state : states2){
				Check( state | st2_stages, all_stages, all_access );
			}
			for (auto state : states3){
				Check( state, all_stages, all_access );
			}
		}

		// FramebufferLocal
		{
			const auto				scope		= EPipelineScope::Graphics;
			const auto				all_stages	= VPipelineScope::GetStages( scope );
			const auto				all_access	= VPipelineScope::GetAccess( scope );
			const auto				st1_stages	= EResourceState::FragmentShader | EResourceState::TileShader;
			const EResourceState	states1 []	= { EResourceState::ShaderStorage_Read, EResourceState::ShaderStorage_Write, EResourceState::ShaderStorage_RW,
													EResourceState::ShaderUniform, EResourceState::ShaderSample, EResourceState::ShaderRTAS,
													EResourceState::InputColorAttachment, EResourceState::InputColorAttachment_RW,
													EResourceState::InputDepthStencilAttachment,
													EResourceState::InputDepthStencilAttachment_RW	| EResourceState::DSTestAfterFS
												  };
			const EResourceState	states2 []	= { EResourceState::DepthStencilAttachment_Read		| EResourceState::DSTestAfterFS,
													EResourceState::DepthStencilAttachment_Write	| EResourceState::DSTestAfterFS,
													EResourceState::DepthStencilAttachment_RW		| EResourceState::DSTestAfterFS
												  };

			for (auto state : states1){
				Check( state | st1_stages, all_stages, all_access );
			}
			for (auto state : states2){
				Check( state, all_stages, all_access );
			}
		}

		// Compute
		{
			const auto				scope		= EPipelineScope::Compute;
			const auto				all_stages	= VPipelineScope::GetStages( scope );
			const auto				all_access	= VPipelineScope::GetAccess( scope );
			const auto				st1_stages	= EResourceState::ComputeShader;
			const EResourceState	states1 []	= { EResourceState::ShaderStorage_Read, EResourceState::ShaderStorage_Write, EResourceState::ShaderStorage_RW,
													EResourceState::ShaderUniform, EResourceState::ShaderSample, EResourceState::ShaderRTAS
												  };
			for (auto state : states1){
				Check( state | st1_stages, all_stages, all_access );
			}
		}

		// Transfer_Graphics
		{
			const auto				scope		= EPipelineScope::Transfer_Graphics;
			const auto				all_stages	= VPipelineScope::GetStages( scope );
			const auto				all_access	= VPipelineScope::GetAccess( scope );
			const EResourceState	states []	= { EResourceState::CopySrc, EResourceState::CopyDst, EResourceState::ClearDst, EResourceState::BlitSrc, EResourceState::BlitDst };
			const EResourceState	states2 []	= { EResourceState::CoopVecConvert_Read, EResourceState::CoopVecConvert_Write };

			for (auto state : states){
				Check( state, all_stages, all_access );
			}
			for (auto state : states2){
				Check( state, all_stages, all_access );
			}
		}

		// Transfer_Copy
		{
			const auto				scope		= EPipelineScope::Transfer_Copy;
			const auto				all_stages	= VPipelineScope::GetStages( scope );
			const auto				all_access	= VPipelineScope::GetAccess( scope );
			const EResourceState	states []	= { EResourceState::CopySrc, EResourceState::CopyDst };

			for (auto state : states){
				Check( state, all_stages, all_access );
			}
		}

		// RayTracing
		{
			const auto				scope		= EPipelineScope::RayTracing;
			const auto				all_stages	= VPipelineScope::GetStages( scope );
			const auto				all_access	= VPipelineScope::GetAccess( scope );
			const auto				st1_stages	= EResourceState::RayTracingShaders;
			const EResourceState	states1 []	= { EResourceState::ShaderStorage_Read, EResourceState::ShaderStorage_Write, EResourceState::ShaderStorage_RW,
													EResourceState::ShaderUniform, EResourceState::ShaderSample, EResourceState::ShaderRTAS
												  };
			const EResourceState	states2 []	= { EResourceState::RTShaderBindingTable };

			for (auto state : states1){
				Check( state | st1_stages, all_stages, all_access );
			}
			for (auto state : states2){
				Check( state, all_stages, all_access );
			}
		}

		// RTAS_Build
		{
			const auto				scope		= EPipelineScope::RTAS_Build;
			const auto				all_stages	= VPipelineScope::GetStages( scope );
			const auto				all_access	= VPipelineScope::GetAccess( scope );
			const EResourceState	states []	= { EResourceState::BuildRTAS_Read, EResourceState::BuildRTAS_Write, EResourceState::BuildRTAS_RW,
													EResourceState::BuildRTAS_ScratchBuffer, EResourceState::BuildRTAS_IndirectBuffer,
													EResourceState::BuildRTAS_MicromapRead
												  };
			for (auto state : states){
				Check( state, all_stages, all_access );
			}
		}

		// RTAS_Copy
		{
			const auto				scope		= EPipelineScope::RTAS_Copy;
			const auto				all_stages	= VPipelineScope::GetStages( scope );
			const auto				all_access	= VPipelineScope::GetAccess( scope );
			const EResourceState	states []	= { EResourceState::CopyRTAS_Read, EResourceState::CopyRTAS_Write };

			for (auto state : states){
				Check( state, all_stages, all_access );
			}
		}

		// MM_Build
		{
			const auto				scope		= EPipelineScope::MM_Build;
			const auto				all_stages	= VPipelineScope::GetStages( scope );
			const auto				all_access	= VPipelineScope::GetAccess( scope );
			const EResourceState	states []	= { EResourceState::BuildMicromap_Read, EResourceState::BuildMicromap_Write, EResourceState::BuildMicromap_ScratchBuffer };

			for (auto state : states){
				Check( state, all_stages, all_access );
			}
		}

		// Video
		{
			const auto				scope		= EPipelineScope::Video;
			const auto				all_stages	= VPipelineScope::GetStages( scope );
			const auto				all_access	= VPipelineScope::GetAccess( scope );
			const EResourceState	states []	= { EResourceState::VideoDecodeSrc, EResourceState::VideoDecodeDst, EResourceState::VideoDecodeDpb,
													EResourceState::VideoEncodeSrc, EResourceState::VideoEncodeDst, EResourceState::VideoEncodeDpb };
			for (auto state : states){
				Check( state, all_stages, all_access );
			}
		}

		// ICB_Preprocess
		{
			const auto				scope		= EPipelineScope::ICB_Preprocess;
			const auto				all_stages	= VPipelineScope::GetStages( scope );
			const auto				all_access	= VPipelineScope::GetAccess( scope );
			const EResourceState	states []	= { EResourceState::ICB_Preprocess_Read, EResourceState::ICB_Preprocess_Write };

			for (auto state : states){
				Check( state, all_stages, all_access );
			}
		}

		StaticAssert( uint(EPipelineScope::_Count) == 14 );
		StaticAssert( uint(EResourceState::AllShaderStages) == 0x1F8000 );
		StaticAssert( uint(_EResState::EState::_Count) == 51 );
	}
#else
	static void  EResourceState_Test2 ()
	{
		// TODO
	}
#endif
}


extern void  UnitTest_EResourceState ()
{
	EResourceState_Test1();
	EResourceState_Test2();

	TEST_PASSED();
}
