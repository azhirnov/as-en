// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Remote/Descriptors/RDescriptorUpdater.h"
# include "graphics_rhi/Remote/RRenderTaskScheduler.h"
# include "graphics_rhi/Private/EnumToString.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	constructor
=================================================
*/
	DescriptorUpdater::DescriptorUpdater () __NE___ :
		_resMngr{GraphicsScheduler().GetResourceManager()}
	{}

/*
=================================================
	destructor
=================================================
*/
	DescriptorUpdater::~DescriptorUpdater () __NE___
	{
		_resMngr.ImmediatelyRelease( INOUT _descSetId );
	}

/*
=================================================
	Set
=================================================
*/
	bool  DescriptorUpdater::Set (DescriptorSetID descrSetId, EDescUpdateMode mode) __NE___
	{
		DRC_EXLOCK( _drCheck );

		// release previous DS
		_resMngr.ImmediatelyRelease( INOUT _descSetId );
		_dsLayout	= null;

		// acquire new DS
		auto*	desc_set = _resMngr.GetResource( descrSetId, True{"incRef"}, True{"quiet"} );
		CHECK_ERR( desc_set != null );

		_descSetId.Attach( descrSetId );

		auto*	ds_layout = _resMngr.GetResource( desc_set->LayoutId(), False{"don't inc ref"}, True{"quiet"} );
		CHECK_ERR( ds_layout != null );
		_dsLayout = ds_layout;

		if ( not _ser )
		{
			_memStream = MakeRC<ArrayWStream>();
			_ser.reset( new Serializing::Serializer{ _memStream });
			_ser->factory = &_resMngr.GetDevice().GetDescUpdaterFactory();
			_descCount = 0;
		}

		Msg::DescUpd_Flush::SetDescSet	cmd;
		cmd.descSet	= desc_set->Handle();
		cmd.mode	= mode;
		CHECK_ERR( _AddCommand( cmd ));

		return true;
	}

/*
=================================================
	Flush
=================================================
*/
	bool  DescriptorUpdater::Flush () __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _dsLayout == null )
			return true;

		_resMngr.ImmediatelyRelease( INOUT _descSetId );

		_ser.reset();
		CHECK_ERR( _memStream );

		Msg::UploadData				msg1;
		Msg::DescUpd_Flush			msg2;
		RC<Msg::DefaultResponse>	res;

		msg1.size	= _memStream->Position();
		msg1.data	= _memStream->GetData().data();
		msg2.count	= _descCount;

		CHECK_ERR( _resMngr.GetDevice().SendAndWait( List<Msg::BaseMsg*>{&msg1, &msg2}, OUT res ));

		_memStream	= null;
		_descCount	= 0;

		return res->ok;
	}

/*
=================================================
	_FindUniform
=================================================
*/
	template <EDescriptorType DescType>
	const DescriptorUpdater::Uniform_t*  DescriptorUpdater::_FindUniform (UniformName::Ref name) C_NE___
	{
		const auto	uniforms	= _dsLayout->GetUniformRange<DescType>();
		const usize	count		= uniforms.template Get<0>();
		auto		un_names	= ArrayView<UniformName::Optimized_t>{ uniforms.template Get<1>(), count };
		usize		index		= BinarySearch( un_names, UniformName::Optimized_t{name} );

		if_likely( index < count )
		{
			return uniforms.template Get<2>() + index;
		}

		#if not AE_OPTIMIZE_IDS
		{
			String	str;
			str << "Can't find uniform '" << name.GetName() << "'\nAvailable uniforms for type " << ToString( DescType, 0 ) << ": ";

			for (auto un : un_names) {
				str << "'" << _resMngr.HashToName( un ) << "', ";
			}
			str.pop_back();
			str.pop_back();
			AE_LOGE( str );
		}
		#endif
		return null;
	}

/*
=================================================
	_GetArraySize
=================================================
*/
	template <EDescriptorType DescType>
	uint  DescriptorUpdater::_GetArraySize (UniformName::Ref name) C_NE___
	{
		auto*	un = _FindUniform< DescType >( name );
		return un->arraySize;
	}

	uint  DescriptorUpdater::ImageCount (UniformName::Ref name) C_NE___
	{
		return _GetArraySize<DT::StorageImage>( name );
	}

	uint  DescriptorUpdater::TextureCount (UniformName::Ref name) C_NE___
	{
		return _GetArraySize<DT::StorageImage>( name );
	}

	uint  DescriptorUpdater::SamplerCount (UniformName::Ref name) C_NE___
	{
		return _GetArraySize<DT::Sampler>( name );
	}

	uint  DescriptorUpdater::BufferCount (UniformName::Ref name) C_NE___
	{
		return _GetArraySize<DT::UniformBuffer>( name );
	}

	uint  DescriptorUpdater::TexelBufferCount (UniformName::Ref name) C_NE___
	{
		return _GetArraySize<DT::UniformTexelBuffer>( name );
	}

	uint  DescriptorUpdater::RayTracingSceneCount (UniformName::Ref name) C_NE___
	{
		return _GetArraySize<DT::RayTracingScene>( name );
	}

	uint  DescriptorUpdater::RayTracingPartitionedSceneCount (UniformName::Ref name) C_NE___
	{
		return _GetArraySize<DT::RayTracingPartitionedScene>( name );
	}

/*
=================================================
	BindImage
=================================================
*/
	bool  DescriptorUpdater::BindImage (UniformName::Ref name, ImageViewID image, uint elementIndex) __NE___
	{
		return BindImages( name, {image}, elementIndex );
	}

	bool  DescriptorUpdater::BindImages (UniformName::Ref name, ArrayView<ImageViewID> images, uint firstIndex) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	un = _FindUniform<DT::StorageImage>( name );

		CHECK_ERR( un != null );
		CHECK_ERR( AnyEqual( un->type, DT::StorageImage, DT::SampledImage, DT::CombinedImage_ImmutableSampler, DT::SubpassInput ));
		CHECK_ERR( firstIndex + uint(images.size()) <= un->arraySize );

		Msg::DescUpd_Flush::BindImages		msg;
		msg.images.resize( images.size() );

		for (usize i = 0; i < images.size(); ++i)
		{
			auto*	view = _resMngr.GetResource( images[i] );
			CHECK_ERR( view != null );
			msg.images[i] = view->Handle();
		}

		msg.unName		= name;
		msg.firstIndex	= ushort(firstIndex);

		return _AddCommand( msg );
	}

/*
=================================================
	BindVideoImage
=================================================
*/
	bool  DescriptorUpdater::BindVideoImage (UniformName::Ref name, VideoImageID image, uint elementIndex) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	res = _resMngr.GetResource( image );
		CHECK_ERR( res != null );

		Msg::DescUpd_Flush::BindVideoImage		msg;

		msg.unName			= name;
		msg.firstIndex		= ushort(elementIndex);
		msg.videoImageId	= res->Handle();

		return _AddCommand( msg );
	}

/*
=================================================
	BindTexture
=================================================
*/
	bool  DescriptorUpdater::BindTexture (UniformName::Ref name, ImageViewID image, SamplerName::Ref sampler, uint elementIndex) __NE___
	{
		return BindTextures( name, {image}, sampler, elementIndex );
	}

	bool  DescriptorUpdater::BindTextures (UniformName::Ref name, ArrayView<ImageViewID> images, SamplerName::Ref sampler, uint firstIndex) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	un = _FindUniform<DT::StorageImage>( name );

		CHECK_ERR( un != null );
		CHECK_ERR( un->type == DT::CombinedImage );
		CHECK_ERR( firstIndex + uint(images.size()) <= un->arraySize );

		Msg::DescUpd_Flush::BindTextures	msg;
		msg.images.resize( images.size() );

		for (usize i = 0; i < images.size(); ++i)
		{
			auto*	view = _resMngr.GetResource( images[i] );
			CHECK_ERR( view != null );
			msg.images[i] = view->Handle();
		}

		msg.unName		= name;
		msg.firstIndex	= ushort(firstIndex);
		msg.sampler		= sampler;

		return _AddCommand( msg );
	}

/*
=================================================
	BindSampler
=================================================
*/
	bool  DescriptorUpdater::BindSampler (UniformName::Ref name, SamplerName::Ref sampler, uint elementIndex) __NE___
	{
		return BindSamplers( name, {&sampler, 1}, elementIndex );
	}

	bool  DescriptorUpdater::BindSamplers (UniformName::Ref name, ArrayView<SamplerName> samplers, uint firstIndex) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	un = _FindUniform<DT::Sampler>( name );

		CHECK_ERR( un != null );
		CHECK_ERR( un->type == DT::Sampler );
		CHECK_ERR( firstIndex + uint(samplers.size()) <= un->arraySize );

		Msg::DescUpd_Flush::BindSamplers	msg;
		msg.samplers.resize( samplers.size() );

		for (usize i = 0; i < samplers.size(); ++i) {
			msg.samplers[i] = samplers[i];
		}

		msg.unName		= name;
		msg.firstIndex	= ushort(firstIndex);

		return _AddCommand( msg );
	}

/*
=================================================
	BindBuffer
=================================================
*/
	bool  DescriptorUpdater::BindBuffer (UniformName::Ref name, ShaderStructName::Ref typeName, BufferID buffer, Bytes offset, Bytes size, uint elementIndex) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	un = _FindUniform<DT::UniformBuffer>( name );

		CHECK_ERR( un != null );
		CHECK_ERR( AnyEqual( un->type, DT::UniformBuffer, DT::StorageBuffer ));
		CHECK_ERR( elementIndex + 1 <= un->arraySize );
		CHECK_ERR( typeName == Default or typeName == un->buffer.typeName );

		auto*	buf = _resMngr.GetResource( buffer );
		CHECK_ERR( buf != null );

		Msg::DescUpd_Flush::BindBufferRange	msg;

		msg.unName	= name;
		msg.index	= ushort(elementIndex);
		msg.buffer	= buf->Handle();
		msg.offset	= offset;
		msg.size	= size;

		return _AddCommand( msg );
	}

/*
=================================================
	BindBuffers
=================================================
*/
	bool  DescriptorUpdater::BindBuffer (UniformName::Ref name, ShaderStructName::Ref typeName, BufferID buffer, uint elementIndex) __NE___
	{
		return BindBuffers( name, typeName, {buffer}, elementIndex );
	}

	bool  DescriptorUpdater::BindBuffers (UniformName::Ref name, ShaderStructName::Ref typeName, ArrayView<BufferID> buffers, uint firstIndex) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	un = _FindUniform<DT::UniformBuffer>( name );

		CHECK_ERR( un != null );
		CHECK_ERR( AnyEqual( un->type, DT::UniformBuffer, DT::StorageBuffer ));
		CHECK_ERR( firstIndex + uint(buffers.size()) <= un->arraySize );
		CHECK_ERR( not un->buffer.HasDynamicOffset() );		// set explicit size
		CHECK_ERR( typeName == Default or typeName == un->buffer.typeName );

		Msg::DescUpd_Flush::BindBuffers	msg;
		msg.buffers.resize( buffers.size() );

		for (usize i = 0; i < buffers.size(); ++i)
		{
			auto*	buf = _resMngr.GetResource( buffers[i] );
			CHECK_ERR( buf != null );
			msg.buffers[i] = buf->Handle();
		}

		msg.unName		= name;
		msg.firstIndex	= ushort(firstIndex);

		return _AddCommand( msg );
	}

/*
=================================================
	BindTexelBuffer
=================================================
*/
	bool  DescriptorUpdater::BindTexelBuffer (UniformName::Ref name, BufferViewID view, uint elementIndex) __NE___
	{
		return BindTexelBuffers( name, {view}, elementIndex );
	}

	bool  DescriptorUpdater::BindTexelBuffers (UniformName::Ref name, ArrayView<BufferViewID> views, uint firstIndex) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	un = _FindUniform<DT::UniformTexelBuffer>( name );

		CHECK_ERR( un != null );
		CHECK_ERR( AnyEqual( un->type, DT::UniformTexelBuffer, DT::StorageTexelBuffer ));
		CHECK_ERR( firstIndex + uint(views.size()) <= un->arraySize );

		Msg::DescUpd_Flush::BindTexelBuffers	msg;
		msg.views.resize( views.size() );

		for (usize i = 0; i < views.size(); ++i)
		{
			auto*	view = _resMngr.GetResource( views[i] );
			CHECK_ERR( view != null );
			msg.views[i] = view->Handle();
		}

		msg.unName		= name;
		msg.firstIndex	= ushort(firstIndex);

		return _AddCommand( msg );
	}

/*
=================================================
	BindRayTracingScene
=================================================
*/
	bool  DescriptorUpdater::BindRayTracingScene (UniformName::Ref name, RTSceneID scene, uint elementIndex) __NE___
	{
		return BindRayTracingScenes( name, {scene}, elementIndex );
	}

	bool  DescriptorUpdater::BindRayTracingScenes (UniformName::Ref name, ArrayView<RTSceneID> scenes, uint firstIndex) __NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	un = _FindUniform<DT::RayTracingScene>( name );

		CHECK_ERR( un != null );
		CHECK_ERR( AnyEqual( un->type, DT::RayTracingScene ));
		CHECK_ERR( firstIndex + uint(scenes.size()) <= un->arraySize );

		Msg::DescUpd_Flush::BindRayTracingScenes	msg;
		msg.scenes.resize( scenes.size() );

		for (usize i = 0; i < scenes.size(); ++i)
		{
			auto*	as = _resMngr.GetResource( scenes[i] );
			CHECK_ERR( as != null );
			msg.scenes[i] = as->Handle();
		}

		msg.unName		= name;
		msg.firstIndex	= ushort(firstIndex);

		return _AddCommand( msg );
	}

/*
=================================================
	BindRayTracingPartitionedScene
=================================================
*/
	bool  DescriptorUpdater::BindRayTracingPartitionedScene (UniformName::Ref, DeviceAddress, uint) __NE___
	{
		UNTESTED;
		return false;
	}

	bool  DescriptorUpdater::BindRayTracingPartitionedScene (UniformName::Ref, BufferID, Bytes, uint) __NE___
	{
		UNTESTED;
		return false;
	}

/*
=================================================
	GetBufferStructName
=================================================
*/
	ShaderStructName  DescriptorUpdater::GetBufferStructName (UniformName::Ref name) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		auto*	un = _FindUniform<DT::UniformBuffer>( name );
		CHECK_ERR( un != null );

		return un->buffer.typeName;
	}

/*
=================================================
	_AddCommand
=================================================
*/
	inline bool  DescriptorUpdater::_AddCommand (const RemoteGraphics::Msg::DescUpd_Flush::BaseUpdCmd &msg) __NE___
	{
		CHECK_ERR( _ser );
		++_descCount;
		return (*_ser)( &msg );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
