// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/RenderGraph/RGCommon.h"

namespace AE::RG::_hidden_
{
	
	//
	// Resource Key
	//
	struct ResourceKey
	{
	// types
		using TypeList_t	= TypeList< int, ImageID, BufferID, RTGeometryID, RTSceneID >;
		using Value_t		= ImageID::Value_t;
		
		static constexpr usize	_Size = TypeList_t::ForEach_Max< TypeListUtils::GetTypeSize >();
		STATIC_ASSERT( _Size == sizeof(Value_t) );


	// variables
		Value_t		id		= UMax;
		Value_t		type	= TypeList_t::Index<int>;


	// methods
		explicit ResourceKey (ImageID id)				__NE___	: id{id.Data()}, type{TypeList_t::Index<ImageID>}		{}
		explicit ResourceKey (BufferID id)				__NE___	: id{id.Data()}, type{TypeList_t::Index<BufferID>}		{}
		explicit ResourceKey (RTGeometryID id)			__NE___	: id{id.Data()}, type{TypeList_t::Index<RTGeometryID>}	{}
		explicit ResourceKey (RTSceneID id)				__NE___	: id{id.Data()}, type{TypeList_t::Index<RTSceneID>}		{}

		ND_ bool  operator == (const ResourceKey &rhs)	C_NE___	{ return (id == rhs.id) & (type == rhs.type); }
		ND_ bool  operator <  (const ResourceKey &rhs)	C_NE___	{ return (type == rhs.type) ? (id < rhs.id) : (type < rhs.type); }

		ND_ bool		IsImage ()						C_NE___	{ return type == TypeList_t::Index<ImageID>; }
		ND_ ImageID		AsImage ()						C_NE___	{ ASSERT(IsImage());  return UnsafeBitCast<ImageID>(id); }
		
		ND_ bool		IsBuffer ()						C_NE___	{ return type == TypeList_t::Index<BufferID>; }
		ND_ BufferID	AsBuffer ()						C_NE___	{ ASSERT(IsBuffer());  return UnsafeBitCast<BufferID>(id); }
	};


	//
	// Resource Key Hash
	//
	struct ResourceKeyHash
	{
		ND_ usize  operator () (const ResourceKey &x) C_NE___
		{
			if constexpr( sizeof(x) >= sizeof(ulong) )
				return ulong(x.id) | (ulong(x.type) << 32);
			else
				return x.id ^ (x.type << 15);
		}
	};



	//
	// Resource State Tracker
	//

	class ResStateTracker
	{
	// types
	private:
		using NativeImageDesc_t		= IResourceManager::NativeImageDesc_t;
		using NativeBufferDesc_t	= IResourceManager::NativeBufferDesc_t;

	  #if defined(AE_ENABLE_VULKAN)
		using CmdBatchDependency_t	= VulkanCmdBatchDependency;
	  #elif defined(AE_ENABLE_METAL)
		using CmdBatchDependency_t	= MetalCmdBatchDependency;
	  #endif

	public:
		struct ResGlobalState
		{
			EResourceState			defaultState		= Default;	// must be supported in all queues where resource will be used
			EResourceState			currentState		= Default;
			CmdBatchDependency_t	lastBatch;
			EQueueType				lastQueue			= Default;
			bool					exclusiveSharing	= true;
		};

	private:
		using ResGlobalMap_t	= FlatHashMap< ResourceKey, ResGlobalState, ResourceKeyHash >;

		static constexpr EResourceState		_DefState	= EResourceState::General;


	// variables
	private:
		mutable SharedMutex	_guard;
		ResGlobalMap_t		_globalStates;


	// methods
	public:
		ResStateTracker ();

		void  AddResource (ImageID      id, EResourceState current = Default, EResourceState defaultState = _DefState, const CommandBatchPtr &batch = null)	__NE___;
		void  AddResource (BufferID     id, EResourceState current = Default, EResourceState defaultState = _DefState, const CommandBatchPtr &batch = null)	__NE___;
		void  AddResource (RTGeometryID id, EResourceState current = Default, EResourceState defaultState = _DefState, const CommandBatchPtr &batch = null)	__NE___;
		void  AddResource (RTSceneID    id, EResourceState current = Default, EResourceState defaultState = _DefState, const CommandBatchPtr &batch = null)	__NE___;
		
		void  RemoveResource (ImageID      id)																												__NE___;
		void  RemoveResource (BufferID     id)																												__NE___;
		void  RemoveResource (RTGeometryID id)																												__NE___;
		void  RemoveResource (RTSceneID    id)																												__NE___;
		
		bool  GetResourceState (ImageID      id, OUT EResourceState &state)																					C_NE___;
		bool  GetResourceState (BufferID     id, OUT EResourceState &state)																					C_NE___;
		bool  GetResourceState (RTGeometryID id, OUT EResourceState &state)																					C_NE___;
		bool  GetResourceState (RTSceneID    id, OUT EResourceState &state)																					C_NE___;
		
		bool  SetDefaultState (ImageID      id, EResourceState defaultState)																				__NE___;
		bool  SetDefaultState (BufferID     id, EResourceState defaultState)																				__NE___;
		bool  SetDefaultState (RTGeometryID id, EResourceState defaultState)																				__NE___;
		bool  SetDefaultState (RTSceneID    id, EResourceState defaultState)																				__NE___;


	// IResourceManager //
		ND_ Strong<ImageID>			CreateImage (const ImageDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)					__NE___;
		ND_ Strong<BufferID>		CreateBuffer (const BufferDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)				__NE___;
		
		ND_ Strong<ImageID>			CreateImage (const NativeImageDesc_t &desc, StringView dbgName)															__NE___;
		ND_ Strong<BufferID>		CreateBuffer (const NativeBufferDesc_t &desc, StringView dbgName)														__NE___;
		
		ND_ Strong<RTGeometryID>	CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)		__NE___;
		ND_ Strong<RTSceneID>		CreateRTScene (const RTSceneDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)				__NE___;
		
			bool					ReleaseResource (INOUT Strong<ImageID>		&id)																		__NE___;
			bool					ReleaseResource (INOUT Strong<BufferID>		&id)																		__NE___;
			bool					ReleaseResource (INOUT Strong<RTGeometryID>	&id)																		__NE___;
			bool					ReleaseResource (INOUT Strong<RTSceneID>	&id)																		__NE___;
			
		template <typename Arg0, typename ...Args>
			bool					ReleaseResources (Arg0 &arg0, Args& ...args)																			__NE___;

	protected:
		ND_ bool  _UpdateResource (ResourceKey key, EResourceState newState, const CommandBatch &newBatch, OUT ResGlobalState &oldState)					__NE___;
		
		template <typename ID>	ND_ bool  _GetResourceState (ID id, OUT EResourceState &state)																C_NE___;
		template <typename ID>	ND_ bool  _SetDefaultState (ID id, EResourceState state)																	__NE___;
		template <typename ID>		void  _RemoveResource (ID id)																							__NE___;
		template <typename ID>	ND_ bool  _ReleaseResource (INOUT ID &id)																					__NE___;
	};

	
	
/*
=================================================
	ReleaseResources
=================================================
*/
	template <typename Arg0, typename ...Args>
	bool  ResStateTracker::ReleaseResources (Arg0 &arg0, Args& ...args) __NE___
	{
		bool	res = ReleaseResource( INOUT arg0 );
		
		if constexpr( CountOf<Args...>() > 0 )
			return res & ReleaseResources( FwdArg<Args&>( args )... );
		else
			return res;
	}


} // AE::RG::_hidden_
