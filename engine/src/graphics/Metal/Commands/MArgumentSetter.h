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



	//
	// Indirect Shader Argument Setter
	//
	template <EShader ShaderType>
	struct MArgumentSetter< ShaderType, false > final : IMetalArgumentSetter
	{
	// variables
	private:
		Ptr<MSoftwareCmdBuf>	_cmdbuf;

	// methods
	public:
		explicit MArgumentSetter (Ptr<MSoftwareCmdBuf> cmdbuf) : _cmdbuf{cmdbuf} {}

		// compute, meshtask
		EnableIf<(ShaderType == EShader::Compute or ShaderType == EShader::MeshTask), void>
			SetThreadgroupMemoryLength (Bytes length, MThreadgroupIndex index);

		// tile
		EnableIf<ShaderType == EShader::Tile, void>
			SetThreadgroupMemoryLength (Bytes length, Bytes offset, MThreadgroupIndex index);

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
//-----------------------------------------------------------------------------


	
	template <EShader ShaderType>
	EnableIf<(ShaderType == EShader::Compute or ShaderType == EShader::MeshTask), void>
		MArgumentSetter<EShader::Compute,false>::SetThreadgroupMemoryLength (Bytes length, MThreadgroupIndex index)
	{
		auto&		cmd	= _cmdbuf->CreateCmd< MSoftwareCmdBuf::SetBufferCmd >();
		cmd.shaderType	= ShaderType;
		cmd.index		= index;
		cmd.length		= length;
		cmd.offset		= 0_b;
	}
	
	EnableIf<ShaderType == EShader::Tile, void>
		MArgumentSetter<EShader::Compute,false>::SetThreadgroupMemoryLength (Bytes length, Bytes offset, MThreadgroupIndex index)
	{
		auto&		cmd	= _cmdbuf->CreateCmd< MSoftwareCmdBuf::SetBufferCmd >();
		cmd.shaderType	= ShaderType;
		cmd.index		= index;
		cmd.length		= length;
		cmd.offset		= offset;
	}

	template <EShader ShaderType>
	void  MArgumentSetter<ShaderType,false>::SetBuffer (MetalBuffer buffer, Bytes offset, MBufferIndex index)
	{
		auto&		cmd	= _cmdbuf->CreateCmd< MSoftwareCmdBuf::SetBufferCmd >();
		cmd.shaderType	= ShaderType;
		cmd.index		= index;
		cmd.buffer		= buffer;
		cmd.offset		= offset;
	}
	
	template <EShader ShaderType>
	void  MArgumentSetter<ShaderType,false>::SetBufferOffset (Bytes offset, MBufferIndex index)
	{
		auto&		cmd	= _cmdbuf->CreateCmd< MSoftwareCmdBuf::SetBufferOffsetCmd >();
		cmd.shaderType	= ShaderType;
		cmd.index		= index;
		cmd.offset		= offset;
	}
	
	template <EShader ShaderType>
	void  MArgumentSetter<ShaderType,false>::SetBytes (const void* data, Bytes dataSize, MBufferIndex index)
	{
		CHECK_ERRV( dataSize <= 1_Kb );	// 4 Kb in specs

		auto&		cmd	= _cmdbuf->CreateCmd< MSoftwareCmdBuf::SetBytesCmd, ubyte >( usize(dataSize) );
		auto*		dst	= static_cast< void *>(&cmd + 1);

		cmd.shaderType	= ShaderType;
		cmd.dataSize	= dataSize;
		cmd.index		= index;

		MemCopy( OUT data, dataSize );
	}
	
	template <EShader ShaderType>
	void  MArgumentSetter<ShaderType,false>::SetSampler (MetalSampler sampler, MSamplerIndex index)
	{
		auto&		cmd	= _cmdbuf->CreateCmd< MSoftwareCmdBuf::SetSamplerCmd >();
		cmd.shaderType	= ShaderType;
		cmd.index		= index;
		cmd.sampler		= sampler;
	}
	
	template <EShader ShaderType>
	void  MArgumentSetter<ShaderType,false>::SetSampler (MetalSampler sampler, float lodMinClamp, float lodMaxClamp, MSamplerIndex index)
	{
		auto&		cmd	= _cmdbuf->CreateCmd< MSoftwareCmdBuf::SetSampler2Cmd >();
		cmd.shaderType	= ShaderType;
		cmd.index		= index;
		cmd.sampler		= sampler;
		cmd.lodMinClamp	= lodMinClamp;
		cmd.lodMaxClamp	= lodMaxClamp;
	}
	
	template <EShader ShaderType>
	void  MArgumentSetter<ShaderType,false>::SetTexture (MetalImage texture, MTextureIndex index)
	{
		auto&		cmd	= _cmdbuf->CreateCmd< MSoftwareCmdBuf::SetTextureCmd >();
		cmd.shaderType	= ShaderType;
		cmd.index		= index;
		cmd.texture		= texture;
	}
	
	template <EShader ShaderType>
	void  MArgumentSetter<ShaderType,false>::SetVisibleFunctionTable (MetalVisibleFnTable table, MBufferIndex index)
	{
		auto&		cmd	= _cmdbuf->CreateCmd< MSoftwareCmdBuf::SetVisibleFunctionTableCmd >();
		cmd.shaderType	= ShaderType;
		cmd.index		= index;
		cmd.table		= table;
	}
	
	template <EShader ShaderType>
	void  MArgumentSetter<ShaderType,false>::SetIntersectionFunctionTable (MetalIntersectionFnTable table, MBufferIndex index)
	{
		auto&		cmd	= _cmdbuf->CreateCmd< MSoftwareCmdBuf::SetIntersectionFunctionTableCmd >();
		cmd.shaderType	= ShaderType;
		cmd.index		= index;
		cmd.table		= table;
	}
	
	template <EShader ShaderType>
	void  MArgumentSetter<ShaderType,false>::SetAccelerationStructure (MetalAccelStruct as, MBufferIndex index)
	{
		auto&		cmd	= _cmdbuf->CreateCmd< MSoftwareCmdBuf::SetAccelerationStructureCmd >();
		cmd.shaderType	= ShaderType;
		cmd.index		= index;
		cmd.as			= as;
	}


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
