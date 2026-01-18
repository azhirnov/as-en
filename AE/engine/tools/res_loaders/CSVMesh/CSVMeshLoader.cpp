// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/CSVMesh/CSVMeshLoader.h"
#include "res_loaders/Intermediate/IntermMesh.h"
#include "res_loaders/Intermediate/IntermVertexAttribs_Setter.h"

namespace AE::ResLoader
{
namespace
{
	enum class CSVMeshFormat : uint
	{
		Unknown = 0,
		UserDefined,
		RenderDoc,
		NSight,
	};

	enum class EAttrib : uint
	{
		Unknown		= 0,
		Position,
		Normal,
		BiTangent,
		Tangent,
		TextureUV,
		TextureUV_1,
		TextureUV_2,
		TextureUV_3,
		Color,
		_Count
	};

	struct VertexMapping2 : CSVMeshLoader::VertexMapping
	{
		Bytes16u	offset;
	};

	static constexpr uint	c_TempVBufSize = 256;

/*
=================================================
	DetectFormat
=================================================
*/
	ND_ static EVertexType  DetectFormat (StringView value)
	{
		if ( HasChar( value, '.' ))
			return EVertexType::Float;

		//if ( HasChar( value, '-' ))
		//	return EVertexType::Short_Norm;
		//else
		//	return EVertexType::UShort_Norm;

		return EVertexType::Short_Norm;
	}

/*
=================================================
	ParseFormat
=================================================
*/
	ND_ static EVertexType  ParseFormat (StringView value)
	{
		value = value.substr( 1, value.size() - 2 );

		#define DX_FORMAT_LIST( _visit_ )\
			_visit_( R32G32B32A32_FLOAT,	Float )\
			_visit_( R32G32B32_FLOAT,		Float )\
			_visit_( R32G32_FLOAT,			Float )\
			_visit_( R32_FLOAT,				Float )\
			_visit_( R16G16B16A16_FLOAT,	Half )\
			_visit_( R16G16B16_FLOAT,		Half )\
			_visit_( R16G16_FLOAT,			Half )\
			_visit_( R16_FLOAT,				Half )\
			_visit_( R16G16B16A16_UNORM,	UShort_Norm )\
			_visit_( R16G16B16_UNORM,		UShort_Norm )\
			_visit_( R16G16_UNORM,			UShort_Norm )\
			_visit_( R16_UNORM,				UShort_Norm )\
			_visit_( R8G8B8A8_UNORM,		UByte_Norm )\
			_visit_( R8G8B8_UNORM,			UByte_Norm )\
			_visit_( R8G8_UNORM,			UByte_Norm )\
			_visit_( R8_UNORM,				UByte_Norm )\
			_visit_( R16G16B16A16_SNORM,	Short_Norm )\
			_visit_( R16G16B16_SNORM,		Short_Norm )\
			_visit_( R16G16_SNORM,			Short_Norm )\
			_visit_( R16_SNORM,				Short_Norm )\
			_visit_( R8G8B8A8_SNORM,		Byte_Norm )\
			_visit_( R8G8B8_SNORM,			Byte_Norm )\
			_visit_( R8G8_SNORM,			Byte_Norm )\
			_visit_( R8_SNORM,				Byte_Norm )\
			_visit_( R32G32B32A32_SINT,		Int )\
			_visit_( R32G32B32_SINT,		Int )\
			_visit_( R32G32_SINT,			Int )\
			_visit_( R32_SINT,				Int )\
			_visit_( R16G16B16A16_SINT,		Short )\
			_visit_( R16G16B16_SINT,		Short )\
			_visit_( R16G16_SINT,			Short )\
			_visit_( R16_SINT,				Short )\
			_visit_( R8G8B8A8_SINT,			Byte )\
			_visit_( R8G8B8_SINT,			Byte )\
			_visit_( R8G8_SINT,				Byte )\
			_visit_( R8_SINT,				Byte )\
			_visit_( R32G32B32A32_UINT,		UInt )\
			_visit_( R32G32B32_UINT,		UInt )\
			_visit_( R32G32_UINT,			UInt )\
			_visit_( R32_UINT,				UInt )\
			_visit_( R16G16B16A16_UINT,		UShort )\
			_visit_( R16G16B16_UINT,		UShort )\
			_visit_( R16G16_UINT,			UShort )\
			_visit_( R16_UINT,				UShort )\
			_visit_( R8G8B8A8_UINT,			UByte )\
			_visit_( R8G8B8_UINT,			UByte )\
			_visit_( R8G8_UINT,				UByte )\
			_visit_( R8_UINT,				UByte )\

		#define VISIT( _dxFormat_, _aeVert_ )\
			if ( value == AE_TOSTRING( _dxFormat_ ))	return EVertexType::_aeVert_;

		DX_FORMAT_LIST( VISIT )

		#undef DX_FORMAT_LIST
		#undef VISIT

		return Default;
	}

/*
=================================================
	ConvertUserDefinedFormat
=================================================
*/
	static bool  ConvertUserDefinedFormat (Array<StringView>										tokens,
										   Array<StringView>										values,
										   const HashMap< String, CSVMeshLoader::VertexMapping >	&columnToVertex,
										   OUT Array<VertexMapping2>								&vertMapping,
										   OUT Bytes												&vertStride)
	{
		// TODO
		return false;
	}

/*
=================================================
	ParseRenderDocFormat
=================================================
*/
	static bool  ParseRenderDocFormat (Array<StringView> tokens, Array<StringView> values, OUT Array<VertexMapping2> &vertMapping, OUT Bytes &vertStride)
	{
		CHECK( vertMapping.empty() );
		vertMapping.resize( tokens.size() );

		StaticArray< ushort, uint(EAttrib::_Count) >	comp_count	= {};

		vertStride = 0_b;

		for (usize i = 0; i < tokens.size(); ++i)
		{
			StringView	name	= tokens[i];
			auto&		mapping = vertMapping[i];

			if ( name == "VTX" )
				continue;

			if ( name == " IDX" )
			{
				mapping.type	= EVertexType::UInt;
				mapping.comp	= 0;
				ASSERT( mapping.IsIndex() );
				continue;
			}

			EAttrib		attr = Default;
			if ( HasSubStringIC( name, "position" ))	attr = EAttrib::Position;		else
			if ( HasSubStringIC( name, "pos" ))			attr = EAttrib::Position;		else
			if ( HasSubStringIC( name, "normal" ))		attr = EAttrib::Normal;			else
			if ( HasSubStringIC( name, "bitangent" ))	attr = EAttrib::BiTangent;		else
			if ( HasSubStringIC( name, "tangent" ))		attr = EAttrib::Tangent;		else
			if ( HasSubStringIC( name, "color" ))		attr = EAttrib::Color;			else
			if ( HasSubStringIC( name, "Texcoord3" ))	attr = EAttrib::TextureUV_3;	else
			if ( HasSubStringIC( name, "Texcoord2" ))	attr = EAttrib::TextureUV_2;	else
			if ( HasSubStringIC( name, "Texcoord1" ))	attr = EAttrib::TextureUV_1;	else
			if ( HasSubStringIC( name, "Texcoord0" ))	attr = EAttrib::TextureUV;		else
			if ( HasSubStringIC( name, "Texcoord" ))	attr = EAttrib::TextureUV;		else
			if ( HasSubString( name, "UV3" ))			attr = EAttrib::TextureUV_3;	else
			if ( HasSubString( name, "UV2" ))			attr = EAttrib::TextureUV_2;	else
			if ( HasSubString( name, "UV1" ))			attr = EAttrib::TextureUV_1;	else
			if ( HasSubString( name, "UV0" ))			attr = EAttrib::TextureUV;		else
			if ( HasSubString( name, "UV" ))			attr = EAttrib::TextureUV;

			if ( attr != Default )
			{
				VertexMapping2	tmp;
				tmp.type	= DetectFormat( values[i] );
				tmp.index	= ushort(attr);
				tmp.comp	= comp_count[ uint(attr) ] ++;
				tmp.offset	= vertStride;

				if ( tmp.comp >= 4 )
					continue;

				if ( tmp.comp > 0 and i > 0 )
				{
					if ( vertMapping[i-1].index != tmp.index )
						continue;
				}

				mapping = tmp;

				vertStride += EVertexType_SizeOf( tmp.type );
			}
		}
		return true;
	}

/*
=================================================
	ParseNSightFormat
=================================================
*/
	static bool  ParseNSightFormat (Array<StringView> tokens, Array<StringView> values, OUT Array<VertexMapping2> &vertMapping, OUT Bytes &vertStride)
	{
		CHECK( vertMapping.empty() );
		vertMapping.resize( tokens.size() );

		StaticArray< ushort, uint(EAttrib::_Count) >		comp_count	= {};
		StaticArray< EVertexType, uint(EAttrib::_Count) >	attr_format	= {};
		Array<StringView>									parts;

		vertStride = 0_b;

		for (usize i = 0; i < tokens.size(); ++i)
		{
			StringView	name	= tokens[i];
			auto&		mapping = vertMapping[i];

			if ( name == "IB Offset" or name == "Index + Base" )
				continue;

			if ( name == "Index" )
			{
				mapping.type	= EVertexType::UInt;
				mapping.comp	= 0;
				ASSERT( mapping.IsIndex() );
				continue;
			}

			// dx11 format: <idx> - <name><nameIdx> (<dxFormat>) : Component <comp>

			Parser::Tokenize( name, ' ', OUT parts );
			CHECK_ERR(	parts.size() == 7		and
						parts[1] == "-"			and
						parts[3].front() == '('	and
						parts[3].back() == ')'	and
						parts[4] == ":"			and
						parts[5] == "Component"	);

			name = parts[2];

			EAttrib		attr = Default;
			if ( HasSubStringIC( name, "position" ))	attr = EAttrib::Position;		else
			if ( HasSubStringIC( name, "pos" ))			attr = EAttrib::Position;		else
			if ( HasSubStringIC( name, "normal" ))		attr = EAttrib::Normal;			else
			if ( HasSubStringIC( name, "bitangent" ))	attr = EAttrib::BiTangent;		else
			if ( HasSubStringIC( name, "tangent" ))		attr = EAttrib::Tangent;		else
			if ( HasSubStringIC( name, "Texcoord3" ))	attr = EAttrib::TextureUV_3;	else
			if ( HasSubStringIC( name, "Texcoord2" ))	attr = EAttrib::TextureUV_2;	else
			if ( HasSubStringIC( name, "Texcoord1" ))	attr = EAttrib::TextureUV_1;	else
			if ( HasSubStringIC( name, "Texcoord0" ))	attr = EAttrib::TextureUV;		else
			if ( HasSubStringIC( name, "Texcoord" ))	attr = EAttrib::TextureUV;		else
			if ( HasSubStringIC( name, "Color" ))		attr = EAttrib::Color;			else
			if ( HasSubString( name, "UV3" ))			attr = EAttrib::TextureUV_3;	else
			if ( HasSubString( name, "UV2" ))			attr = EAttrib::TextureUV_2;	else
			if ( HasSubString( name, "UV1" ))			attr = EAttrib::TextureUV_1;	else
			if ( HasSubString( name, "UV0" ))			attr = EAttrib::TextureUV;		else
			if ( HasSubString( name, "UV" ))			attr = EAttrib::TextureUV;

			if ( attr != Default )
			{
				auto	fmt0	= DetectFormat( values[i] );
				auto	fmt		= ParseFormat( parts[3] );

				if ( fmt == Default )
					fmt = fmt0;

				if ( EVertexType_IsFloat( fmt0 ) != EVertexType_IsFloat( fmt ))
					fmt = fmt0;

				if ( EVertexType_IsInteger( fmt ) and not EVertexType_IsScaledOrNormalized( fmt ))
					fmt |= EVertexType::NormalizedFlag;

				if ( attr_format[ uint(attr) ] == Default )
					attr_format[ uint(attr) ] = fmt;
				else
					CHECK( attr_format[ uint(attr) ] == fmt );

				mapping.type	= fmt;
				mapping.index	= ushort(attr);
				mapping.comp	= comp_count[ uint(attr) ] ++;
				mapping.offset	= vertStride;

				vertStride += EVertexType_SizeOf( mapping.type );

				CHECK( mapping.comp < 4 );
				CHECK_Eq( mapping.comp, StringToInt( parts[6] ));

				if ( mapping.comp > 0 and i > 0 )
					CHECK( vertMapping[i-1].index == mapping.index );
			}
		}
		return true;
	}

/*
=================================================
	CreateVertexAttribs
=================================================
*/
	ND_ static RC<IntermVertexAttribs>  CreateVertexAttribs (ArrayView<VertexMapping2> vertMapping, const Bytes vertStride)
	{
		auto	attribs = MakeRC<IntermVertexAttribs>();

		IntermVertexAttribs::Setter											setter {*attribs};
		StaticArray< VertexAttributeName::Name_t, uint(EAttrib::_Count) >	attr_names;

		attr_names[uint(EAttrib::Position)]		= VertexAttributeName::Position;
		attr_names[uint(EAttrib::Normal)]		= VertexAttributeName::Normal;
		attr_names[uint(EAttrib::BiTangent)]	= VertexAttributeName::BiTangent;
		attr_names[uint(EAttrib::Tangent)]		= VertexAttributeName::Tangent;
		attr_names[uint(EAttrib::TextureUV)]	= VertexAttributeName::TextureUVs[0];
		attr_names[uint(EAttrib::TextureUV_1)]	= VertexAttributeName::TextureUVs[1];
		attr_names[uint(EAttrib::TextureUV_2)]	= VertexAttributeName::TextureUVs[2];
		attr_names[uint(EAttrib::TextureUV_3)]	= VertexAttributeName::TextureUVs[3];
		attr_names[uint(EAttrib::Color)]		= VertexAttributeName::Color;
		StaticAssert( uint(EAttrib::_Count) == 10 );

		Bytes	offset = 0_b;

		for (auto it = vertMapping.begin(); it != vertMapping.end(); ++it)
		{
			if ( it->IsNotDefined() or it->IsIndex() )
				continue;

			if ( it->comp > 0 )
				continue;

			CHECK( offset == it->offset );

			// find last
			for (auto it2 = it; it2 != vertMapping.end(); ++it2)
			{
				if ( it2->index != it->index )
					break;

				it = it2;
			}

			auto&		vert	= *it;
			auto&		name	= attr_names[ vert.index ];
			EVertexType	type	= EVertexType_SetVec( vert.type, vert.comp+1 );

			setter.AddVertex( name, type, offset );
			offset += EVertexType_SizeOf( type );
		}
		CHECK_Eq( offset, vertStride );

		setter.AddBuffer( vertStride, EVertexInputRate::Vertex );
		return attribs;
	}

/*
=================================================
	AppendData
=================================================
*/
	ND_ static bool  AppendData (Array<StringView> tokens, ArrayView<VertexMapping2> vertMapping,
								 const Bytes vertStride, const Bytes indexSize,
								 INOUT uint &maxVertexIndex,
								 INOUT Array<ubyte> &vertexData, INOUT Array<ubyte> &indexData)
	{
		CHECK_ERR( tokens.size() == vertMapping.size() );
		CHECK_ERR( indexSize == 4_b );		// TODO

		ubyte	buf [c_TempVBufSize];
		uint	idx = UMax;

		for (usize i = 0; i < tokens.size(); ++i)
		{
			StringView	value	= tokens[i];
			auto&		mapping	= vertMapping[i];

			if ( mapping.IsNotDefined() )
				continue;

			usize j = 0;
			for (; j < value.size() and value[j] == ' '; ++j);
			value = value.substr( j );

			if_unlikely( mapping.IsIndex() )
			{
				idx = StringToUInt( value );
				maxVertexIndex = Max( maxVertexIndex, idx+1 );

				usize	pos = indexData.size();
				indexData.resize( pos + usize{indexSize} );
				memcpy( OUT indexData.data() + pos, &idx, usize{indexSize} );
			}
			else
			{
				usize	dst_size	= usize{EVertexType_SizeOf( mapping.type )};
				usize	offset		= usize{mapping.offset};

				if ( EVertexType_IsFloat( mapping.type ))
				{
					double	dp = StringToDouble( value );
					switch ( dst_size )
					{
						case 2 :
						{
							half	hp{ float(dp) };
							memcpy( OUT buf + offset, &hp, dst_size );
							break;
						}
						case 4 :
						{
							float	fp = float(dp);
							memcpy( OUT buf + offset, &fp, dst_size );
							break;
						}
						case 8 :
						{
							memcpy( OUT buf + offset, &dp, dst_size );
							break;
						}
					}
				}
				else
				if ( EVertexType_IsUnsignedInteger( mapping.type ))
				{
					ulong	ul = StringToULong( value );
					memcpy( OUT buf + offset, &ul, dst_size );
				}
				else
				{
					slong	sl = StringToSLong( value );
					memcpy( OUT buf + offset, &sl, dst_size );
				}
			}
		}

		vertexData.resize( Max( maxVertexIndex * usize{vertStride}, vertexData.size() ));

		memcpy( OUT vertexData.data() + idx * vertStride, buf, usize{vertStride} );
		return true;
	}

} // namespace


/*
=================================================
	LoadMesh
=================================================
*/
	bool  CSVMeshLoader::LoadMesh (OUT IntermMesh &mesh, RStream &stream, const Config &cfg, EModelFormat fileFormat) __NE___
	{
		if ( not (fileFormat == Default or fileFormat == EModelFormat::CSV) )
			return false;

		Array<VertexMapping2>	vert_mapping;	// per column
		CSVConfig const*		csv_cfg		= null;

		if ( cfg.spec != null and Cast<CSVConfig>(cfg.spec)->magic == CSVConfig::MAGIC )
		{
			csv_cfg = Cast<CSVConfig>( cfg.spec );
		}

		String	buf;
		CHECK_ERR( stream.Read( stream.RemainingSize(), OUT buf ));

		StringView	str {buf};
		SkipUtf8BOM( INOUT str );

		Array<StringView>	tokens;
		CSVMeshFormat		format			= Default;
		EPrimitive			topology		= csv_cfg != null ? csv_cfg->topology : EPrimitive::TriangleList;
		EIndex				index_type		= Default;
		Bytes				vert_stride		= 0_b;
		usize				pos				= 0;
		StringView			line;

		// column names
		{
			Parser::ReadLineToEnd( str, INOUT pos, OUT line );

			Parser::Tokenize( line, ',', OUT tokens );

			Array<StringView>	values;
			usize				pos2 = pos;
			Parser::ReadLineToEnd( str, INOUT pos2, OUT line );
			Parser::Tokenize( line, ',', OUT values );

			CHECK_ERR( tokens.size() == values.size() );


			// user-defined mapping
			if ( csv_cfg != null					and
				 not csv_cfg->columnToVertex.empty() )
			{
				format = CSVMeshFormat::UserDefined;
				ConvertUserDefinedFormat( tokens, values, csv_cfg->columnToVertex, OUT vert_mapping, OUT vert_stride );
			}
			else
			// RenderDoc format
			if ( tokens.size() >= 2	 and
				 tokens[0] == "VTX"	 and
				 tokens[1] == " IDX" )
			{
				format = CSVMeshFormat::RenderDoc;
				ParseRenderDocFormat( tokens, values, OUT vert_mapping, OUT vert_stride );
			}
			else
			// NSight format
			if ( tokens.size() >= 3			and
				 tokens[0] == "IB Offset"	and
				 tokens[1] == "Index"		and
				 tokens[2] == "Index + Base" )
			{
				format = CSVMeshFormat::NSight;
				ParseNSightFormat( tokens, values, OUT vert_mapping, OUT vert_stride );
			}

			for (auto& vert : vert_mapping)
			{
				if ( vert.IsIndex() )
				{
					switch ( vert.type )
					{
					//	case EVertexType::UByte :	index_type = EIndex::UByte;   break;	// TODO
						case EVertexType::UShort :	index_type = EIndex::UShort;  break;
						case EVertexType::UInt :	index_type = EIndex::UInt;    break;
					}
					break;
				}
			}

			CHECK_ERR( index_type != Default );
			CHECK_ERR( tokens.size() == vert_mapping.size() );
			CHECK_ERR( format != Default );
			CHECK_ERR( vert_stride > 0 );
			CHECK_ERR( vert_stride <= c_TempVBufSize );
		}

		// data
		const Bytes		index_size	= EIndex_SizeOf( index_type );
		Array<ubyte>	vertex_data;
		Array<ubyte>	index_data;
		usize			idx_count	= 0;
		uint			max_vertex	= 0;

		vertex_data.reserve( usize{vert_stride} * 1000 );
		index_data.reserve( usize{index_size} * 1000 );

		for (; pos < str.size(); ++idx_count)
		{
			Parser::ReadLineToEnd( str, INOUT pos, OUT line );

			Parser::Tokenize( line, ',', OUT tokens );

			if ( tokens.size() <= 1 )
				break;

			CHECK_ERR( AppendData( tokens, vert_mapping, vert_stride, index_size, INOUT max_vertex, INOUT vertex_data, INOUT index_data ));

			ASSERT( (idx_count+1) * index_size == ArraySizeOf(index_data) );
		}
        Unused( idx_count );

		ASSERT( max_vertex *vert_stride == ArraySizeOf(vertex_data) );

		auto	attribs = CreateVertexAttribs( vert_mapping, vert_stride );
		CHECK_ERR( attribs );

		mesh.Set( RVRef(vertex_data), attribs, vert_stride, topology, RVRef(index_data), index_type );
		return true;
	}

/*
=================================================
	LoadMesh
=================================================
*/
	bool  CSVMeshLoader::LoadMesh (OUT IntermMesh &mesh, const Path &meshPath, const Config &cfg) __NE___
	{
		FileRStream		file {meshPath};
		CHECK_ERR( file.IsOpen() );

		return LoadMesh( OUT mesh, file, cfg );
	}


} // AE::ResLoader
