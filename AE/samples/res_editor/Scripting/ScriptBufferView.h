// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "Scripting/ScriptCommon.h"
#include "Resources/BufferView.h"

namespace AE::ResEditor
{

	//
	// Buffer View
	//

	class ScriptBufferView final : public EnableScriptRC
	{
		friend class ScriptBuffer;

	// types
	public:



	// variables
	private:
		ScriptBufferPtr			_baseBuffer;
		const EPixelFormat		_format			= Default;
		EResourceUsage			_resUsage		= Default;
		const EImageType		_texbufType		= EImageType::Buffer;
		String					_dbgName;

		RC<BufferView>			_resource;


	// methods
	private:
		ScriptBufferView (ScriptBufferPtr	buf,
						  EPixelFormat		format,
						  StringView		dbgName)						__Th___;
	public:
		~ScriptBufferView ();

		void  Name (const String &name)										__Th___;

		void  AddUsage (EResourceUsage usage)								__Th___;

		ND_ StringView		GetName ()										C_NE___	{ return _dbgName; }

		ND_ EPixelFormat	Format ()										C_NE___	{ return _format; }
		ND_ EImageType		TexelBufferType ()								C_NE___	{ return _texbufType; }

		static void  Bind (const ScriptEnginePtr &se)						__Th___;

		// Returns non-null resource or throw exception.
		ND_ RC<BufferView>  ToResource ()									__Th___;
	};


} // AE::ResEditor
