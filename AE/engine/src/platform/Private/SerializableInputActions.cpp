// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "platform/Private/SerializableInputActions.h"
#include "pch/Scripting.h"
#include "platform/Private/EnumToString.h"

namespace AE::App
{
	using namespace AE::Serializing;

/*
=================================================
	destructor
=================================================
*/
	SerializableInputActions::~SerializableInputActions () __NE___
	{
		// will generate error if has hash collisions
	}

/*
=================================================
	Serialize
=================================================
*/
	bool  SerializableInputActions::Serialize (Serializer &ser) C_NE___
	{
		return Serialize( _modeMap, _version, ser );
	}

	bool  SerializableInputActions::Serialize (const ModeMap_t &modeMap, const uint version, Serializer &ser) __NE___
	{
		bool	res = ser( version, uint(modeMap.size()) );
		CHECK_ERR( res );

		for (auto [name, mode] : modeMap)
		{
			res = ser( name, mode.lockAndHideCursor, mode.enableSensors, uint(mode.actions.size()) );

			for (auto& [type, info] : mode.actions)
			{
				res = res and ser( type, info.name, info.valueType, info.gesture, info.swizzle, info.scale );
			}
		}

		CHECK_ERR( res );
		return true;
	}

/*
=================================================
	Deserialize
=================================================
*/
	bool  SerializableInputActions::Deserialize (Deserializer &des) __NE___
	{
		return Deserialize( OUT _modeMap, _version, des );
	}

	bool  SerializableInputActions::Deserialize (OUT ModeMap_t &modeMap, const uint version, Deserializer &des) __NE___
	{
		modeMap.clear();

		bool	res			= true;
		uint	mode_count	= 0;
		uint	ver			= 0;

		res = des( OUT ver );
		CHECK_ERR( res and ver == version );

		res = des( OUT mode_count );
		CHECK_ERR( res and mode_count <= _MaxModes );

		for (uint i = 0; res and (i < mode_count); ++i)
		{
			InputModeName	mode_name;
			uint			act_count		= 0;
			ESensorBits		enable_sensors	= Default;
			bool			lock_cursor		= false;

			res = des( OUT mode_name, OUT lock_cursor, OUT enable_sensors, OUT act_count );
			CHECK_ERR( res and act_count <= _MaxActionsPerMode );

			auto&	mode = modeMap( mode_name );

			mode.lockAndHideCursor	= lock_cursor;
			mode.enableSensors		= enable_sensors;
			mode.actions.reserve( act_count );

			for (uint j = 0; res and (j < act_count); ++j)
			{
				InputKey	key;
				ActionInfo	info;

				res = des( OUT key, OUT info.name, OUT info.valueType, OUT info.gesture, OUT info.swizzle, OUT info.scale );

				CHECK( mode.actions.insert_or_assign( key, info ).second );
			}
		}

		CHECK_ERR( res );
		return true;
	}

/*
=================================================
	Merge
=================================================
*/
	bool  SerializableInputActions::Merge (const SerializableInputActions &other)
	{
		for (auto [name, mode] : other._modeMap)
		{
			CHECK_ERR( not _modeMap.contains( name ));
			CHECK_ERR( _modeMap.emplace( name, mode ).first != Default );
		}

		_hashCollisionCheck.Merge( other._hashCollisionCheck );
		return true;
	}

/*
=================================================
	LoadSerialized
=================================================
*/
	bool  SerializableInputActions::LoadSerialized (OUT ModeMap_t &modeMap, const uint version, const uint nameHash, MemRefRStream &stream) __NE___
	{
		CHECK_ERR( stream.IsOpen() );

		uint	name;
		CHECK_ERR( stream.Read( OUT name ) and name == InputActions_Name );

		Array<uint>	hashes;
		Array<uint>	offsets;

		// read header
		{
			uint	count;
			CHECK_ERR( stream.Read( OUT count ));

			CHECK_ERR( stream.Read( count, OUT hashes ));
			CHECK_ERR( stream.Read( count, OUT offsets ));

			offsets.push_back( uint(stream.Size()) );
		}

		for (usize i = 0; i < hashes.size(); ++i)
		{
			ASSERT( offsets[i] < offsets[i+1] );

			if ( hashes[i] == nameHash )
			{
				Deserializer	des{ stream.ToSubStream( Bytes{offsets[i]}, Bytes{offsets[i+1] - offsets[i]} )};
				CHECK_ERR( Deserialize( OUT modeMap, version, des ));

				ASSERT( des.IsEnd() );
				return true;
			}
		}
		return false;
	}

/*
=================================================
	_ToArray
=================================================
*/
	auto  SerializableInputActions::_ToArray (const ActionMap_t &actions) __Th___ -> Array<Pair<InputKey, const ActionInfo *>>
	{
		Array<Pair<InputKey, const ActionInfo *>>	result;
		result.reserve( actions.size() );

		for (auto& [key, info] : actions) {
			result.emplace_back( key, &info );
		}

		std::sort( result.begin(), result.end(), [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; });
		return result;
	}

/*
=================================================
	_ToArray
=================================================
*/
	auto  SerializableInputActions::_ToArray (const ModeMap_t &modeMap) __Th___ -> Array<Pair<InputModeName, const InputMode *>>
	{
		Array<Pair<InputModeName, const InputMode *>>	result;
		result.reserve( modeMap.size() );

		for (auto [name, mode] : modeMap) {
			result.emplace_back( name, &mode );
		}

		std::sort( result.begin(), result.end(), [](auto &lhs, auto &rhs) { return lhs.first < rhs.first; });
		return result;
	}

/*
=================================================
	ToString
=================================================
*/
#ifdef AE_ENABLE_LOGS
	String  SerializableInputActions::ToString (const Reflection &refl) C_Th___
	{
		String		str		 = "InputActions"s << GetApiName() << " {\n";
		const auto	mode_arr = _ToArray(_modeMap);

		for (auto& [name, mode] : mode_arr)
		{
			str << "  '" << refl.Get( name ) << "' {\n";
			str << "    lockAndHideCursor: " << Base::ToString( mode->lockAndHideCursor ) << "\n";

			if ( mode->enableSensors != Default )
				str << "    enableSensors: { " << SensorBitsToString( mode->enableSensors ) << " }\n";

			const auto	act_arr = _ToArray(mode->actions);
			for (auto& [key, info] : act_arr)
			{
				auto [code, gesture, state] = _Unpack( key );

				str <<   "    InputKey: " << InputTypeToString( code ) << ", state: " << Base::ToString( state )
					<< "\n    {"
					<< "\n      name:    '" << refl.Get( info->name ) << "'"
					<< "\n      value:   " << Base::ToString( info->valueType )
					<< "\n      gesture: " << Base::ToString( info->gesture )
					<< "\n      swizzle: " << Base::ToString( info->swizzle )
					<< "\n      scale:   " << Base::ToString( info->GetScale() )
					<< "\n    }\n";
			}

			str << "  }\n";
		}
		str << "}\n\n";

		return str;
	}
#endif

} // AE::App
