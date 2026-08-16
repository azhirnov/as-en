// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "Scripting/ScriptCommon.h"
#include "Resources/RTScene.h"

namespace AE::ResEditor
{

	//
	// Ray Tracing Micromap
	//

	class ScriptRTMicromap final : public EnableScriptRC
	{
	// types
	private:
		using Usage			= RTMicromapInfo::Usage;
		using UsageArr_t	= Array< Usage >;


	// variables
	private:
		ScriptBufferPtr			_dataBuffer;		// opacity/displacement values
		Bytes					_dataBufferOffset;
		ScriptBufferPtr			_triangleArray;
		Bytes					_triangleArrayOffset;
		String					_dbgName;

		UsageArr_t				_usage;
		const EMicromapType		_type				= Default;
		bool					_allowUpdate		= false;

		RC<RTMicromap>			_resource;


	// methods
	public:
		ScriptRTMicromap (EMicromapType)																__Th___;
		~ScriptRTMicromap ();

		void  Name (const String &name)																	__Th___;
		void  AllowUpdate ()																			__Th___;

		uint  AddTriangleType (uint triangleCount, uint subdivisionLevel, EOpacityMicromapFormat fmt)	__Th___;

		void  SetData1 (const ScriptBufferPtr &buf)														__Th___;
		void  SetData2 (const ScriptBufferPtr &buf, uint offset)										__Th___;

		void  SetTriangles1 (const ScriptBufferPtr &buf)												__Th___;
		void  SetTriangles2 (const ScriptBufferPtr &buf, uint offset)									__Th___;

		ND_ StringView		GetName ()																	C_NE___	{ return _dbgName; }

		static void  Bind (const ScriptEnginePtr &se)													__Th___;

		// Returns non-null resource or throw exception.
		ND_ RC<RTMicromap>  ToResource ()																__Th___;

	private:
		void  _MutableResource ()																		C_Th___;
	};


} // AE::ResEditor
