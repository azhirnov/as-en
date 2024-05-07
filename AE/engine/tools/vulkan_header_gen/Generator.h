// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/ArrayView.h"
#include "base/Containers/FixedArray.h"
#include "base/Algorithms/StringUtils.h"
#include "base/Algorithms/Parser.h"
#include "base/DataSource/File.h"
#include "base/FileSystem/FileSystem.h"
#include "base/Utils/Version.h"

#define VK_NO_PROTOTYPES
#define VK_ENABLE_BETA_EXTENSIONS
#include "vulkan/vulkan.h"
#include "vulkan/vulkan_android.h"

namespace AE::Vulkan
{
	using namespace AE::Base;


	//
	// VkTrace Helpers Generator
	//

	class Generator
	{
	// types
	public:
		enum class EFuncScope
		{
			Unknown,
			Library,
			Instance,
			Device,
		};


		struct FuncArg
		{
			Array<StringView>	type;
			StringView			name;

			FuncArg () {}
			explicit FuncArg (StringView name) : name{name} {}
		};


		struct VkFunctionInfo
		{
			StringView			name;
			FuncArg				result;
			Array<FuncArg>		args;
			uint				fileIndex	= UMax;
			StringView			extension;
			mutable EFuncScope	scope		= EFuncScope::Unknown;

			VkFunctionInfo () {}
		};


		struct EnumField
		{
			StringView			name;
			StringView			value;
		};


		struct VkEnumInfo
		{
			StringView			name;
			Array<EnumField>	fields;
			uint				fileIndex	= UMax;
			StringView			extension;

			VkEnumInfo () {}
		};


		struct VkBitfieldInfo
		{
			StringView			name;
			StringView			enumName;
			uint				fileIndex	= UMax;
			StringView			extension;

			VkBitfieldInfo () {}
		};


		struct VkStructInfo
		{
			StringView			name;
			Array<FuncArg>		fields;
			uint				fileIndex	= UMax;
			StringView			extension;

			VkStructInfo () {}
		};


		struct VkConstInfo
		{
			StringView		type;
			StringView		name;
			StringView		value;
		};


		struct SearchableFunc
		{
		// variables
			VkFunctionInfo		data;

		// methods
			SearchableFunc () {}
			SearchableFunc (const VkFunctionInfo &data) : data{data} {}
			explicit SearchableFunc (StringView name) { data.name = name; }

			ND_ bool  operator == (const SearchableFunc &right) const	{ return data.name == right.data.name; }
			ND_ bool  operator >  (const SearchableFunc &right) const	{ return data.name >  right.data.name; }
		};

		struct SearchableFuncHash {
			ND_ usize  operator () (const SearchableFunc &value) const {
				return usize(HashOf( value.data.name ));
			}
		};


		struct SearchableEnum
		{
		// variables
			VkEnumInfo			data;

		// methods
			SearchableEnum () {}
			SearchableEnum (const VkEnumInfo &data) : data{data} {}
			explicit SearchableEnum (StringView name) { data.name = name; }

			ND_ bool  operator == (const SearchableEnum &right) const	{ return data.name == right.data.name; }
			ND_ bool  operator >  (const SearchableEnum &right) const	{ return data.name >  right.data.name; }
		};

		struct SearchableEnumHash {
			ND_ usize  operator () (const SearchableEnum &value) const {
				return usize(HashOf( value.data.name ));
			}
		};


		struct SearchableBitfield
		{
		// variables
			VkBitfieldInfo		data;

		// methods
			SearchableBitfield () {}
			SearchableBitfield (const VkBitfieldInfo &data) : data{data} {}
			explicit SearchableBitfield (StringView name) { data.name = name; }

			ND_ bool  operator == (const SearchableBitfield &right) const	{ return data.name == right.data.name; }
			ND_ bool  operator >  (const SearchableBitfield &right) const	{ return data.name >  right.data.name; }
		};

		struct SearchableBitfieldHash {
			ND_ usize  operator () (const SearchableBitfield &value) const {
				return usize(HashOf( value.data.name ));
			}
		};


		struct SearchableStruct
		{
		// variables
			VkStructInfo		data;

		// methods
			SearchableStruct () {}
			SearchableStruct (const VkStructInfo &data) : data{data} {}
			explicit SearchableStruct (StringView name) { data.name = name; }

			ND_ bool  operator == (const SearchableStruct &right) const	{ return data.name == right.data.name; }
			ND_ bool  operator >  (const SearchableStruct &right) const	{ return data.name >  right.data.name; }
		};

		struct SearchableStructHash {
			ND_ usize  operator () (const SearchableStruct &value) const {
				return usize(HashOf( value.data.name ));
			}
		};


		struct SearchableConst
		{
		// variables
			VkConstInfo		data;

		// methods
			SearchableConst () {}
			SearchableConst (const VkConstInfo &data) : data{data} {}
			explicit SearchableConst (StringView name) { data.name = name; }

			ND_ bool  operator == (const SearchableConst &right) const	{ return data.name == right.data.name; }
		};

		struct SearchableConstHash {
			ND_ usize  operator () (const SearchableConst &value) const {
				return usize(HashOf( value.data.name ));
			}
		};


		struct FileInfo
		{
		// variables
			String		data;
			String		macro;

		// methods
			FileInfo () {}
			FileInfo (String &&data, StringView macro) : data{RVRef(data)}, macro{macro} {}
		};


		enum class EBasicType
		{
			Void,
			Bool,
			Char,
			Int,
			UInt,
			USize,
			Long,
			ULong,
			Float,
			Double,
			Handle,
			Struct,
			WCharString,
		};


		using FuncMap_t			= HashSet< SearchableFunc, SearchableFuncHash >;
		using EnumMap_t			= HashSet< SearchableEnum, SearchableEnumHash >;
		using BitfieldMap_t		= HashSet< SearchableBitfield, SearchableBitfieldHash >;
		using StructMap_t		= HashSet< SearchableStruct, SearchableStructHash >;
		using ConstSet_t		= Array< VkConstInfo >;	//HashSet< SearchableConst, SearchableConstHash >;
		using ConstMap_t		= HashMap< StringView, ConstSet_t >;

		using CounterMap_t		= HashMap< Pair<StringView, StringView>, StringView >;
		using DestructorSet_t	= HashSet< Pair<StringView, StringView> >;
		using BasicTypeMap_t	= HashMap< StringView, EBasicType >;
		using FileDataArray_t	= FixedArray< FileInfo, 16 >;


		struct PacketInfo
		{
			String		id;
			String		vkFunc;
		};
		using PacketIDs_t		= Array< PacketInfo >;

		using EResourceType		= VkObjectType;


		struct ResourceTypeInfo
		{
			EResourceType		type;
			String				typeName;
		};
		using ResourceTypes_t	= HashMap< StringView, ResourceTypeInfo >;


		struct TypeInfo
		{
			StructMap_t::const_iterator			structInfo;
			EnumMap_t::const_iterator			enumInfo;
			BitfieldMap_t::const_iterator		bitfieldInfo;
			ResourceTypes_t::const_iterator		resInfo;
			BasicTypeMap_t::const_iterator		basicInfo;

			uint								numPointers	= 0;
			bool								isArray		= false;
			bool								isConst		= false;
			bool								hasSType	= false;
			bool								isField		= true;
		};

		struct ExtensionInfo
		{
			Array<StructMap_t::const_iterator>		structs;
			Array<EnumMap_t::const_iterator>		enums;
			Array<BitfieldMap_t::const_iterator>	bitfields;

			Optional<StructMap_t::const_iterator>	feats;
			StringView								featsSType;
			Optional<StructMap_t::const_iterator>	props;
			StringView								propsSType;
		};
		using ExtInfoMap_t = HashMap< StringView, ExtensionInfo >;


		enum class EPropsType : ubyte
		{
			None,
			Device,
			Memory,
			Instance,
		};

		struct FeatureInfo
		{
			StringView			shortName;
			StringView			extension;
			Version2			coreVersion;
			Version2			requireVersion;
			Array<StringView>	requireExts;
			bool				enabled		= false;
			EPropsType			propsType	= EPropsType::Device;

			FeatureInfo () {}
			FeatureInfo (StringView name, StringView ext, Version2 v1, Version2 v2,
						 Array<StringView> req, EPropsType type = EPropsType::Device) :
				shortName{name}, extension{ext}, coreVersion{v1}, requireVersion{v2},
				requireExts{RVRef(req)}, propsType{type}
			{}
		};

		struct FeatureSet
		{
			Array< FeatureInfo >	instance;
			Array< FeatureInfo >	device;
			HashSet< StringView >	enabledExt;		// for loader
			Version2				minVer;
			usize					maxNameLen	= 0;
		};

		struct TypedefInfo
		{
			StringView		dstType;
			StringView		extension;
			uint			fileIndex	= UMax;
		};
		using StructTypedefs_t = HashMap< StringView, TypedefInfo >;


	// variables
	private:
		FileDataArray_t			_fileData;

		FuncMap_t				_funcs;
		EnumMap_t				_enums;
		BitfieldMap_t			_bitfields;
		StructMap_t				_structs;
		ConstMap_t				_constants;
		StructTypedefs_t		_typedefs;

		ResourceTypes_t			_resourceTypes;
		BasicTypeMap_t			_basicTypes;

		HashSet<StringView>		_extensions;
		ExtInfoMap_t			_extInfo;


	// methods
	public:
		Generator () {}

		bool  ParseVkHeaders (const Path &folder);
		bool  BuildBasicTypeMap ();
		bool  BuildResourceTypeMap ();
		bool  SetFunctionsScope ();

		bool  GenVulkanLoaders (const Path &outputFolder, Version2 minVer) const;
		bool  GenVulkanFeatures (const Path &outputFolder, Version2 minVer) const;

		void  RemoveEnumValDuplicates ();
		bool  GenEnumToString (const Path &outputFolder) const;

	private:
		ND_ static bool  _IsWord (StringView value);
		ND_ static bool  _IsTypeOrQual (StringView value);
		ND_ static bool  _IsNumber (StringView value);

		bool  _BuildExtensionList (StringView fileData);
		void  _BuildExtensionInfo ();

		ND_ FeatureSet  _GetFeatures (Version2 minVer) const;

		ND_ String  _GetFeaturesBoolStruct (const FeatureSet &feats) const;
		ND_ String  _GetFeaturesPropsStruct (const FeatureSet &feats) const;
		ND_ String  _GetExtensionsListFunc (const FeatureSet &feats) const;
		ND_ String  _GetFeaturesAndPropertiesFunc (const FeatureSet &feats) const;
		ND_ String  _GetLogFeaturesFunc (const FeatureSet &feats) const;
		ND_ String  _GetCheckFeaturesFunc (const FeatureSet &feats) const;

		bool  _GenerateVK1 (StringView				fileData,
							const uint				fileIndex,
							ArrayView< StringView >	enableIfdef,
							ArrayView< StringView >	disableIfdef,
							bool					defaultSkip);
	};

} // AE::Vulkan
