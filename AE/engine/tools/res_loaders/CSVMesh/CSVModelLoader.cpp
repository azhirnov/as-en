// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	format:
		version: 1

		// model-1
		node:			<name>							- required
		mesh:			<csv file name>					- required
		invertTriOrder: true
		cullBack:		true
		recenter:		true
		alphaCutoff:	0.5								- enable alpha test
		texture:		PBR_BaseColor, <texture file>
*/

#include "res_loaders/CSVMesh/CSVModelLoader.h"
#include "res_loaders/CSVMesh/CSVMeshLoader.h"
#include "res_loaders/Intermediate/IntermScene.h"
#include "res_loaders/AllImages/AllImageLoaders.h"

namespace AE::ResLoader
{
namespace
{
/*
=================================================
	LoadScene
=================================================
*/
	static IntermMaterial::EKey  GetMaterialKey (StringView keyStr)
	{
		#define MTR_KEY( _visit_ )\
			_visit_( Diffuse )\
			_visit_( Specular )\
			_visit_( Ambient )\
			_visit_( Emissive )\
			_visit_( HeightMap )\
			_visit_( NormalMap )\
			_visit_( Shininess )\
			_visit_( Opacity )\
			_visit_( DisplacementMap )\
			_visit_( LightMap )\
			_visit_( ReflectionMap )\
			_visit_( AlphaTestReference )\
			_visit_( Transparency )\
			_visit_( BumpScale )\
			_visit_( Reflectivity )\
			_visit_( ShininessStrength )\
			_visit_( Transparent )\
			_visit_( Reflective )\
			\
			_visit_( PBR_BaseColor )\
			_visit_( PBR_NormalMap )\
			_visit_( PBR_Emission )\
			_visit_( PBR_Roughness )\
			_visit_( PBR_AmbientOcclusionMap )\
			_visit_( PBR_Sheen )\
			_visit_( PBR_Clearcoat )\
			_visit_( PBR_Transmission )\
			_visit_( PBR_Metalness )\
			_visit_( PBR_Subsurface )\
			_visit_( PBR_IndexOfRefraction )\
			_visit_( PBR_OpticalDepth )

		#define VISIT( _name_ )		if ( keyStr == AE_TOSTRING(_name_) )	return IntermMaterial::EKey::_name_;

		MTR_KEY( VISIT );
		return Default;

		#undef MTR_KEY
		#undef VISIT
	}

/*
=================================================
	ParseBool
=================================================
*/
	ND_ static bool  ParseBool (StringView value)
	{
		if ( value == "1" or value == "true" )
			return true;

		if ( value == "0" or value == "false" )
			return false;

		RETURN_ERR( "can't parse bool argument: '"s << value << "'" );
	}

/*
=================================================
	LoadScene
=================================================
*/
	static bool  LoadScene (OUT IntermScene	&scene, StringView sceneDesc, const bool convertToFP,
							ArrayView<Path> meshFolders, ArrayView<Path> textureFolders)
	{
		const auto	FindPath = [] (StringView fname, ArrayView<Path> folders) -> Path
		{{
			for (auto& folder : folders)
			{
				Path	path = folder / fname;

				if ( FileSystem::IsFile( path ))
					return path;
			}
			return {};
		}};


		uint						file_version = UMax;
		Array<RC<IntermMaterial>>	materials;
		Array<RC<IntermMesh>>		meshes;
		IntermScene::SceneNode		root;
		IntermScene::SceneNode		new_node;
		RC<IntermMesh>				new_mesh;
		RC<IntermMaterial>			new_material;

		const auto	AddMeshMtrToNode = [&] () -> bool
		{{
			CHECK_ERR( new_mesh and new_material );

			auto&	mdl = new_node.data.emplace_back().emplace< IntermScene::ModelData >();
			mdl.levels[0].mesh	= new_mesh;
			mdl.levels[0].mtr	= new_material;

			meshes.push_back( RVRef(new_mesh) );
			materials.push_back( RVRef(new_material) );
			return true;
		}};

		for (usize pos = 0; pos < sceneDesc.size();)
		{
			StringView	line;
			Parser::ReadLineToEnd( sceneDesc, INOUT pos, OUT line );

			if ( line.empty() )
				continue;

			if ( StartsWith( line, "//" ))
				continue;

			usize	p = line.find( ':' );
			CHECK_ERR( p < line.size() );

			StringView	key = line.substr( 0, p );

			++p;
			for (; p < line.size() and (line[p] == ' ' or line[p] == '\t'); ++p);

			StringView	value = line.substr( p );
			CHECK_ERR( not value.empty() );

			if ( key == "version" )
			{
				CHECK_ERR( file_version == UMax );

				file_version = StringToUInt( value );
				CHECK_ERR( file_version == 1 );
				continue;
			}

			if ( key == "mesh" )
			{
				usize		p2 = 0;
				StringView	str;
				CHECK_ERR( Parser::ReadString( value, p2, OUT str ));

				Path	path = FindPath( str, meshFolders );
				CHECK_ERR_MSG( not path.empty(), "can't find '"s << str << "'" );

				CHECK_ERR( not new_mesh );
				new_mesh		= MakeRC<IntermMesh>();
				new_material	= MakeRC<IntermMaterial>();

				CSVMeshLoader	loader;
				CHECK_ERR( loader.LoadMesh( OUT *new_mesh, path, Default ));

				if ( convertToFP )
				{
					auto	tmp = RVRef(new_mesh);
					new_mesh = MakeRC<IntermMesh>();

					CHECK_ERR( tmp->ConvertToFloatPointFormat( *new_mesh ));
				}
				continue;
			}

			if ( key == "invertTriOrder" )
			{
				CHECK_ERR( new_mesh );

				if ( ParseBool( value ))
				{
					CHECK( new_mesh->InvertFrontFace() );
					CHECK( new_mesh->InvertNormals() );
				}
				continue;
			}

			if ( key == "alphaCutoff" )
			{
				CHECK_ERR( new_material );

				float	f = Saturate( StringToFloat( value ));

				new_material->EditSettings().alphaCutoff = f;
				continue;
			}

			if ( key == "cullBack" )
			{
				CHECK_ERR( new_material );

				new_material->EditSettings().cullMode = ParseBool( value ) ? ECullMode::Back : ECullMode::None;
				continue;
			}

			if ( key == "recenter" )
			{
				CHECK_ERR( new_mesh );

				CHECK( new_mesh->Recenter() );
				continue;
			}

			if ( key == "texture" )
			{
				CHECK_ERR( new_material );

				Array<StringView>	parts;
				Parser::Tokenize( value, ',', OUT parts );
				CHECK_ERR( parts.size() == 2 );

				auto		mtr_key = GetMaterialKey( parts[0] );
				usize		p2 = 0;
				StringView	str;
				CHECK_ERR( Parser::ReadString( parts[1], p2, OUT str ));

				IntermMaterial::MtrTexture	tex;
				tex.image	= MakeRC<IntermImage>( Path{str} );

				AllImageLoaders	loader;
				CHECK_ERR( loader.LoadImage( OUT *tex.image, textureFolders ));

				new_material->Set( mtr_key, RVRef(tex) );
				continue;
			}

			if ( key == "node" )
			{
				if ( not new_node.name.empty() )
				{
					AddMeshMtrToNode();
					root.nodes.push_back( RVRef(new_node) );
				}

				usize		p2 = 0;
				StringView	str;
				CHECK_ERR( Parser::ReadString( value, p2, OUT str ));

				new_node.name = String{str};
				continue;
			}

			RETURN_ERR( "unknown key: '"s << key << "'" );
		}

		AddMeshMtrToNode();
		root.nodes.push_back( RVRef(new_node) );

		scene.Set( RVRef(materials), RVRef(meshes), Default, RVRef(root) );
		return true;
	}

} // namespace


/*
=================================================
	LoadModel
=================================================
*/
	bool  CSVModelLoader::LoadModel (OUT IntermScene	&scene,
									 RStream			&stream,
									 const Config		&cfg,
									 EModelFormat		fileFormat) __NE___
	{
		if ( not (fileFormat == Default or fileFormat == EModelFormat::CSV) )
			return false;

		String	str;
		CHECK_ERR( stream.Read( stream.RemainingSize(), OUT str ));

		return LoadScene( OUT scene, str, cfg.convertMeshesToFloatPointFormat, cfg.meshFolders, cfg.textureFolders );
	}

/*
=================================================
	LoadModel
=================================================
*/
	bool  CSVModelLoader::LoadModel (OUT IntermScene	&scene,
									 const Path			&scenePath,
									 const Config		&cfg) __NE___
	{
		FileRStream		file {scenePath};
		CHECK_ERR( file.IsOpen() );

		String	str;
		CHECK_ERR( file.Read( file.RemainingSize(), OUT str ));

		Array<Path>		mesh_folders	= cfg.meshFolders;
		Array<Path>		tex_folders		= cfg.textureFolders;

		if ( mesh_folders.empty() )
			mesh_folders.push_back( scenePath.parent_path() );

		if ( tex_folders.empty() )
			tex_folders.push_back( scenePath.parent_path() );

		return LoadScene( OUT scene, str, cfg.convertMeshesToFloatPointFormat, mesh_folders, tex_folders );
	}

} // AE::ResLoader
