// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/IDs.h"
#include "graphics/Public/EResourceState.h"
#include "graphics/Public/ShaderEnums.h"
#include "graphics/Public/RenderStateEnums.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/VertexEnums.h"
#include "graphics/Public/DescriptorSet.h"
#include "graphics/Public/PipelineDesc.h"
#include "Packer/PackCommon.h"

namespace AE::PipelineCompiler
{
	using namespace AE::Graphics;

	struct ShaderTrace;

	using SpirvBytecode_t	= Array<uint>;
	using MetalBytecode_t	= Array<ubyte>;

	using FSNameArr_t	= ArrayView< FeatureSetName::Optimized_t >;



	//
	// Spirv with Trace
	//
	struct SpirvWithTrace
	{
	// variables
		SpirvBytecode_t			bytecode;
		Unique<ShaderTrace>		trace;

	// methods
		SpirvWithTrace ()									__NE___;
		SpirvWithTrace (SpirvWithTrace &&)					__NE___;
		SpirvWithTrace (const SpirvWithTrace &other);
		~SpirvWithTrace ()									__NE___;

		SpirvWithTrace&  operator = (SpirvWithTrace &&)		__NE___;
		SpirvWithTrace&  operator = (const SpirvWithTrace &);

		ND_ bool  operator == (const SpirvWithTrace &rhs)	C_NE___;
	};


	enum class DescrSetUID			: uint { Unknown = ~0u };
	enum class PipelineLayoutUID	: uint { Unknown = ~0u };
	enum class RenderTechUID		: uint { Unknown = ~0u };
	enum class RTShaderBindingUID	: uint { Unknown = ~0u };

	enum class RenderStateUID		: uint { Unknown = ~0u };
	enum class DepthStencilStateUID	: uint { Unknown = ~0u };


	enum class ShaderUID : uint
	{
		SPIRV		= 1u << 28,
		Metal_iOS	= 2u << 28,
		Metal_Mac	= 3u << 28,
		_Mask		= 7u << 28,
		Unknown		= ~0u
	};
	AE_BIT_OPERATORS( ShaderUID );


	enum class PipelineTemplUID : uint
	{
		Graphics		= 1u << 28,
		Mesh			= 2u << 28,
		Compute			= 3u << 28,
		RayTracing		= 4u << 28,
		Tile			= 5u << 28,
		_Mask			= 0xFu << 28,
		Unknown			= ~0u
	};
	AE_BIT_OPERATORS( PipelineTemplUID );


	enum class PipelineSpecUID : uint
	{
		Graphics		= uint(PipelineTemplUID::Graphics),
		Mesh			= uint(PipelineTemplUID::Mesh),
		Compute			= uint(PipelineTemplUID::Compute),
		RayTracing		= uint(PipelineTemplUID::RayTracing),
		Tile			= uint(PipelineTemplUID::Tile),
		_Mask			= uint(PipelineTemplUID::_Mask),
		Unknown			= uint(PipelineTemplUID::Unknown),
	};
	AE_BIT_OPERATORS( PipelineSpecUID );


	enum class EImageType : ushort
	{
		Unknown				= 0,

		_TexMask			= 0xF,
		Img1D				= 1,
		Img1DArray			= 2,
		Img2D				= 3,
		Img2DArray			= 4,
		Img2DMS				= 5,
		Img2DMSArray		= 6,
		ImgCube				= 7,
		ImgCubeArray		= 8,
		Img3D				= 9,
		Buffer				= 10,	// for texel buffer
		_TexCount,

		_ValMask			= 0xF << 4,
		Float				= 0x1 << 4,
		Half				= 0x2 << 4,
		SNorm				= 0x3 << 4,
		UNorm				= 0x4 << 4,
		Int					= 0x5 << 4,
		UInt				= 0x6 << 4,
		sRGB				= 0x7 << 4,	// unorm
		Depth				= 0x8 << 4,
		Stencil				= 0x9 << 4,
		DepthStencil		= 0xA << 4,
		SLong				= 0xB << 4,
		ULong				= 0xC << 4,
		_LastVal,

		_QualMask			= 0xF << 8,
		Shadow				= 1 << 8,
		_LastQual,
	};
	AE_BIT_OPERATORS( EImageType );

	StaticAssert( EImageType::_TexCount < EImageType::_TexMask );
	StaticAssert( EImageType::_LastVal  < EImageType::_ValMask );
	StaticAssert( EImageType::_LastQual < EImageType::_QualMask );

	ND_ bool		EImageType_IsCompatible (EImageType lhs, EImageType rhs)	__NE___;
	ND_ EImageType	EImageType_FromPixelFormat (EPixelFormat fmt)				__NE___;
	ND_ EImageType	EImageType_FromPixelFormatRelaxed (EPixelFormat fmt)		__NE___;
	ND_ EImageType	EImageType_FromImage (EImage type, bool ms)					__NE___;
	ND_ String		EImageType_ToString (EImageType type)						__Th___;
	ND_ EShaderIO	EImageType_ToShaderIO (EImageType type)						__NE___;



	//
	// Descriptor Set Layout description
	//
	class DescriptorSetLayoutDesc final : public Serializing::ISerializable
	{
	// types
	public:
		struct Binding
		{
			// Vulkan: unique binding index
			uint		vkIndex			= UMax;

			// Metal
			union {
				uint					mtlIndex		= UMax;	// index in argument buffer, if specified in 'usage'
				MetalBindingPerStage	mtlPerStageIndex;
			};

			Binding () {}

			ND_ bool  IsVkDefined  ()	const	{ return vkIndex != UMax; }
			ND_ bool  IsMetalDefined ()	const	{ return mtlIndex != UMax; }

			ND_ bool  operator == (const Binding &rhs) const	{ return (vkIndex == rhs.vkIndex) and All( mtlIndex == rhs.mtlIndex ); }
			ND_ bool  operator != (const Binding &rhs) const	{ return not (*this == rhs); }
		};
		StaticAssert( sizeof(Binding) == 8 );

		using BindingIndex_t		= decltype(Binding::vkIndex);
		using ArraySize_t			= uint;
		using DynamicOffsetIdx_t	= ushort;
		using SamplerIdx_t			= ushort;	// in 'samplerStorage'

		static constexpr BindingIndex_t	InvalidIdx		= UMax;
		static constexpr BindingIndex_t	UnassignedIdx	= ~0u - 1;	// for immutable sampler in Metal

		struct Buffer
		{
			EResourceState		state					= Default;
			DynamicOffsetIdx_t	dynamicOffsetIndex		= UMax;
			Bytes32u			staticSize;
			Bytes32u			arrayStride;
			ShaderStructName	typeName;

			ND_ bool  HasDynamicOffset ()	const	{ return dynamicOffsetIndex != UMax; }
		};

		struct TexelBuffer
		{
			EResourceState		state					= Default;
			EImageType			type					= Default;
		};

		struct Image
		{
			EResourceState		state;
			EImageType			type;
			EPixelFormat		format;								// if explicitly defined
			ubyte				subpassInputIdx			= UMax;		// only for input attachment
			SamplerIdx_t		samplerOffsetInStorage	= UMax;
		};

		struct Sampler
		{};

		struct ImmutableSampler : Sampler
		{
			SamplerIdx_t		offsetInStorage			= UMax;
		};

		struct SubpassInput : Image
		{
			ubyte				index					= UMax;
		};

		struct RayTracingScene
		{};

		struct Uniform
		{
			Binding				binding;
			ArraySize_t			arraySize				= UMax;		// 0 for runtime sized array
			EDescriptorType		type					= Default;
			EShaderStages		stages					= Default;
			union {
				Buffer				buffer;
				TexelBuffer			texelBuffer;
				Image				image;
				Sampler				sampler;
				ImmutableSampler	immutableSampler;
				RayTracingScene		rtScene;
			};

			Uniform () __NE___ {}
		};

		using Uniforms_t	= Array<Pair< UniformName, Uniform >>;
		using Samplers_t	= Array< SamplerName >;

		// uniforms are sorted by types, this array map desc type to uniform offset to speedup search
		using UniformOffsets_t = StaticArray< ushort, 6 >;

	public:
		static constexpr usize	MaxUniforms	= 1 << 10;
		static constexpr usize	MaxSamplers	= 1 << 6;


	// variables
	public:
		FSNameArr_t		features;
		Uniforms_t		uniforms;
		Samplers_t		samplerStorage;
		DSLayoutName	name;
		EDescSetUsage	usage	= Default;
		EShaderStages	stages	= Default;


	// methods
	public:
		DescriptorSetLayoutDesc () {}

		ND_ bool	Merge (const DescriptorSetLayoutDesc &other);
			void	SortUniforms ();

		ND_ static bool	IsCompatible (const DescriptorSetLayoutDesc &fromReflection,
									  const DescriptorSetLayoutDesc &fromScript);

		ND_ bool	operator == (const DescriptorSetLayoutDesc &rhs) const;
		ND_ HashVal	CalcHash () const;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Push Constants
	//
	class PushConstants final : public Serializing::ISerializable
	{
	// types
	public:
		struct PushConst
		{
			ShaderStructName	typeName;
			EShader				stage			= Default;
			ubyte				metalBufferId	= UMax;
			Bytes16u			vulkanOffset;
			Bytes16u			size;

			PushConst () {}
			PushConst (EShader stage, uint bufferId, Bytes32u offset, Bytes32u size, ShaderStructName::Ref typeName) :
				typeName{typeName}, stage{stage}, metalBufferId{ubyte(bufferId)}, vulkanOffset{offset}, size{size} {}

			ND_ bool	operator == (const PushConst &rhs) const;
			ND_ HashVal	CalcHash () const;
		};
		using PushConstMap_t = FixedMap< PushConstantName, PushConst, GraphicsConfig::MaxPushConstants >;


	// variables
	public:
		PushConstMap_t		items;


	// methods
	public:
		PushConstants () {}

		ND_ bool	Merge (const PushConstants &);

		ND_ bool	operator == (const PushConstants &rhs) const	{ return items == rhs.items; }
		ND_ HashVal	CalcHash () const								{ return HashOf(items); }

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Pipeline Layout description
	//
	class PipelineLayoutDesc final : public Serializing::ISerializable
	{
	// types
	public:
		struct DescSetLayout
		{
			DescrSetUID				uid			= Default;
			uint					vkIndex		= UMax;
			MetalBindingPerStage	mtlIndex;

			ND_ bool	operator == (const DescSetLayout &rhs) const;
			ND_ HashVal	CalcHash () const;
		};
		using DescrSetMap_t = FixedMap< DescriptorSetName, DescSetLayout, GraphicsConfig::MaxDescriptorSets >;


	// variables
	public:
		DescrSetMap_t		descrSets;
		PushConstants		pushConstants;


	// methods
	public:
		PipelineLayoutDesc () {}

		ND_ bool	operator == (const PipelineLayoutDesc &rhs) const;
		ND_ HashVal	CalcHash () const;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Render State
	//
	class SerializableRenderState final : public Serializing::ISerializable
	{
	// variables
	public:
		RenderState		rs;

	// methods
	public:
		SerializableRenderState () {}
		explicit SerializableRenderState (const RenderState &inRS) : rs{inRS} {}

		ND_ bool	operator == (const SerializableRenderState &rhs) const	{ return rs == rhs.rs; }
		ND_ HashVal	CalcHash () const										{ return rs.CalcHash(); }

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Depth Stencil State (only for Metal)
	//
	class SerializableDepthStencilState final : public Serializing::ISerializable
	{
	// variables
	public:
		MDepthStencilState		ds;

	// methods
	public:
		SerializableDepthStencilState () {}
		explicit SerializableDepthStencilState (const RenderState &rs) { ds.Set( rs ); }
		explicit SerializableDepthStencilState (const MDepthStencilState &inDS) : ds{inDS} {}

		ND_ bool	operator == (const SerializableDepthStencilState &rhs) const	{ return ds == rhs.ds; }
		ND_ HashVal	CalcHash () const												{ return ds.CalcHash(); }

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Graphics Pipeline Template
	//
	class SerializableGraphicsPipeline final : public Serializing::ISerializable
	{
	// types
	public:
		using Shaders_t			= FixedMap< EShader, ShaderUID, 5 >;
		using TopologyBits_t	= EnumSet< EPrimitive >;

		struct VertexAttrib
		{
			EVertexType		type	= Default;
			ubyte			index	= UMax;

			ND_ bool  operator == (const VertexAttrib &rhs) const;
		};


	// variables
	public:
		FSNameArr_t				features;
		PipelineLayoutUID		layout					= Default;
		Shaders_t				shaders;
		TopologyBits_t			supportedTopology;
		ArrayView<VertexAttrib>	vertexAttribs;
		uint					patchControlPoints		= 0;
		bool					earlyFragmentTests		= true;


	// methods
	public:
		SerializableGraphicsPipeline ()					__NE___	{}

		ND_ bool	operator == (const SerializableGraphicsPipeline &rhs) C_NE___;
		ND_ HashVal	CalcHash ()							C_NE___;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Graphics Pipeline Specialization
	//
	class SerializableGraphicsPipelineSpec final : public Serializing::ISerializable
	{
	// variables
	public:
		PipelineTemplUID			templUID		= Default;
		RenderStateUID				rStateUID		= Default;
		DepthStencilStateUID		dsStateUID		= Default;
		GraphicsPipelineDesc		desc;


	// methods
	public:
		SerializableGraphicsPipelineSpec ()				__NE___	{}

		ND_ bool	operator == (const SerializableGraphicsPipelineSpec &rhs) C_NE___;
		ND_ HashVal	CalcHash ()							C_NE___;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Compute Pipeline Template
	//
	class SerializableComputePipeline final : public Serializing::ISerializable
	{
	// variables
	public:
		FSNameArr_t				features;
		PipelineLayoutUID		layout				= Default;
		ShaderUID				shader				= Default;
		packed_ushort3			defaultLocalSize	{0};
		packed_ushort3			localSizeSpec		{ComputePipelineDesc::UndefinedLocalSize};


	// methods
	public:
		SerializableComputePipeline ()					__NE___	{}

		ND_ bool	operator == (const SerializableComputePipeline &rhs) C_NE___;
		ND_ HashVal	CalcHash ()							C_NE___;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Compute Pipeline Specialization
	//
	class SerializableComputePipelineSpec final : public Serializing::ISerializable
	{
	// variables
	public:
		PipelineTemplUID		templUID	= Default;
		ComputePipelineDesc		desc;


	// methods
	public:
		SerializableComputePipelineSpec ()				__NE___	{}

		ND_ bool	operator == (const SerializableComputePipelineSpec &rhs) C_NE___;
		ND_ HashVal	CalcHash ()							C_NE___;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Mesh Pipeline Template
	//
	class SerializableMeshPipeline final : public Serializing::ISerializable
	{
	// types
	public:
		using Shaders_t	= FixedMap< EShader, ShaderUID, 4 >;


	// variables
	public:
		FSNameArr_t				features;
		PipelineLayoutUID		layout					= Default;
		Shaders_t				shaders;
		EPrimitive				outputTopology			= Default;
		uint					maxVertices				= 0;
		uint					maxIndices				= 0;
		packed_ushort3			taskDefaultLocalSize	{0};
		packed_ushort3			taskLocalSizeSpec		{MeshPipelineDesc::UndefinedLocalSize};
		packed_ushort3			meshDefaultLocalSize	{0};
		packed_ushort3			meshLocalSizeSpec		{MeshPipelineDesc::UndefinedLocalSize};
		bool					earlyFragmentTests		= true;


	// methods
	public:
		SerializableMeshPipeline ()						__NE___	{}

		ND_ bool	operator == (const SerializableMeshPipeline &rhs) C_NE___;
		ND_ HashVal	CalcHash ()							C_NE___;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Mesh Pipeline Specialization
	//
	class SerializableMeshPipelineSpec final : public Serializing::ISerializable
	{
	// variables
	public:
		PipelineTemplUID		templUID		= Default;
		RenderStateUID			rStateUID		= Default;
		DepthStencilStateUID	dsStateUID		= Default;
		MeshPipelineDesc		desc;


	// methods
	public:
		SerializableMeshPipelineSpec ()					__NE___	{}

		ND_ bool	operator == (const SerializableMeshPipelineSpec &rhs) C_NE___;
		ND_ HashVal	CalcHash ()							C_NE___;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Ray Tracing Pipeline Template
	//
	class SerializableRayTracingPipeline final : public Serializing::ISerializable
	{
	// types
	public:
		using Name_t = RayTracingGroupName::Optimized_t;

		struct GeneralShader
		{
			Name_t		name;
			uint		shader		= UMax;

			ND_ bool  operator == (const GeneralShader &rhs) const {
				return name == rhs.name and shader == rhs.shader;
			}
		};

		struct TriangleHitGroup
		{
			Name_t		name;
			uint		closestHit	= UMax;
			uint		anyHit		= UMax;

			ND_ bool  operator == (const TriangleHitGroup &rhs) const {
				return name == rhs.name and closestHit == rhs.closestHit and anyHit == rhs.anyHit;
			}
		};

		struct ProceduralHitGroup
		{
			Name_t		name;
			uint		intersection	= UMax;
			uint		closestHit		= UMax;
			uint		anyHit			= UMax;

			ND_ bool  operator == (const ProceduralHitGroup &rhs) const	{
				return name == rhs.name and intersection == rhs.intersection and closestHit == rhs.closestHit and anyHit == rhs.anyHit;
			}
		};

		static constexpr uint	MaxShaders	= 0xFFFF;
		static constexpr uint	MaxGroups	= 0xFFFF;


	// variables
	public:
		FSNameArr_t								features;
		PipelineLayoutUID						layout		= Default;

		ArrayView<Tuple< ShaderUID, EShader >>	shaderArr;

		ArrayView< GeneralShader >				generalShaders;
		ArrayView< TriangleHitGroup >			triangleGroups;
		ArrayView< ProceduralHitGroup >			proceduralGroups;


	// methods
	public:
		SerializableRayTracingPipeline ()				__NE___	{}

		ND_ bool	operator == (const SerializableRayTracingPipeline &rhs) C_NE___;
		ND_ HashVal	CalcHash ()							C_NE___;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Ray Tracing Pipeline Specialization
	//
	class SerializableRayTracingPipelineSpec final : public Serializing::ISerializable
	{
	// variables
	public:
		PipelineTemplUID		templUID	= Default;
		RayTracingPipelineDesc	desc;


	// methods
	public:
		SerializableRayTracingPipelineSpec ()			__NE___	{}

		ND_ bool	operator == (const SerializableRayTracingPipelineSpec &rhs) C_NE___;
		ND_ HashVal	CalcHash ()							C_NE___;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Tile Pipeline Template
	//
	class SerializableTilePipeline final : public Serializing::ISerializable
	{
	// variables
	public:
		FSNameArr_t				features;
		PipelineLayoutUID		layout				= Default;
		ShaderUID				shader				= Default;
		ushort2					defaultLocalSize	{0};
		ushort2					localSizeSpec		{TilePipelineDesc::UndefinedLocalSize};


	// methods
	public:
		SerializableTilePipeline ()						__NE___	{}

		ND_ bool	operator == (const SerializableTilePipeline &rhs) C_NE___;
		ND_ HashVal	CalcHash ()							C_NE___;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Tile Pipeline Specialization
	//
	class SerializableTilePipelineSpec final : public Serializing::ISerializable
	{
	// variables
	public:
		PipelineTemplUID		templUID	= Default;
		TilePipelineDesc		desc;


	// methods
	public:
		SerializableTilePipelineSpec ()					__NE___	{}

		ND_ bool	operator == (const SerializableTilePipelineSpec &rhs) C_NE___;
		ND_ HashVal	CalcHash ()							C_NE___;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Ray Tracing Shader Binding Table
	//
	class SerializableRTShaderBindingTable final : public Serializing::ISerializable
	{
	// types
	public:
		static constexpr uint	INVALID_INDEX	= UMax;

		struct BindingInfo
		{
			uint	index	= INVALID_INDEX;

			ND_ bool	operator == (const BindingInfo &rhs)	const	{ return index == rhs.index; }
			ND_ HashVal	CalcHash ()								const	{ return HashOf(index); }
		};
		using BindingTable_t	= ArrayView< BindingInfo >;


	// variables
	public:
		PipelineName::Optimized_t	pplnName;

		BindingInfo					raygen;
		BindingTable_t				miss;
		BindingTable_t				hit;
		BindingTable_t				callable;
		uint						numRayTypes	= 0;


	// methods
	public:
		SerializableRTShaderBindingTable ()				__NE___ {}

		ND_ bool	operator == (const SerializableRTShaderBindingTable &rhs) C_NE___;
		ND_ HashVal	CalcHash ()							C_NE___;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Serializable Render Technique
	//
	class SerializableRenderTechnique final : public Serializing::ISerializable
	{
	// types
	public:
		using PipelineList_t	= ArrayView<Pair< PipelineName, PipelineSpecUID >>;
		using FeatureSetList_t	= ArrayView< FeatureSetName >;
		using SBTList_t			= ArrayView<Pair< RTShaderBindingName, RTShaderBindingUID >>;

		class Pass final : public Serializing::ISerializable
		{
		// variables
		public:
			RenderTechPassName	name;
			DescrSetUID			dsLayout	= Default;
			RenderPassName		renderPass;
			SubpassName			subpass;

		// methods
		public:
			ND_ bool  IsGraphics ()	const	{ return renderPass.IsDefined() and subpass.IsDefined(); }

			// ISerializable
			bool  Serialize (Serializing::Serializer &)		C_NE_OV;
			bool  Deserialize (Serializing::Deserializer &)	__NE_OV;
		};

	public:
		static constexpr uint	MaxPassCount		= 1 << 8;
		static constexpr uint	MaxPipelineCount	= 1 << 16;


	// variables
	public:
		FSNameArr_t			features;
		RenderTechName		name;
		ArrayView< Pass >	passes;
		PipelineList_t		pipelines;
		SBTList_t			rtSBTs;


	// methods
	public:
		SerializableRenderTechnique () {}

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



	//
	// Shader Bytecode
	//
	class ShaderBytecode final : public Serializing::ISerializable
	{
	// types
	public:
		using OptSpecConst_t	= FixedMap< SpecializationName::Optimized_t, /*location*/uint, GraphicsConfig::MaxSpecConstants >;
		using SpecConstants_t	= FixedMap< SpecializationName, /*location*/uint, GraphicsConfig::MaxSpecConstants >;
		using Bytecode_t		= Union< NullUnion, SpirvBytecode_t, MetalBytecode_t, SpirvWithTrace >;

		static constexpr uint	MaxBytecodeSize = 4u << 20;	// 4 Mb


	// variables
	public:
		// header
		mutable Bytes		offset;
		Bytes32u			dataSize;
		Bytes32u			data2Size;

		// data
		Bytecode_t			code;
		OptSpecConst_t		spec;
		ubyte				typeIdx	= UMax;


	// methods
	public:
		ShaderBytecode () {}
		ShaderBytecode (SpirvBytecode_t code, const SpecConstants_t &spec);
		ShaderBytecode (MetalBytecode_t code, const SpecConstants_t &spec);
		ShaderBytecode (SpirvWithTrace code, const SpecConstants_t &spec);

		ND_ bool	operator == (const ShaderBytecode &rhs) C_NE___;
		ND_ HashVal	CalcHash ()								C_NE___;

		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &)			const;
		ND_ String  ToString2 (const HashToName &)			const;
		#endif

		ND_ Bytes	GetDataSize ()							C_NE___	{ return dataSize + data2Size; }
		ND_ bool	WriteData (WStream &)					C_NE___;
		ND_ bool	ReadData (RStream &stream)				__NE___;

		ND_ bool	IsSpirv ()								C_NE___	{ return HoldsAlternative< SpirvBytecode_t >( code ); }
		ND_ bool	IsSpirvWithTrace ()						C_NE___	{ return HoldsAlternative< SpirvWithTrace >( code ); }
		ND_ bool	IsMetalBytecode ()						C_NE___	{ return HoldsAlternative< MetalBytecode_t >( code ); }

		// ISerializable
		bool  Serialize (Serializing::Serializer &)			C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &)		__NE_OV;

	private:
		ND_ bool  _ReadSpirvData (Bytes spvDataSize, RStream &, OUT SpirvBytecode_t &)	__NE___;
		ND_ bool  _ReadMetalData (RStream &, OUT MetalBytecode_t &)						__NE___;
		ND_ bool  _ReadDbgSpirvData (RStream &, OUT SpirvWithTrace &)					__NE___;

		void  _CopySpecConst (const SpecConstants_t &spec);
	};



	//
	// Pipeline Storage
	//
	class PipelineStorage
	{
	// types
	private:
		using Hash_t						= usize;

		using RenderStates_t				= Array< SerializableRenderState >;
		using RenderStateMap_t				= HashMultiMap< Hash_t, RenderStateUID >;

		using DepthStencilStates_t			= Array< SerializableDepthStencilState >;
		using DepthStencilStateMap_t		= HashMultiMap< Hash_t, DepthStencilStateUID >;

		using DescriptorSetLayouts_t		= Array< DescriptorSetLayoutDesc >;
		using DSLayoutMap_t					= HashMultiMap< Hash_t, DescrSetUID >;

		using PipelineLayouts_t				= Array< PipelineLayoutDesc >;
		using PipelineLayoutMap_t			= HashMultiMap< Hash_t, PipelineLayoutUID >;

		using GraphicsPipelineTempl_t		= Array< SerializableGraphicsPipeline >;
		using GraphicsPipelineTemplMap_t	= HashMultiMap< Hash_t, PipelineTemplUID >;

		using GraphicsPipelineSpec_t		= Array< SerializableGraphicsPipelineSpec >;
		using GraphicsPipelineSpecMap_t		= HashMultiMap< Hash_t, PipelineSpecUID >;

		using MeshPipelineTempl_t			= Array< SerializableMeshPipeline >;
		using MeshPipelineTemplMap_t		= HashMultiMap< Hash_t, PipelineTemplUID >;

		using MeshPipelineSpec_t			= Array< SerializableMeshPipelineSpec >;
		using MeshPipelineSpecMap_t			= HashMultiMap< Hash_t, PipelineSpecUID >;

		using ComputePipelineTempl_t		= Array< SerializableComputePipeline >;
		using ComputePipelineTemplMap_t		= HashMultiMap< Hash_t, PipelineTemplUID >;

		using ComputePipelineSpec_t			= Array< SerializableComputePipelineSpec >;
		using ComputePipelineSpecMap_t		= HashMultiMap< Hash_t, PipelineSpecUID >;

		using TilePipelineTempl_t			= Array< SerializableTilePipeline >;
		using TilePipelineTemplMap_t		= HashMultiMap< Hash_t, PipelineTemplUID >;

		using TilePipelineSpec_t			= Array< SerializableTilePipelineSpec >;
		using TilePipelineSpecMap_t			= HashMultiMap< Hash_t, PipelineSpecUID >;

		using RayTracingPipelineTempl_t		= Array< SerializableRayTracingPipeline >;
		using RayTracingPipelineTemplMap_t	= HashMultiMap< Hash_t, PipelineTemplUID >;

		using RayTracingPipelineSpec_t		= Array< SerializableRayTracingPipelineSpec >;
		using RayTracingPipelineSpecMap_t	= HashMultiMap< Hash_t, PipelineSpecUID >;

		using RenderTechniques_t			= Array< SerializableRenderTechnique >;

		using ShaderBytecodeArr_t			= Array< ShaderBytecode >;
		using ShaderBytecodeMap_t			= HashMultiMap< Hash_t, ShaderUID >;
		using SpecConstants_t				= ShaderBytecode::SpecConstants_t;

		using PipelineTemplMap_t			= HashMap< PipelineTmplName, PipelineTemplUID >;
		using PipelineSpecMap_t				= HashMap< PipelineName, PipelineSpecUID >;

		using RTShaderBindingTables_t		= Array< SerializableRTShaderBindingTable >;

	public:
		enum class EMarker : uint
		{
			Unknown		= 0,

			DescrSetLayouts,
			PipelineLayouts,

			RenderStates,
			DepthStencilStates,

			PipelineTemplNames,

			GraphicsPipelineTempl,
			MeshPipelineTempl,
			ComputePipelineTempl,
			RayTracingPipelineTempl,
			TilePipelineTempl,

			GraphicsPipelineSpec,
			MeshPipelineSpec,
			ComputePipelineSpec,
			RayTracingPipelineSpec,
			TilePipelineSpec,

			RTShaderBindingTable,

			RenderTechniques,

			SpirvShaders,		// Vulkan
			MetaliOSShaders,	// Metal iOS/iPad
			MetalMacShaders,	// Metal Mac

			_Count
		};

		using BlockOffsets_t = StaticArray< Bytes32u, uint(EMarker::_Count) >;

		static constexpr uint	MaxDSLayoutCount		= 1 << 16;
		static constexpr uint	MaxPplnLayoutCount		= 1 << 16;
		static constexpr uint	MaxPipelineCount		= 1 << 16;
		static constexpr uint	MaxPipelineNameCount	= 1 << 24;
		static constexpr uint	MaxRenTechCount			= 1 << 12;
		static constexpr uint	MaxShaderCount			= 1 << 24;
		static constexpr uint	MaxStateCount			= 1 << 16;
		static constexpr uint	MaxSBTCount				= 1 << 16;



	// variables
	private:
		RenderStates_t				_renderStates;
		RenderStateMap_t			_renderStateMap;

		DepthStencilStates_t		_dsStates;
		DepthStencilStateMap_t		_dsStateMap;

		DescriptorSetLayouts_t		_dsLayouts;
		DSLayoutMap_t				_dsLayoutMap;

		PipelineLayouts_t			_pplnLayouts;
		PipelineLayoutMap_t			_pplnLayoutMap;

		GraphicsPipelineTempl_t		_gpipelineTempl;
		GraphicsPipelineTemplMap_t	_gpipelineTemplMap;
		GraphicsPipelineSpec_t		_gpipelineSpec;
		GraphicsPipelineSpecMap_t	_gpipelineSpecMap;

		MeshPipelineTempl_t			_mpipelineTempl;
		MeshPipelineTemplMap_t		_mpipelineTemplMap;
		MeshPipelineSpec_t			_mpipelineSpec;
		MeshPipelineSpecMap_t		_mpipelineSpecMap;

		ComputePipelineTempl_t		_cpipelineTempl;
		ComputePipelineTemplMap_t	_cpipelineTemplMap;
		ComputePipelineSpec_t		_cpipelineSpec;
		ComputePipelineSpecMap_t	_cpipelineSpecMap;

		TilePipelineTempl_t			_tpipelineTempl;
		TilePipelineTemplMap_t		_tpipelineTemplMap;
		TilePipelineSpec_t			_tpipelineSpec;
		TilePipelineSpecMap_t		_tpipelineSpecMap;

		RayTracingPipelineTempl_t	 _rtpipelineTempl;
		RayTracingPipelineTemplMap_t _rtpipelineTemplMap;
		RayTracingPipelineSpec_t	 _rtpipelineSpec;
		RayTracingPipelineSpecMap_t	 _rtpipelineSpecMap;

		PipelineTemplMap_t			_pipelineTemplMap;

		RTShaderBindingTables_t		_shaderBindingTables;

		RenderTechniques_t			_rtech;

		ShaderBytecodeArr_t			_spirvShaders;
		ShaderBytecodeMap_t			_spirvShaderMap;

		ShaderBytecodeArr_t			_metaliOSShaders;
		ShaderBytecodeMap_t			_metaliOSShaderMap;

		ShaderBytecodeArr_t			_metalMacShaders;
		ShaderBytecodeMap_t			_metalMacShaderMap;


	// methods
	public:
		PipelineStorage ();

		ND_ DescrSetUID			AddDescriptorSetLayout (DescriptorSetLayoutDesc);
		ND_ PipelineLayoutUID	AddPipelineLayout (PipelineLayoutDesc);

		ND_ RenderStateUID		AddRenderState (SerializableRenderState);
		ND_ DepthStencilStateUID AddDepthStencilState (SerializableDepthStencilState);

		ND_ Pair<PipelineTemplUID, bool>	AddPipeline (PipelineTmplName::Ref name, SerializableGraphicsPipeline);
		ND_ Pair<PipelineTemplUID, bool>	AddPipeline (PipelineTmplName::Ref name, SerializableMeshPipeline);
		ND_ Pair<PipelineTemplUID, bool>	AddPipeline (PipelineTmplName::Ref name, SerializableComputePipeline);
		ND_ Pair<PipelineTemplUID, bool>	AddPipeline (PipelineTmplName::Ref name, SerializableTilePipeline);
		ND_ Pair<PipelineTemplUID, bool>	AddPipeline (PipelineTmplName::Ref name, SerializableRayTracingPipeline);

		ND_ PipelineSpecUID		AddPipeline (PipelineName::Ref name, SerializableGraphicsPipelineSpec);
		ND_ PipelineSpecUID		AddPipeline (PipelineName::Ref name, SerializableMeshPipelineSpec);
		ND_ PipelineSpecUID		AddPipeline (PipelineName::Ref name, SerializableComputePipelineSpec);
		ND_ PipelineSpecUID		AddPipeline (PipelineName::Ref name, SerializableTilePipelineSpec);
		ND_ PipelineSpecUID		AddPipeline (PipelineName::Ref name, SerializableRayTracingPipelineSpec);

		ND_ RenderTechUID		AddRenderTech (SerializableRenderTechnique);

		ND_ RTShaderBindingUID	AddSBT (SerializableRTShaderBindingTable);

		ND_ ShaderUID			AddSpirvShader (SpirvBytecode_t spirv, const SpecConstants_t &spec);
		ND_ ShaderUID			AddSpirvShader (SpirvWithTrace dbgSpirv, const SpecConstants_t &spec);

		ND_ ShaderUID			AddMsliOSShader (MetalBytecode_t msl, const SpecConstants_t &spec);
		ND_ ShaderUID			AddMslMacShader (MetalBytecode_t msl, const SpecConstants_t &spec);

		ND_ DescriptorSetLayoutDesc const*  GetDescriptorSetLayout (DescrSetUID uid) const;

		ND_ usize	RenderStateCount ()			const	{ return _renderStates.size(); }
		ND_ usize	DepthStencilStateCount ()	const	{ return _dsStates.size(); }

		ND_ usize	DSLayoutCount ()			const	{ return _dsLayouts.size(); }
		ND_ usize	PipelineLayoutCount ()		const	{ return _pplnLayouts.size(); }

		ND_ usize	GraphicsPplnTemplCount ()	const	{ return _gpipelineTempl.size(); }
		ND_ usize	ComputePplnTemplCount ()	const	{ return _cpipelineTempl.size(); }
		ND_ usize	TilePplnTemplCount ()		const	{ return _tpipelineTempl.size(); }
		ND_ usize	MeshPplnTemplCount ()		const	{ return _mpipelineTempl.size(); }
		ND_ usize	RayTracingPplnTemplCount ()	const	{ return _rtpipelineTempl.size(); }

		ND_ usize	GraphicsPplnSpecCount ()	const	{ return _gpipelineSpec.size(); }
		ND_ usize	ComputePplnSpecCount ()		const	{ return _cpipelineSpec.size(); }
		ND_ usize	TilePplnSpecCount ()		const	{ return _tpipelineSpec.size(); }
		ND_ usize	MeshPplnSpecCount ()		const	{ return _mpipelineSpec.size(); }
		ND_ usize	RayTracingPplnSpecCount ()	const	{ return _rtpipelineSpec.size(); }

		ND_ usize	PplnTemplNameCount ()		const	{ return _pipelineTemplMap.size(); }

		ND_ usize	RenderTechCount ()			const	{ return _rtech.size(); }

		ND_ usize	SpirvShaderCount ()			const	{ return _spirvShaders.size(); }
		ND_ usize	MetaliOSShaderCount ()		const	{ return _metaliOSShaders.size(); }
		ND_ usize	MetalMacShaderCount ()		const	{ return _metalMacShaders.size(); }

		ND_ bool	Empty () const;
		ND_ Bytes	CalcAllocationSize (Bytes align) const;

		ND_ bool	SerializePipelines (WStream &) const;
		ND_ bool	WriteShaders (WStream &) const;


	private:
		ND_ bool	_SerializePipelines (ArrayWStream &, OUT BlockOffsets_t &, OUT Bytes &pos) const;

		template <typename UID, typename T, typename ArrType, typename MapType>
		ND_ UID  _Add (T desc, ArrType &arr, MapType &map) __NE___;

		template <typename T>
		ND_ ShaderUID  _AddSpvShader (T spirv, const SpecConstants_t &spec);

		template <typename ArrType, typename MapType>
		ND_ ShaderUID  _AddMslShader (MetalBytecode_t msl, const SpecConstants_t &spec, ShaderUID mask, ArrType &arr, MapType &mapType);

		template <typename PplnType, typename PplnArr, typename PplnMap>
		ND_ Pair<PipelineTemplUID, bool>  _AddPipelineTmpl (PipelineTmplName::Ref name, PplnType, PplnArr&, PplnMap&, PipelineTemplUID uidType);

		template <typename PplnType, typename PplnArr, typename PplnMap>
		ND_ PipelineSpecUID  _AddPipelineSpec (PipelineName::Ref name, PplnType, PplnArr&, PplnMap&, PipelineSpecUID uidType);
	};


} // AE::PipelineCompiler


namespace AE::Base
{
	template <>	struct TTriviallySerializable< PipelineCompiler::SerializableRayTracingPipeline::GeneralShader >		: CT_True {};
	template <>	struct TTriviallySerializable< PipelineCompiler::SerializableRayTracingPipeline::TriangleHitGroup >		: CT_True {};
	template <>	struct TTriviallySerializable< PipelineCompiler::SerializableRayTracingPipeline::ProceduralHitGroup >	: CT_True {};

	template <>	struct TTriviallySerializable< PipelineCompiler::SerializableGraphicsPipeline::VertexAttrib >			: CT_True {};

	template <> struct TTriviallyDestructible< PipelineCompiler::SerializableRenderTechnique::Pass >					: CT_True {};

} // AE::Base


template <>
struct std::hash< AE::PipelineCompiler::PushConstants::PushConst > final :
	AE::Base::DefaultHasher_CalcHash< AE::PipelineCompiler::PushConstants::PushConst >
{};

template <>
struct std::hash< AE::PipelineCompiler::PushConstants > final :
	AE::Base::DefaultHasher_CalcHash< AE::PipelineCompiler::PushConstants >
{};

template <>
struct std::hash< AE::PipelineCompiler::DescriptorSetLayoutDesc > final :
	AE::Base::DefaultHasher_CalcHash< AE::PipelineCompiler::DescriptorSetLayoutDesc >
{};

template <>
struct std::hash< AE::PipelineCompiler::PipelineLayoutDesc::DescSetLayout > final :
	AE::Base::DefaultHasher_CalcHash< AE::PipelineCompiler::PipelineLayoutDesc::DescSetLayout >
{};

template <>
struct std::hash< AE::PipelineCompiler::SpirvWithTrace > {
	ND_ size_t  operator () (const AE::PipelineCompiler::SpirvWithTrace &x) const {
		return size_t(AE::Base::HashOf( x.bytecode ));
	}
};

template <>
struct std::hash< AE::PipelineCompiler::SerializableGraphicsPipeline::VertexAttrib > {
	size_t  operator () (const AE::PipelineCompiler::SerializableGraphicsPipeline::VertexAttrib &x) const {
		return size_t(AE::Base::HashOf(x.type) + AE::Base::HashOf(x.index));
	}
};
