// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/SourceLoc.h"
#include "serializing/ISerializable.h"
#include "platform/Public/AppEnums.h"

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
		
		struct DataTypes
		{
			struct Cursor1D
			{
				float			prev;
				float			curr;
				EGestureState	state;
				Duration_t		duration;
			};

			struct Cursor2D
			{
				packed_float2	prev;
				packed_float2	curr;
				EGestureState	state;
				Duration_t		duration;
			};

			struct Cursor3D
			{
				packed_float3	prev;
				packed_float3	curr;
				EGestureState	state;
				Duration_t		duration;
			};
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
			ActionQueue () {}
			ActionQueue (ActionQueue &&);
			ActionQueue (void* ptr, Bytes headerSize, Bytes dataSize);

			template <typename T>
			bool  Insert (const InputActionName &name, ControllerID id, const T &data);
			bool  Insert (const InputActionName &name, ControllerID id);

			void  Reset ();

			ND_ uint			ReadPos ()				const	{ return _readPos.load(); }
			ND_ Header const&	GetHeader (uint idx)	const	{ return _headers[idx]; }
			ND_ void const*		GetData ()				const	{ return _data; }
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
			ActionQueueReader () {}
			ActionQueueReader (const ActionQueueReader &) = default;
			ActionQueueReader (ActionQueueReader &&) = default;

			explicit ActionQueueReader (const ActionQueue &);

			ND_ bool  ReadHeader (OUT Header &);

			template <typename T>
			ND_ T const&  Data (Bytes offset) const;

				void  Restart ()	{ _pos = 0; }
		};

		
	// interface
	public:
		virtual ~IInputActions () {}
		
		// TODO
		//   Thread safe: yes
		//
		ND_ virtual ActionQueueReader  ReadInput (FrameUID frameId) const = 0;
		
		// TODO
		//   Thread safe: yes
		//
			virtual void  NextFrame (FrameUID frameId) = 0;
			
		// TODO
		//   Thread safe: no
		//
		ND_ virtual bool  SetMode (const InputModeName &value) = 0;
		
		// TODO
		//   Thread safe: no
		//
		ND_ virtual bool  LoadSerialized (RStream &stream) = 0;

		// TODO: get reflection for UI


		// Serialize key/gesture to action.
		//   Thread safe: no
		//
		//ND_ virtual bool  Serialize (struct Serializer &) const = 0;
		//ND_ virtual bool  Deserialize (struct Deserializer &) = 0;

		// TODO: set sensity
		
		// TODO
		//   Thread safe: no
		//
		ND_ virtual bool  GetReflection (const InputModeName &mode, const InputActionName &action, OUT Reflection &) const = 0;


	// bind action to input key/gesture //
	
		// Begin user input recording.
		//   Thread safe: no
		//
		virtual bool  BeginBindAction (const InputModeName &mode, const InputActionName &action, EValueType type, EGestureType gesture) = 0;
		
		// Stop recording and create key/gesture to action mapping.
		//   Thread safe: no
		//
		virtual bool  EndBindAction () = 0;
		
		// Returns 'true' if used between 'BeginBindAction()' and 'EndBindAction()',
		// but can returns 'false' when first key/gesture are recorded.
		//   Thread safe: no
		//
		ND_ virtual bool  IsBindActionActive () const = 0;
	};


} // AE::App
