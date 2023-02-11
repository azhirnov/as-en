// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_VULKAN)
#	define DESCSETLAYOUT	VDescriptorSetLayout
#elif defined(AE_ENABLE_METAL)
#	define DESCSETLAYOUT	MDescriptorSetLayout
#else
#	error not implemented
#endif

/*
=================================================
	GetUniformRange
=================================================
*/
	template <EDescriptorType DescType>
	DESCSETLAYOUT::Uniforms_t  DESCSETLAYOUT::GetUniformRange () C_NE___
	{
		const bool	has_upd_tmpl = _uniforms.Get<3>() != null;

		BEGIN_ENUM_CHECKS();
		switch ( DescType )
		{
			case EDescriptorType::UniformBuffer :
			case EDescriptorType::StorageBuffer :
				return Uniforms_t( _unOffsets[1] - _unOffsets[0],
								   _uniforms.Get<1>() + _unOffsets[0],
								   _uniforms.Get<2>() + _unOffsets[0],
								   has_upd_tmpl ? _uniforms.Get<3>() + _unOffsets[0] : null );
		
			case EDescriptorType::UniformTexelBuffer :
			case EDescriptorType::StorageTexelBuffer :
				return Uniforms_t( _unOffsets[2] - _unOffsets[1],
								   _uniforms.Get<1>() + _unOffsets[1],
								   _uniforms.Get<2>() + _unOffsets[1],
								   has_upd_tmpl ? _uniforms.Get<3>() + _unOffsets[1] : null );
		
			case EDescriptorType::StorageImage :
			case EDescriptorType::SampledImage :
			case EDescriptorType::CombinedImage :
			case EDescriptorType::CombinedImage_ImmutableSampler :
			case EDescriptorType::SubpassInput :
				return Uniforms_t( _unOffsets[3] - _unOffsets[2],
								   _uniforms.Get<1>() + _unOffsets[2],
								   _uniforms.Get<2>() + _unOffsets[2],
								   has_upd_tmpl ? _uniforms.Get<3>() + _unOffsets[2] : null );

			case EDescriptorType::Sampler :
				return Uniforms_t( _unOffsets[4] - _unOffsets[3],
								   _uniforms.Get<1>() + _unOffsets[3],
								   _uniforms.Get<2>() + _unOffsets[3],
								   has_upd_tmpl ? _uniforms.Get<3>() + _unOffsets[3] : null );

			case EDescriptorType::RayTracingScene :
				return Uniforms_t( _unOffsets[5] - _unOffsets[4],
								   _uniforms.Get<1>() + _unOffsets[4],
								   _uniforms.Get<2>() + _unOffsets[4],
								   has_upd_tmpl ? _uniforms.Get<3>() + _unOffsets[4] : null );

			case EDescriptorType::ImmutableSampler :
			case EDescriptorType::_Count :
			case EDescriptorType::Unknown :
			default :
				return Uniforms_t{};
		}
		END_ENUM_CHECKS();
	}
	
/*
=================================================
	_UpdateUniformOffsets
=================================================
*/
	inline void  DESCSETLAYOUT::_UpdateUniformOffsets () __NE___
	{
		_unOffsets.fill( UMax );

		EDescriptorType	prev_type = EDescriptorType::Unknown;

		for (uint i = 0, cnt = _uniforms.Get<0>(); i < cnt; ++i)
		{
			auto	next_type = _uniforms.Get<2>()[i].type;
			
			if_likely( prev_type == next_type )
				continue;

			prev_type = next_type;

			BEGIN_ENUM_CHECKS();
			switch ( next_type )
			{
				case EDescriptorType::UniformBuffer :
				case EDescriptorType::StorageBuffer :		_unOffsets[0] = ushort(i);	break;
		
				case EDescriptorType::UniformTexelBuffer :
				case EDescriptorType::StorageTexelBuffer :	_unOffsets[1] = ushort(i);	break;
		
				case EDescriptorType::StorageImage :
				case EDescriptorType::SampledImage :
				case EDescriptorType::CombinedImage :
				case EDescriptorType::CombinedImage_ImmutableSampler :
				case EDescriptorType::SubpassInput :		_unOffsets[2] = ushort(i);	break;

				case EDescriptorType::Sampler :				_unOffsets[3] = ushort(i);	break;

				case EDescriptorType::RayTracingScene :		_unOffsets[4] = ushort(i);	break;

				case EDescriptorType::ImmutableSampler :
				case EDescriptorType::_Count :
				case EDescriptorType::Unknown :				break;
			}
			END_ENUM_CHECKS();
		}

		_unOffsets[5] = ushort(_uniforms.Get<0>());

		ushort	prev_off = 0;
		for (auto& off : Reverse(_unOffsets))
		{
			off			= (off == UMax ? prev_off : off);
			prev_off	= off;
		}
	}
	
/*
=================================================
	_ValidateUniforms
=================================================
*/
	inline void  DESCSETLAYOUT::_ValidateUniforms (const CreateInfo &ci) __NE___
	{
		DEBUG_ONLY(
		for (uint i = 1, cnt = ci.uniforms.Get<0>(); i < cnt; ++i)
		{
			UniformName::Optimized_t	prev_name	= ci.uniforms.Get<1>()[i-1];
			EDescriptorType				prev_type	= ci.uniforms.Get<2>()[i-1].type;
			
			UniformName::Optimized_t	next_name	= ci.uniforms.Get<1>()[i];
			EDescriptorType				next_type	= ci.uniforms.Get<2>()[i].type;

			ASSERT( prev_type != next_type ? prev_type < next_type : prev_name < next_name );
		})
		Unused( ci );
	}
//-----------------------------------------------------------------------------

#undef DESCSETLAYOUT
