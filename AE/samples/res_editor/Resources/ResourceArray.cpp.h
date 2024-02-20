// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Resources/Buffer.h"
#include "res_editor/Resources/RTScene.h"
#include "res_editor/Resources/Image.h"
#include "res_editor/Resources/VideoImage.h"
#include "res_editor/Resources/VideoImage2.h"
#include "res_editor/Resources/ResourceArray.h"

namespace AE::ResEditor
{

/*
=================================================
    SetStates
=================================================
*/
    template <typename CtxType>
    void  ResourceArray::SetStates (CtxType &ctx, EResourceState shaderStages) const
    {
        const FrameUID  fid = ctx.GetFrameId();
        for (auto& [un, res, in_state] : _resources)
        {
            const EResourceState    state   = in_state | shaderStages;
            Visit( res,
                [&] (const RC<Buffer> &buf) {
                    ctx.ResourceState( buf->GetBufferId( fid ), state );

                    const auto  ref_state = EResourceState::ShaderAddress_RW | (state & EResourceState::AllShaders);
                    for (auto& ref : buf->GetRefBuffers()) {
                        ctx.ResourceState( ref->GetBufferId( fid ), ref_state );
                    }
                },
                [&] (const RC<RTScene> &scene) {
                    scene->Validate( fid );
                    ctx.ResourceState( scene->GetSceneId( fid ), state );
                },
                [&] (const RC<Image> &img) {
                    ctx.ResourceState( img->GetImageId(), state );
                },
                [&] (const RC<VideoImage> &video) {
                    ctx.ResourceState( video->GetImageId(), state );
                },
                [&] (const RC<VideoImage2> &video) {
                    ctx.ResourceState( video->GetVideoImageId(), state );
                },
                [&] (const Array<RC<Image>> &arr) {
                    for (auto& img : arr) {
                        ctx.ResourceState( img->GetImageId(), state );
                    }
                },
                [](NullUnion) {}
            );
        }
    }

/*
=================================================
    Bind
=================================================
*/
    inline bool  ResourceArray::Bind (FrameUID fid, DescriptorUpdater &updater) const
    {
        for (auto& [in_un, res, state] : _resources)
        {
            auto&   un  = in_un;
            CHECK_ERR( Visit( res,
                [&] (const RC<Buffer> &buf) {
                    return updater.BindBuffer( un, buf->GetBufferId( fid ));
                },
                [&] (const RC<RTScene> &scene) {
                    return updater.BindRayTracingScene( un, scene->GetSceneId( fid ));
                },
                [&] (const RC<Image> &img) {
                    return updater.BindImage( un, img->GetViewId() );
                },
                [&] (const RC<VideoImage> &video) {
                    return updater.BindImage( un, video->GetViewId() );
                },
                [&] (const RC<VideoImage2> &video) {
                    return updater.BindVideoImage( un, video->GetVideoImageId() );
                },
                [&] (const Array<RC<Image>> &arr) {
                    bool    res = true;
                    for (usize i : IndicesOnly( arr )) {
                        res &= updater.BindImage( un, arr[i]->GetViewId(), uint(i) );
                    }
                    return res;
                },
                [](NullUnion) {
                    return true;
                }
            ));
        }
        return true;
    }

/*
=================================================
    GetResourcesToResize
=================================================
*/
    inline void  ResourceArray::GetResourcesToResize (INOUT Array<RC<IResource>> &result) const
    {
        for (auto& [un, res, state] : _resources)
        {
            Visit( res,
                [&] (const RC<Buffer> &buf) {
                    if_unlikely( buf->RequireResize() )
                        result.push_back( buf );
                },
                [&] (const RC<RTScene> &scene) {
                    if_unlikely( scene->RequireResize() )
                        result.push_back( scene );
                },
                [&] (const RC<Image> &img) {
                    if_unlikely( img->RequireResize() )
                        result.push_back( img );
                },
                [&] (const RC<VideoImage> &video) {
                    if_unlikely( video->RequireResize() )
                        result.push_back( video );
                },
                [&] (const RC<VideoImage2> &video) {
                    if_unlikely( video->RequireResize() )
                        result.push_back( video );
                },
                [&] (const Array<RC<Image>> &arr) {
                    for (auto& img : arr) {
                        if_unlikely( img->RequireResize() )
                            result.push_back( img );
                    }
                },
                [](NullUnion) {
                }
            );
        }
    }


} // AE::ResEditor
