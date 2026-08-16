// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Common.h"

namespace AE::PipelineCompiler
{

/*
=================================================
	ParseAsserts
=================================================
*/
	bool  ShaderTrace::ParseAsserts (const void* ptr, Bytes size, ELogFormat format, OUT Array<String> &urls) const
	{
		if ( format == Default )
			format = ELogFormat::Text;

		urls.clear();

		if ( _assertLocations.empty() )
		{
			ASSERT_MSG( _exprLocations.empty(),
				"ShaderTrace module initialized to record trace or function time, but used to get triggered asserts."
				"You should call 'ParseShaderTrace()' method instead." );
			return true;
		}

		const uint*		bits		= Cast<uint>(ptr);
		const void*		end			= ptr + Min( size, DivCeil( _assertLocations.size(), 32 ) * sizeof(uint) );
		uint			idx_offset	= 0;

		for (; bits < end; ++bits)
		{
			// for each non-zero bit
			for (uint bit_idx : BitIndexIterate(*bits))
			{
				uint	idx = bit_idx + idx_offset;

				ASSERT_Lt( idx, _assertLocations.size() );
				if ( idx >= _assertLocations.size() )
					continue;

				auto&	info = _assertLocations[idx];
				ASSERT_Eq( info.bitIndex, idx );

				CHECK_ERR( info.sourceId < _sources.size() );

				String&			result		= urls.emplace_back();
				const auto&		src			= _sources[ info.sourceId ];
				const uint		file_line	= info.point.Line() >= src.firstLine ? info.point.Line() - src.firstLine : 0;

				CHECK_ERR( info.point.Line() < src.lines.size() );

				const auto		line_range	= src.lines[ info.point.Line()-1 ];	// TODO
				StringView		line_str	= SubString( &src.code[line_range.first], &src.code[line_range.second] );

				switch_enum( format )
				{
					// pattern: 'file (line): ...'
					case ELogFormat::VS_Console :
						result << src.filename << " (" << ToString(file_line) << "):\n";
						break;

					// pattern: 'url (line)'
					case ELogFormat::FileURL :
						result << "file:///" << src.filename << " (" << ToString(file_line) << ")\n";
						break;

					// pattern: 'url#line'
					case ELogFormat::VSCode :
						result << "file:///" << src.filename << "#" << ToString(file_line) << "\n";
						break;

					case ELogFormat::Text :
					case ELogFormat::Unknown :
					case ELogFormat::_Count :	break;
				}
				switch_end

				result << line_str;
			}
			idx_offset += CT_SizeOfInBits<uint>;
		}

		return true;
	}

} // AE::PipelineCompiler
