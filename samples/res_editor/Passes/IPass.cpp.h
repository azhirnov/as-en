// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Resources/Buffer.h"
#include "res_editor/Resources/RTScene.h"
#include "res_editor/Resources/Image.h"
#include "res_editor/Resources/VideoImage.h"

namespace AE::ResEditor
{

/*
=================================================
    _SetResStates
=================================================
*/
    template <typename CtxType>
    void  IPass::_SetResStates (FrameUID fid, CtxType &ctx, const Resources_t &resources)
    {
        for (auto& [un, res, in_state] : resources)
        {
            auto&   state   = in_state;
            Visit( res,
                [&] (const RC<Buffer> &buf) {
                    ctx.ResourceState( buf->GetBufferId( fid ), state );
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
                [](NullUnion) {}
            );
        }
    }

/*
=================================================
    _ResizeRes
=================================================
*/
    template <typename CtxType>
    void  IPass::_ResizeRes (CtxType &ctx, Resources_t &resources)
    {
        for (auto& [un, res, state] : resources)
        {
            Visit( res,
                [&ctx] (const RC<Buffer> &buf)       { buf->Resize( ctx ); },
                [&ctx] (const RC<RTScene> &scene)    { scene->Resize( ctx ); },
                [&ctx] (const RC<Image> &img)        { img->Resize( ctx ); },
                [&ctx] (const RC<VideoImage> &video) { video->Resize( ctx ); },
                [](NullUnion) {}
            );
        }
    }

/*
=================================================
    _BindRes
=================================================
*/
    inline bool  IPass::_BindRes (FrameUID fid, DescriptorUpdater &updater, Resources_t &resources)
    {
        for (auto& [in_un, res, state] : resources)
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
                            [](NullUnion) {
                                    return true;
                            }
                        ));
        }
        return true;
    }


} // AE::ResEditor
