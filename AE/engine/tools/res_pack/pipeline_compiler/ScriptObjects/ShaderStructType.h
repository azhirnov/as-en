#pragma once

#include "ScriptObjects/ScriptFeatureSet.h"

namespace AE::PipelineCompiler
{

	//
	// Array Size Helper
	//
	struct ArraySize
	{
	// variables
		uint		value	= 0;

	// methods
		ArraySize ()									__NE___	{}
		explicit ArraySize (uint v)						__NE___	: value{v} {}

		static void  Bind (const ScriptEnginePtr &se)	__Th___;
	};



	//
	// Align Helper
	//
	struct Align
	{
	// variables
		uint		value	= 0;

	// methods
		Align ()										__NE___	{}
		explicit Align (uint v)							__NE___	: value{v} {}

		static void  Bind (const ScriptEnginePtr &se)	__Th___;
	};


	struct ShaderStructType;
	using ShaderStructTypePtr = ScriptRC< ShaderStructType >;



	//
	// Shader Structure Type
	//
	struct ShaderStructType final : EnableScriptRC
	{
	// types
	public:
		enum class EUsage
		{
			Unknown			= 0,
			ShaderIO		= 1 << 0,
			VertexAttribs	= 1 << 1,	// vertex input in shader
			VertexLayout	= 1 << 2,	// vertex format in buffer
			BufferLayout	= 1 << 3,
			BufferReference	= 1 << 4,
			_Last,
			All				= ((_Last - 1) << 1) - 1
		};

		enum class EFlags : ushort
		{
			Unknown						= 0,

			// precision
			HighPrecision				= 0,
			MediumPrecision				= 1 << 1,
			LowPrecision				= 1 << 2,	// not supported on most GPUs

			// interpolation
			FlatInterpolation			= 1 << 3,	// disable attribute interpolation, used last vertex of a triangle, only for 'InternalIO' layout
			SmoothInterpolation			= 0,		// perspective correct interpolation
			NoPerspectiveInterpolation	= 1 << 4,	// linear interpolation in screen space

			// multisampling
			CentroidInterpolation		= 1 << 5,	// single value may be assigned to that variable for all samples in the pixel,
													// derivatives of centroid-sampled inputs may be less accurate.
			PerSampleInterpolation		= 1 << 6,	// separate value must be assigned to that variable for each covered sample in the pixel,
													// and that value must be sampled at the location of the individual sample.

			Invariant					= 1 << 7,	// all shaders must output same result on same input

			Packed						= 1 << 8,	// align of vec/mat is same as for scalar
			PackedAlias					= 1 << 9,	// GLSL allows to pack 'float3' with scalar, MSL and C++ doesn't allow this.

			Padding_GLSL				= 1 << 10,	// field used for padding
			Padding_MSL					= 1 << 11,

			Address						= 1 << 12,	// typed device address
			Pointer						= 1 << 13,

			//Atomic					= 1 << 14,
		};

		struct Field
		{
			String				name;
			ShaderStructTypePtr	stType;
			uint				arraySize	= 0;		// 0 - non-array, UMax - dynamic
			EValueType			type		= Default;
			ubyte				rows		= 0;
			ubyte				cols		= 0;
			EFlags				flags		= Default;
			Bytes				size;
			Bytes				align;
			Bytes				offset;

			Field () {}

			ND_ bool	IsScalar ()						const	{ return (rows == 1)  and (cols == 1) and (not IsStruct()); }
			ND_ bool	IsVec ()						const	{ return (rows >  1)  and (cols == 1) and (not IsStruct()); }
			ND_ bool	IsMat ()						const	{ return (cols >  1)  and (not IsStruct()); }
			ND_ bool	IsStruct ()						const	{ return bool(stType) and (not IsDeviceAddress()); }
			ND_ bool	IsBufferRef ()					const	{ return bool(stType) and IsAddress(); }

			ND_ bool	IsPointer ()					const	{ return AllBits( flags, EFlags::Pointer ); }
			ND_ bool	IsAddress ()					const	{ return AllBits( flags, EFlags::Address ); }
			ND_ bool	IsAnyPadding ()					const;
			ND_ bool	IsPacked ()						const	{ return AllBits( flags, EFlags::Packed ); }

			ND_ bool	IsDeviceAddress ()				const	{ return IsAddress() or (type == EValueType::DeviceAddress); }		// typed or untyped
			ND_ bool	IsUntypedDeviceAddress ()		const	{ return (not IsAddress()) and (type == EValueType::DeviceAddress); }
			ND_ bool	IsDynamicArray ()				const	{ return arraySize == UMax; }
			ND_ bool	IsArray ()						const	{ return arraySize != 0; }
			ND_ bool	IsStaticArray ()				const	{ return (arraySize != 0) and (not IsDynamicArray()); }
			ND_ bool	operator == (const Field &rhs)	const;
		};

	private:
		struct ValidationData
		{
			// in
			ArrayView<ScriptFeatureSetPtr>	features;
			EStructLayout					layout;
			Bytes							baseOffset;

			// mutable
			Bytes		mslOffset;
			Bytes		glslOffset;
			Bytes		cppOffset;

			ValidationData (ArrayView<ScriptFeatureSetPtr> inFeatures, EStructLayout inLayout) :
				features{inFeatures}, layout{inLayout} {}
		};

	public:
		using UniqueTypes_t = HashSet< String >;
		using VertexInput	= GraphicsPipelineDesc::VertexInput;

		struct Constants
		{
			struct TypeInfo
			{
				EValueType		type;
				uint			size;
				uint			align;
			};
			const FlatHashMap< StringView, TypeInfo >		typeNames;
			const FlatHashMap< StringView, StringView >		renameMap;

			Constants ();
		};


	// variables
	private:
		const String	_originName;	// ShaderStructName
		const String	_typeName;

		Array<Field>	_fields;
		Bytes			_align;
		Bytes			_structAlign;	// align for array or ...
		Bytes			_size;
		EStructLayout	_layout		= EStructLayout::Compatible_Std140;
		mutable EUsage	_usage		= Default;

		Array< ScriptFeatureSetPtr >	_features;


	// methods
	public:
		ShaderStructType () {}
		explicit ShaderStructType (const String &name)													__Th___;

		void  AddFeatureSet (const String &name)														__Th___;
		void  Set (EStructLayout layout, const String &fields)											__Th___;
		void  Set2 (const String &fields)																__Th___;

		ND_ String			FieldsToString ()															C_Th___;

		ND_ StringView		Name ()																		const	{ return _originName; }
		ND_ StringView		Typename ()																	const	{ return _typeName; }
		ND_ bool			HasDynamicArray ()															const	{ return _fields.size() > 0 and _fields.back().IsDynamicArray(); }

		ND_ Bytes			TotalSize (uint arraySize)													const;
		ND_ Bytes			StaticSize ()																const	{ return AlignUp( _size, _align ); }
		ND_ Bytes			ArrayStride ()																const	{ return HasDynamicArray() ? _fields.back().size : 0_b; }
		ND_ Bytes			Align ()																	const	{ return _structAlign; }
		ND_ EStructLayout	Layout ()																	const	{ return _layout; }
		ND_ EUsage			Usage ()																	const	{ return _usage; }

			void  AddUsage (EUsage usage);
		ND_ bool  Compare (const ShaderStructType &rhs)													const;

		ND_ ArrayView<ScriptFeatureSetPtr>	Features ()													const	{ return _features; }
		ND_ ArrayView<Field>				Fields ()													const	{ return _fields; }

		ND_ bool  FieldCount (INOUT usize &count)														const;

		ND_ bool  ToGLSL (bool withOffsets, INOUT String &types, INOUT String &fields, INOUT UniqueTypes_t &uniqueTypes, Bytes baseOffset = 0_b) const;
		ND_ bool  ToGLSL (bool withOffsets, INOUT String &types, INOUT String &fields, Bytes baseOffset = 0_b) const;

		ND_ bool  StructToGLSL (INOUT String &types, INOUT UniqueTypes_t &uniqueTypes)					const;

		ND_ bool  ToMSL (INOUT String &types, INOUT UniqueTypes_t &uniqueTypes)							const;
		ND_ bool  ToMSL (INOUT String &types)															const;

		ND_ bool  ToCPP (INOUT String &types, INOUT UniqueTypes_t &uniqueTypes)							const;
		ND_ bool  ToCPP (INOUT String &types)															const;

		ND_ String  VertexInputToGLSL (const String &prefix, INOUT uint &loc)							C_Th___;
		ND_ String  VertexInputToMSL (const String &prefix, INOUT uint &index)							C_Th___;

		ND_ String  ToShaderIO_GLSL (EShader shaderType, bool input, INOUT UniqueTypes_t &uniqueTypes)	C_Th___;
		ND_ String  ToShaderIO_MSL (EShader shaderType, bool input, INOUT UniqueTypes_t &uniqueTypes)	C_Th___;

		void  GetVertexInput (INOUT uint &loc, INOUT Array<VertexInput> &arr)							C_Th___;

		static void  Bind (const ScriptEnginePtr &se)													__Th___;

	private:
		ND_ uint	_StaticSize ()																		const	{ return uint(StaticSize()); }
		ND_ uint	_ArrayStride ()																		const	{ return uint(ArrayStride()); }

		static void  _ParseFields (const String &fields, OUT Array<Field> &outFields)					__Th___;
		static void  _CalcOffsets (StringView, EStructLayout layout, INOUT Array<Field> &fields,
								   OUT Bytes &maxAlign, OUT Bytes &structAlign, OUT Bytes &totalSize)	__Th___;

		static void  _Validate (StringView, StringView, ArrayView<Field> fields, ValidationData &data)	__Th___;
		static void  _ValidateOffsets (const ValidationData &data, Bytes offset)						__Th___;
		static void  _AddPadding (EStructLayout layout, INOUT Array<Field> &fields)						__Th___;

		ND_ static EValueType  _VertexToAttrib (EValueType type)										__Th___;

		ND_ String  _VertexInputToGLSL (const String &prefix, INOUT uint &loc)							C_Th___;
		ND_ String  _VertexInputToMSL (const String &prefix, INOUT uint &index)							C_Th___;

		void  _ToShaderIO_GLSL (EShader, const String &prefix, bool useLocations, INOUT uint &loc,
								INOUT Array<Tuple< String, String, String >> &parts)					C_Th___;

		void  _ToShaderIO_MSL (EShader, const String &prefix,
								INOUT Array<Tuple< String, String, String >> &fieldParts)				C_Th___;

		ND_ static SizeAndAlign  _GetCPPSizeAndAlign2 (const Field &field);
		ND_ static SizeAndAlign  _GetCPPSizeAndAlign (const Field &field, EStructLayout layout);

		ND_ static SizeAndAlign  _GetMSLSizeAndAlign2 (const Field &field);
		ND_ static SizeAndAlign  _GetMSLSizeAndAlign (const Field &field, EStructLayout layout);

		ND_ static SizeAndAlign  _GetGLSLSizeAndAlign2 (const Field &field);
		ND_ static SizeAndAlign  _GetGLSLSizeAndAlign (const Field &field, EStructLayout layout);

		ND_ static bool  _CreatePackedTypeGLSL1 (INOUT String &outTypes, StringView packedTypeName, StringView memberTypeName, StringView dstType, const Field &);
		ND_ static bool  _CreatePackedTypeGLSL2 (INOUT String &outTypes, StringView packedTypeName, StringView memberTypeName, StringView dstType, const Field &);
		ND_ static bool  _CreatePackedTypeMSL (INOUT String &outTypes, StringView packedTypeName, StringView memberTypeName, StringView dstType, const Field &);
	};

	AE_BIT_OPERATORS( ShaderStructType::EUsage );
	AE_BIT_OPERATORS( ShaderStructType::EFlags );


	inline void  ShaderStructType::AddUsage (EUsage usage)	{ _usage |= usage; }

} // AE::PipelineCompiler
