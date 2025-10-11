// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#define XR_NO_PROTOTYPES
#include "base/Defines/StdInclude.h"
#include "openxr.h"

#include "shared/HeaderParser.h"

namespace AE::Parsers
{

	//
	// OpenXR Header Parser
	//

	class OpenXRParser : public HeaderParser
	{
	// methods
	public:
		OpenXRParser ();
		
		bool  ParseXrHeaders (const Path &folder);
		
		bool  SetFunctionsScope ();
		bool  BuildBasicTypeMap ();
		bool  BuildResourceTypeMap ();

	private:
		void  _BuildExtensionInfo ();

		void  _AddResourceType (const char* name, XrObjectType type, const char* typeName);
	};

} // AE::Parsers
