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
		
			void  NextFrame (FrameUID frameId);
			
			ND_ ActionQueueReader  ReadInput (FrameUID frameId) const;

			template <typename T>
			bool  Insert (const InputActionName &name, ControllerID id, const T &data)	{ return CurrentQueue().Insert( name, id, data ); }
			bool  Insert (const InputActionName &name, ControllerID id)					{ return CurrentQueue().Insert( name, id ); }

			ND_ ActionQueue&  CurrentQueue ()	{ return _actionQueues[ ulong(_curFrameId.load().Unique()) & 1 ]; }
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
			
			ND_ ActionQueueReader  ReadInput (FrameUID frameId) const	{ return _ref->ReadInput( frameId ); }

			template <typename T>
			bool  Insert (const InputActionName &name, ControllerID id, const T &data)	{ return _ref->Insert( name, id, data ); }
			bool  Insert (const InputActionName &name, ControllerID id)					{ return _ref->Insert( name, id ); }
		};


	// variables
	protected:
		Ptr<InputMode const>		_curMode;

		DubleBufferedQueueRef		_dbQueueRef;
		
		PressedKeys_t				_pressedKeys;

		ModeMap_t					_modeMap;

		BindAction					_bindAction;

		DRC_ONLY(
			DataRaceCheck			_drCheck;
		)


	// methods
	public:
		explicit InputActionsBase (DubleBufferedQueue &q) : _dbQueueRef{q} {}
		~InputActionsBase () override;
		
			void  Update (Duration_t timeSinceStart);
		
		ND_ bool  RequiresLockAndHideCursor () const;


	// IInputActions //
		ND_ ActionQueueReader  ReadInput (FrameUID frameId) const override	{ return _dbQueueRef.ReadInput( frameId ); }

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
	};


} // AE::App
