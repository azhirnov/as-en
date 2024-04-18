// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Packer/FeatureSetPack.h"
#include "graphics/Private/EnumToString.h"

#ifdef AE_BUILD_PIPELINE_COMPILER
# include "ScriptObjects/ObjectStorage.h"
#endif

namespace AE::PipelineCompiler
{

/*
=================================================
	Create
=================================================
*/
#ifdef AE_BUILD_PIPELINE_COMPILER
	bool  FeatureSetSerializer::Create (const ScriptFeatureSet &fs)
	{
		//CHECK_ERR( fs.fs.IsValid() );

		_fs = fs.fs;
		_fs.Validate();

		return true;
	}
#endif

/*
=================================================
	ToString
=================================================
*/
#ifdef AE_TEST_PIPELINE_COMPILER
	String  FeatureSetSerializer::ToString () const
	{
		String	str;
		// TODO
		return str;
	}
#endif

/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  FeatureSetSerializer::Serialize (Serializing::Serializer &ser) C_NE___
	{
		return ser( _fs );
	}

	bool  FeatureSetSerializer::Deserialize (Serializing::Deserializer &des) __NE___
	{
		return des( OUT _fs );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	Serialize
=================================================
*/
#ifdef AE_BUILD_PIPELINE_COMPILER
	bool  FeatureSetPacker::Serialize (Serializing::Serializer &ser) __NE___
	{
		const auto&	feat_sets = ObjectStorage::Instance()->featureSets;

		CHECK_ERR( not feat_sets.empty() );
		CHECK_ERR( feat_sets.size() <= FeatureSetSerializer::MaxCount );

		bool	result = true;
		result &= ser( FeatureSetPack_Name );
		result &= ser( FeatureSetPack_Version );
		result &= ser( ulong(FeatureSet::GetHashOfFS()) );
		result &= ser( uint(feat_sets.size()) );

		for (auto& [name, src_fs] : feat_sets)
		{
			FeatureSetSerializer	dst_fs;
			CHECK_ERR( dst_fs.Create( *src_fs ));
			CHECK_ERR( name.IsDefined() );

			result &= ser( name, dst_fs );
		}

		AE_LOG_DBG( "Serialized feature sets: "s << ToString(feat_sets.size()) );
		return result;
	}
#endif

} // AE::PipelineCompiler
