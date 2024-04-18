// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_loaders/Intermediate/IntermScene.h"

namespace AE::ResLoader
{

/*
=================================================
	Set
=================================================
*/
	void  IntermScene::Set (MaterialMap_t	materials,	MeshMap_t	meshes,
							LightMap_t		lights,		SceneNode	root) __NE___
	{
		_root		= RVRef(root);
		_materials	= RVRef(materials);
		_meshes		= RVRef(meshes);
		_lights		= RVRef(lights);
	}

/*
=================================================
	Set
=================================================
*/
	void  IntermScene::Set (ArrayView<RC<IntermMaterial>>	materials,
							ArrayView<RC<IntermMesh>>		meshes,
							ArrayView<RC<IntermLight>>		lights,
							SceneNode						root) __NE___
	{
		_root = RVRef(root);

		for (auto& mtr : materials) {
			_materials.emplace( mtr, uint(_materials.size()) );
		}
		for (auto& mesh : meshes) {
			_meshes.emplace( mesh, uint(_meshes.size()) );
		}
		for (auto& light : lights) {
			_lights.emplace( light, uint(_lights.size()) );
		}
	}

/*
=================================================
	IndexOfMesh
=================================================
*/
	uint  IntermScene::IndexOfMesh (const RC<IntermMesh> &ptr) C_NE___
	{
		auto	iter = _meshes.find( ptr );
		return iter != _meshes.end() ? iter->second : UMax;
	}

/*
=================================================
	IndexOfMaterial
=================================================
*/
	uint  IntermScene::IndexOfMaterial (const RC<IntermMaterial> &ptr) C_NE___
	{
		auto	iter = _materials.find( ptr );
		return iter != _materials.end() ? iter->second : UMax;
	}

/*
=================================================
	Append
=================================================
*/
	bool  IntermScene::Append (const IntermScene &other, const Transform_t &transform) __NE___
	{
		for (auto& mtr : other._materials) {
			_materials.insert({ mtr.first, uint(_materials.size()) });
		}
		for (auto& mesh : other._meshes) {
			_meshes.insert({ mesh.first, uint(_meshes.size()) });
		}
		for (auto& light : other._lights) {
			_lights.insert({ light.first, uint(_lights.size()) });
		}

		_root.nodes.push_back( other.Root() );
		_root.nodes.back().localTransform += transform;

		return true;
	}

/*
=================================================
	ModelNodeCount
=================================================
*/
	usize  IntermScene::ModelNodeCount () C_NE___
	{
		usize	count = 0;
		ForEachModel( [&count] (const ModelData &) { ++count; });
		return count;
	}

/*
=================================================
	IsValid
=================================================
*/
	bool  IntermScene::IsValid () C_NE___
	{
		for (auto& [mesh, idx] : _meshes)
		{
			CHECK_ERR( mesh );
			CHECK_ERR( mesh->IsValid() );
		}
		return true;
	}


} // AE::ResLoader
