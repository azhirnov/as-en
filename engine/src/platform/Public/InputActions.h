// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/SourceLoc.h"
#include "serializing/ISerializable.h"
#include "platform/Public/AppEnums.h"

namespace AE::Base
{
	class MemRefRStream;
}
namespace AE::App
{

	//
	// Input Actions interface
	//

	class IInputActions : public Serializing::ISerializable
	{
	// types
	public:
		using Duration_t = std::chrono::duration< int, std::milli >;	// +-596 h

		enum class EValueType : ubyte
		{
			Bool,
			Int,
			Float,		// float,	unorm,		snorm
			Float2,		// float2,	unorm2,		snorm2
			Float3,		// float3,	unorm3,		snorm3
			Float4,		// float4
			Quat,
			Float4x4,
			String,		// including position in target text field and backspace/delete keys

			//Cursor1D,
			//Cursor2D,
			//Cursor3D,

			_Count,
			Unknown	= 0xFF,
		};

		
		struct KeyReflection
		{
			//ImageInAtlasName	imageName;	// for UI
			//FixedString<32>	name;
		};

		struct Reflection
		{
			EValueType		valueType	= Default;
			EGestureType	gesture		= Default;

			FixedArray< KeyReflection, 8 >	keys;
		};


		//
		// Action Queue
		//
		struct ActionQueue
		{
		// types
		public:
			struct Header
			{
				InputActionName	name;
				Bytes32u		offset;			// TODO: use ushort ?
				ControllerID	controllerId;
			};
			static constexpr Bytes	_DataAlign {4};


		// variables
		private:
			Atomic<uint>	_writePos	{0};
			Atomic<uint>	_readPos	{0};

			Header *		_headers	= null;
			void *			_data		= null;
			
			Bytes32u		_dataPos;
			const uint		_maxHeaders	= 0;
			const Bytes32u	_dataSize;


		// methods
		public:
			ActionQueue ()																					__NE___	{}
			ActionQueue (ActionQueue &&)																	__NE___;
			ActionQueue (void* ptr, Bytes headerSize, Bytes dataSize)										__NE___;

			bool  Insert (const InputActionName &name, ControllerID id, const void* data, Bytes dataSize)	__NE___;
			bool  Insert (const InputActionName &name, ControllerID id)										__NE___;

			void  Reset ()																					__NE___;

			ND_ uint			ReadPos ()																	C_NE___	{ return _readPos.load(); }
			ND_ Header const&	GetHeader (uint idx)														C_NE___	{ return _headers[idx]; }
			ND_ void const*		GetData ()																	C_NE___	{ return _data; }
		};


		//
		// Action Queue Reader
		//
		struct ActionQueueReader
		{
		// types
		public:
			using Header = ActionQueue::Header;

		// variables
		private:
			uint				_pos	= 0;
			uint				_count	= 0;
			ActionQueue const*	_queue	= null;

		// methods
		public:
			ActionQueueReader ()								__NE___	{}
			ActionQueueReader (const ActionQueueReader &)		__NE___	= default;
			ActionQueueReader (ActionQueueReader &&)			__NE___	= default;

			explicit ActionQueueReader (const ActionQueue &)	__NE___;

			ND_ bool  ReadHeader (OUT Header &)					__NE___;

			template <typename T>
			ND_ T const&  Data (Bytes offset)					C_NE___;

				void  Restart ()								__NE___	{ _pos = 0; }
		};

		
	// interface
	public:
		
		// TODO
		//   Thread safe: yes
		//
		ND_ virtual ActionQueueReader  ReadInput (FrameUID frameId)																		C_NE___ = 0;
		
		// TODO
		//   Thread safe: yes
		//
			virtual void  NextFrame (FrameUID frameId)																					__NE___	= 0;
			
		// TODO
		//   Thread safe: no
		//
		ND_ virtual bool  SetMode (const InputModeName &value)																			__NE___	= 0;
		
		// TODO
		//   Thread safe: no
		//
		ND_ virtual InputModeName  GetMode ()																							C_NE___ = 0;

		// TODO
		//   Thread safe: no
		//
		ND_ virtual bool  LoadSerialized (MemRefRStream &stream)																		__NE___	= 0;

		// TODO: get reflection for UI


		// Serialize key/gesture to action.
		//   Thread safe: no
		//
		//ND_ virtual bool  Serialize (struct Serializer &) const																		__NE___	= 0;
		//ND_ virtual bool  Deserialize (struct Deserializer &)																			__NE___	= 0;
		
		// TODO
		//ND_ virtual bool  SetScale (const float4 &scale)																				__NE___	= 0;

		// TODO
		//   Thread safe: no
		//
		ND_ virtual bool  GetReflection (const InputModeName &mode, const InputActionName &action, OUT Reflection &)					C_NE___ = 0;


	// bind action to input key/gesture //
	
		// Begin user input recording.
		//   Thread safe: no
		//
		virtual bool  BeginBindAction (const InputModeName &mode, const InputActionName &action, EValueType type, EGestureType gesture)	__NE___ = 0;
		
		// Stop recording and create key/gesture to action mapping.
		//   Thread safe: no
		//
		virtual bool  EndBindAction ()																									__NE___	= 0;
		
		// Returns 'true' if used between 'BeginBindAction()' and 'EndBindAction()',
		// but can returns 'false' when first key/gesture are recorded.
		//   Thread safe: no
		//
		ND_ virtual bool  IsBindActionActive ()																							C_NE___ = 0;
	};



	//
	// Thread-safe Input Actions
	//
	struct TsInputActions
	{
	public:
		using ActionQueueReader = IInputActions::ActionQueueReader;

	private:
		Ptr<IInputActions>	_ia;

	public:
		TsInputActions ()										__NE___	{}
		TsInputActions (Ptr<IInputActions> ia)					__NE___ : _ia{ia} {}
		TsInputActions (const TsInputActions &other)			__NE___ : _ia{other._ia} {}

		ND_ ActionQueueReader	ReadInput (FrameUID frameId)	C_NE___	{ return _ia->ReadInput( frameId ); }
			void				NextFrame (FrameUID frameId)	__NE___	{ return _ia->NextFrame( frameId ); }

		ND_ explicit operator bool ()							C_NE___	{ return bool{_ia}; }

		ND_ Ptr<IInputActions>	Unsafe ()						C_NE___	{ return _ia; }
	};
//-----------------------------------------------------------------------------
	


/*
=================================================
	constructor
=================================================
*/
	inline IInputActions::ActionQueueReader::ActionQueueReader (const ActionQueue &q) __NE___ :
		_count{ q.ReadPos() },
		_queue{ &q }
	{
		// invalidate cache for 'q._headers' and 'q._data' for '_count' elements
		Threading::MemoryBarrier( EMemoryOrder::Acquire );
	}
	
/*
=================================================
	ReadHeader
=================================================
*/
	inline bool  IInputActions::ActionQueueReader::ReadHeader (OUT Header &header) __NE___
	{
		if_likely( _pos < _count )
		{
			header = _queue->GetHeader( _pos++ );
			return true;
		}

		// try to get new actions
		_count = _queue->ReadPos();

		if ( _pos < _count )
		{
			// invalidate cache for '_queue->GetHeader()' and '_queue->GetData()' for new '_count' elements
			Threading::MemoryBarrier( EMemoryOrder::Acquire );

			header = _queue->GetHeader( _pos++ );
			return true;
		}

		return false;
	}
	
/*
=================================================
	Data
=================================================
*/
	template <typename T>
	T const&  IInputActions::ActionQueueReader::Data (Bytes offset) C_NE___
	{
		STATIC_ASSERT( AlignOf<T> <= IInputActions::ActionQueue::_DataAlign );
		return *Cast<T>( _queue->GetData() + offset );
	}
//-----------------------------------------------------------------------------
	


/*
=================================================
	constructor
=================================================
*/
	inline IInputActions::ActionQueue::ActionQueue (void* ptr, Bytes headerSize, Bytes dataSize) __NE___ :
		_headers{ Cast<Header>( ptr )},
		_data{ ptr + headerSize },
		_maxHeaders{ headerSize / SizeOf<Header> },
		_dataSize{ dataSize }
	{}
	
	inline IInputActions::ActionQueue::ActionQueue (ActionQueue &&other) __NE___ :
		_headers{ other._headers },
		_data{ other._data },
		_maxHeaders{ other._maxHeaders },
		_dataSize{ other._dataSize }
	{}

/*
=================================================
	Insert
=================================================
*/
	inline bool  IInputActions::ActionQueue::Insert (const InputActionName &name, ControllerID id, const void* data, Bytes dataSize) __NE___
	{
		const uint		hdr_idx		= _writePos.fetch_add( 1 );
		const Bytes		data_pos	= AlignUp( _dataPos, _DataAlign );

		if_unlikely( hdr_idx >= _maxHeaders )
		{
			//AE_LOG_DBG( "input actions queue overflow" );
			return false;
		}
		if_unlikely( data_pos + dataSize > _dataSize )
		{
			//AE_LOG_DBG( "input actions queue overflow" );
			return false;
		}

		_headers[hdr_idx].name			= name;
		_headers[hdr_idx].offset		= data_pos;
		_headers[hdr_idx].controllerId	= id;

		std::memcpy( OUT _data + data_pos, data, usize(dataSize) );

		_dataPos = data_pos + dataSize;
		_readPos.store( hdr_idx+1, EMemoryOrder::Release );	// TODO: fetch_add ???
		return true;
	}
	
	inline bool  IInputActions::ActionQueue::Insert (const InputActionName &name, ControllerID id) __NE___
	{
		const uint	hdr_idx = _writePos.fetch_add( 1 );
		
		if_unlikely( hdr_idx >= _maxHeaders )
		{
			//AE_LOG_DBG( "input actions queue overflow" );
			return false;
		}
		
		_headers[hdr_idx].name			= name;
		_headers[hdr_idx].offset		= UMax;
		_headers[hdr_idx].controllerId	= id;

		_readPos.store( hdr_idx+1, EMemoryOrder::Release );
		return true;
	}

/*
=================================================
	Reset
=================================================
*/
	inline void  IInputActions::ActionQueue::Reset () __NE___
	{
		_dataPos = 0_b;
		_writePos.store( 0 );
		_readPos.store( 0, EMemoryOrder::Release );
	}


} // AE::App
