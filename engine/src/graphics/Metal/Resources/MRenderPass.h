// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL

# include "base/Containers/FixedTupleArray.h"
# include "graphics/Public/RenderPassDesc.h"
# include "graphics/Metal/MCommon.h"
# include "Packer/RenderPassPack.h"

namespace AE::Graphics
{
	using AE::PipelineCompiler::SerializableRenderPassInfo;
	using AE::PipelineCompiler::SerializableMtlRenderPass;



	//
	// Metal Render Pass
	//

	class MRenderPass final
	{
	// types
	public:
		using MtlPixelFormat	= AE::PipelineCompiler::MtlPixelFormat;
	
		struct PipelineRenderPassInfo
		{
			ushort				colorCount	= 0;
			MtlPixelFormat		color [GraphicsConfig::MaxColorAttachments]	= {};
			MtlPixelFormat		depth		= MtlPixelFormat::Invalid;
			MtlPixelFormat		stencil		= MtlPixelFormat::Invalid;
		};

		/*struct AttachmentIdx
		{
			static constexpr uint	_IdxBits	= 4;
			STATIC_ASSERT( GraphicsConfig::MaxAttachments < (1u << _IdxBits) );

			ushort		index		: _IdxBits;		// index for 'MTLRenderPassAttachmentDescriptor::texture' and in '_attStates'
			//ushort	clearIdx	: _IdxBits;		// index for 'MTLRenderPassDescriptor::colorAttachments'
			ushort		resolveIdx	: _IdxBits;		// index for 'MTLRenderPassAttachmentDescriptor::resolveTexture'

			AttachmentIdx () : index{0xF}, clearIdx{0xF}, resolveIdx{0xF} {}
		};*/


	private:
		using AttachmentMap_t		= FixedMap< AttachmentName::Optimized_t, ubyte, GraphicsConfig::MaxAttachments >;
		using AttachmentStates_t	= SerializableMtlRenderPass::AttachmentStates_t;
		using MtlAttachments_t		= SerializableMtlRenderPass::MtlAttachments_t;

		using MtlMultisampleDepthResolveFilter		= AE::PipelineCompiler::MtlMultisampleDepthResolveFilter;
		using MtlMultisampleStencilResolveFilter	= AE::PipelineCompiler::MtlMultisampleStencilResolveFilter;


	// variables
	private:
		AttachmentMap_t						_attachmentMap;
		//AttachmentStates_t				_attStates;
		MtlAttachments_t					_mtlAttachments;
		MtlMultisampleDepthResolveFilter	_depthResolve;
		MtlMultisampleStencilResolveFilter	_stencilResolve;

		DEBUG_ONLY(	DebugName_t				_debugName;	)
		DRC_ONLY(	RWDataRaceCheck			_drCheck;	)


	// methods
	public:
		MRenderPass () {}
		~MRenderPass ();

		ND_ bool  Create (MResourceManager &, const SerializableRenderPassInfo &compatInfo, const SerializableMtlRenderPass &mtlInfo, StringView dbgName = Default);
			void  Destroy (MResourceManager &);

		ND_ NS::ObjectRef  CreateRPDesc (const MResourceManager &, const RenderPassDesc &desc) const;	// MTLRenderPassDescriptor autorelease

		ND_ PipelineRenderPassInfo		GetInfo ()		const;
		
		DEBUG_ONLY(  ND_ StringView		GetDebugName ()	const	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })
	};


}	// AE::Graphics

#endif	// AE_ENABLE_METAL