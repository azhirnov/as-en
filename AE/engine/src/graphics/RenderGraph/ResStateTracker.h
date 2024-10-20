// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	thread-safe:  yes
*/

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
		using TypeList_t	= TypeList< ImageID, BufferID, RTGeometryID, RTSceneID, int >;
		using Value_t		= ImageID::Value_t;
		using Index_t		= ImageID::Index_t;
		using Generation_t	= ImageID::Generation_t;

		static constexpr usize	_Size = TypeList_t::ForEach_Max< TypeListUtils::GetTypeSize >();
		StaticAssert( _Size == sizeof(Value_t) );

		static constexpr Value_t	_IndexMask	= Math::ToBitMask<Value_t>( CT_SizeOfInBits<Index_t> );
		static constexpr Value_t	_GenMask	= Math::ToBitMask<Value_t>( CT_SizeOfInBits<Generation_t> );
		static constexpr Value_t	_GenOffset	= CT_SizeOfInBits<Index_t>;


	// variables
		Value_t		id		= UMax;
		Value_t		type	= TypeList_t::Index<int>;


	// methods
		explicit ResourceKey (ImageID id)				__NE___	: id{id.Data()}, type{TypeList_t::Index<ImageID>}		{ ASSERT( Index() == id.Index() and Generation() == id.Generation() ); }
		explicit ResourceKey (BufferID id)				__NE___	: id{id.Data()}, type{TypeList_t::Index<BufferID>}		{ ASSERT( Index() == id.Index() and Generation() == id.Generation() ); }
		explicit ResourceKey (RTGeometryID id)			__NE___	: id{id.Data()}, type{TypeList_t::Index<RTGeometryID>}	{ ASSERT( Index() == id.Index() and Generation() == id.Generation() ); }
		explicit ResourceKey (RTSceneID id)				__NE___	: id{id.Data()}, type{TypeList_t::Index<RTSceneID>}		{ ASSERT( Index() == id.Index() and Generation() == id.Generation() ); }

		ND_ bool  operator == (const ResourceKey &rhs)	C_NE___	{ return (id == rhs.id) and (type == rhs.type); }
		ND_ bool  operator <  (const ResourceKey &rhs)	C_NE___	{ return (type == rhs.type) ? (id < rhs.id) : (type < rhs.type); }

		ND_ Index_t			Index ()					C_NE___	{ return id & _IndexMask; }
		ND_ Generation_t	Generation ()				C_NE___	{ return id >> _GenOffset; }

		ND_ bool			IsValid ()					C_NE___	{ return id != UMax; }	// same as 'HandleTmpl<>::IsValid()'
		ND_ bool			IsInvalid ()				C_NE___	{ return id == UMax; }

		ND_ bool			IsImage ()					C_NE___	{ return type == TypeList_t::Index<ImageID>; }
		ND_ ImageID			AsImage ()					C_NE___	{ ASSERT(IsImage());  return BitCast<ImageID>(id); }

		ND_ bool			IsBuffer ()					C_NE___	{ return type == TypeList_t::Index<BufferID>; }
		ND_ BufferID		AsBuffer ()					C_NE___	{ ASSERT(IsBuffer());  return BitCast<BufferID>(id); }

		ND_ bool			IsRTGeometry ()				C_NE___	{ return type == TypeList_t::Index<RTGeometryID>; }
		ND_ RTGeometryID	AsRTGeometry ()				C_NE___	{ ASSERT(IsRTGeometry());  return BitCast<RTGeometryID>(id); }

		ND_ bool			IsRTScene ()				C_NE___	{ return type == TypeList_t::Index<RTSceneID>; }
		ND_ RTSceneID		AsRTScene ()				C_NE___	{ ASSERT(IsRTScene());  return BitCast<RTSceneID>(id); }
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

	class ResStateTracker : public EnableRC<ResStateTracker>
	{
	// types
	private:
		using NativeImageDesc_t		= IResourceManager::NativeImageDesc_t;
		using NativeBufferDesc_t	= IResourceManager::NativeBufferDesc_t;
		using NativeMemObjInfo_t	= IResourceManager::NativeMemObjInfo_t;

	  #if defined(AE_ENABLE_VULKAN)
		using CmdBatchDependency_t	= VulkanCmdBatchDependency;

	  #elif defined(AE_ENABLE_METAL)
		using CmdBatchDependency_t	= MetalCmdBatchDependency;

	  #elif defined(AE_ENABLE_REMOTE_GRAPHICS)
		using CmdBatchDependency_t	= RemoteCmdBatchDependency;
	  #endif

	public:
		struct ResGlobalState
		{
			// TODO: separate default states for graphics & compute queues
			// TODO: currentReadState, currentWriteState
			EResourceState			defaultState		= Default;	// must be supported in all queues where resource will be used
			EResourceState			currentState		= Default;
			CmdBatchDependency_t	lastBatch;
			EQueueType				lastQueue			= Default;
			bool					exclusiveSharing	= true;
		};

	private:
		struct ResGlobalState2
		{
			mutable RWSpinLock		guard;			// TODO: TValueWithSpinLockBit
			ResGlobalState			state;
			ResourceKey::Value_t	id		= UMax;
		};

		struct ResourceMap
		{
		// types
		private:
			using Index_t						= ResourceKey::Index_t;
			static constexpr uint	ChunkSize	= (4<<10) / sizeof(Index_t);
			static constexpr uint	MaxChunks	= CT_FloorPOT< MaxValue<Index_t>() / ChunkSize >;
			using IDtoIdxChunk_t				= StaticArray< Atomic<Index_t>, ChunkSize >;
			using IDtoIdxChunkArr_t				= StaticArray< Atomic< IDtoIdxChunk_t *>, MaxChunks >;
			using GlobalStates_t				= Threading::LfIndexedPool< ResGlobalState2, Index_t, ChunkSize, MaxChunks, GlobalLinearAllocatorRef >;

			struct SearchResult
			{
			private:
				ResGlobalState2*	_result;

			public:
				explicit SearchResult (ResGlobalState2* res)		__NE___ : _result{res}				{ ASSERT( _result == null or _result->guard.is_locked() ); }
				SearchResult (SearchResult &&other)					__NE___	: _result{other._result}	{ other._result = null; }
				~SearchResult ()									__NE___	{ if ( _result != null ) _result->guard.unlock(); }

				ND_ explicit operator bool ()						C_NE___	{ return _result != null; }

				ND_ ResGlobalState*  operator -> ()					C_NE___	{ NonNull( _result );  return &_result->state; }
				ND_ ResGlobalState&  operator *  ()					C_NE___	{ NonNull( _result );  return _result->state; }
			};

			struct CSearchResult
			{
			private:
				ResGlobalState2 const*	_result;

			public:
				explicit CSearchResult (ResGlobalState2 const* res)	__NE___ : _result{res}				{ ASSERT( _result == null or _result->guard.is_shared_locked() ); }
				CSearchResult (CSearchResult &&other)				__NE___	: _result{other._result}	{ other._result = null; }
				~CSearchResult ()									__NE___	{ if ( _result != null ) _result->guard.unlock_shared(); }

				ND_ explicit operator bool ()						C_NE___	{ return _result != null; }

				ND_ ResGlobalState const*  operator -> ()			C_NE___	{ NonNull( _result );  return &_result->state; }
				ND_ ResGlobalState const&  operator *  ()			C_NE___	{ NonNull( _result );  return _result->state; }
			};


		// variables
		private:
			IDtoIdxChunkArr_t		_idToStateIdx;	// map 'ImageID' to index in '_statePool'
			GlobalStates_t			_statePool;

			SpinLock				_allocGuard;


		// methods
		public:
			ResourceMap ()													__NE___;
			~ResourceMap ()													__NE___;

			ND_ bool  Contains (ResourceKey key)							C_NE___;
			ND_ bool  Assign (ResourceKey key, const ResGlobalState &info)	__NE___;
				bool  Remove (ResourceKey key)								__NE___;

			ND_ SearchResult   Find (ResourceKey key)						__NE___;
			ND_ CSearchResult  Find (ResourceKey key)						C_NE___;
		};

		using PerResTypeMap_t	= StaticArray< ResourceMap, ResourceKey::TypeList_t::Count-1 >;


	// variables
	private:
		PerResTypeMap_t		_globalStates;


	// methods
	public:
		ResStateTracker ()																																__NE___ {}

		// Will replace previous state.
		// Returns 'true' if resource is added.
		template <typename ID>	bool  AddResource (const ID &id)																						__NE___	{ return _AddResource( id, Default, Default,      null,  Default ); }
		template <typename ID>	bool  AddResource (const ID &id, EResourceState current)																__NE___	{ return _AddResource( id, current, Default,      null,  Default ); }
		template <typename ID>	bool  AddResource (const ID &id, EResourceState current, EResourceState defaultState)									__NE___	{ return _AddResource( id, current, defaultState, null,  Default ); }
		template <typename ID>	bool  AddResource (const ID &id, EResourceState current, EResourceState defaultState, EQueueType queue)					__NE___	{ return _AddResource( id, current, defaultState, null,  queue   ); }
		template <typename ID>	bool  AddResource (const ID &id, EResourceState current, EResourceState defaultState, const CommandBatchPtr &batch)		__NE___	{ return _AddResource( id, current, defaultState, batch, Default ); }

		// Keep previous state if exists.
		// Returns 'true' if resource is already tracked or added.
		template <typename ID>	bool  AddResourceIfNotTracked (const ID &id)																			__NE___ { return IsTracked( id ) or AddResource( id ); }
		template <typename ID>	bool  AddResourceIfNotTracked (const ID &id, EResourceState current)													__NE___ { return IsTracked( id ) or AddResource( id, current ); }
		template <typename ID>	bool  AddResourceIfNotTracked (const ID &id, EResourceState current, EResourceState defaultState)						__NE___ { return IsTracked( id ) or AddResource( id, current, defaultState ); }
		template <typename ID>	bool  AddResourceIfNotTracked (const ID &id, EResourceState current, EResourceState defaultState, EQueueType queue)		__NE___ { return IsTracked( id ) or AddResource( id, current, defaultState, queue ); }
		template <typename ID>	bool  AddResourceIfNotTracked (const ID &id, EResourceState current, EResourceState defaultState, const CommandBatchPtr &batch)	__NE___ { return IsTracked( id ) or AddResource( id, current, defaultState, batch ); }

		template <typename ID>	ND_ auto  AcquireResource (const ID &id)																				__NE___	{ return _ResMngr().AcquireResource( id ); }

		// Returns 'true' if resource is tracked.
		template <typename ID>	ND_ bool  IsTracked (const ID    &id)																					C_NE___	{ return IsTracked( ResourceKey{id} ); }
								ND_ bool  IsTracked (ResourceKey key)																					C_NE___;

		template <typename ID>	void  RemoveResource (const ID    &id)																					__NE___	{ return RemoveResource( ResourceKey{id} ); }
								void  RemoveResource (ResourceKey key)																					__NE___;

		// Returns default state and 'resource may be undefined' flag.
		// Resource may be undefined if it is not tracked or image in first use.
		template <typename ID>	Tuple<EResourceState, bool>  GetDefaultState (const ID    &id)															C_NE___	{ return GetDefaultState( ResourceKey{id} ); }
								Tuple<EResourceState, bool>  GetDefaultState (ResourceKey key)															C_NE___;

		// Returns 'true' if resource is tracked.
		template <typename ID>	bool  SetDefaultState (const ID    &id, EResourceState defaultState)													__NE___	{ return SetDefaultState( ResourceKey{id}, defaultState ); }
								bool  SetDefaultState (ResourceKey key, EResourceState defaultState)													__NE___;

		template <typename ID0, typename ...IDs> void  SetDefaultState (EResourceState defaultState, const ID0 &id0, const IDs& ...ids)					__NE___;

		// Returns 'true' if resource is tracked.
		ND_ bool  UpdateResource (ResourceKey key, EResourceState newState, const CommandBatch &newBatch, OUT ResGlobalState &oldState)					__NE___;


	// IResourceManager //
		ND_ Strong<ImageID>			CreateImage (const ImageDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)				__NE___;
		ND_ Strong<BufferID>		CreateBuffer (const BufferDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)			__NE___;

		ND_ Strong<ImageID>			CreateImage (const NativeImageDesc_t &desc, StringView dbgName)														__NE___;
		ND_ Strong<BufferID>		CreateBuffer (const NativeBufferDesc_t &desc, StringView dbgName)													__NE___;

		ND_ Strong<ImageViewID>		CreateImageView (const ImageViewDesc &desc, ImageID image, StringView dbgName = Default)							__NE___;
		ND_ Strong<BufferViewID>	CreateBufferView (const BufferViewDesc &desc, BufferID buffer, StringView dbgName = Default)						__NE___;

		ND_ Strong<RTGeometryID>	CreateRTGeometry (const RTGeometryDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)	__NE___;
		ND_ Strong<RTSceneID>		CreateRTScene (const RTSceneDesc &desc, StringView dbgName = Default, GfxMemAllocatorPtr allocator = null)			__NE___;

			bool					ReleaseResource (INOUT Strong<ImageID>		&id)																	__NE___;
			bool					ReleaseResource (INOUT Strong<BufferID>		&id)																	__NE___;
			bool					ReleaseResource (INOUT Strong<ImageViewID>	&id)																	__NE___;
			bool					ReleaseResource (INOUT Strong<BufferViewID>	&id)																	__NE___;
			bool					ReleaseResource (INOUT Strong<RTGeometryID>	&id)																	__NE___;
			bool					ReleaseResource (INOUT Strong<RTSceneID>	&id)																	__NE___;
			bool					ReleaseResource (INOUT Strong<VideoImageID>	&id)																	__NE___;

			template <typename Arg0, typename ...Args>
			bool					ReleaseResources (Arg0 &arg0, Args& ...args)																		__NE___;

			template <typename ArrayType>
			void					ReleaseResourceArray (INOUT ArrayType &arr)																			__NE___;

		ND_ RTASBuildSizes			GetRTGeometrySizes (const RTGeometryBuild &desc)																	C_NE___	{ return _ResMngr().GetRTGeometrySizes( desc ); }
		ND_ RTASBuildSizes			GetRTSceneSizes (const RTSceneBuild &desc)																			C_NE___	{ return _ResMngr().GetRTSceneSizes( desc ); }

		template <typename ID> ND_ auto			GetDeviceAddress (ID id)																				C_NE___ { return _ResMngr().GetDeviceAddress( id ); }

		template <typename ID> ND_ auto const&	GetDescription (ID id)																					C_NE___	{ return _ResMngr().GetDescription( id ); }
		template <typename ID> ND_ bool			IsAlive (ID id)																							C_NE___	{ return _ResMngr().IsAlive( id ); }

			bool					GetMemoryInfo (ImageID id, OUT NativeMemObjInfo_t &info)															C_NE___ { return _ResMngr().GetMemoryInfo( id, OUT info ); }
			bool					GetMemoryInfo (BufferID id, OUT NativeMemObjInfo_t &info)															C_NE___ { return _ResMngr().GetMemoryInfo( id, OUT info ); }

		ND_ FeatureSet const&		GetFeatureSet ()																									C_NE___	{ return _ResMngr().GetFeatureSet(); }
		ND_ ResourceManager&		GetResourceManager ()																								C_NE___	{ return _ResMngr(); }


	  #if AE_GRAPHICS_DBG_SYNC
		ND_ String					KeyToString (ResourceKey key)																						C_Th___;
		ND_ String					BarrierToString (ResourceKey key, EResourceState oldState, EResourceState newState)									C_Th___;
	  #endif

	protected:
		ND_ bool  _AddResource2 (ResourceKey key, const ResGlobalState &info)																			__NE___;
		ND_ bool  _AddResource (ImageID      id, EResourceState current, EResourceState defaultState, const CommandBatchPtr &batch, EQueueType queue)	__NE___;
		ND_ bool  _AddResource (BufferID     id, EResourceState current, EResourceState defaultState, const CommandBatchPtr &batch, EQueueType queue)	__NE___;
		ND_ bool  _AddResource (RTGeometryID id, EResourceState current, EResourceState defaultState, const CommandBatchPtr &batch, EQueueType queue)	__NE___;
		ND_ bool  _AddResource (RTSceneID    id, EResourceState current, EResourceState defaultState, const CommandBatchPtr &batch, EQueueType queue)	__NE___;
		ND_ bool  _AddResource (VideoImageID id, EResourceState current, EResourceState defaultState, const CommandBatchPtr &batch, EQueueType queue)	__NE___;

		template <typename ID>	ND_ bool  _ReleaseResource (INOUT ID &id)																				__NE___;

		ND_ ResourceManager&  _ResMngr ()																												C_NE___;
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

/*
=================================================
	ReleaseResourceArray
=================================================
*/
	template <typename ArrayType>
	void  ResStateTracker::ReleaseResourceArray (INOUT ArrayType &arr) __NE___
	{
		for (auto& id : arr) { ReleaseResource( INOUT id ); }
	}

/*
=================================================
	SetDefaultState
=================================================
*/
	template <typename ID0, typename ...IDs>
	void  ResStateTracker::SetDefaultState (EResourceState defaultState, const ID0 &id0, const IDs& ...ids) __NE___
	{
		SetDefaultState( ResourceKey{id0}, defaultState );

		if constexpr( CountOf<IDs...>() > 0 )
			SetDefaultState( defaultState, ids... );
	}


} // AE::RG::_hidden_
