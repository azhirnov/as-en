// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "glslang/Include/ResourceLimits.h"

#include "Packer/PipelinePack.h"
#include "Packer/RenderPassPack.h"
#include "Compiler/IShaderPreprocessor.h"

class TIntermNode;

namespace glslang
{
	class TType;
	class TIntermediate;
}

namespace AE::PipelineCompiler
{

	//
	// SPIRV Compiler
	//

	class SpirvCompiler final : public NothrowAllocatable
	{
	// types
	private:
		struct GLSLangResult;

	public:
		struct ShaderReflection
		{
		// types
			using FragmentIO_t		= SubpassShaderIO;
			using TopologyBits_t	= SerializableGraphicsPipeline::TopologyBits_t;
			using SpecConstants_t	= ShaderBytecode::SpecConstants_t;
			using VertexAttrib		= SerializableGraphicsPipeline::VertexAttrib;

			struct DescriptorSet
			{
			// variables
				uint						bindingIndex	= UMax;
				DescriptorSetName			name;
				DescriptorSetLayoutDesc		layout;

			// methods
				DescriptorSet ()									__NE___	= default;

				ND_ bool	operator == (const DescriptorSet &rhs)	C_NE___;
				ND_ HashVal	CalcHash ()								C_NE___;
			};
			using DescSets_t = FixedArray< DescriptorSet, PipelineLayoutDesc::DescrSetMap_t::capacity() >;


		// variables
			struct {
				DescSets_t			descrSets;
				PushConstants		pushConstants;
				SpecConstants_t		specConstants;
			}					layout;

			struct {
				TopologyBits_t		supportedTopology;
				Array<VertexAttrib>	vertexAttribs;
			}					vertex;

			struct {
				uint				patchControlPoints	= 0;
			}					tessellation;

			struct {
				FragmentIO_t		fragmentIO;		// color/input attachments
				bool				earlyFragmentTests	= true;
			}					fragment;

			struct {
				uint3				localGroupSize	{0};
				uint3				localGroupSpec	{~0u};
			}					compute;

			struct {
				uint3				taskGroupSize			{0};		// 0 if not defined
				uint3				taskGroupSpec			{~0u};		// UMax if has specialization
				uint3				meshGroupSize			{0};		// 0 if not defined
				uint3				meshGroupSpec			{~0u};		// UMax if has specialization
				EPrimitive			topology				= Default;
				uint				maxPrimitives			= 0;
				uint				maxIndices				= 0;
				uint				maxVertices				= 0;
			}					mesh;


		// methods
			ND_ bool	operator == (const ShaderReflection &rhs)	C_NE___;
			ND_ HashVal	CalcHash ()									C_NE___;
		};

		struct Input
		{
			EShader			shaderType			= Default;
			Version2		spirvVersion;
			uint			dbgDescSetIdx		= UMax;
			EShaderOpt		options				= Default;
			StringView		entry;
			StringView		header;
			StringView		source;
			PathAndLine		fileLoc;						// only for output parser
			bool			shaderSubgroupClock	= false;	// \__ only for shader profiling
			bool			shaderDeviceClock	= false;	// /
		};

		struct Output
		{
			ShaderReflection		reflection;
			SpirvBytecode_t			spirv;
			Unique<ShaderTrace>		trace;
			String					log;

			Output ();
			~Output ();
		};


	private:
		class  ShaderIncluder;

		using PushConstant		= PushConstants::PushConst;
		using TopologyBits_t	= SerializableGraphicsPipeline::TopologyBits_t;
		using VertexAttrib		= SerializableGraphicsPipeline::VertexAttrib;

		using IncludeDirsRef_t	= Array< Path > const &;


	// variables
	private:
		Array< Path >					_directories;

		glslang::TIntermediate *		_intermediate		= null;
		EShaderStages					_currentStage		= Default;
		EShader							_currentShader		= Default;
		uint							_spirvTraget		= 0;		// spv_target_env

		TBuiltInResource				_builtinResource;
		Unique<IShaderPreprocessor>		_preprocessor;

		static constexpr bool			_quietWarnings		= true;


	// methods
	public:
		explicit SpirvCompiler (ArrayView<Path> includeDirs) __NE___;
		~SpirvCompiler ();

			bool  SetDefaultResourceLimits ();
			void  SetPreprocessor (IShaderPreprocessor*);

		ND_ bool  BuildReflection (const Input &in, OUT ShaderReflection &outReflection, OUT String &log);
		ND_ bool  Compile (const Input &in, OUT Output &out);


	private:
		ND_ bool  _ParseGLSL (const Input &in, INOUT ShaderIncluder &includer, OUT GLSLangResult &glslangData, INOUT String &log);
		ND_ bool  _CompileSPIRV (const GLSLangResult &glslangData, EShaderOpt options, OUT SpirvBytecode_t &spirv, INOUT String &log) const;
		ND_ bool  _OptimizeSPIRV (INOUT SpirvBytecode_t &spirv, INOUT String &log) const;
		ND_ bool  _DisassembleSPIRV (const SpirvBytecode_t &spirv, OUT String &outDisasm) const;
		ND_ bool  _ValidateSPIRV (const SpirvBytecode_t &spirv, INOUT String &log) const;

		ND_ bool  _BuildReflection (const GLSLangResult &glslangData, OUT ShaderReflection &reflection);

			void  _OnCompilationFailed (const Input &in, const ShaderIncluder &includer, INOUT String &log) const;

		static void  _GenerateResources (OUT TBuiltInResource& res);


	// GLSL deserializer
	private:
		ND_ bool  _ProcessExternalObjects (TIntermNode* root, TIntermNode* node, INOUT ShaderReflection &result) const;

		ND_ bool  _DeserializeExternalObjects (TIntermNode* node, INOUT ShaderReflection &result) const;

		ND_ bool  _ProcessShaderInfo (INOUT ShaderReflection &result) const;

		ND_ bool  _CalculateStructSize (const glslang::TType &bufferType, OUT Bytes32u &staticSize, OUT Bytes32u &arrayStride, OUT Bytes32u &offset) const;

			void  _MergeWithGeometryInputPrimitive (INOUT TopologyBits_t &topologyBits, /*TLayoutGeometry*/uint type) const;

		ND_ ShaderReflection::DescriptorSet&  _GetDescriptorSet (uint dsIndex, INOUT SpirvCompiler::ShaderReflection &reflection) const;

		ND_ EImageType		_ExtractImageType (const glslang::TType &type) const;
		ND_ EVertexType		_ExtractVertexType (const glslang::TType &type) const;
		ND_ EPixelFormat	_ExtractImageFormat (/*TLayoutFormat*/uint format) const;
		ND_ EShaderIO		_ExtractShaderIOType (const glslang::TType &type) const;
	};


} // AE::PipelineCompiler
