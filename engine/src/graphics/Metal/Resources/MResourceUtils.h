// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/MetalTypes.h"
#include "graphics/Public/BufferDesc.h"
#include "graphics/Public/ImageDesc.h"

#include "graphics/Metal/MEnumCast.h"
#include "graphics/Metal/MInternal.h"

namespace AE::Graphics
{

	//
	// Metal Resource Utils
	//

	struct MResourceUtils : Noninstancable
	{
	public:

	// Identifying the Resource //
		template <EMetalObjType ID>	ND_ static id<MTLDevice>	GetDevice (MetalObject<ID> obj);
		template <EMetalObjType ID>	ND_ static NS::String		GetLabel (MetalObject<ID> obj);
		template <EMetalObjType ID>		static void				SetLabel (MetalObject<ID> obj, const NS::String &value);
		template <EMetalObjType ID>		static void				SetLabel (MetalObject<ID> obj, StringView value);
		
	// Reading Memory and Storage Properties //
		template <EMetalObjType ID>	ND_ static MTLCPUCacheMode 			GetCpuCacheMode (MetalObject<ID> obj);
		template <EMetalObjType ID>	ND_ static MTLStorageMode			GetStorageMode (MetalObject<ID> obj);
		template <EMetalObjType ID>	ND_ static MTLHazardTrackingMode	GetHazardTrackingMode (MetalObject<ID> obj);
		template <EMetalObjType ID>	ND_ static MTLResourceOptions		GetResourceOptions (MetalObject<ID> obj);

	// Setting the Purgeable State of the Resource //
		template <EMetalObjType ID>	ND_	static MTLPurgeableState		SetPurgeableState (MetalObject<ID> obj, MTLPurgeableState state);

	// Managing Heap Resources //
		template <EMetalObjType ID>	ND_ static Bytes		GetHeapOffset	(MetalObject<ID> obj)  API_AVAILABLE(macos(10.15), ios(13));
		template <EMetalObjType ID>	ND_ static id<MTLHeap>	GetHeap			(MetalObject<ID> obj);
		template <EMetalObjType ID>		static void			MakeAliasable	(MetalObject<ID> obj);
		template <EMetalObjType ID>	ND_ static bool			IsAliasable		(MetalObject<ID> obj);

	// Querying the Allocated Size //
		template <EMetalObjType ID>	ND_ static Bytes		GetAllocatedSize (MetalObject<ID> obj);


		ND_ static MTLTextureDescriptor*  ConvertDesc (const ImageDesc &desc);			// autorelease
		ND_ static MTLTextureDescriptor*  ConvertDesc (const BufferViewDesc &desc);		// autorelease
		

	private:
		template <EMetalObjType ID>
		ND_ static constexpr bool		_IsResource ();

		ND_ static id<MTLDevice>		_GetDevice	(const void* ptr);
		ND_ static NS::String			_GetLabel	(const void* ptr);
			static void					_SetLabel	(const void* ptr, const NS::String &value);

		ND_ static MTLCPUCacheMode		 _GetCpuCacheMode		(const void* obj);
		ND_ static MTLStorageMode		 _GetStorageMode		(const void* obj);
		ND_ static MTLHazardTrackingMode _GetHazardTrackingMode	(const void* obj);
		ND_ static MTLResourceOptions	 _GetResourceOptions	(const void* obj);

		ND_ static Bytes				_GetHeapOffset	(const void* obj);
		ND_ static id<MTLHeap>			_GetHeap		(const void* obj);
			static void					_MakeAliasable	(const void* obj);
		ND_ static bool					_IsAliasable	(const void* obj);

		ND_ static Bytes				_GetAllocatedSize (const void* obj);
	};

	
	
/*
=================================================
	_IsResource
=================================================
*/
	template <EMetalObjType ID>
	constexpr bool  MResourceUtils::_IsResource ()
	{
		return ID > EMetalObjType::_ResourceBegin and ID < EMetalObjType::_ResourceEnd;
	}
	
/*
=================================================
	GetDevice
=================================================
*/
	template <EMetalObjType ID>
	id<MTLDevice>  MResourceUtils::GetDevice (MetalObject<ID> obj)
	{
		STATIC_ASSERT( _IsResource<ID>() );
		return _GetDevice( obj.Ptr() );
	}
	
/*
=================================================
	GetLabel
=================================================
*/
	template <EMetalObjType ID>
	NS::String  MResourceUtils::GetLabel (MetalObject<ID> obj)
	{
		STATIC_ASSERT( _IsResource<ID>() );
		return _GetLabel( obj.Ptr() );
	}
	
/*
=================================================
	SetLabel
=================================================
*/
	template <EMetalObjType ID>
	void  MResourceUtils::SetLabel (MetalObject<ID> obj, const NS::String &value)
	{
		STATIC_ASSERT( _IsResource<ID>() );
		_SetLabel( obj.Ptr(), value );
	}
	
	template <EMetalObjType ID>
	void  MResourceUtils::SetLabel (MetalObject<ID> obj, StringView value)
	{
		NS::String	str {value};
		SetLabel( obj, str );
	}

/*
=================================================
	GetCpuCacheMode
=================================================
*/
	template <EMetalObjType ID>
	MTLCPUCacheMode  MResourceUtils::GetCpuCacheMode (MetalObject<ID> obj)
	{
		STATIC_ASSERT( _IsResource<ID>() );
		return _GetCpuCacheMode( obj.Ptr() );
	}
	
/*
=================================================
	GetStorageMode
=================================================
*/
	template <EMetalObjType ID>
	MTLStorageMode  MResourceUtils::GetStorageMode (MetalObject<ID> obj)
	{
		STATIC_ASSERT( _IsResource<ID>() );
		return _GetStorageMode( obj.Ptr() );
	}
	
/*
=================================================
	GetHazardTrackingMode
=================================================
*/
	template <EMetalObjType ID>
	MTLHazardTrackingMode  MResourceUtils::GetHazardTrackingMode (MetalObject<ID> obj)
	{
		STATIC_ASSERT( _IsResource<ID>() );
		return _GetHazardTrackingMode( obj.Ptr() );
	}
	
/*
=================================================
	GetResourceOptions
=================================================
*/
	template <EMetalObjType ID>
	MTLResourceOptions  MResourceUtils::GetResourceOptions (MetalObject<ID> obj)
	{
		STATIC_ASSERT( _IsResource<ID>() );
		return _GetResourceOptions( obj.Ptr() );
	}
	
/*
=================================================
	GetHeapOffset
=================================================
*/
	template <EMetalObjType ID>
	Bytes  MResourceUtils::GetHeapOffset (MetalObject<ID> obj)  API_AVAILABLE(macos(10.15), ios(13))
	{
		STATIC_ASSERT( _IsResource<ID>() );
		return _GetHeapOffset( obj.Ptr() );
	}
	
/*
=================================================
	GetHeap
=================================================
*/
	template <EMetalObjType ID>
	id<MTLHeap>  MResourceUtils::GetHeap (MetalObject<ID> obj)
	{
		STATIC_ASSERT( _IsResource<ID>() );
		return _GetHeap( obj.Ptr() );
	}
	
/*
=================================================
	MakeAliasable
=================================================
*/
	template <EMetalObjType ID>
	void  MResourceUtils::MakeAliasable (MetalObject<ID> obj)
	{
		STATIC_ASSERT( _IsResource<ID>() );
		return _MakeAliasable( obj.Ptr() );
	}
	
/*
=================================================
	IsAliasable
=================================================
*/
	template <EMetalObjType ID>
	bool  MResourceUtils::IsAliasable (MetalObject<ID> obj)
	{
		STATIC_ASSERT( _IsResource<ID>() );
		return _IsAliasable( obj.Ptr() );
	}


} // AE::Graphics
