// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "platform/Private/SerializableInputActions.h"
#include "platform/Public/IApplication.h"

namespace AE::App
{

	//
	// Input Actions base implementation
	//

	class InputActionsBase : public IInputActions
	{
	// types
	protected:
		static constexpr uint	_MaxActionsPerMode	= SerializableInputActions::_MaxActionsPerMode;
		static constexpr uint	_MaxHeaders			= 2000;
		static constexpr Bytes	_DataSizePerHeader	{8};

		static constexpr int	_LongPressDuration	= 2 * 1000;		// milliseconds
		
		using InputKey		= SerializableInputActions::InputKey;
		using ActionInfo	= SerializableInputActions::ActionInfo;
		using ActionMap_t	= SerializableInputActions::ActionMap_t;
		using InputMode		= SerializableInputActions::InputMode;
		using ModeMap_t		= SerializableInputActions::ModeMap_t;
		
		using Duration_t	= IApplication::Duration_t;

		struct PressedKey
		{
			Duration_t		duration		{0};
			ControllerID	controllerId	= ControllerID::Default;
		};
		using PressedKeys_t = FixedMap< ushort, PressedKey, 32 >;


		struct BindAction
		{
			bool				isActive	= false;

			// from request
			InputModeName		mode;
			InputActionName		action;
			EValueType			valueType	= Default;
			EGestureType		gesture		= Default;

			// result
			FixedSet< InputKey, 8 >	keys;
		};


		//
		// Duble Buffered Queue
		//
		class DubleBufferedQueue
		{
		// variables
		private:
			Graphics::AtomicFrameUID	_curFrameId;
			ActionQueue					_actionQueues [2];
			void*						_actionsQueueMem	= null;


		// methods
		public:
			DubleBufferedQueue ();
			~DubleBufferedQueue ();
			
			bool  Insert (const InputActionName &name, ControllerID id, const void* data, Bytes dataSize)	{ return CurrentQueue().Insert( name, id, data, dataSize ); }
			bool  Insert (const InputActionName &name, ControllerID id)										{ return CurrentQueue().Insert( name, id ); }

			ND_ ActionQueue&  CurrentQueue ()	{ return _actionQueues[ ulong(_curFrameId.load().Unique()) & 1 ]; }
		
			void  _NextFrame (FrameUID frameId);
			
			ND_ ActionQueueReader  _ReadInput (FrameUID frameId) const;
		};


		class DubleBufferedQueueRef
		{
		// variables
		private:
			Ptr<DubleBufferedQueue>		_ref;

		// methods
		public:
			DubleBufferedQueueRef (DubleBufferedQueue &q) : _ref{&q} {}
			DubleBufferedQueueRef (DubleBufferedQueueRef &&other) : _ref{other._ref} { other._ref = null; }

			void  operator = (const DubleBufferedQueueRef &rhs)			{ _ref = rhs._ref; }
		
			void  NextFrame (FrameUID frameId)							{ _ref->_NextFrame( frameId ); }

			ND_ ActionQueueReader  ReadInput (FrameUID frameId) const	{ return _ref->_ReadInput( frameId ); }
			
			ND_ DubleBufferedQueue*			Get ()			{ return _ref.get(); }
			ND_ DubleBufferedQueue const*	Get ()	const	{ return _ref.get(); }

			template <typename T>
			bool  Insert (const InputActionName &name, ControllerID id, const T &data)						{ return _ref->Insert( name, id, &data, Bytes::SizeOf(data) ); }
			bool  Insert (const InputActionName &name, ControllerID id, const void* data, Bytes dataSize)	{ return _ref->Insert( name, id, data, dataSize ); }
			bool  Insert (const InputActionName &name, ControllerID id)										{ return _ref->Insert( name, id ); }
		};


	// variables
	protected:
		Ptr<InputMode const>		_curMode;

		DubleBufferedQueueRef		_dbQueueRef;
		
		PressedKeys_t				_pressedKeys;

		ModeMap_t					_modeMap;

		BindAction					_bindAction;

		bool						_vrEmulation	= false;

		DRC_ONLY(
			DataRaceCheck			_drCheck;
		)


	// methods
	public:
		explicit InputActionsBase (DubleBufferedQueue &q) : _dbQueueRef{q} {}
		~InputActionsBase () override;
		
			void  Update (Duration_t timeSinceStart);
		
		ND_ bool  RequiresLockAndHideCursor () const;

		ND_ static DubleBufferedQueue*  GetQueue (IInputActions* act);
		
			void EnableVREmulation ()	{ _vrEmulation = true; }


	// IInputActions //
		void  NextFrame (FrameUID frameId) override						{ _dbQueueRef.NextFrame( frameId ); }

		ActionQueueReader  ReadInput (FrameUID frameId) const override	{ return _dbQueueRef.ReadInput( frameId ); }

		bool  SetMode (const InputModeName &value) override;
		
		bool  GetReflection (const InputModeName &mode, const InputActionName &action, OUT Reflection &) const override;

		bool  BeginBindAction (const InputModeName &mode, const InputActionName &action, EValueType type, EGestureType gesture) override;
		bool  EndBindAction () override;
		bool  IsBindActionActive () const override		{ DRC_EXLOCK( _drCheck );  return _bindAction.isActive; }


	protected:
		void  _Reset ();
		
		template <typename T>
		ND_ static constexpr InputKey	_Pack (T key, EGestureState state = EGestureState::Update) { return SerializableInputActions::_Pack( key, state ); }

		void  _Update1F (InputKey key, ControllerID id, float value);
		void  _Update2F (InputKey key, ControllerID id, float2 value);
		
		void  _Set2F (InputKey key, ControllerID id, EGestureType gesture, float2 value);
		
		template <typename T>
		void  _UpdateKey (T type, EGestureState state, ControllerID id, Duration_t timestamp);

		ND_ bool  _IsActiveQueue (const DubleBufferedQueue &q) const { return _dbQueueRef.Get() == &q; }
	};
//-----------------------------------------------------------------------------


	
/*
=================================================
	ValueType_ElementCount
=================================================
*/
	ND_ constexpr uint  ValueType_ElementCount (IInputActions::EValueType type)
	{
		using EValueType = IInputActions::EValueType;
		BEGIN_ENUM_CHECKS();
		switch ( type )
		{
			case EValueType::Bool :
			case EValueType::Int :
			case EValueType::Float :	return 1;
			case EValueType::Float2 :	return 2;
			case EValueType::Float3 :	return 3;
			case EValueType::Float4 :	return 4;
			case EValueType::Quat :
			case EValueType::Float4x4 :
			case EValueType::Unknown :
			case EValueType::String :	break;	// default
			case EValueType::_Count :
			default :					DBG_WARNING( "unknown value type" ); break;
		}
		END_ENUM_CHECKS();
		return 0;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	_Update1F
=================================================
*/
	forceinline void  InputActionsBase::_Update1F (InputKey key, ControllerID id, float value)
	{
		auto	it = _curMode->actions.find( key );

		if_unlikely( it != _curMode->actions.end() )
		{
			auto&		info	= it->second;
			const auto	v		= info.Transform( float4{value, 0.f, 0.f, 0.f} );
			const auto	v_size	= SizeOf<float> * ValueType_ElementCount( info.valueType );
				
			ASSERT( info.gesture == EGestureType::Move );
			//ASSERT( info.valueType == EValueType::Float );

			_dbQueueRef.Insert( info.name, id, &v[0], v_size );
		}
	}
	
/*
=================================================
	_Update2F
=================================================
*/
	forceinline void  InputActionsBase::_Update2F (InputKey key, ControllerID id, float2 value)
	{
		auto	it = _curMode->actions.find( key );

		if_unlikely( it != _curMode->actions.end() )
		{
			auto&		info	= it->second;
			const auto	v		= info.Transform( float4{value, 0.f, 0.f} );
			const auto	v_size	= SizeOf<float> * ValueType_ElementCount( info.valueType );

			ASSERT( info.gesture == EGestureType::Move );
			//ASSERT( info.valueType == EValueType::Float2 );

			_dbQueueRef.Insert( info.name, id, &v[0], v_size );
		}
	}

/*
=================================================
	RequiresLockAndHideCursor
=================================================
*/
	forceinline bool  InputActionsBase::RequiresLockAndHideCursor () const
	{
		if_likely( _curMode )
		{
			return _curMode->lockAndHideCursor;
		}
		return false;
	}
	
/*
=================================================
	_UpdateKey
=================================================
*/
	template <typename T>
	forceinline void  InputActionsBase::_UpdateKey (T type, EGestureState state, ControllerID id, Duration_t timestamp)
	{
		ASSERT( state != EGestureState::Update );

		auto	it = _curMode->actions.find( _Pack( type, state ));

		if_likely( it != _curMode->actions.end() )
		{
			auto&		info	= it->second;
			const auto	v		= info.Transform( float4{0.f} );
			const auto	v_size	= SizeOf<float> * ValueType_ElementCount( info.valueType );

			DEBUG_ONLY(
				bool	is_down		= (info.gesture == EGestureType::Down)  & (state == EGestureState::Begin);
				bool	is_click	= (info.gesture == EGestureType::Click) & (state == EGestureState::End);
				bool	is_hold		= (info.gesture == EGestureType::Hold)  & ((state == EGestureState::Begin) | (state == EGestureState::End));
				ASSERT( is_down or is_click or is_hold );
			)

			if ( info.gesture == EGestureType::Hold )
			{
				if ( state == EGestureState::Begin )
				{
					_pressedKeys.try_emplace( ushort(type), timestamp );
					_dbQueueRef.Insert( info.name, id, &v[0], v_size );
				}
				else
				{
					_pressedKeys.EraseByKey( ushort(type) );
				}
			}
			else
			{
				_dbQueueRef.Insert( info.name, id, &v[0], v_size );
			}
		}
		
		if_unlikely( _bindAction.isActive )
		{
			_bindAction.keys.try_insert( _Pack( type, state ));
		}
	}


} // AE::App
