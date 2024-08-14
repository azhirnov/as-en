// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics/Remote/RMessages.h"


	#define REGISTER_CMDBUF_COMMANDS( _reg_ )\
		CHECK_ERR(\
			_reg_( CmdBuf_Bake::BeginGraphics						)	and\
			_reg_( CmdBuf_Bake::BeginCompute						)	and\
			_reg_( CmdBuf_Bake::BeginTransfer						)	and\
			_reg_( CmdBuf_Bake::BeginASBuild						)	and\
			_reg_( CmdBuf_Bake::BeginRayTracing						)	and\
			_reg_( CmdBuf_Bake::DebugMarkerCmd						)	and\
			_reg_( CmdBuf_Bake::PushDebugGroupCmd					)	and\
			_reg_( CmdBuf_Bake::PopDebugGroupCmd					)	and\
			_reg_( CmdBuf_Bake::WriteTimestampCmd					)	and\
			/* pipeline barriers */\
			_reg_( CmdBuf_Bake::BufferBarrierCmd					)	and\
			_reg_( CmdBuf_Bake::BufferViewBarrierCmd				)	and\
			_reg_( CmdBuf_Bake::ImageBarrierCmd						)	and\
			_reg_( CmdBuf_Bake::ImageRangeBarrierCmd				)	and\
			_reg_( CmdBuf_Bake::ImageViewBarrierCmd					)	and\
			_reg_( CmdBuf_Bake::MemoryBarrierCmd					)	and\
			_reg_( CmdBuf_Bake::MemoryBarrier2Cmd					)	and\
			_reg_( CmdBuf_Bake::MemoryBarrier3Cmd					)	and\
			_reg_( CmdBuf_Bake::ExecutionBarrierCmd					)	and\
			_reg_( CmdBuf_Bake::ExecutionBarrier2Cmd				)	and\
			_reg_( CmdBuf_Bake::AcquireBufferOwnershipCmd			)	and\
			_reg_( CmdBuf_Bake::ReleaseBufferOwnershipCmd			)	and\
			_reg_( CmdBuf_Bake::AcquireImageOwnershipCmd			)	and\
			_reg_( CmdBuf_Bake::ReleaseImageOwnershipCmd			)	and\
			_reg_( CmdBuf_Bake::CommitBarriersCmd					)	and\
			/* transfer commands */\
			_reg_( CmdBuf_Bake::ClearColorImageCmd					)	and\
			_reg_( CmdBuf_Bake::ClearDepthStencilImageCmd			)	and\
			_reg_( CmdBuf_Bake::UpdateBufferCmd						)	and\
			_reg_( CmdBuf_Bake::FillBufferCmd						)	and\
			_reg_( CmdBuf_Bake::CopyBufferCmd						)	and\
			_reg_( CmdBuf_Bake::CopyImageCmd						)	and\
			_reg_( CmdBuf_Bake::CopyBufferToImageCmd				)	and\
			_reg_( CmdBuf_Bake::CopyImageToBufferCmd				)	and\
			_reg_( CmdBuf_Bake::CopyBufferToImage2Cmd				)	and\
			_reg_( CmdBuf_Bake::CopyImageToBuffer2Cmd				)	and\
			_reg_( CmdBuf_Bake::BlitImageCmd						)	and\
			_reg_( CmdBuf_Bake::ResolveImageCmd						)	and\
			_reg_( CmdBuf_Bake::GenerateMipmapsCmd					)	and\
			_reg_( CmdBuf_Bake::UpdateHostBufferCmd					)	and\
			_reg_( CmdBuf_Bake::ReadHostBufferCmd					)	and\
			/* compute commands */\
			_reg_( CmdBuf_Bake::Compute_BindPipelineCmd				)	and\
			_reg_( CmdBuf_Bake::Compute_BindDescriptorSetCmd		)	and\
			_reg_( CmdBuf_Bake::Compute_PushConstantCmd				)	and\
			_reg_( CmdBuf_Bake::Compute_Dispatch					)	and\
			_reg_( CmdBuf_Bake::Compute_DispatchIndirect			)	and\
			/* graphics commands */\
			_reg_( CmdBuf_Bake::Graphics_BeginRenderPass			)	and\
			_reg_( CmdBuf_Bake::Graphics_NextSubpass				)	and\
			_reg_( CmdBuf_Bake::Graphics_EndRenderPass				)	and\
			_reg_( CmdBuf_Bake::Graphics_BeginMtRenderPass			)	and\
			_reg_( CmdBuf_Bake::Graphics_NextMtSubpass				)	and\
			_reg_( CmdBuf_Bake::Graphics_EndMtRenderPass			)	and\
			_reg_( CmdBuf_Bake::Graphics_ExecuteSecondary			)	and\
			/* draw commands */\
			_reg_( CmdBuf_Bake::Draw_BindGraphicsPipelineCmd		)	and\
			_reg_( CmdBuf_Bake::Draw_BindMeshPipelineCmd			)	and\
			_reg_( CmdBuf_Bake::Draw_BindTilePipelineCmd			)	and\
			_reg_( CmdBuf_Bake::Draw_BindDescriptorSetCmd			)	and\
			_reg_( CmdBuf_Bake::Draw_PushConstantCmd				)	and\
			_reg_( CmdBuf_Bake::Draw_SetViewportsCmd				)	and\
			_reg_( CmdBuf_Bake::Draw_SetScissorsCmd					)	and\
			_reg_( CmdBuf_Bake::Draw_SetDepthBiasCmd				)	and\
			_reg_( CmdBuf_Bake::Draw_SetStencilCompareMaskCmd		)	and\
			_reg_( CmdBuf_Bake::Draw_SetStencilWriteMaskCmd			)	and\
			_reg_( CmdBuf_Bake::Draw_SetStencilReferenceCmd			)	and\
			_reg_( CmdBuf_Bake::Draw_SetBlendConstantsCmd			)	and\
			_reg_( CmdBuf_Bake::Draw_SetDepthBoundsCmd				)	and\
			_reg_( CmdBuf_Bake::Draw_SetFragmentShadingRateCmd		)	and\
			_reg_( CmdBuf_Bake::Draw_BindIndexBufferCmd				)	and\
			_reg_( CmdBuf_Bake::Draw_BindVertexBuffersCmd			)	and\
			_reg_( CmdBuf_Bake::DrawCmd								)	and\
			_reg_( CmdBuf_Bake::DrawIndexedCmd						)	and\
			_reg_( CmdBuf_Bake::DrawIndirectCmd						)	and\
			_reg_( CmdBuf_Bake::DrawIndexedIndirectCmd				)	and\
			_reg_( CmdBuf_Bake::DrawMeshTasksCmd					)	and\
			_reg_( CmdBuf_Bake::DrawMeshTasksIndirectCmd			)	and\
			_reg_( CmdBuf_Bake::DrawIndirectCountCmd				)	and\
			_reg_( CmdBuf_Bake::DrawIndexedIndirectCountCmd			)	and\
			_reg_( CmdBuf_Bake::DrawMeshTasksIndirectCountCmd		)	and\
			_reg_( CmdBuf_Bake::DispatchTileCmd						)	and\
			_reg_( CmdBuf_Bake::Draw_AttachmentBarrierCmd			)	and\
			_reg_( CmdBuf_Bake::Draw_CommitBarriersCmd				)	and\
			_reg_( CmdBuf_Bake::Draw_ClearAttachmentCmd				)	and\
			/* acceleration structure build commands */\
			_reg_( CmdBuf_Bake::ASBuild_BuildGeometryCmd			)	and\
			_reg_( CmdBuf_Bake::ASBuild_BuildSceneCmd				)	and\
			_reg_( CmdBuf_Bake::ASBuild_UpdateGeometryCmd			)	and\
			_reg_( CmdBuf_Bake::ASBuild_UpdateSceneCmd				)	and\
			_reg_( CmdBuf_Bake::ASBuild_CopyGeometryCmd				)	and\
			_reg_( CmdBuf_Bake::ASBuild_CopySceneCmd				)	and\
			_reg_( CmdBuf_Bake::ASBuild_WriteGeometryPropertyCmd	)	and\
			_reg_( CmdBuf_Bake::ASBuild_WriteScenePropertyCmd		)	and\
			/* ray tracing commands */\
			_reg_( CmdBuf_Bake::RayTracing_BindPipelineCmd			)	and\
			_reg_( CmdBuf_Bake::RayTracing_BindDescriptorSetCmd		)	and\
			_reg_( CmdBuf_Bake::RayTracing_PushConstantCmd			)	and\
			_reg_( CmdBuf_Bake::RayTracing_SetStackSizeCmd			)	and\
			_reg_( CmdBuf_Bake::RayTracing_TraceRaysCmd				)	and\
			_reg_( CmdBuf_Bake::RayTracing_TraceRaysIndirectCmd		)	and\
			_reg_( CmdBuf_Bake::RayTracing_TraceRaysIndirect2Cmd	));

	#define REGISTER_DESCUPD_COMMANDS( _reg_ )\
		CHECK_ERR(\
			_reg_( DescUpd_Flush::SetDescSet			)	and\
			_reg_( DescUpd_Flush::BindVideoImage		)	and\
			_reg_( DescUpd_Flush::BindImages			)	and\
			_reg_( DescUpd_Flush::BindBuffers			)	and\
			_reg_( DescUpd_Flush::BindBufferRange		)	and\
			_reg_( DescUpd_Flush::BindTextures			)	and\
			_reg_( DescUpd_Flush::BindSamplers			)	and\
			_reg_( DescUpd_Flush::BindTexelBuffers		)	and\
			_reg_( DescUpd_Flush::BindRayTracingScenes	));


	#define REGISTER_MESSAGES( _regMsg_, _regResp_ )\
		CHECK_ERR(\
			_regMsg_(  UploadData						)	and\
			_regResp_( UploadDataAndCopy				)	and\
			_regResp_( Log								)	and\
			_regResp_( DefaultResponse					));\
		\
		CHECK_ERR(\
			_regMsg_(  Device_Init						)	and\
			_regMsg_(  Device_DestroyLogicalDevice		)	and\
			_regMsg_(  Device_DestroyInstance			)	and\
			_regMsg_(  Device_EnableSyncLog				)	and\
			_regMsg_(  Device_GetSyncLog				)	and\
			_regResp_( Device_Init_Response				)	and\
			_regResp_( Device_GetSyncLog_Response		));\
		\
		CHECK_ERR(\
			_regMsg_(  Surface_Create					)	and\
			_regMsg_(  Surface_Destroy					)	and\
			_regMsg_(  Swapchain_Create					)	and\
			_regMsg_(  Swapchain_Destroy				)	and\
			_regMsg_(  Swapchain_IsSupported			)	and\
			_regMsg_(  Swapchain_AcquireNextImage		)	and\
			_regMsg_(  Swapchain_Present				)	and\
			_regResp_( Surface_Create_Response			)	and\
			_regResp_( Swapchain_Create_Response		)	and\
			_regResp_( Swapchain_AcquireNextImage_Response)	and\
			_regResp_( Swapchain_Present_Response		));\
		\
		CHECK_ERR(\
			_regMsg_(  RTS_Initialize					)	and\
			_regMsg_(  RTS_Deinitialize					)	and\
			_regMsg_(  RTS_BeginFrame					)	and\
			_regMsg_(  RTS_EndFrame						)	and\
			_regMsg_(  RTS_WaitNextFrame				)	and\
			_regMsg_(  RTS_WaitAll						)	and\
			_regMsg_(  RTS_SkipCmdBatches				)	and\
			_regMsg_(  RTS_CreateBatch					)	and\
			_regMsg_(  RTS_SubmitBatch					)	and\
			_regMsg_(  RTS_DestroyBatch					)	and\
			_regMsg_(  RTS_WaitBatch					)	and\
			_regResp_( RTS_CreateBatch_Response			)	and\
			_regResp_( RTS_WaitNextFrame_Response		));\
		\
		CHECK_ERR(\
			_regMsg_( ResMngr_CreateImage					)	and\
			_regMsg_( ResMngr_CreateImageView				)	and\
			_regMsg_( ResMngr_CreateBuffer					)	and\
			_regMsg_( ResMngr_CreateBufferView				)	and\
			_regMsg_( ResMngr_CreateRTGeometry				)	and\
			_regMsg_( ResMngr_CreateRTScene					)	and\
			_regMsg_( ResMngr_GetRTGeometrySizes			)	and\
			_regMsg_( ResMngr_GetRTSceneSizes				)	and\
			_regMsg_( ResMngr_IsSupported_BufferDesc		)	and\
			_regMsg_( ResMngr_IsSupported_ImageDesc			)	and\
			_regMsg_( ResMngr_IsSupported_VideoImageDesc	)	and\
			_regMsg_( ResMngr_IsSupported_VideoBufferDesc	)	and\
			_regMsg_( ResMngr_IsSupported_VideoSessionDesc	)	and\
			_regMsg_( ResMngr_IsSupported_BufferViewDesc	)	and\
			_regMsg_( ResMngr_IsSupported_ImageViewDesc		)	and\
			_regMsg_( ResMngr_IsSupported_RTGeometryDesc	)	and\
			_regMsg_( ResMngr_IsSupported_RTGeometryBuild	)	and\
			_regMsg_( ResMngr_IsSupported_RTSceneDesc		)	and\
			_regMsg_( ResMngr_IsSupported_RTSceneBuild		)	and\
			_regMsg_( ResMngr_ReleaseResource				)	and\
			_regMsg_( ResMngr_CreateDescriptorSets2			)	and\
			_regMsg_( ResMngr_CreateDescriptorSets3			)	and\
			_regMsg_( ResMngr_GetRTechPipeline				)	and\
			_regMsg_( ResMngr_CreateGraphicsPipeline		)	and\
			_regMsg_( ResMngr_CreateMeshPipeline			)	and\
			_regMsg_( ResMngr_CreateComputePipeline			)	and\
			_regMsg_( ResMngr_CreateRayTracingPipeline		)	and\
			_regMsg_( ResMngr_CreateTilePipeline			)	and\
			_regMsg_( ResMngr_CreateVideoSession			)	and\
			_regMsg_( ResMngr_CreateVideoBuffer				)	and\
			_regMsg_( ResMngr_CreateVideoImage				)	and\
			_regMsg_( ResMngr_CreatePipelineCache			)	and\
			_regMsg_( ResMngr_InitializeResources			)	and\
			_regMsg_( ResMngr_LoadPipelinePack				)	and\
			_regMsg_( ResMngr_GetSupportedRenderTechs		)	and\
			_regMsg_( ResMngr_LoadRenderTech				)	and\
			_regMsg_( ResMngr_CreateLinearGfxMemAllocator	)	and\
			_regMsg_( ResMngr_CreateBlockGfxMemAllocator	)	and\
			_regMsg_( ResMngr_CreateUnifiedGfxMemAllocator	)	and\
			_regMsg_( ResMngr_GetRenderPass					)	and\
			_regMsg_( ResMngr_ForceReleaseResources			)	and\
			_regMsg_( ResMngr_GetShaderGroupStackSize		)	and\
			\
			_regResp_( ResMngr_CreateImage_Response				)	and\
			_regResp_( ResMngr_CreateImageView_Response			)	and\
			_regResp_( ResMngr_CreateBuffer_Response			)	and\
			_regResp_( ResMngr_CreateBufferView_Response		)	and\
			_regResp_( ResMngr_CreateRTGeometry_Response		)	and\
			_regResp_( ResMngr_CreateRTScene_Response			)	and\
			_regResp_( ResMngr_GetRTGeometrySizes_Response		)	and\
			_regResp_( ResMngr_GetRTSceneSizes_Response			)	and\
			_regResp_( ResMngr_IsSupported_Response				)	and\
			_regResp_( ResMngr_ReleaseResource_Response			)	and\
			_regResp_( ResMngr_CreateDescriptorSets_Response	)	and\
			_regResp_( ResMngr_CreateGraphicsPipeline_Response	)	and\
			_regResp_( ResMngr_CreateMeshPipeline_Response		)	and\
			_regResp_( ResMngr_CreateComputePipeline_Response	)	and\
			_regResp_( ResMngr_CreateRayTracingPipeline_Response)	and\
			_regResp_( ResMngr_CreateTilePipeline_Response		)	and\
			_regResp_( ResMngr_CreateVideoSession_Response		)	and\
			_regResp_( ResMngr_CreateVideoBuffer_Response		)	and\
			_regResp_( ResMngr_CreateVideoImage_Response		)	and\
			_regResp_( ResMngr_CreatePipelineCache_Response		)	and\
			_regResp_( ResMngr_LoadPipelinePack_Response		)	and\
			_regResp_( ResMngr_GetSupportedRenderTechs_Response	)	and\
			_regResp_( ResMngr_LoadRenderTech_Response			)	and\
			_regResp_( ResMngr_CreateGfxMemAllocator_Response	)	and\
			_regResp_( ResMngr_GetRenderPass_Response			)	and\
			_regResp_( ResMngr_GetShaderGroupStackSize_Response	));\
		\
		CHECK_ERR(\
			_regMsg_(  Query_Init								)	and\
			_regMsg_(  Query_Alloc								)	and\
			_regMsg_(  Query_GetTimestampUL						)	and\
			_regMsg_(  Query_GetTimestampD						)	and\
			_regMsg_(  Query_GetTimestampNs						)	and\
			_regMsg_(  Query_GetTimestampCalibratedUL			)	and\
			_regMsg_(  Query_GetTimestampCalibratedD			)	and\
			_regMsg_(  Query_GetTimestampCalibratedNs			)	and\
			_regMsg_(  Query_GetPipelineStatistic				)	and\
			\
			_regResp_( Query_Init_Response						)	and\
			_regResp_( Query_Alloc_Response						)	and\
			_regResp_( Query_GetTimestampUL_Response			)	and\
			_regResp_( Query_GetTimestampD_Response				)	and\
			_regResp_( Query_GetTimestampNs_Response			)	and\
			_regResp_( Query_GetTimestampCalibratedUL_Response	)	and\
			_regResp_( Query_GetTimestampCalibratedD_Response	)	and\
			_regResp_( Query_GetTimestampCalibratedNs_Response	)	and\
			_regResp_( Query_GetPipelineStatistic_Response		));\
		\
		CHECK_ERR(\
			_regMsg_(  ProfArm_Initialize						)	and\
			_regMsg_(  ProfArm_Sample							)	and\
			_regMsg_(  ProfMali_Initialize						)	and\
			_regMsg_(  ProfMali_Sample							)	and\
			_regMsg_(  ProfAdreno_Initialize					)	and\
			_regMsg_(  ProfAdreno_Sample						)	and\
			_regMsg_(  ProfPVR_Initialize						)	and\
			_regMsg_(  ProfPVR_Tick								)	and\
			_regMsg_(  ProfPVR_Sample							)	and\
			_regMsg_(  ProfNVidia_Initialize					)	and\
			_regMsg_(  ProfNVidia_Sample						)	and\
			_regResp_( ProfArm_Initialize_Response				)	and\
			_regResp_( ProfArm_Sample_Response					)	and\
			_regResp_( ProfMali_Initialize_Response				)	and\
			_regResp_( ProfMali_Sample_Response					)	and\
			_regResp_( ProfAdreno_Initialize_Response			)	and\
			_regResp_( ProfAdreno_Sample_Response				)	and\
			_regResp_( ProfPVR_Initialize_Response				)	and\
			_regResp_( ProfPVR_Tick_Response					)	and\
			_regResp_( ProfPVR_Sample_Response					)	and\
			_regResp_( ProfNVidia_Initialize_Response			)	and\
			_regResp_( ProfNVidia_Sample_Response				));\
		\
		CHECK_ERR(\
			_regMsg_(  DescUpd_Flush					)	and\
			_regMsg_(  CmdBuf_Bake						)	and\
			_regMsg_(  CmdBuf_BakeDraw					)	and\
			_regResp_( CmdBuf_Bake_Response				)	and\
			_regResp_( CmdBuf_BakeDraw_Response			));\
		\
		CHECK_ERR(\
			_regMsg_(  SBM_GetBufferRanges				)	and\
			_regMsg_(  SBM_GetImageRanges				)	and\
			_regMsg_(  SBM_GetImageRanges2				)	and\
			_regMsg_(  SBM_AllocVStream					)	and\
			_regMsg_(  SBM_GetFrameStat					)	and\
			_regResp_( SBM_GetBufferRanges_Response		)	and\
			_regResp_( SBM_GetImageRanges_Response		)	and\
			_regResp_( SBM_AllocVStream_Response		)	and\
			_regResp_( SBM_GetFrameStat_Response		));


