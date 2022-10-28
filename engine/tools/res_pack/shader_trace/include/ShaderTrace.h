// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	GLSL Trace project
	https://github.com/azhirnov/glsl_trace
*/

#pragma once

#include "serializing/ISerializable.h"

namespace glslang {
	class TIntermediate;
}

namespace AE::PipelineCompiler
{
	using namespace AE;
	using namespace AE::Base;


	//
	// Shader Trace
	//

	struct ShaderTrace final : public Serializing::ISerializable
	{
	// types
	public:
		enum class VariableID : uint { Unknown = ~0u };

		struct SourcePoint
		{
			ulong	value	= UMax;

			SourcePoint () {}
			SourcePoint (uint line, uint column) : value{(ulong(line) << 32) | column } {}

			ND_ bool  operator == (const SourcePoint &rhs) const	{ return value == rhs.value; }

			ND_ uint  Line ()	const	{ return uint(value >> 32); }
			ND_ uint  Column ()	const	{ return uint(value & 0xFFFFFFFF); }
		};

		struct SourceLocation
		{
			uint			sourceId	= UMax;
			SourcePoint		begin;
			SourcePoint		end;

			SourceLocation () {}
			SourceLocation (uint sourceId, uint line, uint column) : sourceId{sourceId}, begin{line, column}, end{line, column} {}

			ND_ bool  operator == (const SourceLocation &rhs) const {
				return (sourceId == rhs.sourceId) & (begin == rhs.begin) & (end == rhs.end);
			}
		};

		struct ExprInfo
		{
			VariableID			varID	= Default;	// ID of output variable
			uint				swizzle	= 0;
			SourceLocation		range;				// begin and end location of expression
			SourcePoint			point;				// location of operator
			Array<VariableID>	vars;				// all variables IDs in this expression
			
			ND_ bool  operator == (const ExprInfo &rhs) const {
				return	(varID == rhs.varID) & (swizzle == rhs.swizzle) & (range == rhs.range) & (point == rhs.point) & (vars == rhs.vars);
			}
		};

		struct SourceInfo
		{
			using LineRange = Pair< usize, usize >;

			String				code;
			Array<LineRange>	lines;		// offset in bytes for each line in 'code'
			
			ND_ bool  operator == (const SourceInfo &rhs) const {
				return	(code == rhs.code) & (lines == rhs.lines);
			}
		};

		using VarNames_t	= HashMap< VariableID, String >;
		using ExprInfos_t	= Array< ExprInfo >;
		using Sources_t		= Array< SourceInfo >;
		using FileMap_t		= HashMap< String, uint >;	// index in '_sources'

		static constexpr int	TBasicType_Clock	= 0xcc;	// 4x uint64

	private:
		static constexpr uint	InitialPositionMask	= 0x80000000u;
		static constexpr uint	MaxCount			= 1 << 10;


	// variables
	private:
		ExprInfos_t		_exprLocations;
		VarNames_t		_varNames;
		Sources_t		_sources;
		FileMap_t		_fileMap;
		ulong			_posOffset			= 0;
		ulong			_dataOffset			= 0;
		uint			_initialPosition	= 0;


	// methods
	public:
		ShaderTrace () {}

		ShaderTrace (ShaderTrace &&) = delete;
		ShaderTrace (const ShaderTrace &) = delete;
		ShaderTrace& operator = (ShaderTrace &&) = delete;
		ShaderTrace& operator = (const ShaderTrace &) = delete;

		// Log all function results, log all function calls, log some operator results.
		// Use 'ParseShaderTrace' to get trace as string.
		ND_ bool  InsertTraceRecording (glslang::TIntermediate &, uint descSetIndex);

		// Insert time measurement into user-defined functions.
		// Use 'ParseShaderTrace' to get trace as string.
		ND_ bool  InsertFunctionProfiler (glslang::TIntermediate &, uint descSetIndex, bool shaderSubgroupClock, bool shaderDeviceClock);

		// Insert time measurement into entry function, summarize shader invocation times in storage buffer.
		ND_ bool  InsertShaderClockHeatmap (glslang::TIntermediate &, uint descSetIndex);
	
		// Converts binary trace into string.
		ND_ bool  ParseShaderTrace (const void *ptr, Bytes maxSize, OUT Array<String> &result) const;

		// Source code required for 'ParseShaderTrace' function.
		void  SetSource (const char* const* sources, const usize *lengths, usize count);
		void  SetSource (const char* source, usize length);
		void  IncludeSource (const char* filename, const char* source, usize length);	// if used '#include'
		void  GetSource (OUT String &result) const;

		ND_ bool  operator == (const ShaderTrace &rhs) const;

		ND_ Unique<ShaderTrace>  Clone () const;
		
		// ISerializable
		bool  Serialize (Serializing::Serializer &) const override;
		bool  Deserialize (Serializing::Deserializer &) override;

	private:
		void  _AppendSource (const char* source, usize length);
	};

} // AE::PipelineCompiler
