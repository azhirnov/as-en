// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/ResourceEditor.pch.h"

namespace AE::ResEditor::_hidden_
{
    // tag: NamedID UID
    static constexpr uint   NamedIDs_Start  = 5 << 24;

} // AE::ResEditor::_hidden_


namespace AE::ResEditor
{
    using namespace AE::App;
    using namespace AE::Graphics;

    using AE::Threading::AsyncTask;
    using AE::Threading::CoroTask;

    using AE::Threading::AsyncDSRequestResult;
    using AE::Threading::AsyncRDataSource;
    using AE::Threading::AsyncDSRequest;
    using AE::Threading::Synchronized;

    using AE::ResLoader::EImageFormat;

    using AE::Video::IVideoEncoder;
    using AE::Video::EVideoFormat;
    using AE::Video::EVideoCodec;
    using EVideoColorPreset = AE::Video::EColorPreset;

    using AE::RG::DirectCtx;    // override Graphics::DirectCtx
    using AE::RG::IndirectCtx;  // override Graphics::IndirectCtx

    using ActionQueueReader = IInputActions::ActionQueueReader;


    class ScriptExe;
    class IPass;
    class Image;
    class VideoImage;
    class PassGroup;


    struct RTechInfo
    {
        GAutorelease<PipelinePackID>    packId;
        RenderTechPipelinesPtr          rtech;

        ND_ explicit operator bool ()   C_NE___ { return packId and rtech; }
    };

    static constexpr uint   ResNameMaxLen = 60;

} // AE::ResEditor
