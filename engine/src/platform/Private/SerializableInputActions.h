// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/CompileTime/StringToID.h"

#if not AE_OPTIMIZE_IDS
#	include "base/Utils/NamedID_HashCollisionCheck.h"
#endif
#ifdef AE_ENABLE_SCRIPTING
#	include "scripting/Impl/ScriptTypes.h"
#endif
#include "platform/Public/InputActions.h"

#if AE_NO_EXCEPTIONS == 1
#	define THROW()
#endif

namespace AE::App
{
	static constexpr uint	InputActions_Name	= uint("InAct"_StringToID);


	//
	// Serializable Input Actions
	//

	class SerializableInputActions : public Serializing::ISerializable
	{
		friend class InputActionsBase;

	// types
	public:
		using EValueType	= IInputActions::EValueType;

		struct ScriptActionInfo
		{
			FixedString<32>		action;		// InputActionName
			EValueType			type		= Default;
			EGestureType		gesture		= Default;

			ScriptActionInfo () {}
			ScriptActionInfo (StringView a, EValueType t, EGestureType g) : action{a}, type{t}, gesture{g} {}
		};


	protected:
		static constexpr uint	_MaxModes			= 32;
		static constexpr uint	_MaxActionsPerMode	= 1024;
		
		enum class InputKey : uint {};

		struct ActionInfo
		{
			InputActionName		name;
			EValueType			valueType	= Default;
			EGestureType		gesture		= Default;
		};

		using ActionMap_t = FlatHashMap< InputKey, ActionInfo >;

		struct InputMode
		{
			ActionMap_t		actions;
			bool			lockAndHideCursor	= false;	// lock into window (for shooter game)
		};

		using ModeMap_t	= FixedMap< InputModeName, InputMode, _MaxModes >;
		
		// for serialization
		static constexpr uint	_BaseVersion	= 1;
		
		
	  #ifdef AE_ENABLE_SCRIPTING
		//
		// Action Bindings
		//
		struct ScriptActionBindingsBase
		{
		private:
			SerializableInputActions *	_self = null;

		public:
			ScriptActionBindingsBase () {}
			ScriptActionBindingsBase (SerializableInputActions& self) : _self{&self} {}

			template <typename T>
			T*  _CreateMode (const String &name) THROW()
			{
				CHECK_THROW( _self != null );

				const InputModeName		name_id {name};
			
				#if not AE_OPTIMIZE_IDS
				_self->_hashCollisionCheck.Add( name_id );
				#endif
			
				InputMode&	mode = _self->_modeMap( name_id );
				ASSERT( mode.actions.empty() );

				Scripting::AngelScriptHelper::SharedPtr< T >	ptr{ new T{ *_self, mode }};
				return ptr.Detach();
			}
		};

		//
		// Base Bindings Mode
		//
		struct ScriptBindingsModeBase : Scripting::AngelScriptHelper::SimpleRefCounter
		{
		protected:
			SerializableInputActions *	_self	= null;
			InputMode *					_mode	= null;
			
		protected:
			ScriptBindingsModeBase () {}
			ScriptBindingsModeBase (SerializableInputActions& self, InputMode& mode) : _self{&self}, _mode{&mode} {}

			void  _Add (ushort type, const ScriptActionInfo &value) THROW();
			void  _Inherit (const String &name) THROW();
		};
	  #endif


	// variables
	protected:
		ModeMap_t		_modeMap;
		
		const uint		_version;

		#if not AE_OPTIMIZE_IDS
		  NamedID_HashCollisionCheck	_hashCollisionCheck;
		#endif


	// methods
	public:
		SerializableInputActions (uint ver) : _version{ver} {}
		virtual ~SerializableInputActions ();

		ND_ virtual bool  IsKey (ushort type)		const = 0;
		ND_ virtual bool  IsCursor1D (ushort type)	const = 0;
		ND_ virtual bool  IsCursor2D (ushort type)	const = 0;
		
	  #ifdef AE_ENABLE_SCRIPTING
		ND_ virtual bool  LoadFromScript (const Scripting::ScriptEnginePtr &se, String script, const SourceLoc &loc) = 0;
	  #endif

		ND_ bool  Merge (const SerializableInputActions &other);
		
	  #if not AE_OPTIMIZE_IDS
		ND_ virtual String  ToString () const = 0;
	  #endif

		ND_ static bool  LoadSerialized (OUT ModeMap_t &modeMap, uint version, uint nameHash, RStream &stream);


	// ISerializable //
		bool  Serialize (Serializing::Serializer &) const override final;
		bool  Deserialize (Serializing::Deserializer &) override final;

		ND_ static bool  Serialize (const ModeMap_t &modeMap, uint version, Serializing::Serializer &);
		ND_ static bool  Deserialize (OUT ModeMap_t &modeMap, uint version, Serializing::Deserializer &);

		
	  #ifdef AE_ENABLE_SCRIPTING
		ND_ static bool  BindBase (const Scripting::ScriptEnginePtr &se);
	  #endif


	protected:
		template <typename T>
		ND_ static constexpr InputKey	_Pack (T key, EGestureState state = EGestureState::Update);

		ND_ static constexpr Pair<uint,EGestureState>  _Unpack (InputKey key);
		
		ND_ static Array<Pair<InputKey,		 const ActionInfo *>>	_ToArray (const ActionMap_t &actions);
		ND_ static Array<Pair<InputModeName, const InputMode *>>	_ToArray (const ModeMap_t &modes);
	};


} // AE::App


#ifdef AE_ENABLE_SCRIPTING
namespace AE::Scripting
{
	AE_DECL_SCRIPT_OBJ(		AE::App::SerializableInputActions::ScriptActionInfo,	"ActionInfo"	);
	AE_DECL_SCRIPT_TYPE(	AE::App::EGestureState,									"EGestureState"	);
	AE_DECL_SCRIPT_TYPE(	AE::App::EGestureType,									"EGestureType"	);
	AE_DECL_SCRIPT_TYPE(	AE::App::SerializableInputActions::EValueType,			"EValueType"	);
}
#endif

#if AE_NO_EXCEPTIONS == 1
#	undef THROW
#endif
