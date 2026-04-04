// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_rhi/Public/IndirectCommandBuffer.h"
#include "graphics_rhi/GraphicsImpl.h"

namespace AE::Graphics
{

	template <typename PipeID>
	bool  IndirectCommandsLayoutDescBuilder::_PipelineLayout (PipeID pplnId, EShaderStages stages)
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		auto*	ppln = res_mngr.GetResource( pplnId, False{"don't inc RC"}, True{"quiet"} );
		CHECK_ERR( ppln != null );

		_pl = res_mngr.GetResource( ppln->LayoutId(), False{"don't inc RC"}, True{"quiet"} );
		CHECK_ERR( _pl != null );

		_desc.pipelineLayout = ppln->LayoutId();
		_plStages = stages;

		return true;
	}

	bool  IndirectCommandsLayoutDescBuilder::PipelineLayout (GraphicsPipelineID		pplnId)	__NE___	{ return _PipelineLayout( pplnId, EShaderStages::GraphicsPipeStages ); }
	bool  IndirectCommandsLayoutDescBuilder::PipelineLayout (MeshPipelineID			pplnId)	__NE___	{ return _PipelineLayout( pplnId, EShaderStages::MeshPipeStages ); }
	bool  IndirectCommandsLayoutDescBuilder::PipelineLayout (ComputePipelineID		pplnId)	__NE___	{ return _PipelineLayout( pplnId, EShaderStages::Compute ); }
	bool  IndirectCommandsLayoutDescBuilder::PipelineLayout (RayTracingPipelineID	pplnId)	__NE___	{ return _PipelineLayout( pplnId, EShaderStages::AllRayTracing ); }


	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::Usage (EIndirectCommandsLayoutUsage value) __NE___
	{
		_desc.usage = value;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::Stages (EShaderStages value) __NE___
	{
		ASSERT_MSG( _plStages == Default or AllBits( value, _plStages ),
			"PipelineLayout stages doesn't match with specified shader stages" );

		_desc.stages = value;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::SequenceStride (Bytes value) __NE___
	{
		_desc.sequenceStride = value;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::BaseOffset (Bytes value) __NE___
	{
		CHECK_ERR( value >= _offset, *this );

		_offset = value;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::Advance (Bytes delta) __NE___
	{
		_offset += delta;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::ExecutionSet (EShaderStages stages) __NE___
	{
		auto&	dst = _desc.tokens.emplace_back();
		dst.type	= ETokenType::ExecutionSet;
		dst.offset	= _offset;

		dst.data.executionSet = {};
		dst.data.executionSet.stages = stages;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::PushConstant (EShaderStages stages, Bytes32u pcOffset, Bytes32u pcSize) __NE___
	{
		auto&	dst = _desc.tokens.emplace_back();
		dst.type	= ETokenType::PushConstant;
		dst.offset	= _offset;

		dst.data.pushConstant = {};
		dst.data.pushConstant.stages = stages;
		dst.data.pushConstant.offset = pcOffset;
		dst.data.pushConstant.size	 = pcSize;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::PushConstant (PushConstantName::Ref pcName, ShaderStructName::Ref typeName) __NE___
	{
		CHECK_ERR( _pl != null, *this );

	  #ifdef AE_ENABLE_VULKAN
		auto&	pc_map = Cast<VPipelineLayout>(_pl)->GetPushConstants();

		auto	it = pc_map.find( pcName );
		CHECK_ERR( it != pc_map.end(), *this );

		CHECK_ERR( it->second.typeName == typeName, *this );

		return PushConstant( EShaderStages(0) | it->second.stage, it->second.vulkanOffset, it->second.size );
	  #else

		CHECK_MSG( false, "not implemented" );
		return *this;
	  #endif
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::SequenceIndex (EShaderStages stages, Bytes32u idxOffset) __NE___
	{
		auto&	dst = _desc.tokens.emplace_back();
		dst.type	= ETokenType::SequenceIndex;
		dst.offset	= _offset;

		dst.data.sequenceIndex = {};
		dst.data.sequenceIndex.stages = stages;
		dst.data.sequenceIndex.offset = idxOffset;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::IndexBuffer (EIndirectCommandsInputMode mode) __NE___
	{
		auto&	dst = _desc.tokens.emplace_back();
		dst.type	= ETokenType::IndexBuffer;
		dst.offset	= _offset;

		dst.data.indexBuffer = {};
		dst.data.indexBuffer.mode = mode;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::VertexBuffer (uint vertexBindingUnit) __NE___
	{
		auto&	dst = _desc.tokens.emplace_back();
		dst.type	= ETokenType::VertexBuffer;
		dst.offset	= _offset;

		dst.data.vertexBuffer = {};
		dst.data.vertexBuffer.vertexBindingUnit = vertexBindingUnit;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::VertexBuffer (GraphicsPipelineID pplnId, VertexBufferName::Ref name) __NE___
	{
		auto&	res_mngr = GraphicsScheduler().GetResourceManager();

		auto*	ppln = res_mngr.GetResource( pplnId, False{"don't inc RC"}, True{"quiet"} );
		CHECK_ERR( ppln != null, *this );

		uint	idx = ppln->GetVertexBufferIndex( name );
		CHECK_ERR( idx != UMax, *this );

		return VertexBuffer( idx );
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::Draw () __NE___
	{
		auto&	dst = _desc.tokens.emplace_back();
		dst.type	= ETokenType::Draw;
		dst.offset	= _offset;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::DrawIndexed () __NE___
	{
		auto&	dst = _desc.tokens.emplace_back();
		dst.type	= ETokenType::DrawIndexed;
		dst.offset	= _offset;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::DrawIndexedCount () __NE___
	{
		auto&	dst = _desc.tokens.emplace_back();
		dst.type	= ETokenType::DrawIndexedCount;
		dst.offset	= _offset;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::DrawMeshTasks () __NE___
	{
		auto&	dst = _desc.tokens.emplace_back();
		dst.type	= ETokenType::DrawMeshTasks;
		dst.offset	= _offset;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::DrawMeshTasksCount () __NE___
	{
		auto&	dst = _desc.tokens.emplace_back();
		dst.type	= ETokenType::DrawMeshTasksCount;
		dst.offset	= _offset;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::Dispatch () __NE___
	{
		auto&	dst = _desc.tokens.emplace_back();
		dst.type	= ETokenType::Dispatch;
		dst.offset	= _offset;
		return *this;
	}

	IndirectCommandsLayoutDescBuilder&  IndirectCommandsLayoutDescBuilder::TraceRays () __NE___
	{
		auto&	dst = _desc.tokens.emplace_back();
		dst.type	= ETokenType::TraceRays;
		dst.offset	= _offset;
		return *this;
	}

	IndirectCommandsLayoutDesc  IndirectCommandsLayoutDescBuilder::Build () __NE___
	{
		if ( _desc.sequenceStride == 0 )
			_desc.sequenceStride = SequenceSize();

		ASSERT( _desc.sequenceStride >= SequenceSize() );
		return RVRef(_desc);
	}

} // AE::Graphics
