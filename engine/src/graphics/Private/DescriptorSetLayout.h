// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'


	// types
	private:
		using Image				= PipelineCompiler::DescriptorSetLayoutDesc::Image;
		using Buffer			= PipelineCompiler::DescriptorSetLayoutDesc::Buffer;
		using TexelBuffer		= PipelineCompiler::DescriptorSetLayoutDesc::TexelBuffer;
		using Sampler			= PipelineCompiler::DescriptorSetLayoutDesc::Sampler;
		using ImmutableSampler	= PipelineCompiler::DescriptorSetLayoutDesc::ImmutableSampler;
		using RayTracingScene	= PipelineCompiler::DescriptorSetLayoutDesc::RayTracingScene;
		

	// variables
	private:
		EDescSetUsage				_usage			= Default;
		UniformOffsets_t			_unOffsets		{};
		Uniforms_t					_uniforms;		// allocated by pipeline pack linear allocator

		DEBUG_ONLY(	DebugName_t		_debugName;	)
		DRC_ONLY(	RWDataRaceCheck	_drCheck;	)


	// methods
	public:
		ND_ Uniforms_t const&			GetUniforms ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _uniforms; }

		DEBUG_ONLY(  ND_ StringView		GetDebugName ()			C_NE___	{ DRC_SHAREDLOCK( _drCheck );  return _debugName; })

		template <EDescriptorType DescType>
		ND_ Uniforms_t					GetUniformRange ()		C_NE___
		{
			STATIC_ASSERT(	DescType != EDescriptorType::ImmutableSampler and
							DescType <  EDescriptorType::_Count );

			const bool		has_upd_tmpl = _uniforms.Get<3>() != null;
			constexpr uint	idx			 = EDescriptorType_ToIndex( DescType );

			return Uniforms_t(	_unOffsets[idx+1] - _unOffsets[idx],
								_uniforms.Get<1>() + _unOffsets[idx],
								_uniforms.Get<2>() + _unOffsets[idx],
								has_upd_tmpl ? _uniforms.Get<3>() + _unOffsets[idx] : null );
		}
			
//-----------------------------------------------------------------------------
