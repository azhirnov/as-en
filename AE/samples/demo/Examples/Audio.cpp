// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#if defined(AE_ENABLE_IMGUI) and defined(AE_ENABLE_AUDIO)
# include "demo/Examples/Audio.h"
# include "imgui.h"

# include "res_loaders/WAV/WaveSoundSaver.h"
# include "res_loaders/Intermediate/IntermSound.h"

namespace AE::Samples::Demo
{
    using namespace AE::Audio;

    INTERNAL_LINKAGE( constexpr auto&   RTech   = RenderTechs::ImGui_RTech );
    INTERNAL_LINKAGE( constexpr auto&   IA      = InputActions::imGUI );


    //
    // Process Input Task
    //
    class AudioSample::ProcessInputTask final : public IAsyncTask
    {
    public:
        RC<AudioSample>     t;
        ActionQueueReader   reader;

        ProcessInputTask (AudioSample* p, ActionQueueReader reader) __NE___ :
            IAsyncTask{ ETaskQueue::PerFrame },
            t{ p }, reader{ RVRef(reader) }
        {}

        void  Run () __Th_OV;

        StringView  DbgName ()  C_NE_OV { return "Audio::ProcessInput"; }
    };

/*
=================================================
    ProcessInputTask::Run
=================================================
*/
    void  AudioSample::ProcessInputTask::Run () __Th___
    {
        t->imgui.mouseLBDown    = false;
        t->imgui.mouseWheel     = {};

        ActionQueueReader::Header   hdr;
        for (; reader.ReadHeader( OUT hdr );)
        {
            switch_IA2( IA.Desktop, hdr.name )
            {
                case IA.Desktop.MousePos :
                    t->imgui.mousePos = reader.Data<packed_float2>( hdr.offset );   break;

                case IA.Desktop.MouseWheel :
                    t->imgui.mouseWheel = reader.Data<packed_float2>( hdr.offset ); break;

                case IA.Desktop.MouseLBDown :
                    t->imgui.mouseLBDown = true;                                    break;
            }
            switch_end
            switch_IA( hdr.name )
            {
                case IA.Touch_Move :
                    t->imgui.mousePos    = reader.Data<packed_float2>( hdr.offset );
                    t->imgui.touchActive = hdr.state != EGestureState::End;         break;

                case IA.Touch_Click :
                    t->imgui.mousePos    = reader.Data<packed_float2>( hdr.offset );
                    t->imgui.mouseLBDown = true;                                    break;
            }
            switch_end
        }
    }
//-----------------------------------------------------------------------------



    //
    // Save Sound Task
    //
    class AudioSample::SaveSoundTask final : public IAsyncTask
    {
    // variables
    private:
        RC<MemWStream>      stream;
        AudioDataDesc       desc;


    // methods
    public:
        SaveSoundTask (RC<MemWStream> s, AudioDataDesc d) __NE___ :
            IAsyncTask{ ETaskQueue::Background },
            stream{ RVRef(s) }, desc{ d }
        {}

        void  Run () __Th_OV;

        StringView  DbgName ()  C_NE_OV { return "Audio::SaveSound"; }
    };

/*
=================================================
    SaveSoundTask::Run
=================================================
*/
    void  AudioSample::SaveSoundTask::Run () __Th___
    {
        ResLoader::IntermSound  interm;
        CHECK_TE( interm.SetData( desc, stream->GetData().data() ));

        ResLoader::WaveSoundSaver   saver;
        CHECK_TE( saver.SaveSound( R"(sound.wav)", interm ));
    }
//-----------------------------------------------------------------------------



    //
    // Draw Task
    //
    class AudioSample::DrawTask final : public RenderTask
    {
    // variables
    private:
        RC<AudioSample>     t;
        IOutputSurface &    surface;


    // methods
    public:
        DrawTask (AudioSample* p, IOutputSurface &surf, CommandBatchPtr batch, DebugLabel) __NE___ :
            RenderTask{ batch, {"AudioUI::Draw"} },
            t{ p }, surface{ surf }
        {}

        void  Run () __Th_OV
        {
            CHECK_TE( t->imgui.Draw( *this, surface, [this](){ _Update(); }, Default ));
        }

        void  _Update ();
    };

/*
=================================================
    DrawTask::_Update
=================================================
*/
    void  AudioSample::DrawTask::_Update ()
    {
        ImGui::Begin( "Player" );

        if ( ImGui::Button( "Play" ))
            t->sound->Play();

        ImGui::SameLine();
        if ( ImGui::Button( "Pause" ))
            t->sound->Pause();

        ImGui::SameLine();
        if ( ImGui::Button( "Stop" ))
            t->sound->Stop();

        ImGui::NewLine();

        if ( t->recorder->IsStarted() )
        {
            if ( ImGui::Button( "Stop Recording" ))
            {
                RC<WStream>             stream;
                Audio::AudioDataDesc    desc;
                CHECK( t->recorder->End( OUT stream, OUT desc ));
                CHECK( stream == t->inStream );

                Scheduler().Run< SaveSoundTask >( Tuple{ t->inStream, desc });
            }
        }
        else
        {
            if ( ImGui::Button( "Start Recording" ))
                CHECK( t->recorder->Begin( t->inStream ));
        }

        ImGui::NewLine();

        float   vol = t->sound->Volume();
        ImGui::SliderFloat( "Volume", &vol, 0.f, 10.f );
        t->sound->SetVolume( vol );

        ImGui::NewLine();

        auto        pos = t->sound->Position().GetNonScaledRef();
        const float max_pos = 3.f;
        ImGui::SliderFloat( "PosX", &pos.x, -max_pos, max_pos );
        ImGui::SliderFloat( "PosY", &pos.y, -max_pos, max_pos );
        ImGui::SliderFloat( "PosZ", &pos.z, -max_pos, max_pos );
        t->sound->SetPosition( pos );

        ImGui::End();

        AudioSystem().Apply3D();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Init
=================================================
*/
    bool  AudioSample::Init (PipelinePackID pack, IApplicationTS) __NE___
    {
        CHECK( profiler.Initialize( null ));

        auto&   res_mngr    = GraphicsScheduler().GetResourceManager();
        auto    gfx_alloc   = res_mngr.CreateLinearGfxMemAllocator();
        auto    rtech       = res_mngr.LoadRenderTech( pack, RTech, Default );

        CHECK_ERR( imgui.Initialize( gfx_alloc, rtech,
                                     ImGuiRenderer::PipelineInfo_t{
                                        Tuple{ EPixelFormat::SwapchainColor, RTech.Main, RTech.Main.imgui }}
                                    ));

        #ifdef AE_PLATFORM_ANDROID
            imgui.SetScale( 0.8f );
        #endif

        RC<RStream>     stream;
        CHECK_ERR( GetVFS().Open( OUT stream, VFS::FileName{"Sound.ogg"} ));

        RC<IAudioData>  sound_data = AudioSystem().CreateData( stream, ESoundFlags::Enable3D );
        CHECK_ERR( sound_data );

        sound = AudioSystem().CreateOutput( sound_data );
        CHECK_ERR( sound );

        sound->SetLooping( true );
        sound->Play();


        recorder = AudioSystem().CreateInput( Default );
        CHECK_ERR( recorder );

        inStream = MakeRC<MemWStream>();

        return true;
    }

/*
=================================================
    Update
=================================================
*/
    AsyncTask  AudioSample::Update (const IInputActions::ActionQueueReader &reader, ArrayView<AsyncTask> deps) __NE___
    {
        return Scheduler().Run< ProcessInputTask >( Tuple{ this, reader }, Tuple{deps} );
    }

/*
=================================================
    GetInputMode
=================================================
*/
    InputModeName  AudioSample::GetInputMode () C_NE___
    {
        return IA;
    }

/*
=================================================
    Draw
=================================================
*/
    AsyncTask  AudioSample::Draw (RenderGraph &rg, ArrayView<AsyncTask> deps) __NE___
    {
        auto    batch = rg.Render( "Audio UI pass" );
        CHECK_ERR( batch );

        auto    surf_acquire = rg.BeginOnSurface( batch, deps );
        CHECK_ERR( surf_acquire );

        return batch->Run< DrawTask >( Tuple{ this, rg.GetSurfaceArg() }, Tuple{surf_acquire}, True{"Last"}, Default );
    }


} // AE::Samples::Demo

#endif // AE_ENABLE_IMGUI and AE_ENABLE_AUDIO
