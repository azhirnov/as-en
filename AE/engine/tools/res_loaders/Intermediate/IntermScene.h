// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_loaders/Intermediate/IntermMesh.h"
#include "res_loaders/Intermediate/IntermMaterial.h"
#include "res_loaders/Intermediate/IntermLight.h"
#include "res_loaders/Intermediate/IntermVertexAttribs.h"

namespace AE::ResLoader
{

    enum class EDetailLevel : ubyte
    {
        High        = 0,
        Low         = 7,
        _Count,
        Unknown     = 0xFF,
    };


    //
    // Intermediate Scene
    //

    class IntermScene final : public EnableRC<IntermScene>
    {
    // types
    public:
        using Transform_t   = TTransformation<float>;

        struct Model
        {
            RC<IntermMesh>      mesh;
            RC<IntermMaterial>  mtr;
        };
        using DetailLevels_t    = StaticArray< Model, uint(EDetailLevel::_Count) >;

        struct ModelData
        {
            DetailLevels_t      levels;
        };

        using NodeData_t        = Union< NullUnion, ModelData >;

        struct SceneNode
        {
            Transform_t         localTransform;
            String              name;
            Array<NodeData_t>   data;
            Array<SceneNode>    nodes;
        };

        using MaterialMap_t     = HashMap< RC<IntermMaterial>,  uint >;
        using MeshMap_t         = HashMap< RC<IntermMesh>,      uint >;
        using LightMap_t        = HashMap< RC<IntermLight>,     uint >;


    // variables
    private:
        SceneNode           _root;
        MaterialMap_t       _materials;
        MeshMap_t           _meshes;
        LightMap_t          _lights;


    // methods
    public:
        IntermScene ()                                                              __NE___ {}

        void  Set  (MaterialMap_t   materials,  MeshMap_t   meshes,
                    LightMap_t      lights,     SceneNode   root)                   __NE___;

        void  Set  (ArrayView<RC<IntermMaterial>>   materials,
                    ArrayView<RC<IntermMesh>>       meshes,
                    ArrayView<RC<IntermLight>>      lights,
                    SceneNode                       root)                           __NE___;

        ND_ MaterialMap_t &         EditMaterials ()                                __NE___ { return _materials; }
        ND_ MeshMap_t &             EditMeshes ()                                   __NE___ { return _meshes; }
        ND_ LightMap_t &            EditLights ()                                   __NE___ { return _lights; }

        ND_ MaterialMap_t const&    Materials ()                                    C_NE___ { return _materials; }
        ND_ MeshMap_t const&        Meshes ()                                       C_NE___ { return _meshes; }
        ND_ LightMap_t const&       Lights ()                                       C_NE___ { return _lights; }
        ND_ SceneNode const&        Root ()                                         C_NE___ { return _root; }

    //  ND_ Ptr<SceneNode const>    FindNode (StringView name)                      C_NE___;

        ND_ uint  IndexOfMesh (const RC<IntermMesh> &)                              C_NE___;
        ND_ uint  IndexOfMaterial (const RC<IntermMaterial> &)                      C_NE___;

            bool  Append (const IntermScene &scene,
                          const Transform_t &transform = Default)                   __NE___;

        template <typename FN>
        void  ForEachNode (FN &&fn, const Transform_t &initialTransform = Default)  C_Th___;

        template <typename FN>
        void  ForEachModel (FN &&fn, const Transform_t &initialTransform = Default) C_Th___;

        ND_ usize  ModelNodeCount ()                                                C_NE___;

        ND_ bool  IsValid ()                                                        C_NE___;
    };



/*
=================================================
    ForEachNode
=================================================
*/
    template <typename FN>
    void  IntermScene::ForEachNode (FN &&fn, const Transform_t &initialTransform) C_Th___
    {
        Array<Pair< SceneNode const*, Transform_t >>    stack;
        stack.emplace_back( &Root(), initialTransform );

        for (bool continue_ = true;  continue_ and not stack.empty();)
        {
            const auto&         node        = stack.back();
            const Transform_t   transform   = node.second + node.first->localTransform;

            for (auto& data : node.first->data)
            {
                continue_ = fn( node.first->name, data, transform );
                if ( not continue_ )
                    break;
            }

            stack.pop_back();
            for (auto& n : node.first->nodes) {
                stack.emplace_back( &n, transform );
            }
        }
    }

/*
=================================================
    ForEachModel
=================================================
*/
    template <typename FN>
    void  IntermScene::ForEachModel (FN &&fn, const Transform_t &initialTransform) C_Th___
    {
        ForEachNode(
            [&fn] (StringView, const NodeData_t &data, const Transform_t &) __Th___ -> bool
            {
                Visit( data,
                    [&fn] (const ModelData &mdl) __Th___ {
                        fn( mdl );
                    },
                    [] (const NullUnion &) {}
                );
                return true;  // continue iterating
            },
            initialTransform );
    }


} // AE::ResLoader
