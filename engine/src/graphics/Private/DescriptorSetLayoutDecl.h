// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'


	// types
	private:
		using Image				= PipelineCompiler::DescriptorSetLayoutDesc::Image;
		using Buffer			= PipelineCompiler::DescriptorSetLayoutDesc::Buffer;
		using TexelBuffer		= PipelineCompiler::DescriptorSetLayoutDesc::TexelBuffer;
		using Sampler			= PipelineCompiler::DescriptorSetLayoutDesc::Sampler;
		using ImmutableSampler	= PipelineCompiler::DescriptorSetLayoutDesc::ImmutableSampler;
		using ImageWithSampler	= PipelineCompiler::DescriptorSetLayoutDesc::ImageWithSampler;
		using SubpassInput		= PipelineCompiler::DescriptorSetLayoutDesc::SubpassInput;
		using RayTracingScene	= PipelineCompiler::DescriptorSetLayoutDesc::RayTracingScene;

		// uniforms are sorted by types, this array map desc type to uniform offset to speedup search
		using UniformOffsets_t = StaticArray< ushort, 6 >;
		

	// variables
	private:
		EDescSetUsage				_usage			= Default;
		UniformOffsets_t			_unOffsets;
		Uniforms_t					_uniforms;		// allocated by pipeline pack linear allocator

		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		template <EDescriptorType DescType>
		ND_ Uniforms_t					GetUniformRange ()		C_NE___;
		
		ND_ Uniforms_t const&			GetUniforms ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _uniforms; }

		DEBUG_ONLY(  ND_ StringView		GetDebugName ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })


	private:
				void  _UpdateUniformOffsets ()					__NE___;
		static	void  _ValidateUniforms (const CreateInfo &ci)	__NE___;

			
//-----------------------------------------------------------------------------
