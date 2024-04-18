// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "ScriptObjects/ObjectStorage.h"

namespace AE::PipelineCompiler
{
	using namespace AE::Scripting;


	//
	// Feature Set Counter
	//

	struct FeatureSetCounter
	{
	// variables
	private:
		StaticArray< uint, uint(EFeature::_Count) >		_count = {};


	// methods
	public:
		FeatureSetCounter () {}

			void  Add (EFeature feat)			{ ++_count[uint(feat)]; }
		ND_ bool  Has (EFeature feat)	const	{ return _count[uint(feat)] > 0; }
		ND_ bool  IsFalse ()			const	{ return Has( EFeature::RequireFalse ); }
		ND_ bool  IsTrue ()				const	{ return Has( EFeature::RequireTrue ); }
		ND_ bool  IsEnable ()			const	{ return IsTrue() or not IsFalse(); }
	};


/*
=================================================
	SortDefines
=================================================
*/
	inline void  SortDefines (INOUT ShaderDefines_t &defines)
	{
		const auto	CmpStrings = [](StringView lhs, StringView rhs) -> bool
		{{
			const usize	len = Min( lhs.size(), rhs.size() );
			for (usize i = 0; i < len; ++i)
			{
				if ( lhs[i] == rhs[i] )
					continue;

				return ( lhs[i] > rhs[i] );
			}
			return lhs.size() > rhs.size();
		}};
		std::sort( defines.begin(), defines.end(), CmpStrings );
	}

/*
=================================================
	StagesToStr
=================================================
*/
	ND_ inline String  ShaderToStr (EShader shader)
	{
		switch_enum( shader )
		{
			case EShader::Vertex :				return "SH_VERT";
			case EShader::TessControl :			return "SH_TESS_CTRL";
			case EShader::TessEvaluation :		return "SH_TESS_EVAL";
			case EShader::Geometry :			return "SH_GEOM";
			case EShader::Fragment :			return "SH_FRAG";
			case EShader::Compute :				return "SH_COMPUTE";
			case EShader::Tile :				return "SH_TILE";
			case EShader::MeshTask :			return "SH_MESH_TASK";
			case EShader::Mesh :				return "SH_MESH";
			case EShader::RayGen :				return "SH_RAY_GEN";
			case EShader::RayAnyHit :			return "SH_RAY_AHIT";
			case EShader::RayClosestHit :		return "SH_RAY_CHIT";
			case EShader::RayMiss :				return "SH_RAY_MISS";
			case EShader::RayIntersection :		return "SH_RAY_INT";
			case EShader::RayCallable :			return "SH_RAY_CALL";
			case EShader::_Count :
			case EShader::Unknown :				break;
		}
		switch_end
		RETURN_ERR( "unknown shader type" );
	}

	ND_ inline String  StagesToStr (EShaderStages stages)
	{
		String	str;
		for (auto sh : BitIndexIterate<EShader>( stages ))
		{
			if ( not str.empty() )
				str << " | ";
			str << ShaderToStr( sh );
		}
		return str;
	}

} // AE::PipelineCompiler
