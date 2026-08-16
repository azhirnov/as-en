// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#ifdef VULKAN_HEADER_PATH
# include "VulkanLoaderGen.h"

namespace AE::Parsers
{
namespace
{
/*
=================================================
	CalcPrefixSize
=================================================
*/
	ND_ static usize  CalcPrefixSize2 (StringView s1, StringView s2)
	{
		const usize		min_cnt = Min( s1.length(), s2.length() );

		for (usize i = 0; i < min_cnt; ++i)
		{
			if ( s1[i] != s2[i] )
				return i;
		}
		return min_cnt;
	}

	ND_ static usize  CalcPrefixSize (const Array<VulkanLoaderGen::EnumField> &fields)
	{
		if ( fields.size() < 2 )
			return 0;

		usize	size = UMax;
		for (usize i = 1; i < fields.size(); ++i)
		{
			const auto&		s1 = fields[i-1].name;
			const auto&		s2 = fields[i].name;

			size = Min( size, CalcPrefixSize2( s1, s2 ));
		}
		return size;
	}
}

/*
=================================================
	GenEnumToString
=================================================
*/
	bool  VulkanLoaderGen::GenEnumToString (const Path &outputFolder) const
	{
		const char*		req_enums[] = {
			"VkFormat", "VkPipelineStageFlagBits", "VkAccessFlagBits", "VkImageLayout",
			"VkDependencyFlagBits", "VkSampleCountFlagBits", "VkAttachmentLoadOp", "VkAttachmentStoreOp",
			"VkImageAspectFlagBits", "VkStructureType", "VkRenderPassCreateFlagBits", "VkAttachmentDescriptionFlagBits",
			"VkSubpassDescriptionFlagBits", "VkPipelineBindPoint", "VkQueueFlagBits", "VkDeviceMemoryReportEventTypeEXT",
			"VkComponentTypeKHR", "VkScopeKHR", "VkCopyMicromapModeEXT", "VkMemoryPropertyFlagBits"
		};
		const Pair<const char*, const char*>	req_bitfields[] = {
			{ "VkPipelineStageFlags",			"VkPipelineStageFlagBits" },
			{ "VkAccessFlags",					"VkAccessFlagBits" },
			{ "VkDependencyFlags",				"VkDependencyFlagBits" },
			{ "VkImageAspectFlags",				"VkImageAspectFlagBits" },
			{ "VkPipelineStageFlags2",			"VkPipelineStageFlagBits2" },
			{ "VkAccessFlags2",					"VkAccessFlagBits2" },
			{ "VkRenderPassCreateFlags",		"VkRenderPassCreateFlagBits" },
			{ "VkAttachmentDescriptionFlags",	"VkAttachmentDescriptionFlagBits" },
			{ "VkSubpassDescriptionFlags",		"VkSubpassDescriptionFlagBits" },
			{ "VkQueueFlags",					"VkQueueFlagBits" },
			{ "VkMemoryPropertyFlags",			"VkMemoryPropertyFlagBits" }
		};

		const char*		req_const_set[]	= { "VkPipelineStageFlagBits2", "VkAccessFlagBits2" };
		const char*		ext_names[]		= { "_EXT", "_KHR", "_AMD", "_NV", "_NVX", "_HUAWEI", "_QCOM",
											"_BIT" };
		const char*		except_str[]	= { "_MAX_ENUM" };
		usize			prefix_size		= 0;

		const auto	ClipStr = [&prefix_size, &ext_names] (StringView inStr)
		{{
				String	str {inStr.substr( prefix_size )};

			for (StringView ext : ext_names)
			{
				if ( EndsWith( str, ext ))
					str = str.substr( 0, str.length() - ext.length() );
			}
			return str;
		}};

		const auto	SkipVal = [&except_str] (StringView str)
		{{
			for (StringView exc : except_str) {
				if ( HasSubString( str, exc ))
					return true;
			}
			return false;
		}};


		String	str;

		for (auto& name : req_enums)
		{
			auto	it = _enums.find( SearchableEnum{name} );
			CHECK_ERR_MSG( it != _enums.end(),
				"Not found enum '"s << name << "'" );

			str << "ND_ String  " << it->data.name << "ToString (const " << it->data.name << " value)\n{\n"
				<< "\tswitch ( value )\n\t{";

			prefix_size	= CalcPrefixSize( it->data.fields );

			for (const EnumField& val : it->data.fields)
			{
				if ( SkipVal( val.name ))
					continue;

				str << "\n\t\tcase " << val.name << " : return \"" << ClipStr( val.name ) << "\";";
			}

			str << "\n\t}"
				<< "\n\treturn \"<unknown>\";"
				<< "\n}\n\n";
		}

		for (auto& name : req_const_set)
		{
			auto	it = _constants.find( StringView{name} );
			CHECK_ERR( it != _constants.end() );

			str << "ND_ String  " << name << "ToString (const " << name << " value)\n{\n"
				<< "\tswitch ( value )\n\t{";

			prefix_size	= it->second.size() > 2 ? CalcPrefixSize2( it->second.begin()->name, (++it->second.begin())->name ) : 0;

			for (const auto& val : it->second)
			{
				if ( SkipVal( val.name ))
					continue;

				str << "\n\t\tcase " << val.name << " : return \"" << ClipStr( val.name ) << "\";";
			}

			str << "\n\t}"
				<< "\n\treturn \"<unknown>\";"
				<< "\n}\n\n";
		}

		for (auto& [bf, en] : req_bitfields)
		{
			str << "ND_ String  " << bf << "ToString (" << bf << " bits)\n{"
				<< "\n\tString  str;"
				<< "\n\twhile ( bits != 0 )\n\t{"
				<< "\n\t\t" << en << " bit = ExtractBit<" << en << "," << bf << ">( INOUT bits );"
				<< "\n\t\tif ( not str.empty() ) str += \" | \";"
				<< "\n\t\tstr += " << en << "ToString( bit );"
				<< "\n\t}"
				<< "\n\treturn str.size() ? str : \"0\";"
				<< "\n}\n\n";
		}

		// save to file
		{
			CHECK_ERR( FileSystem::IsDirectory( outputFolder ));

			FileWStream	file{ Path{outputFolder}.append( "vkenum_to_str.h" ) };
			CHECK_ERR( file.IsOpen() );
			CHECK_ERR( file.Write( str ));
		}
		return true;
	}

} // AE::Parsers
#endif // VULKAN_HEADER_PATH
