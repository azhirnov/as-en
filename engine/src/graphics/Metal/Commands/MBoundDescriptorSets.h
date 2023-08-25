// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
    Helper class to emulate Vulkan Descriptor Set behaviour.
*/

#pragma once

#ifdef AE_ENABLE_METAL
# include "graphics/Metal/Commands/MArgumentSetter.h"
# include "graphics/Metal/Resources/MDescriptorSet.h"

namespace AE::Graphics::_hidden_
{
    template <typename CtxImpl>
    class _MDrawContextImpl;

    template <typename CtxImpl>
    class _MComputeContextImpl;

    template <typename CtxImpl>
    class _MRayTracingContextImpl;



    //
    // Metal Bound Descriptor Sets
    //

    class MBoundDescriptorSets final
    {
    // types
    private:
        using BoundDS_t                 = StaticArray< DescriptorSetID, GraphicsConfig::MaxDescriptorSets >;
        using Heaps_t                   = Array< MetalMemory >;     // TODO: NS::MutableArray or FixedSet ?
        using ShaderWriteResources_t    = Array< MetalResource >;
        using RenderTargetResources_t   = Array< MetalResource >;


    // variables
    private:
        bool            _changed    = false;
        BoundDS_t       _boundDS    {};

        // call 'UseHeapsAndResources()' before using argument buffers
        Heaps_t         _heapsFromDS;


    // methods
    public:
        MBoundDescriptorSets ()                                                                                 __NE___ {}

        template <typename Ctx>
        void  Bind (Ctx &ctx, const DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets)   __Th___;

        template <typename Ctx>
        void  UseHeapsAndResources (Ctx &ctx)                                                                   __Th___;

        void  Reset ()                                                                                          __NE___;

    private:
        template <typename ArgSetter>
        static void  _BindDynamicBuffers (ArgSetter && args, uint idx, bool isBound, ArrayView<uint>, const MDescriptorSet &) __Th___;
    };



/*
=================================================
    Bind
=================================================
*/
    template <typename Ctx>
    void  MBoundDescriptorSets::Bind (Ctx &ctx, const DescSetBinding index, DescriptorSetID ds, ArrayView<uint> dynamicOffsets) __Th___
    {
        const bool  is_bound = (_boundDS[ index.mtlIndex.BindingIndex() ] == ds);

        if_unlikely( is_bound and dynamicOffsets.empty() )
            return;

        _boundDS[ index.mtlIndex.BindingIndex() ]   = ds;
        _changed                                    = true;

        auto&   desc_set = ctx._GetResourcesOrThrow( ds );
        CHECK_THROW( desc_set.GetDynamicBuffers().size() == dynamicOffsets.size() );

        if ( not is_bound )
        {
            // TODO: merge heaps
        }

        if constexpr( IsSpecializationOf< Ctx, _MDrawContextImpl >)
        {
            STATIC_ASSERT( Ctx::IsMetalDrawContext );
            for (EShaderStages stages = desc_set.ShaderStages(); stages != Default;)
            {
                const EShader   type = ExtractBitLog2<EShader>( INOUT stages );
                switch ( type )
                {
                    case EShader::Vertex :      _BindDynamicBuffers( ctx.VertexArguments(),     index.mtlIndex.Vertex(),    is_bound, dynamicOffsets, desc_set );   break;
                    case EShader::Fragment :    _BindDynamicBuffers( ctx.FragmentArguments(),   index.mtlIndex.Fragment(),  is_bound, dynamicOffsets, desc_set );   break;
                    case EShader::MeshTask :    _BindDynamicBuffers( ctx.MeshTaskArguments(),   index.mtlIndex.MeshTask(),  is_bound, dynamicOffsets, desc_set );   break;
                    case EShader::Mesh :        _BindDynamicBuffers( ctx.MeshArguments(),       index.mtlIndex.Mesh(),      is_bound, dynamicOffsets, desc_set );   break;
                    case EShader::Tile :        _BindDynamicBuffers( ctx.TileArguments(),       index.mtlIndex.Tile(),      is_bound, dynamicOffsets, desc_set );   break;
                    default_unlikely :          CHECK_THROW(false);
                }
            }
            return;
        }

        if constexpr( IsSpecializationOf< Ctx, _MComputeContextImpl >)
        {
            STATIC_ASSERT( Ctx::IsMetalComputeContext );
            ASSERT( desc_set.ShaderStages() == EShaderStages::Compute );

            _BindDynamicBuffers( ctx.Arguments(), index.mtlIndex.Compute(), is_bound, dynamicOffsets, desc_set );
            return;
        }

        if constexpr( IsSpecializationOf< Ctx, _MRayTracingContextImpl >)
        {
            STATIC_ASSERT( Ctx::IsMetalRayTracingContext );
            ASSERT( desc_set.ShaderStages() == EShaderStages::Compute );

            _BindDynamicBuffers( ctx.Arguments(), index.mtlIndex.Compute(), is_bound, dynamicOffsets, desc_set );
            return;
        }
    }

/*
=================================================
    _BindDynamicBuffers
=================================================
*/
    template <typename ArgSetter>
    forceinline void  MBoundDescriptorSets::_BindDynamicBuffers (ArgSetter &&           args,
                                                                 uint                   idx,
                                                                 const bool             isBound,
                                                                 ArrayView<uint>        dynamicOffsets,
                                                                 const MDescriptorSet & descSet) __Th___
    {
        if ( isBound )
        {
            // argument buffer is already bound, so update only dynamic buffer offsets
            for (usize i = 0; i < dynamicOffsets.size(); ++i, ++idx)
            {
                args.SetBufferOffset( Bytes{dynamicOffsets[i]}, MBufferIndex(idx) );    // throw
            }
        }
        else
        {
            auto    dyn_bufs = descSet.GetDynamicBuffers();

            for (usize i = 0; i < dynamicOffsets.size(); ++i, ++idx)
            {
                args.SetBuffer( dyn_bufs.at<MetalBuffer>(i), dyn_bufs.at<Bytes>(i), MBufferIndex(idx) );    // throw
                args.SetBufferOffset( Bytes{dynamicOffsets[i]}, MBufferIndex(idx) );                        // throw
            }

            args.SetBuffer( descSet.Handle(), 0_b, MBufferIndex(idx) ); // throw
        }
    }

/*
=================================================
    UseHeapsAndResources
=================================================
*/
    template <typename Ctx>
    forceinline void  MBoundDescriptorSets::UseHeapsAndResources (Ctx &ctx) __Th___
    {
        if_likely( not _changed )
            return;

        _changed = false;

        // TODO
        Unused( ctx );
        //ctx.UseHeaps( _heapsFromDS );
        //ctx.UseResources( ... );
    }

/*
=================================================
    Reset
=================================================
*/
    forceinline void  MBoundDescriptorSets::Reset () __NE___
    {
        _changed    = false;
        _boundDS.fill( Default );
        _heapsFromDS.clear();
    }


} // AE::Graphics::_hidden_

#endif // AE_ENABLE_METAL
