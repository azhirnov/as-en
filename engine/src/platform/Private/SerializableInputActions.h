// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/CompileTime/StringToID.h"
#include "base/Math/VecSwizzle.h"
#include "base/Utils/NamedID_HashCollisionCheck.h"

#ifdef AE_ENABLE_SCRIPTING
#	include "scripting/Impl/ScriptTypes.h"
#endif
#include "platform/Public/InputActions.h"

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
		using EValueType		= IInputActions::EValueType;
		using PackedScale_t		= half;
		using PackedScale4_t	= PackedVec< PackedScale_t, 4 >;

		struct ScriptActionInfo
		{
			FixedString<32>		action;		// InputActionName
			EValueType			type		= Default;
			EGestureType		gesture		= Default;
			VecSwizzle			swizzle		= VecSwizzle::VecDefault(4);
			PackedScale4_t		scale		{float4{1.0f}};

			ScriptActionInfo () {}
			ScriptActionInfo (StringView a, EValueType t, EGestureType g, VecSwizzle sw, const PackedScale4_t &sc) :
				action{a}, type{t}, gesture{g}, swizzle{sw}, scale{sc} {}
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
			VecSwizzle			swizzle		= VecSwizzle::VecDefault(4);
			PackedScale4_t		scale		{float4{1.0f}};

			ND_ packed_float4	Transform (const float4 &in) const;
			ND_ float4			GetScale () const;
		};

		using ActionMap_t = FlatHashMap< InputKey, ActionInfo >;

		struct InputMode
		{
			ActionMap_t		actions;
			bool			lockAndHideCursor	= false;	// lock into window (for shooter game)
		};

		using ModeMap_t	= FixedMap< InputModeName, InputMode, _MaxModes >;
		
		// for serialization
		static constexpr uint	_BaseVersion	= 2;
		
		
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
			T*  _CreateMode (const String &name) __Th___
			{
				CHECK_THROW_MSG( _self != null );

				const InputModeName		name_id {name};
			
				_self->_hashCollisionCheck.Add( name_id, name );
			
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

			void  _Add (ushort type, const ScriptActionInfo &value) __Th___;
			void  _Inherit (const String &name) __Th___;
		};
	  #endif


	// variables
	protected:
		ModeMap_t					_modeMap;
		
		const uint					_version;

		NamedID_HashCollisionCheck	_hashCollisionCheck;


	// methods
	public:
		SerializableInputActions (uint ver)				__NE___	: _version{ver} {}
		virtual ~SerializableInputActions ()			__NE___;

		ND_ virtual bool  IsKey (ushort type)			const = 0;
		ND_ virtual bool  IsKeyOrTouch (ushort type)	const = 0;
		ND_ virtual bool  IsCursor1D (ushort type)		const = 0;
		ND_ virtual bool  IsCursor2D (ushort type)		const = 0;
		
	  #ifdef AE_ENABLE_SCRIPTING
		ND_ virtual bool  LoadFromScript (const Scripting::ScriptEnginePtr &se, String script, const SourceLoc &loc) = 0;
	  #endif

		ND_ bool  Merge (const SerializableInputActions &other);
		
		ND_ virtual String  ToString ()					const = 0;

		ND_ static bool  LoadSerialized (OUT ModeMap_t &modeMap, uint version, uint nameHash, MemRefRStream &stream);


	// ISerializable //
		bool  Serialize (Serializing::Serializer &)		C_NE_OF;
		bool  Deserialize (Serializing::Deserializer &)	__NE_OF;

		ND_ static bool  Serialize (const ModeMap_t &modeMap, uint version, Serializing::Serializer &)	__NE___;
		ND_ static bool  Deserialize (OUT ModeMap_t &modeMap, uint version, Serializing::Deserializer &)__NE___;

		
	  #ifdef AE_ENABLE_SCRIPTING
		static bool  BindBase (const Scripting::ScriptEnginePtr &se) __Th___;
	  #endif


	protected:
		template <typename T>
		ND_ static constexpr InputKey	_Pack (T key, EGestureType gesture, EGestureState state = EGestureState::Update) __NE___;

		ND_ static constexpr auto		_Unpack (InputKey key) __NE___ -> Tuple< uint, EGestureType, EGestureState >;
		
		ND_ static Array<Pair<InputKey,		 const ActionInfo *>>	_ToArray (const ActionMap_t &actions);
		ND_ static Array<Pair<InputModeName, const InputMode *>>	_ToArray (const ModeMap_t &modes);
	};
	
/*
=================================================
	_Pack
=================================================
*/
	template <typename T>
	forceinline constexpr SerializableInputActions::InputKey
		SerializableInputActions::_Pack (T key, EGestureType gesture, EGestureState state) __NE___
	{
		ASSERT( uint(key) <= 0xFF'FFFF );
		ASSERT( uint(gesture) <= 0xF );
		ASSERT( uint(state) <= 0xF );

		return InputKey( uint(key) | (uint(gesture) << 24) | (uint(state) << 28) );
	}

/*
=================================================
	_Unpack
=================================================
*/
	forceinline constexpr auto  SerializableInputActions::_Unpack (InputKey key) __NE___ -> Tuple< uint, EGestureType, EGestureState >
	{
		return Tuple{	(uint(key) & 0xFF'FFFF),
						EGestureType( (uint(key) >> 24) & 0xF ),
						EGestureState( (uint(key) >> 28) & 0xF )};
	}

/*
=================================================
	ActionInfo::Transform
=================================================
*/
	forceinline packed_float4  SerializableInputActions::ActionInfo::Transform (const float4 &value) const
	{
		return	value * swizzle * GetScale();
	}
	
	forceinline float4  SerializableInputActions::ActionInfo::GetScale () const
	{
		return float4{ scale.x.GetFast(), scale.y.GetFast(), scale.z.GetFast(), scale.w.GetFast() };
	}


} // AE::App


#ifdef AE_ENABLE_SCRIPTING
	AE_DECL_SCRIPT_OBJ(		AE::App::SerializableInputActions::ScriptActionInfo,	"ActionInfo"	);
	AE_DECL_SCRIPT_TYPE(	AE::App::EGestureState,									"EGestureState"	);
	AE_DECL_SCRIPT_TYPE(	AE::App::EGestureType,									"EGestureType"	);
	AE_DECL_SCRIPT_TYPE(	AE::App::SerializableInputActions::EValueType,			"EValueType"	);
#endif
