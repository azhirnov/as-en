// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "pch/Base.h"

namespace AE::Parsers
{
	using namespace AE::Base;


	//
	// Vk/Xr Header Parser
	//

	class HeaderParser
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


		struct FunctionInfo
		{
			StringView			name;
			FuncArg				result;
			Array<FuncArg>		args;
			uint				fileIndex	= UMax;
			StringView			extension;
			mutable EFuncScope	scope		= EFuncScope::Unknown;

			FunctionInfo () {}
		};


		struct EnumField
		{
			StringView			name;
			String				value;
		};


		struct EnumInfo
		{
			StringView			name;
			Array<EnumField>	fields;
			uint				fileIndex	= UMax;
			StringView			extension;

			EnumInfo () {}
		};


		struct BitfieldInfo
		{
			StringView			name;
			StringView			enumName;
			uint				fileIndex	= UMax;
			StringView			extension;

			BitfieldInfo () {}
		};


		struct StructInfo
		{
			StringView			name;
			Array<FuncArg>		fields;
			uint				fileIndex	= UMax;
			StringView			extension;

			StructInfo () {}
		};


		struct ConstInfo
		{
			StringView		type;
			StringView		name;
			StringView		value;
		};


		struct SearchableFunc
		{
		// variables
			FunctionInfo		data;

		// methods
			SearchableFunc () {}
			SearchableFunc (const FunctionInfo &data) : data{data} {}
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
			EnumInfo			data;

		// methods
			SearchableEnum () {}
			SearchableEnum (const EnumInfo &data) : data{data} {}
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
			BitfieldInfo		data;

		// methods
			SearchableBitfield () {}
			SearchableBitfield (const BitfieldInfo &data) : data{data} {}
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
			StructInfo		data;

		// methods
			SearchableStruct () {}
			SearchableStruct (const StructInfo &data) : data{data} {}
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
			ConstInfo		data;

		// methods
			SearchableConst () {}
			SearchableConst (const ConstInfo &data) : data{data} {}
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
		using ConstSet_t		= Array< ConstInfo >;	//HashSet< SearchableConst, SearchableConstHash >;
		using ConstMap_t		= HashMap< StringView, ConstSet_t >;

		using CounterMap_t		= HashMap< Pair<StringView, StringView>, StringView >;
		using DestructorSet_t	= HashSet< Pair<StringView, StringView> >;
		using BasicTypeMap_t	= HashMap< StringView, EBasicType >;
		using FileDataArray_t	= FixedArray< FileInfo, 16 >;


		struct PacketInfo
		{
			String		id;
			String		func;
		};
		using PacketIDs_t		= Array< PacketInfo >;

		enum class EResourceType : uint {};		// VkObjectType


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

		struct TypedefInfo
		{
			StringView		dstType;
			StringView		extension;
			uint			fileIndex	= UMax;
		};
		using StructTypedefs_t = HashMap< StringView, TypedefInfo >;

		using ExtensionsMap_t = HashMap< StringView, ubyte >;


	// variables
	protected:
		FileDataArray_t			_fileData;

		FuncMap_t				_funcs;
		EnumMap_t				_enums;
		BitfieldMap_t			_bitfields;
		StructMap_t				_structs;
		ConstMap_t				_constants;
		StructTypedefs_t		_typedefs;

		ResourceTypes_t			_resourceTypes;
		BasicTypeMap_t			_basicTypes;

		ExtensionsMap_t			_extensions;
		ExtInfoMap_t			_extInfo;

		String					_macroPrefix;	// macros, enum - upper case style
		String					_structPrefix;	// struct, types - camel case style


	// methods
	public:
		HeaderParser () {}

		bool  ParseHeaders (const Path &folder);
		bool  BuildBasicTypeMap ();
		bool  BuildResourceTypeMap ();
		bool  SetFunctionsScope ();

		void  RemoveEnumValDuplicates ();

	protected:
		ND_ static bool  _IsWord (StringView value);
		ND_ static bool  _IsTypeOrQual (StringView value);
		ND_ static bool  _IsNumber (StringView value);

		bool  _BuildExtensionList (StringView fileData, usize fileIdx);

		bool  _ParseHeader (StringView fileData,
							const uint fileIndex,
							ArrayView< StringView > enableIfdef,
							ArrayView< StringView > disableIfdef,
							ArrayView< StringView > skiFunctions,
							const bool defaultSkip);
	};

} // AE::Parsers
