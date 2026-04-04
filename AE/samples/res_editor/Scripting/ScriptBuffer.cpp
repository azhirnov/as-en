// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Scripting/ScriptExe.h"
#include "Scripting/PipelineCompiler.inl.h"

namespace AE::ResEditor
{
namespace
{
	static ScriptBuffer*  ScriptBuffer_Ctor1 (uint size) {
		return ScriptBufferPtr{ new ScriptBuffer{ Bytes{size} }}.Detach();
	}

	static ScriptBuffer*  ScriptBuffer_Ctor2 (const String &filename) {
		return ScriptBufferPtr{ new ScriptBuffer{ filename }}.Detach();
	}

} // namespace


/*
=================================================
	BufferLayout::Put
=================================================
*/
	uint  ScriptBuffer::BufferLayout::Put (const void* data, const Bytes rowDataSize, const usize rowCount, const Bytes align)
	{
		Bytes	offset		= AlignUp( Bytes{_data.size()}, align );
		Bytes	aligned_row	= AlignUp( rowDataSize, align );
		Bytes	new_size	= AlignUp( offset, align ) + aligned_row * rowCount;

		_data.resize( usize(new_size) );

		for (uint row = 0; row < rowCount; ++row)
		{
			MemCopy( OUT _data.data() + offset + aligned_row * row, data + rowDataSize * row, rowDataSize );
		}
		return uint{offset};
	}

	uint  ScriptBuffer::BufferLayout::Put (const void* data, const Bytes dataSize, const Bytes align)
	{
		return Put( data, dataSize, 1u, align );
	}

/*
=================================================
	BufferLayout::Put
=================================================
*/
	uint  ScriptBuffer::BufferLayout::Put (const Bytes dataSize, const Bytes align)
	{
		Bytes	offset		= AlignUp( Bytes{_data.size()}, align );
		Bytes	new_size	= AlignUp( offset + dataSize, align );

		_data.resize( usize(new_size) );
		return uint{offset};
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	ScriptBuffer::ScriptBuffer (Bytes size) __Th___ :
		_type{ EBufferType::MutableData_NonInitialized }
	{
		_desc.size = size;
	}

	ScriptBuffer::ScriptBuffer (const String &filename) __Th___ :
		_filename{ filename },
		_type{ EBufferType::ConstDataFromFile },
		_dbgName{ filename.substr( 0, ResNameMaxLen ) }
	{
		CHECK_THROW_MSG( GetVFS().Exists( _filename ),
			"File '"s << filename << "' is not exists" );

		AddUsage( EResourceUsage::UploadedData );

		_outDynCount = ScriptDynamicUIntPtr{ new ScriptDynamicUInt{ MakeRC<DynamicUInt>() }};
	}

/*
=================================================
	destructor
=================================================
*/
	ScriptBuffer::~ScriptBuffer ()
	{
		if ( not _resource )
			AE_LOGW( "Unused buffer '"s << _dbgName << "'" );
	}

/*
=================================================
	GetTypeName
=================================================
*/
	String  ScriptBuffer::GetTypeName () C_NE___
	{
		return	_IsArray() ?
					_layout.typeName + (_layout.staticSrc.empty() ? "_Array" : "_Array2") :
					_layout.typeName;
	}

/*
=================================================
	_IsArray
=================================================
*/
	bool  ScriptBuffer::_IsArray () C_NE___
	{
		return	_inDynCount or
				(_staticCount > 0 and _staticCount != UMax);
	}

/*
=================================================
	GetDeviceAddress
=================================================
*/
	ulong  ScriptBuffer::GetDeviceAddress () __Th___
	{
		CHECK_THROW_MSG( not IsDynamicSize(),
			"Can not get DeviceAddress from buffer with dynamic size" );

		AddUsage( EResourceUsage::ShaderAddress );

		// can not new usage after 'ToResource()', so add usages which may be required later
		AddUsage( EResourceUsage::Transfer );
		AddUsage( EResourceUsage::WillReadback );

		auto	buf = ToResource();
		CHECK_THROW( buf );
		CHECK_THROW( not buf->HasHistory() );	// TODO: return dynamic value?

		ulong	addr = buf->GetDeviceAddress( 0 );
		CHECK_THROW( addr != 0 );

		return addr;
	}

/*
=================================================
	_SetType
=================================================
*/
	void  ScriptBuffer::_SetType (EBufferType type) __Th___
	{
		CHECK_THROW_MSG( not _resource,
			"Resource is already created, can not change buffer type" );

		if ( _type == type )
			return;

		if ( _type == Default )
		{
			_type = type;
			return;
		}

		if ( _type == EBufferType::ConstDataFromScript and type == EBufferType::MutableDataFromScript )
		{
			_type = type;
			return;
		}

		if ( type == EBufferType::ConstDataFromScript and _type == EBufferType::MutableDataFromScript )
			return;	// ignore

		if ( type == EBufferType::MutableData_NonInitialized and
			 (_type == EBufferType::ConstDataFromFile or _type == EBufferType::ConstDataFromScript) )
			return;	// ignore

		CHECK_THROW_MSG( false,
			"Can not change buffer layout type" );
	}

/*
=================================================
	AddUsage
=================================================
*/
	void  ScriptBuffer::AddUsage (EResourceUsage usage) __Th___
	{
		if ( not AllBits( _resUsage, usage ))
		{
			CHECK_THROW_MSG( not _resource,
				"Resource is already created, can not change usage or content" );
		}

		_resUsage |= usage;

		_ValidateResourceUsage( _resUsage );
	}

/*
=================================================
	_ValidateResourceUsage
=================================================
*/
	void  ScriptBuffer::_ValidateResourceUsage (const EResourceUsage usage) __Th___
	{
		CHECK_THROW_MSG( NoBits( usage, EResourceUsage::ColorAttachment ));
		CHECK_THROW_MSG( NoBits( usage, EResourceUsage::DepthStencil ));
		CHECK_THROW_MSG( NoBits( usage, EResourceUsage::GenMipmaps ));

		// 'UploadedData' is compatible with any shader write usage.
		// allow 'Sampled' and 'TexelStorage' usage for texel buffer.

		auto&	fs = ScriptExe::ScriptResourceApi::GetFeatureSet();

		if ( AnyBits( usage, EResourceUsage::ASBuild ))
		{
			CHECK_THROW_MSG( fs.accelerationStructure() == FeatureSet::EFeature::RequireTrue,
				"AccelerationStructures are not supported" );
		}
		if ( AnyBits( usage, EResourceUsage::ShaderAddress ))
		{
			CHECK_THROW_MSG( fs.bufferDeviceAddress == FeatureSet::EFeature::RequireTrue,
				"ShaderAddress is not supported" );
		}
	}

/*
=================================================
	SetSize
=================================================
*/
	void  ScriptBuffer::SetSize (Bytes size, const String &typeName) __Th___
	{
		CHECK_THROW_MSG( not _resource,
			"Resource is already created, can not change size" );
		CHECK_THROW_MSG( _layout.Empty(),
			"Buffer has const data which will be lost" );

		_desc.size	= size;
		_layout		= BufferLayout{typeName};
	}

/*
=================================================
	SetLayoutName
=================================================
*/
	void  ScriptBuffer::SetLayoutName (const String &typeName) __Th___
	{
		CHECK_THROW_MSG( not _resource,
			"Resource is already created, can not change size" );
		CHECK_THROW_MSG( StartsWith( _layout.typeName, "ConstLayout-" ),
			"Buffer layout is already defined" );

		_layout.typeName = typeName;
	}

/*
=================================================
	_CanSetLayout
=================================================
*/
	void  ScriptBuffer::_CanSetLayout () __Th___
	{
		CHECK_THROW_MSG( _layout.typeName.empty(), "Buffer layout is already defined" );
		CHECK_THROW_MSG( _layout.source.empty(), "Buffer layout is already defined" );
		CHECK_THROW_MSG( _layout.staticSrc.empty(), "Buffer layout is already defined" );
		CHECK_THROW_MSG( (not _inDynCount) and (_staticCount == 0), "array size is already defined" );

		_SetType( EBufferType::MutableData_NonInitialized );  // throw
	}

/*
=================================================
	SetLayout*
=================================================
*/
	void  ScriptBuffer::SetLayout1 (const String &typeName) __Th___
	{
		_CanSetLayout();  // throw

		_layout.typeName = typeName;
		_staticCount	 = UMax;
	}

	void  ScriptBuffer::SetLayout2 (const String &typeName, const String &source) __Th___
	{
		_CanSetLayout();  // throw

		_layout.typeName = typeName;
		_layout.source	 = source;
		_staticCount	 = UMax;
	}

/*
=================================================
	SetArrayLayout*
=================================================
*/
	void  ScriptBuffer::SetArrayLayout1 (const String &typeName, uint count) __Th___
	{
		_CanSetLayout();  // throw
		CHECK_THROW_MSG( count > 0 );

		_layout.typeName = typeName;
		_staticCount	 = count;
	}

	void  ScriptBuffer::SetArrayLayout2 (const String &typeName, const ScriptDynamicUIntPtr &count) __Th___
	{
		_CanSetLayout();  // throw

		_layout.typeName = typeName;
		_inDynCount		 = count;
	}

	void  ScriptBuffer::SetArrayLayout3 (const String &typeName, const String &elemSource, uint count) __Th___
	{
		_CanSetLayout();  // throw
		CHECK_THROW_MSG( count > 0 );

		_layout.typeName = typeName;
		_layout.source	 = elemSource;
		_staticCount	 = count;
	}

	void  ScriptBuffer::SetArrayLayout4 (const String &typeName, const String &elemSource, const ScriptDynamicUIntPtr &count) __Th___
	{
		_CanSetLayout();  // throw

		_layout.typeName = typeName;
		_layout.source	 = elemSource;
		_inDynCount		 = count;
	}

/*
=================================================
	SetArrayLayout*
=================================================
*/
	void  ScriptBuffer::SetArrayLayout5 (const String &typeName, const String &elemSource, const String &staticSrc, uint count) __Th___
	{
		_CanSetLayout();  // throw
		CHECK_THROW_MSG( count > 0 );

		_layout.typeName	= typeName;
		_layout.source		= elemSource;
		_layout.staticSrc	= staticSrc;
		_staticCount		= count;
	}

	void  ScriptBuffer::SetArrayLayout6 (const String &typeName, const String &elemSource, const String &staticSrc, const ScriptDynamicUIntPtr &count) __Th___
	{
		_CanSetLayout();  // throw

		_layout.typeName	= typeName;
		_layout.source		= elemSource;
		_layout.staticSrc	= staticSrc;
		_inDynCount			= count;
	}

/*
=================================================
	Name
=================================================
*/
	void  ScriptBuffer::Name (const String &name) __Th___
	{
		CHECK_THROW_MSG( not _resource,
			"Resource is already created, can not change debug name" );

		_dbgName = name.substr( 0, ResNameMaxLen );
	}

/*
=================================================
	AddReference
=================================================
*/
	void  ScriptBuffer::AddReference (const ScriptBufferPtr &buf) __Th___
	{
		CHECK_THROW_MSG( buf,
			"Argument is null" );
		CHECK_THROW_MSG( not _resource,
			"Resource is already created, can not add buffer reference" );

		if ( not buf->_resource ) {
			buf->AddUsage( EResourceUsage::ShaderAddress );
		}else{
			CHECK_THROW_MSG( AllBits( buf->_resUsage, EResourceUsage::ShaderAddress ),
				"AddReference() used only to attach buffer with ShaderAddress usage" );
		}

		_refBuffers.push_back( buf );
	}

/*
=================================================
	Float* (vector)
=================================================
*/
	uint  ScriptBuffer::Float1 (const String &name, float x) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  float  " << name << ";\n";
		return _layout.Put( &x, Sizeof(x), 4_b );
	}

	uint  ScriptBuffer::Float2v (const String &name, const packed_float2 &v) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  float2  " << name << ";\n";
		return _layout.Put( &v, Sizeof(v), 8_b );
	}

	uint  ScriptBuffer::Float3v (const String &name, const packed_float3 &v) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  float3  " << name << ";\n";
		return _layout.Put( &v, Sizeof(v), 16_b );
	}

	uint  ScriptBuffer::Float4v (const String &name, const packed_float4 &v) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  float4  " << name << ";\n";
		return _layout.Put( &v, Sizeof(v), 16_b );
	}

/*
=================================================
	Float* (matrix)
=================================================
*/
	uint  ScriptBuffer::Float2x2 (const String &name, const packed_float2x2 &m) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  float2x2  " << name << ";\n";
		return _layout.Put( &m, Sizeof(m[0]), m.size(), 16_b );
	}

	uint  ScriptBuffer::Float2x3 (const String &name, const packed_float2x3 &m) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  float2x3  " << name << ";\n";
		return _layout.Put( &m, Sizeof(m[0]), m.size(), 16_b );
	}

	uint  ScriptBuffer::Float2x4 (const String &name, const packed_float2x4 &m) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  float2x4  " << name << ";\n";
		return _layout.Put( &m, Sizeof(m[0]), m.size(), 16_b );
	}

	uint  ScriptBuffer::Float3x2 (const String &name, const packed_float3x2 &m) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  float3x2  " << name << ";\n";
		return _layout.Put( &m, Sizeof(m[0]), m.size(), 16_b );
	}

	uint  ScriptBuffer::Float3x3 (const String &name, const packed_float3x3 &m) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  float3x3  " << name << ";\n";
		return _layout.Put( &m, Sizeof(m[0]), m.size(), 16_b );
	}

	uint  ScriptBuffer::Float3x4 (const String &name, const packed_float3x4 &m) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  float3x4  " << name << ";\n";
		return _layout.Put( &m, Sizeof(m[0]), m.size(), 16_b );
	}

	uint  ScriptBuffer::Float4x2 (const String &name, const packed_float4x2 &m) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  float4x2  " << name << ";\n";
		return _layout.Put( &m, Sizeof(m[0]), m.size(), 16_b );
	}

	uint  ScriptBuffer::Float4x3 (const String &name, const packed_float4x3 &m) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  float4x3  " << name << ";\n";
		return _layout.Put( &m, Sizeof(m[0]), m.size(), 16_b );
	}

	uint  ScriptBuffer::Float4x4 (const String &name, const packed_float4x4 &m) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  float4x4  " << name << ";\n";
		return _layout.Put( &m, Sizeof(m[0]), m.size(), 16_b );
	}

/*
=================================================
	Int*
=================================================
*/
	uint  ScriptBuffer::Int1 (const String &name, int x) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  int  " << name << ";\n";
		return _layout.Put( &x, Sizeof(x), 4_b );
	}

	uint  ScriptBuffer::Int2v (const String &name, const packed_int2 &v) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  int2  " << name << ";\n";
		return _layout.Put( &v, Sizeof(v), 8_b );
	}

	uint  ScriptBuffer::Int3v (const String &name, const packed_int3 &v) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  int3  " << name << ";\n";
		return _layout.Put( &v, Sizeof(v), 16_b );
	}

	uint  ScriptBuffer::Int4v (const String &name, const packed_int4 &v) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  int4  " << name << ";\n";
		return _layout.Put( &v, Sizeof(v), 16_b );
	}

/*
=================================================
	UInt*
=================================================
*/
	uint  ScriptBuffer::UInt1 (const String &name, uint x) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  uint  " << name << ";\n";
		return _layout.Put( &x, Sizeof(x), 4_b );
	}

	uint  ScriptBuffer::UInt2v (const String &name, const packed_uint2 &v) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  uint2  " << name << ";\n";
		return _layout.Put( &v, Sizeof(v), 8_b );
	}

	uint  ScriptBuffer::UInt3v (const String &name, const packed_uint3 &v) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  uint3  " << name << ";\n";
		return _layout.Put( &v, Sizeof(v), 16_b );
	}

	uint  ScriptBuffer::UInt4v (const String &name, const packed_uint4 &v) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  uint4  " << name << ";\n";
		return _layout.Put( &v, Sizeof(v), 16_b );
	}

/*
=================================================
	ULong*
=================================================
*/
	uint  ScriptBuffer::ULong1 (const String &name, ulong x) __Th___
	{
		_InitConstDataFromScriptLayout();
		_layout.source << "  ulong  " << name << ";\n";
		return _layout.Put( &x, Sizeof(x), 8_b );
	}

/*
=================================================
	Float*Array2 (vec)
=================================================
*/
	uint  ScriptBuffer::Float1Array2 (StringView name, ArrayView<float> arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  float  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x), 4_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Float2Array2 (StringView name, ArrayView<packed_float2> arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  float2  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x), 8_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Float3Array2 (StringView name, ArrayView<packed_float3> arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  float3  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Float4Array2 (StringView name, ArrayView<packed_float4> arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  float4  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
		}
		return result;
	}

/*
=================================================
	Float*Array (matrix)
=================================================
*/
	uint  ScriptBuffer::Float2x2Array (const String &name, const ScriptArray<packed_float2x2> &arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  float2x2  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x[0]), x.size(), 16_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Float2x3Array (const String &name, const ScriptArray<packed_float2x3> &arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  float2x3  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x[0]), x.size(), 16_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Float2x4Array (const String &name, const ScriptArray<packed_float2x4> &arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  float2x4  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x[0]), x.size(), 16_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Float3x2Array (const String &name, const ScriptArray<packed_float3x2> &arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  float3x2  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x[0]), x.size(), 16_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Float3x3Array (const String &name, const ScriptArray<packed_float3x3> &arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  float3x3  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x[0]), x.size(), 16_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Float3x4Array (const String &name, const ScriptArray<packed_float3x4> &arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  float3x4  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x[0]), x.size(), 16_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Float4x2Array (const String &name, const ScriptArray<packed_float4x2> &arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  float4x2  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x[0]), x.size(), 16_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Float4x3Array (const String &name, const ScriptArray<packed_float4x3> &arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  float4x3  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x[0]), x.size(), 16_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Float4x4Array (const String &name, const ScriptArray<packed_float4x4> &arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  float4x4  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x[0]), x.size(), 16_b ));
		}
		return result;
	}

/*
=================================================
	Int*Array2
=================================================
*/
	uint  ScriptBuffer::Int1Array2 (StringView name, ArrayView<int> arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  int  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x), 4_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Int2Array2 (StringView name, ArrayView<packed_int2> arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  int2  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x), 8_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Int3Array2 (StringView name, ArrayView<packed_int3> arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  int3  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
		}
		return result;
	}

	uint  ScriptBuffer::Int4Array2 (StringView name, ArrayView<packed_int4> arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  int4  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
		}
		return result;
	}

/*
=================================================
	UInt*Array2
=================================================
*/
	uint  ScriptBuffer::UInt1Array2 (StringView name, ArrayView<uint> arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  uint  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x), 4_b ));
		}
		return result;
	}

	uint  ScriptBuffer::UInt2Array2 (StringView name, ArrayView<packed_uint2> arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  uint2  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x), 8_b ));
		}
		return result;
	}

	uint  ScriptBuffer::UInt3Array2 (StringView name, ArrayView<packed_uint3> arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  uint3  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
		}
		return result;
	}

	uint  ScriptBuffer::UInt4Array2 (StringView name, ArrayView<packed_uint4> arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  uint4  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x), 16_b ));
		}
		return result;
	}

/*
=================================================
	ULong*Array2
=================================================
*/
	uint  ScriptBuffer::ULong1Array2 (StringView name, ArrayView<ulong> arr) __Th___
	{
		CHECK_THROW_MSG( not arr.empty() );
		_InitConstDataFromScriptLayout();
		_layout.source << "  ulong  " << name << " [" << ToString(arr.size()) << "];\n";

		uint	result = UMax;
		for (auto x : arr) {
			result = Min( result, _layout.Put( &x, Sizeof(x), 8_b ));
		}
		return result;
	}

/*
=================================================
	Bind
=================================================
*/
	void  ScriptBuffer::Bind (const ScriptEnginePtr &se) __Th___
	{
		Scripting::ClassBinder<ScriptBuffer>	binder{ se };
		binder.CreateRef();
		binder.AddFactoryCtor( &ScriptBuffer_Ctor1,		{"sizeInBytes"} );
		binder.AddFactoryCtor( &ScriptBuffer_Ctor2,		{"filenameInVFS"} );

		binder.Comment( "Set resource name. It is used for debugging." );
		AS_METHOD( binder, ScriptBuffer::Name,					"Name",				{} );

		binder.Comment( "Get buffer type name. Result is valid only after 'Layout*()' methods.\n"
						"Can be used for debugging: 'LogInfo( buf.TypeName() );'" );
		AS_METHOD( binder, ScriptBuffer::GetTypeName,			"TypeName",			{} );

		binder.Comment( "Set explicit name of the 'ShaderStructType' which will be created for buffer data layout.\n"
						"It is used when buffer is passed to the pipeline which is explicitly declared (in 'pipelines' folder)\n"
						"so typename must match in 'Layout()' and in 'ds.StorageBuffer()' call in pipeline script." );
		AS_METHOD( binder, ScriptBuffer::SetLayoutName,			"LayoutName",		{"typeName"} );

		binder.Comment( "Allow to declare array of struct with constant or dynamic size.\n"
						"Layout will be '{ <arrayElementTypeName>  elements [<count>]; }'.\n"
						"'arrayElementTypeName' must be previously declared or one of built-in type:\n"
						"\t'DispatchIndirectCommand', 'DrawIndirectCommand', 'DrawIndexedIndirectCommand',\n"
						"\t'DrawMeshTasksIndirectCommand', 'TraceRayIndirectCommand', 'ASBuildIndirectCommand'\n"
						"\t'AccelStructInstance'.\n"
						"Buffer typename: '<arrayElementTypeName>_Array'." );
		AS_METHOD( binder, ScriptBuffer::SetArrayLayout1,		"ArrayLayout",		{"arrayElementTypeName", "count"} );
		AS_METHOD( binder, ScriptBuffer::SetArrayLayout2,		"ArrayLayout",		{"arrayElementTypeName", "count"} );

		binder.Comment( "Allow to declare array of struct with constant or dynamic size.\n"
						"Created a new structure with type 'arrayElementTypeName' and fields in 'arrayElementSource'.\n"
						"See field declaration rules for 'ShaderStructType::Set()' method in [pipeline_compiler.as](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/pipeline_compiler.as).\n"
						"Buffer typename: '<arrayElementTypeName>_Array'." );
		AS_METHOD( binder, ScriptBuffer::SetArrayLayout3,		"ArrayLayout",		{"arrayElementTypeName", "arrayElementSource", "count"} );
		AS_METHOD( binder, ScriptBuffer::SetArrayLayout4,		"ArrayLayout",		{"arrayElementTypeName", "arrayElementSource", "count"} );

		binder.Comment( "Buffer typename: '<arrayElementTypeName>_Array2'." );
		AS_METHOD( binder, ScriptBuffer::SetArrayLayout5,		"ArrayLayout",		{"arrayElementTypeName", "arrayElementSource", "staticSource", "count"} );
		AS_METHOD( binder, ScriptBuffer::SetArrayLayout6,		"ArrayLayout",		{"arrayElementTypeName", "arrayElementSource", "staticSource", "count"} );

		binder.Comment( "Allow to declare single structure as a buffer layout.\n"
						"'typeName' must be previously declared or one of built-in type (see 'ArrayLayout')." );
		AS_METHOD( binder, ScriptBuffer::SetLayout1,			"UseLayout",		{"typeName"} );

		binder.Comment( "Created a new structure with type 'typeName' and fields in 'source'.\n"
						"See field declaration rules for 'ShaderStructType::Set()' method in [pipeline_compiler.as](https://github.com/azhirnov/as-en/blob/dev/AE/engine/shared_data/scripts/pipeline_compiler.as).");
		AS_METHOD( binder, ScriptBuffer::SetLayout2,			"UseLayout",		{"typeName", "source"} );

		binder.Comment( "Returns buffer device address.\n"
						"Requires 'GL_EXT_buffer_reference' extension in GLSL.\n"
						"It passed as 'uint64' type so you should cast it to buffer reference type." );
		AS_METHOD( binder, ScriptBuffer::GetDeviceAddress,		"DeviceAddress",	{} );

		binder.Comment( "Force enable buffer content history.\n"
						"It store copy of the buffer content on last N frames." );
		AS_METHOD( binder, ScriptBuffer::EnableHistory,			"EnableHistory",	{} );

		binder.Comment( "Call this method if 'DeviceAddress()' of another buffer is used in current buffer to avoid missed synchronizations." );
		AS_METHOD( binder, ScriptBuffer::AddReference,			"AddReference",		{} );

		binder.Comment( "Dynamic array size, can be used for draw call." );
		AS_METHOD( binder, ScriptBuffer::ArraySize,				"ArraySize",		{} );

		binder.Comment( "Constant array size, can be used for draw call." );
		AS_METHOD( binder, ScriptBuffer::ConstArraySize,		"ConstArraySize",	{} );

		binder.Comment( "Load CSV and put it to buffer with same column names." );
		AS_METHOD( binder, ScriptBuffer::LoadCSV,				"LoadCSV",			{"filename", "elemTypeName"} );

		binder.Comment( "Build buffer data layout with initial content.\n"
						"Returns offset in bytes where data is begin." );

		AS_METHOD( binder, ScriptBuffer::Float1,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float2,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float3,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float4,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float2v,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float3v,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float4v,		"Float",		{} );

		AS_METHOD( binder, ScriptBuffer::Float2x2,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float2x3,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float2x4,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float3x2,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float3x3,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float3x4,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float4x2,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float4x3,		"Float",		{} );
		AS_METHOD( binder, ScriptBuffer::Float4x4,		"Float",		{} );

		AS_METHOD( binder, ScriptBuffer::Int1,			"Int",			{} );
		AS_METHOD( binder, ScriptBuffer::Int2,			"Int",			{} );
		AS_METHOD( binder, ScriptBuffer::Int3,			"Int",			{} );
		AS_METHOD( binder, ScriptBuffer::Int4,			"Int",			{} );
		AS_METHOD( binder, ScriptBuffer::Int2v,			"Int",			{} );
		AS_METHOD( binder, ScriptBuffer::Int3v,			"Int",			{} );
		AS_METHOD( binder, ScriptBuffer::Int4v,			"Int",			{} );

		AS_METHOD( binder, ScriptBuffer::UInt1,			"Uint",			{} );
		AS_METHOD( binder, ScriptBuffer::UInt2,			"Uint",			{} );
		AS_METHOD( binder, ScriptBuffer::UInt3,			"Uint",			{} );
		AS_METHOD( binder, ScriptBuffer::UInt4,			"Uint",			{} );
		AS_METHOD( binder, ScriptBuffer::UInt2v,		"Uint",			{} );
		AS_METHOD( binder, ScriptBuffer::UInt3v,		"Uint",			{} );
		AS_METHOD( binder, ScriptBuffer::UInt4v,		"Uint",			{} );

		AS_METHOD( binder, ScriptBuffer::ULong1,		"ULong",		{} );

		AS_METHOD( binder, ScriptBuffer::Float1Array,	"FloatArray",	{} );
		AS_METHOD( binder, ScriptBuffer::Float2Array,	"FloatArray",	{} );
		AS_METHOD( binder, ScriptBuffer::Float3Array,	"FloatArray",	{} );
		AS_METHOD( binder, ScriptBuffer::Float4Array,	"FloatArray",	{} );

		AS_METHOD( binder, ScriptBuffer::Float2x2Array,	"FloatArray",	{} );
		AS_METHOD( binder, ScriptBuffer::Float2x3Array,	"FloatArray",	{} );
		AS_METHOD( binder, ScriptBuffer::Float2x4Array,	"FloatArray",	{} );
		AS_METHOD( binder, ScriptBuffer::Float3x2Array,	"FloatArray",	{} );
		AS_METHOD( binder, ScriptBuffer::Float3x3Array,	"FloatArray",	{} );
		AS_METHOD( binder, ScriptBuffer::Float3x4Array,	"FloatArray",	{} );
		AS_METHOD( binder, ScriptBuffer::Float4x2Array,	"FloatArray",	{} );
		AS_METHOD( binder, ScriptBuffer::Float4x3Array,	"FloatArray",	{} );
		AS_METHOD( binder, ScriptBuffer::Float4x4Array,	"FloatArray",	{} );

		AS_METHOD( binder, ScriptBuffer::Int1Array,		"IntArray",		{} );
		AS_METHOD( binder, ScriptBuffer::Int2Array,		"IntArray",		{} );
		AS_METHOD( binder, ScriptBuffer::Int3Array,		"IntArray",		{} );
		AS_METHOD( binder, ScriptBuffer::Int4Array,		"IntArray",		{} );

		AS_METHOD( binder, ScriptBuffer::UInt1Array,	"UIntArray",	{} );
		AS_METHOD( binder, ScriptBuffer::UInt2Array,	"UIntArray",	{} );
		AS_METHOD( binder, ScriptBuffer::UInt3Array,	"UIntArray",	{} );
		AS_METHOD( binder, ScriptBuffer::UInt4Array,	"UIntArray",	{} );

		AS_METHOD( binder, ScriptBuffer::ULong1Array,	"ULongArray",	{} );
	}

/*
=================================================
	_InitConstDataFromScriptLayout
=================================================
*/
	void  ScriptBuffer::_InitConstDataFromScriptLayout () __Th___
	{
		_SetType( EBufferType::ConstDataFromScript );  // throw

		if ( _layout.typeName.empty() )
			_layout.typeName = "ConstLayout-"s << ToString<16>( BitCast<usize>(this) );
	}

/*
=================================================
	_InitMutableDataFromScriptLayout
=================================================
*/
	void  ScriptBuffer::_InitMutableDataFromScriptLayout () __Th___
	{
		_SetType( EBufferType::MutableDataFromScript );  // throw

		if ( _layout.typeName.empty() )
			_layout.typeName = "MutableLayout-"s << ToString<16>( BitCast<usize>(this) );
	}

/*
=================================================
	ToResource
=================================================
*/
	RC<Buffer>  ScriptBuffer::ToResource () __Th___
	{
		if ( _resource )
			return _resource;

		AddLayoutReflection();  // throw

		Buffer::EBufferFlags	flags = Default;

		CHECK_THROW_MSG( _resUsage != Default, "failed to create buffer '"s << _dbgName << "'" );
		for (auto usage : BitfieldIterate( _resUsage ))
		{
			switch_enum( usage )
			{
				case EResourceUsage::ComputeRead :		_desc.usage |= EBufferUsage::Storage | EBufferUsage::TransferSrc;		break;
				case EResourceUsage::ComputeWrite :		_desc.usage |= EBufferUsage::Storage;									break;

				case EResourceUsage::UploadedData :		_desc.usage |= EBufferUsage::TransferDst;								break;
				case EResourceUsage::WillReadback :		_desc.usage |= EBufferUsage::TransferSrc;								break;

				case EResourceUsage::VertexInput :		_desc.usage |= EBufferUsage::Vertex | EBufferUsage::Index;				break;

				case EResourceUsage::IndirectBuffer :	_desc.usage |= EBufferUsage::Indirect;									break;
				case EResourceUsage::ASBuild :			_desc.usage |= EBufferUsage::ASBuild_ReadOnly;							break;
				case EResourceUsage::MMBuild :			_desc.usage |= EBufferUsage::MMBuild_ReadOnly;							break;
				case EResourceUsage::ShaderAddress :	_desc.usage |= EBufferUsage::ShaderAddress;								break;

				case EResourceUsage::WithHistory :		flags |= Buffer::EBufferFlags::WithHistory;								break;
				case EResourceUsage::Transfer :			_desc.usage |= EBufferUsage::Transfer;									break;

				case EResourceUsage::Sampled :			_desc.usage |= EBufferUsage::UniformTexel;								break;
				case EResourceUsage::TexelStorage :		_desc.usage |= EBufferUsage::StorageTexel;								break;

				case EResourceUsage::Unknown :
				case EResourceUsage::GenMipmaps :
				case EResourceUsage::ColorAttachment :
				case EResourceUsage::DepthStencil :
				case EResourceUsage::ComputeRW :
				case EResourceUsage::Present :
				case EResourceUsage::InputAttachment :
				case EResourceUsage::FragDensityMap :
				case EResourceUsage::FragShadingRate :
				case EResourceUsage::SubsampledAttachment :
				default :								RETURN_ERR( "unsupported usage" );
			}
			switch_end
		}

		switch_enum( _type )
		{
			case EBufferType::ConstDataFromScript :
			{
				CHECK_THROW_MSG( HasLayout() );
				_desc.size = Bytes{_layout._data.size()};
				break;
			}
			case EBufferType::ConstDataFromFile :				break;
			case EBufferType::MutableData_NonInitialized :		break;
			case EBufferType::MutableDataFromScript :			break;
			case EBufferType::Unknown :							break;
		}
		switch_end

		Bytes	static_size;
		Bytes	elem_size;

		if ( HasLayout() )
		{
			using namespace AE::PipelineCompiler;

			auto	storage = ObjectStorage::Instance();
			if ( storage )	// must be inside '_RunWithPipelineCompiler()'
			{
				auto&	st_types	= storage->structTypes;
				auto	it			= st_types.find( GetTypeName() );
				CHECK_THROW_MSG( it != st_types.end(),
					"Can't find ShaderStructType '"s << GetTypeName() << "'" );

				if ( _inDynCount )
				{
					CHECK_THROW_MSG( it->second->HasDynamicArray() );
					CHECK_THROW_MSG( _desc.size == 0 );

					elem_size	= it->second->ArrayStride();
					static_size	= it->second->StaticSize();
					_desc.size	= static_size + elem_size;
				}

				if ( _staticCount > 0 )
				{
					const bool	is_array = (_staticCount != UMax);
					CHECK_THROW_MSG( is_array == it->second->HasDynamicArray() );
					CHECK_THROW_MSG( _desc.size == 0 );

					elem_size	= it->second->ArrayStride();
					static_size	= it->second->StaticSize();
					_desc.size	= static_size + (is_array ? elem_size * _staticCount : 0_b);
				}
			}
			ASSERT( not (_inDynCount or _staticCount > 0) or storage );
		}

		Buffer::LoadOp		load_op;
		load_op.clear		= (not _filename.IsDefined()) and _layout._data.empty();
		load_op.data		= RVRef(_layout._data);

		_desc.usage |= EBufferUsage::TransferDst;

		if ( _filename.IsDefined() )
		{
			CHECK_THROW_MSG( GetVFS().Open( OUT load_op.file, _filename ));
			_desc.size = load_op.file->Size();

			if ( HasLayout() )
			{
				CHECK_THROW_MSG( _desc.size >= static_size );
				CHECK_THROW_MSG( elem_size == 0 or IsMultipleOf( _desc.size - static_size, elem_size ));
			}
		}

		auto&				res_mngr	= GraphicsScheduler().GetResourceManager();
		Renderer&			renderer	= ScriptExe::ScriptResourceApi::GetRenderer(); // throw
		GfxMemAllocatorPtr	gfx_alloc	= renderer.ChooseAllocator( Bool{_inDynCount}, _desc.size );

		CHECK_THROW_MSG( _desc.size > 0,
			"Buffer '"s << _dbgName << "' size is 0" );
		CHECK_THROW_MSG( res_mngr.IsSupported( _desc ),
			"Buffer '"s << _dbgName << "' description is not supported by GPU device" );

		const ShaderStructName	struct_type	{GetTypeName()};
		Buffer::IDs_t			buf_ids;

		ON_DESTROY( [&buf_ids, &res_mngr](){ res_mngr.ReleaseResourceArray( buf_ids ); });

		if ( AllBits( flags, Buffer::EBufferFlags::WithHistory ))
		{
			for (auto& id : buf_ids) {
				id = res_mngr.CreateBuffer( _desc, _dbgName, gfx_alloc );
				CHECK_THROW_MSG( id );
			}
		}
		else
		{
			buf_ids[0] = res_mngr.CreateBuffer( _desc, _dbgName, gfx_alloc );
			CHECK_THROW_MSG( buf_ids[0] );

			for (usize i = 1; i < buf_ids.size(); ++i) {
				buf_ids[i] = res_mngr.AcquireResource( buf_ids[0].Get() );
			}
		}

		Array<RC<Buffer>>	ref_buffers;
		ref_buffers.reserve( _refBuffers.size() );

		for (auto& src : _refBuffers) {
			ref_buffers.push_back( src->ToResource() );
		}

		Array<RC<BufferView>>	views;
		views.reserve( _views.size() );

		for (auto& src : _views) {
			views.push_back( src->ToResource() );
		}

		_resource = Buffer::Create( RVRef(buf_ids), _desc, static_size, elem_size, RVRef(load_op), struct_type,
									renderer, (_inDynCount ? _inDynCount->Get() : null), (_outDynCount ? _outDynCount->Get() : null),
									_dbgName, flags, RVRef(ref_buffers), RVRef(views) );  // throw
		return _resource;
	}

/*
=================================================
	AddLayoutReflection
=================================================
*/
	void  ScriptBuffer::AddLayoutReflection () C_Th___
	{
		using namespace AE::PipelineCompiler;

		if ( not HasLayout() )
			return;

		const auto	AddStructType = [] (const String &typeName, const String &source, Bool srcIsOptional = False{})
		{{
			auto	storage		= ObjectStorage::Instance();
			if ( not storage )
				return;	// must be inside '_RunWithPipelineCompiler()'

			auto&	st_types	= storage->structTypes;
			auto	it			= st_types.find( typeName );

			if ( it != st_types.end() )
			{
				const String	tmp_typename = "_Temp_"s + typeName;

				if ( st_types.contains( tmp_typename ))
					return;	// double check

				// compare 'source' with existing structure
				if ( not source.empty() )
				{
					ShaderStructTypePtr		tmp = ShaderStructType::Create( tmp_typename );
					tmp->Set( EStructLayout::Compatible_Std430, source );

					CHECK_THROW_MSG( it->second->Compare( *tmp ),
						"Typename '"s << typeName << "' is already defined with different layout" );

					st_types.erase( tmp_typename );
				}
				return;
			}

			if ( source.empty() and srcIsOptional )
				return;

			ShaderStructTypePtr		st = ShaderStructType::Create( typeName );
			st->Set( EStructLayout::Compatible_Std430, source );
		}};

		if ( _IsArray() )
		{
			String	src;
			src << _layout.staticSrc << '\n';
			src << _layout.typeName << "  elements [];";

			AddStructType( _layout.typeName, _layout.source, True{"opt"} );
			AddStructType( GetTypeName(), src );
		}
		else
		{
			AddStructType( _layout.typeName, _layout.source );
		}
	}

/*
=================================================
	GetFieldOffset
=================================================
*/
	Bytes  ScriptBuffer::GetFieldOffset (const String &name) __Th___
	{
		auto*	field = GetField( name ).GetIf< PipelineCompiler::ShaderStructType::Field >();	// throw
		CHECK_THROW_MSG( field != null,
			"field '"s << name << "' is not exists in '" << GetTypeName() << "'" );
		return field->offset;
	}

/*
=================================================
	GetFieldStructName
=================================================
*/
	StringView  ScriptBuffer::GetFieldStructName (const String &name) __Th___
	{
		auto*	field = GetField( name ).GetIf< PipelineCompiler::ShaderStructType::Field >();	// throw
		CHECK_THROW_MSG( field != null,
			"field '"s << name << "' is not exists in '" << GetTypeName() << "'" );
		return	field->IsStruct() ? field->stType->Typename() : Default;
	}

/*
=================================================
	GetFieldType
=================================================
*/
	uint  ScriptBuffer::GetFieldType (const String &name) __Th___
	{
		auto*	field = GetField( name ).GetIf< PipelineCompiler::ShaderStructType::Field >();	// throw
		CHECK_THROW_MSG( field != null,
			"field '"s << name << "' is not exists in '" << GetTypeName() << "'" );
		CHECK_THROW_MSG( field->IsScalar(),
			"field '"s << name << "' is not scalar" );
		CHECK_THROW_MSG( not field->stType,
			"field '"s << name << "' must not be structure" );
		return uint(field->type);
	}

/*
=================================================
	GetField
=================================================
*/
	AnyTypeCRef  ScriptBuffer::GetField (const String &name) __Th___
	{
		using namespace AE::PipelineCompiler;

		CHECK_THROW_MSG( HasLayout(),
			"Buffer layout is not defined" );

		auto	storage		= ObjectStorage::Instance();
		CHECK_THROW_MSG( storage );	// must be inside '_RunWithPipelineCompiler()'

		auto&	st_types	= storage->structTypes;
		auto	it			= st_types.find( GetTypeName() );

		// use 'AddLayoutReflection()' if see this error.
		CHECK_THROW_MSG( it != st_types.end(),
			"Can't find buffer layout type '"s << GetTypeName() << "'" );

		for (auto& field : it->second->Fields())
		{
			if ( field.name == name )
				return AnyTypeCRef{field};
		}

		CHECK_THROW_MSG( false,
			"Failed to find field '"s << name << "' for type '" << GetTypeName() << "'" );
	}

/*
=================================================
	ArraySize*
=================================================
*/
	ScriptDynamicUInt*  ScriptBuffer::ArraySize () C_Th___
	{
		return ArraySizeRC().Detach();
	}

	ScriptDynamicUIntPtr  ScriptBuffer::ArraySizeRC () C_Th___
	{
		ScriptDynamicUIntPtr	result;
		if ( _inDynCount )		result = _inDynCount;
		if ( _outDynCount )		result = _outDynCount;

		ASSERT_MSG( _staticCount == 0, "use ConstArraySize() instead" );
		return result;
	}

/*
=================================================
	LoadCSV
=================================================
*/
	void  ScriptBuffer::LoadCSV (const String &filename, const String &elemTypeName) __Th___
	{
		_CanSetLayout();  // throw
		CHECK_THROW_MSG( not _filename.IsDefined(), "Buffer data is already loaded from file" );

		const VFS::FileName		fname {filename};
		CHECK_THROW_MSG( GetVFS().Exists( fname ), "File '"s << filename << "' is not exists" );

		String	csv_data;
		{
			RC<RStream>		file;
			CHECK_THROW( GetVFS().Open( OUT file, fname ));
			CHECK_THROW( file->Read( file->RemainingSize(), OUT csv_data ));
		}

		enum class EType
		{
			Unknown		= 0,
			Float		= 1 << 0,
			Int			= 1 << 1,
			Vec			= 1 << 2,
			Array		= 1 << 3,
			Scalar		= 1 << 4,

			ValueMask	= Float | Int,
			DimMask		= Vec | Array | Scalar,

			_BITOPS_
		};

		Array< Array<StringView> >							rows;
		Array< Tuple< StringView, EType, Array<uint> >>		column_groups;
		{
			HashMap<StringView, uint>	column_names;
			CHECK_THROW( Parser::ParseCSV( csv_data, OUT column_names, OUT rows ));

			CHECK_THROW( not column_names.empty() and not rows.empty() );

			// find vectors and arrays
			for (auto& [name, col] : column_names)
			{
				if ( col == UMax )
					continue; // already processed

				// array of vectors
				if ( EndsWith( name, "[0].x" ))
				{
					ASSERT( false );	// not supported yet
				}

				// vector
				if ( EndsWith( name, ".x" ))
				{
					auto&	[group_name, type, col_arr] = column_groups.emplace_back();

					group_name	= name.substr( 0, name.size()-2 );
					type		= EType::Vec;

					col_arr.push_back( col );
					col = UMax;

					const char	xyzw[] = "xyzw";
					for (uint i = 1; i < 4; ++i)
					{
						auto	it = column_names.find( String{group_name} << '.' << xyzw[i] );
						if ( it == column_names.end() )
							break;

						ASSERT( it->second != UMax );
						col_arr.push_back( it->second );
						it->second = UMax;
					}
					continue;
				}

				// array
				if ( EndsWith( name, "[0]" ))
				{
					auto&	[group_name, type, col_arr] = column_groups.emplace_back();

					group_name	= name.substr( 0, name.size()-3 );
					type		= EType::Array;

					col_arr.push_back( col );
					col = UMax;

					for (uint i = 1;; ++i)
					{
						auto	it = column_names.find( String{group_name} << '[' << ToString(i) << ']' );
						if ( it == column_names.end() )
							break;

						ASSERT( it->second != UMax );
						col_arr.push_back( it->second );
						it->second = UMax;
					}
					continue;
				}
			}

			// add scalars
			for (auto& [name, col] : column_names)
			{
				if ( col == UMax )
					continue; // already processed

				auto&	[group_name, type, col_arr] = column_groups.emplace_back();

				group_name	= name;
				type		= EType::Scalar;

				col_arr.push_back( col );
				col = UMax;
			}

			// detect type
			usize	count = 0;
			for (auto& [name, type, col_arr] : column_groups)
			{
				ASSERT( not name.empty() );
				ASSERT( not col_arr.empty() );
				ASSERT( type != Default );

				count += col_arr.size();
				bool	is_int	= true;

				for (uint col : col_arr)
				{
					if ( rows[0][col].contains( '.' ))
					{
						is_int = false;
						break;
					}
				}

				type |= (is_int ? EType::Int : EType::Float);
			}

			CHECK( count == column_names.size() );
		}

		_staticCount = uint(rows.size());
		_layout.typeName = elemTypeName;

		Bytes	elem_size;
		for (auto& [name, type, col_arr] : column_groups)
		{
			Bytes	size = 4_b;

			switch ( type & EType::ValueMask )
			{
				case EType::Float :		_layout.source << "float";	break;
				case EType::Int :		_layout.source << "int";	break;
				default :				CHECK( false );				break;
			}

			switch ( type & EType::DimMask )
			{
				case EType::Scalar :
					_layout.source << "  " << name << ";\n";
					ASSERT( col_arr.size() == 1 );
					break;

				case EType::Vec :
					_layout.source << ToString( col_arr.size() ) << "  " << name << ";\n";
					size *= (col_arr.size() == 3 ? 4 : col_arr.size());
					elem_size = AlignUp( elem_size, size );
					break;

				case EType::Array :
					_layout.source << "  " << name << '[' << ToString( col_arr.size() ) << "];\n";
					size *= col_arr.size();
					break;

				case EType::Vec | EType::Array :	// TODO
				default :
					CHECK( false );
					break;
			}

			elem_size += size;
		}

		_layout._data.reserve( usize{ elem_size * _staticCount });  // throw

		AddUsage( EResourceUsage::UploadedData );  // throw


		// copy data with correct alignment
		for (const auto& columns : rows)
		{
			const Bytes	begin = Bytes{_layout._data.size()};

			for (const auto& [name, type, col_arr] : column_groups)
			{
				Bytes	align	= 4_b;
				Bytes	size	= 4_b * col_arr.size();

				if ( AllBits( type, EType::Vec ))
				{
					align *= (col_arr.size() == 3 ? 4 : col_arr.size());
					size  = Max( align, size );		// TODO: incorrect for Vec|Array
				}

				Bytes	offset	= AlignUp( Bytes{_layout._data.size()}, align );
				_layout._data.resize( usize{ offset + size });

				switch ( type & EType::ValueMask )
				{
					case EType::Float :
					{
						for (uint col : col_arr)
						{
							float	f = float(StringToDouble( columns[col] ));
							memcpy( OUT _layout._data.data() + offset, &f, sizeof(f) );
							offset += 4_b;
						}
						break;
					}

					case EType::Int :
					{
						for (uint col : col_arr)
						{
							int		i = CheckCast{StringToSLong( columns[col] )};
							memcpy( OUT _layout._data.data() + offset, &i, sizeof(i) );
							offset += 4_b;
						}
						break;
					}
				}
			}

			Bytes	row_size = Bytes{_layout._data.size()} - begin;
			ASSERT_Eq( row_size, elem_size );
		}

		ASSERT_Eq( _layout._data.size(), usize{elem_size * _staticCount} );
	}

/*
=================================================
	CreateView*
=================================================
*/
	ScriptBufferView*  ScriptBuffer::CreateView1 (EPixelFormat format) __Th___
	{
		CHECK_THROW_MSG( not _resource,
			"Resource is already created, can not create view" );

		CHECK_THROW_MSG( not HasLayout() );

		CHECK_THROW_MSG( format != Default );

		auto&	fmt_info = EPixelFormat_GetInfo( format );
		CHECK_THROW_MSG( fmt_info.IsColor() );
		CHECK_THROW_MSG( not fmt_info.IsCompressed() );
		CHECK_THROW_MSG( not fmt_info.IsMultiPlanar() );
		CHECK_THROW_MSG( fmt_info.channels != 3 );

		ScriptBufferViewPtr	result {new ScriptBufferView{ ScriptBufferPtr{this}, format, _dbgName }};  // throw

		_views.push_back( result );

		return result.Detach();
	}

} // AE::ResEditor
