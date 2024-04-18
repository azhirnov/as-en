// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	GLSL Trace project.

	[old standalone project](https://github.com/azhirnov/glsl_trace)
	[new project as part of AsEn](https://github.com/azhirnov/as-en/tree/preview/engine/tools/res_pack/shader_trace)
	[new project as part of AsEn - mirror](https://gitflic.ru/project/azhirnov/as-en/file?file=engine%2Ftools%2Fres_pack%2Fshader_trace)
	[new standalone project](https://github.com/azhirnov/glsl_trace/tree/v2023)
*/

#pragma once

#include "base/Utils/FileSystem.h"
#include "serializing/Basic/ISerializable.h"

namespace glslang {
	class TIntermediate;
	struct TSourceLoc;
}

namespace AE::PipelineCompiler
{
	using namespace AE;
	using namespace AE::Base;


	//
	// Shader Trace
	//

	struct ShaderTrace final :
		public Serializing::ISerializable,
		public NothrowAllocatable
	{
	// types
	public:
		enum class ELogFormat : uint
		{
			Unknown,
			Text,			// as plane text with part of source code
			VS_Console,		// compatible with VS output, allow navigation to code by click
			VS,				// click to file path will open shader source file
			VSCode,			// click to file path will open shader source file in specified line
			_Count
		};

		enum class VariableID : uint { Unknown = ~0u };

		union SourcePoint
		{
			struct {
				uint	column;
				uint	line;
			}		_packed;
			ulong	_ul			= UMax;

			SourcePoint ()											__NE___	{}
			SourcePoint (uint line, uint column)					__NE___	: _ul{(ulong(line) << 32) | column } {}
			explicit SourcePoint (const glslang::TSourceLoc &)		__NE___;

			ND_ bool  operator == (const SourcePoint &rhs)			C_NE___	{ return _ul == rhs._ul; }
			ND_ bool  operator >  (const SourcePoint &rhs)			C_NE___	{ return _ul >  rhs._ul; }

				void  SetMin (const SourcePoint &rhs)				__NE___	{ _ul = Min( _ul, rhs._ul ); }
				void  SetMax (const SourcePoint &rhs)				__NE___	{ _ul = Max( _ul, rhs._ul ); }

			ND_ uint  Line ()										C_NE___	{ return uint(_ul >> 32); }
			ND_ uint  Column ()										C_NE___	{ return uint(_ul & 0xFFFFFFFF); }
		};

		struct SourceLocation
		{
			uint			sourceId	= UMax;
			SourcePoint		begin;
			SourcePoint		end;

			SourceLocation ()										__NE___	{}
			SourceLocation (uint sourceId, uint line, uint column)	__NE___;

			ND_ bool  operator == (const SourceLocation &rhs)		C_NE___;
			ND_ bool  IsNotDefined ()								C_NE___;
		};

		struct ExprInfo
		{
			VariableID			varID	= Default;	// ID of output variable
			uint				swizzle	= 0;
			SourceLocation		range;				// begin and end location of expression
			SourcePoint			point;				// location of operator
			Array<VariableID>	vars;				// all variables IDs in this expression

			ExprInfo ()												__NE___	= default;
			ExprInfo (VariableID id, uint sw, const SourceLocation &range, const SourcePoint &pt) __NE___ : varID{id}, swizzle{sw}, range{range}, point{pt} {}

			ND_ bool  operator == (const ExprInfo &rhs)				C_NE___;
		};

		struct SourceInfo
		{
			using LineRange = Pair< usize, usize >;

			String				filename;
			String				code;
			uint				firstLine	= 0;
			Array<LineRange>	lines;				// offset in bytes for each line in 'code'

			ND_ bool  operator == (const SourceInfo &rhs)			C_NE___;
		};

		using VarNames_t	= HashMap< VariableID, String >;
		using ExprInfos_t	= Array< ExprInfo >;
		using Sources_t		= Array< SourceInfo >;
		using FileMap_t		= HashMap< String, uint >;	// index in '_sources'

		static constexpr uint	TBasicType_Clock	= 0xcc;	// 4x uint64

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
		ShaderTrace ()									__NE___ {}

		ShaderTrace (ShaderTrace &&)					= delete;
		ShaderTrace (const ShaderTrace &)				= delete;
		ShaderTrace& operator = (ShaderTrace &&)		= delete;
		ShaderTrace& operator = (const ShaderTrace &)	= delete;

		// Log all function results, log all function calls, log some operator results.
		// Use 'ParseShaderTrace' to get trace as string.
		ND_ bool  InsertTraceRecording (glslang::TIntermediate &, uint descSetIndex);

		// Insert time measurement into user-defined functions.
		// Use 'ParseShaderTrace' to get trace as string.
		ND_ bool  InsertFunctionProfiler (glslang::TIntermediate &, uint descSetIndex, bool shaderSubgroupClock, bool shaderDeviceClock);

		// Insert time measurement into entry function, summarize shader invocation times in storage buffer.
		ND_ bool  InsertShaderClockHeatmap (glslang::TIntermediate &, uint descSetIndex);

		// Converts binary trace into string.
		ND_ bool  ParseShaderTrace (const void* ptr, Bytes maxSize, ELogFormat format, OUT Array<String> &result) const;

		// Source code required for 'ParseShaderTrace' function.
		void  AddSource (StringView source);
		void  AddSource (StringView filename, uint firstLine, StringView source);
		void  AddSource (const Path &filename, uint firstLine, StringView source);

		void  IncludeSource (StringView headerName, const Path &fullPath, StringView source);	// if used '#include'

		void  GetSource (OUT String &result) const;

		ND_ bool  operator == (const ShaderTrace &rhs) const;

		ND_ Unique<ShaderTrace>  Clone () const;

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;

	private:
		void  _AppendSource (String filename, uint firstLine, String source);
	};


} // AE::PipelineCompiler
