// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#ifdef VULKAN_HEADER_PATH
# define VK_NO_PROTOTYPES
# define VK_ENABLE_BETA_EXTENSIONS
# include "base/Defines/StdInclude.h"
# include "vulkan/vulkan.h"
# include "vulkan/vulkan_android.h"

# include "shared/HeaderParser.h"

namespace AE::Parsers
{

	//
	// Vulkan Header Parser
	//

	class VulkanParser : public HeaderParser
	{
	// methods
	public:
		VulkanParser ();

		bool  ParseVkHeaders (const Path &folder);

		bool  SetFunctionsScope ();
		bool  BuildBasicTypeMap ();
		bool  BuildResourceTypeMap ();

	private:
		void  _BuildExtensionInfo ();

		void  _AddResourceType (const char* name, VkObjectType type, const char* typeName);
	};


} // AE::Parsers

#endif // VULKAN_HEADER_PATH
