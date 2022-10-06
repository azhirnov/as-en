// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Public/CommandBuffer.h"
# include "graphics/Metal/Commands/MBaseIndirectContext.h"

namespace AE::Graphics::_hidden_
{

	//
	// Metal Argument Setter interface
	//
	struct IMetalArgumentSetter
	{
		virtual void  SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index) = 0;
		virtual void  SetBufferOffset (Bytes offset, MBufferIndex index) = 0;

		virtual void  SetBytes (const void* data, Bytes dataSize, MBufferIndex index) = 0;

		virtual void  SetSampler (MetalSampler sampler, MSamplerIndex index) = 0;
		virtual void  SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index) = 0;

		virtual void  SetTexture (MetalImage texture, MTextureIndex index) = 0;
	};


	template <EShader ShaderType, bool Direct>
	struct MArgumentSetter
	{};
	


	//
	// Compute Shader Argument Setter
	//
	template <>
	struct MArgumentSetter< EShader::Compute, true > final : IMetalArgumentSetter
	{
	// variables
	private:
		MetalComputeCommandEncoder	_encoder;

	// methods
	public:
		explicit MArgumentSetter (MetalComputeCommandEncoder encoder) : _encoder{encoder} {}
		
		void  SetThreadgroupMemoryLength (Bytes length, MThreadgroupIndex index);

		void  SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index) override;
		void  SetBufferOffset (Bytes offset, MBufferIndex index) override;

		void  SetBytes (const void* data, Bytes dataSize, MBufferIndex index) override;

		void  SetSampler (MetalSampler sampler, MSamplerIndex index) override;
		void  SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index) override;

		void  SetTexture (MetalImage texture, MTextureIndex index) override;

		void  SetVisibleFunctionTable (MetalVisibleFnTable table, MBufferIndex index);
		void  SetIntersectionFunctionTable (MetalIntersectionFnTable table, MBufferIndex index);
		void  SetAccelerationStructure (MetalAccelStruct as, MBufferIndex index);
	};


	template <>
	struct MArgumentSetter< EShader::Compute, false > final : IMetalArgumentSetter
	{
	// variables
	private:
		Ptr<MSoftwareCmdBuf>	_cmdbuf;

	// methods
	public:
		explicit MArgumentSetter (Ptr<MSoftwareCmdBuf> cmdbuf) : _cmdbuf{cmdbuf} {}

		void  SetThreadgroupMemoryLength (Bytes length, MThreadgroupIndex index);

		void  SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index) override;
		void  SetBufferOffset (Bytes offset, MBufferIndex index) override;

		void  SetBytes (const void* data, Bytes dataSize, MBufferIndex index) override;

		void  SetSampler (MetalSampler sampler, MSamplerIndex index) override;
		void  SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index) override;

		void  SetTexture (MetalImage texture, MTextureIndex index) override;

		void  SetVisibleFunctionTable (MetalVisibleFnTable table, MBufferIndex index);
		void  SetIntersectionFunctionTable (MetalIntersectionFnTable table, MBufferIndex index);
		void  SetAccelerationStructure (MetalAccelStruct as, MBufferIndex index);
	};


	
	//
	// Vertex Shader Argument Setter
	//
	template <>
	struct MArgumentSetter< EShader::Vertex, true > final : IMetalArgumentSetter
	{
	// variables
	private:
		MetalRenderCommandEncoder	_encoder;

	// methods
	public:
		explicit MArgumentSetter (MetalRenderCommandEncoder encoder) : _encoder{encoder} {}

		void  SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index) override;
		void  SetBufferOffset (Bytes offset, MBufferIndex index) override;

		void  SetBytes (const void* data, Bytes dataSize, MBufferIndex index) override;

		void  SetSampler (MetalSampler sampler, MSamplerIndex index) override;
		void  SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index) override;

		void  SetTexture (MetalImage texture, MTextureIndex index) override;

		void  SetVisibleFunctionTable (MetalVisibleFnTable table, MBufferIndex index);
		void  SetIntersectionFunctionTable (MetalIntersectionFnTable table, MBufferIndex index);
		void  SetAccelerationStructure (MetalAccelStruct as, MBufferIndex index);
	};


	template <>
	struct MArgumentSetter< EShader::Vertex, false > final : IMetalArgumentSetter
	{
	// variables
	private:
		Ptr<MSoftwareCmdBuf>	_cmdbuf;

	// methods
	public:
		explicit MArgumentSetter (Ptr<MSoftwareCmdBuf> cmdbuf) : _cmdbuf{cmdbuf} {}

		void  SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index) override;
		void  SetBufferOffset (Bytes offset, MBufferIndex index) override;

		void  SetBytes (const void* data, Bytes dataSize, MBufferIndex index) override;

		void  SetSampler (MetalSampler sampler, MSamplerIndex index) override;
		void  SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index) override;

		void  SetTexture (MetalImage texture, MTextureIndex index) override;

		void  SetVisibleFunctionTable (MetalVisibleFnTable table, MBufferIndex index);
		void  SetIntersectionFunctionTable (MetalIntersectionFnTable table, MBufferIndex index);
		void  SetAccelerationStructure (MetalAccelStruct as, MBufferIndex index);
	};


	
	//
	// Fragment Shader Argument Setter
	//
	template <>
	struct MArgumentSetter< EShader::Fragment, true > final : IMetalArgumentSetter
	{
	// variables
	private:
		MetalRenderCommandEncoder	_encoder;

	// methods
	public:
		explicit MArgumentSetter (MetalRenderCommandEncoder encoder) : _encoder{encoder} {}

		void  SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index) override;
		void  SetBufferOffset (Bytes offset, MBufferIndex index) override;

		void  SetBytes (const void* data, Bytes dataSize, MBufferIndex index) override;

		void  SetSampler (MetalSampler sampler, MSamplerIndex index) override;
		void  SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index) override;

		void  SetTexture (MetalImage texture, MTextureIndex index) override;

		void  SetVisibleFunctionTable (MetalVisibleFnTable table, MBufferIndex index);
		void  SetIntersectionFunctionTable (MetalIntersectionFnTable table, MBufferIndex index);
		void  SetAccelerationStructure (MetalAccelStruct as, MBufferIndex index);
	};


	template <>
	struct MArgumentSetter< EShader::Fragment, false > final : IMetalArgumentSetter
	{
	// variables
	private:
		Ptr<MSoftwareCmdBuf>	_cmdbuf;

	// methods
	public:
		explicit MArgumentSetter (Ptr<MSoftwareCmdBuf> cmdbuf) : _cmdbuf{cmdbuf} {}

		void  SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index) override;
		void  SetBufferOffset (Bytes offset, MBufferIndex index) override;

		void  SetBytes (const void* data, Bytes dataSize, MBufferIndex index) override;

		void  SetSampler (MetalSampler sampler, MSamplerIndex index) override;
		void  SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index) override;

		void  SetTexture (MetalImage texture, MTextureIndex index) override;

		void  SetVisibleFunctionTable (MetalVisibleFnTable table, MBufferIndex index);
		void  SetIntersectionFunctionTable (MetalIntersectionFnTable table, MBufferIndex index);
		void  SetAccelerationStructure (MetalAccelStruct as, MBufferIndex index);
	};

	

	//
	// Tile Shader Argument Setter
	//
	template <>
	struct MArgumentSetter< EShader::Tile, true > final : IMetalArgumentSetter
	{
	// variables
	private:
		MetalRenderCommandEncoder	_encoder;

	// methods
	public:
		explicit MArgumentSetter (MetalRenderCommandEncoder encoder) : _encoder{encoder} {}
		
		void  SetThreadgroupMemoryLength (Bytes length, Bytes offset, MThreadgroupIndex index);

		void  SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index) override;
		void  SetBufferOffset (Bytes offset, MBufferIndex index) override;

		void  SetBytes (const void* data, Bytes dataSize, MBufferIndex index) override;

		void  SetSampler (MetalSampler sampler, MSamplerIndex index) override;
		void  SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index) override;

		void  SetTexture (MetalImage texture, MTextureIndex index) override;

		void  SetVisibleFunctionTable (MetalVisibleFnTable table, MBufferIndex index);
		void  SetIntersectionFunctionTable (MetalIntersectionFnTable table, MBufferIndex index);
		void  SetAccelerationStructure (MetalAccelStruct as, MBufferIndex index);
	};


	template <>
	struct MArgumentSetter< EShader::Tile, false > final : IMetalArgumentSetter
	{
	// variables
	private:
		Ptr<MSoftwareCmdBuf>	_cmdbuf;

	// methods
	public:
		explicit MArgumentSetter (Ptr<MSoftwareCmdBuf> cmdbuf) : _cmdbuf{cmdbuf} {}

		void  SetThreadgroupMemoryLength (Bytes length, Bytes offset, MThreadgroupIndex index);

		void  SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index) override;
		void  SetBufferOffset (Bytes offset, MBufferIndex index) override;

		void  SetBytes (const void* data, Bytes dataSize, MBufferIndex index) override;

		void  SetSampler (MetalSampler sampler, MSamplerIndex index) override;
		void  SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index) override;

		void  SetTexture (MetalImage texture, MTextureIndex index) override;

		void  SetVisibleFunctionTable (MetalVisibleFnTable table, MBufferIndex index);
		void  SetIntersectionFunctionTable (MetalIntersectionFnTable table, MBufferIndex index);
		void  SetAccelerationStructure (MetalAccelStruct as, MBufferIndex index);
	};

	

	//
	// Mesh Shader Argument Setter
	//
	template <>
	struct MArgumentSetter< EShader::Mesh, true > final : IMetalArgumentSetter
	{
	// variables
	private:
		MetalRenderCommandEncoder	_encoder;

	// methods
	public:
		explicit MArgumentSetter (MetalRenderCommandEncoder encoder) : _encoder{encoder} {}

		void  SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index) override;
		void  SetBufferOffset (Bytes offset, MBufferIndex index) override;

		void  SetBytes (const void* data, Bytes dataSize, MBufferIndex index) override;

		void  SetSampler (MetalSampler sampler, MSamplerIndex index) override;
		void  SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index) override;

		void  SetTexture (MetalImage texture, MTextureIndex index) override;
	};


	template <>
	struct MArgumentSetter< EShader::Mesh, false > final : IMetalArgumentSetter
	{
	// variables
	private:
		Ptr<MSoftwareCmdBuf>	_cmdbuf;

	// methods
	public:
		explicit MArgumentSetter (Ptr<MSoftwareCmdBuf> cmdbuf) : _cmdbuf{cmdbuf} {}

		void  SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index) override;
		void  SetBufferOffset (Bytes offset, MBufferIndex index) override;

		void  SetBytes (const void* data, Bytes dataSize, MBufferIndex index) override;

		void  SetSampler (MetalSampler sampler, MSamplerIndex index) override;
		void  SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index) override;

		void  SetTexture (MetalImage texture, MTextureIndex index) override;
	};

	

	//
	// Mesh Task Shader Argument Setter
	//
	template <>
	struct MArgumentSetter< EShader::MeshTask, true > final : IMetalArgumentSetter
	{
	// variables
	private:
		MetalRenderCommandEncoder	_encoder;

	// methods
	public:
		explicit MArgumentSetter (MetalRenderCommandEncoder encoder) : _encoder{encoder} {}
		
		void  SetThreadgroupMemoryLength (Bytes length, MThreadgroupIndex index);

		void  SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index) override;
		void  SetBufferOffset (Bytes offset, MBufferIndex index) override;

		void  SetBytes (const void* data, Bytes dataSize, MBufferIndex index) override;

		void  SetSampler (MetalSampler sampler, MSamplerIndex index) override;
		void  SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index) override;

		void  SetTexture (MetalImage texture, MTextureIndex index) override;
	};


	template <>
	struct MArgumentSetter< EShader::MeshTask, false > final : IMetalArgumentSetter
	{
	// variables
	private:
		Ptr<MSoftwareCmdBuf>	_cmdbuf;

	// methods
	public:
		explicit MArgumentSetter (Ptr<MSoftwareCmdBuf> cmdbuf) : _cmdbuf{cmdbuf} {}

		void  SetThreadgroupMemoryLength (Bytes length, MThreadgroupIndex index);

		void  SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index) override;
		void  SetBufferOffset (Bytes offset, MBufferIndex index) override;

		void  SetBytes (const void* data, Bytes dataSize, MBufferIndex index) override;

		void  SetSampler (MetalSampler sampler, MSamplerIndex index) override;
		void  SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index) override;

		void  SetTexture (MetalImage texture, MTextureIndex index) override;
	};


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
