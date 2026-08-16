// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef AE_ENABLE_REMOTE_GRAPHICS
# include "graphics_rhi/Private/ResourceValidation.h"
# include "graphics_rhi/Remote/Resources/RBuffer.h"
# include "graphics_rhi/Remote/RResourceManager.h"
# include "graphics_rhi/Remote/REnumCast.h"

namespace AE::Graphics
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	destructor
=================================================
*/
	RBuffer::~RBuffer () __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( not _bufferId );
	}

/*
=================================================
	Create
=================================================
*/
	bool  RBuffer::Create (ResourceManager &resMngr, const BufferDesc &desc, GfxMemAllocatorPtr allocator, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _bufferId );
		CHECK_ERR( desc.size > 0 );
		CHECK_ERR( desc.usage != Default );

		_desc = desc;
		_desc.Validate();
		GRES_CHECK( Buffer_IsSupported( resMngr, _desc ));

		auto&	dev = resMngr.GetDevice();

		Msg::ResMngr_CreateBuffer				msg;
		RC<Msg::ResMngr_CreateBuffer_Response>	res;

		msg.desc		= _desc;
		msg.gfxAlloc	= dev.Cast( allocator );
		msg.dbgName		= dbgName;

		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		_bufferId	= res->bufferId;
		_memoryId	= res->memoryId;
		_address	= res->addr;
		_desc		= res->desc;
		_devicePtr	= res->mapped;
		_releaseRef	= true;
		CHECK_ERR( _bufferId );

		if ( _devicePtr != Default ) {
			_hostMem = SharedMem::Create( AE::GetDefaultAllocator(), _desc.size );
			CHECK_ERR( _hostMem );
		}

		_allocator = RVRef(allocator);
		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Create
=================================================
*/
	bool  RBuffer::Create (ResourceManager &resMngr, const RemoteBufferDesc &desc, GfxMemAllocatorPtr, StringView dbgName) __NE___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERR( not _bufferId );

		_desc = desc.desc;
		_desc.Validate();
		GRES_CHECK( Buffer_IsSupported( resMngr, _desc ));

		_bufferId	= desc.bufferId;
		_address	= desc.address;
		_devicePtr	= desc.devicePtr;
		_hostMem	= desc.hostMem;
		_releaseRef	= desc.releaseRef;
		CHECK_ERR( _bufferId );

		if ( _devicePtr != Default and not _hostMem ) {
			_hostMem = SharedMem::Create( AE::GetDefaultAllocator(), _desc.size );
			CHECK_ERR( _hostMem );
		}

		if ( _hostMem )
			CHECK_GE( _hostMem->Size(), _desc.size );

		GFX_DBG_ONLY( _debugName = dbgName; )
		return true;
	}

/*
=================================================
	Destroy
=================================================
*/
	void  RBuffer::Destroy (ResourceManager &resMngr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		if ( _bufferId and _releaseRef )
			CHECK( resMngr.GetDevice().Send( Msg::ResMngr_ReleaseResource{_bufferId} ));

		_bufferId	= Default;
		_memoryId	= Default;
		_address	= Default;
		_desc		= Default;
		_allocator	= null;
		_devicePtr	= Default;
		_hostMem	= null;

		GFX_DBG_ONLY( _debugName.clear() );
	}

/*
=================================================
	GetMemoryInfo
=================================================
*/
	bool  RBuffer::GetMemoryInfo (OUT RemoteMemoryObjInfo &info) C_NE___
	{
		DRC_SHAREDLOCK( _drCheck );

		info.devicePtr	= _devicePtr;
		info.mappedPtr	= (_hostMem ? _hostMem->Data() : null);
		return true;
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  RBuffer::IsSupported (const ResourceManager &resMngr, const BufferDesc &desc) __NE___
	{
		if ( not Buffer_IsSupported( resMngr, desc ))
			return false;

		Msg::ResMngr_IsSupported_BufferDesc		msg;
		RC<Msg::ResMngr_IsSupported_Response>	res;

		msg.desc = desc;

		CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));
		return res->supported;
	}

/*
=================================================
	IsSupported
=================================================
*/
	bool  RBuffer::IsSupported (const ResourceManager &resMngr, const BufferDesc &desc, const BufferViewDesc &viewDesc) __NE___
	{
		if ( not BufferView_IsSupported( resMngr, desc, viewDesc ))
			return false;

		Msg::ResMngr_IsSupported_BufferViewDesc		msg;
		RC<Msg::ResMngr_IsSupported_Response>		res;

		msg.bufDesc		= desc;
		msg.viewDesc	= viewDesc;

		CHECK_ERR( resMngr.GetDevice().SendAndWait( msg, OUT res ));
		return res->supported;
	}

/*
=================================================
	IsSupportedForVertex
=================================================
*/
	bool  RBuffer::IsSupportedForVertex (const ResourceManager &resMngr, EVertexType type) __NE___
	{
		return resMngr.GetFeatureSet().vertexFormats.contains( type );
	}

/*
=================================================
	IsSupportedForASVertex
=================================================
*/
	bool  RBuffer::IsSupportedForASVertex (const ResourceManager &resMngr, EVertexType type) __NE___
	{
		return resMngr.GetFeatureSet().accelStructVertexFormats.contains( type );
	}


} // AE::Graphics

#endif // AE_ENABLE_REMOTE_GRAPHICS
