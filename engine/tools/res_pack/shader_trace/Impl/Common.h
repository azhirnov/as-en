// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "../Public/ShaderTrace.h"
#include "base/Algorithms/StringUtils.h"

#define HIGH_DETAIL_TRACE
//#define USE_STORAGE_QUALIFIERS


// glslang includes
#ifdef AE_ENABLE_GLSLANG
# ifdef AE_COMPILER_MSVC
#	pragma warning (push, 0)
#	pragma warning (disable: 4005)
#	pragma warning (disable: 4668)
# endif

# include "glslang/MachineIndependent/localintermediate.h"
# include "glslang/Include/intermediate.h"

# ifdef AE_COMPILER_MSVC
#	pragma warning (pop)
# endif
#endif // AE_ENABLE_GLSLANG


namespace AE::PipelineCompiler
{
	using VariableID = ShaderTrace::VariableID;
	

# ifdef AE_ENABLE_GLSLANG

	using namespace glslang;


	ND_ String  GetFunctionName (glslang::TIntermOperator *op);

	ND_ bool  ValidateInterm (glslang::TIntermediate &intermediate);
	

/*
=================================================
	TSourceLoc::operator ==
=================================================
*/
	ND_ inline bool  operator == (const TSourceLoc &lhs, const TSourceLoc &rhs)
	{
		if ( lhs.name != rhs.name )
		{
			if ( lhs.name == null  or
				 rhs.name == null  or
				*lhs.name != *rhs.name )
				return false;
		}

		return	lhs.string	== rhs.string	and
				lhs.line	== rhs.line		and
				lhs.column	== rhs.column;
	}

	ND_ inline bool  operator != (const TSourceLoc &lhs, const TSourceLoc &rhs)
	{
		return not (lhs == rhs);
	}

	ND_ inline bool  operator < (const TSourceLoc &lhs, const TSourceLoc &rhs)
	{
		if ( lhs.name != rhs.name )
		{
			if ( lhs.name == null  or
				 rhs.name == null )
				return false;

			if ( *lhs.name != *rhs.name )
				return *lhs.name < *rhs.name;
		}

		return	lhs.string	!= rhs.string	? lhs.string < rhs.string	:
				lhs.line	!= rhs.line		? lhs.line	 < rhs.line		:
											  lhs.column < rhs.column;
	}
	
/*
=================================================
	SourcePoint
=================================================
*/
	inline ShaderTrace::SourcePoint::SourcePoint (const TSourceLoc &loc) :
		SourcePoint{ uint(loc.line), uint(loc.column) }
	{}

# endif // AE_ENABLE_GLSLANG


} // AE::PipelineCompiler
