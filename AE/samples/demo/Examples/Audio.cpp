// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#if defined(AE_ENABLE_IMGUI) and defined(AE_ENABLE_AUDIO)
# include "Examples/Audio.h"
# include "imgui.h"

# include "res_loaders/WAV/WaveSoundSaver.h"
# include "res_loaders/Intermediate/IntermSound.h"

namespace AE::Samples::Demo
{
	using namespace AE::Audio;

	INTERNAL_LINKAGE( constexpr auto&	RTech	= RenderTechs::ImGui_RTech );
	INTERNAL_LINKAGE( constexpr auto&	IA		= InputActions::imGUI );


/*
=================================================
	_SaveSoundTask
=================================================
*/
	AsyncCoro  AudioSample::_SaveSoundTask (RC<ArrayWStream> stream, AudioDataDesc desc) __NE___
	{
		ResLoader::IntermSound	interm;
		CHECK_CE( interm.SetData( desc, stream->GetData().data() ));

		ResLoader::WaveSoundSaver	saver;
		CHECK_CE( saver.SaveSound( "sound.wav", interm ));
		co_return;
	}

/*
=================================================
	_DrawTask
=================================================
*/
	RenderCoro  AudioSample::_DrawTask (RC<AudioSample> t, IOutputSurface &surface) __NE___
	{
		const auto	UpdateUI = [t] ()
		{{
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
					RC<WStream>				stream;
					Audio::AudioDataDesc	desc;
					CHECK( t->recorder->End( OUT stream, OUT desc ));
					CHECK( stream == t->inStream );

					Scheduler().Run(
						ETaskQueue::Background,
						_SaveSoundTask( t->inStream, desc ),
						{},
						"Audio::SaveSound"
					);
				}
			}
			else
			{
				if ( ImGui::Button( "Start Recording" ))
					CHECK( t->recorder->Begin( t->inStream ));
			}

			ImGui::NewLine();

			float	vol = t->sound->Volume();
			ImGui::SliderFloat( "Volume", &vol, 0.f, 10.f );
			t->sound->SetVolume( vol );

			ImGui::NewLine();

			auto		pos = t->sound->Position().GetNonScaledRef();
			const float	max_pos = 3.f;
			ImGui::SliderFloat( "PosX", &pos.x, -max_pos, max_pos );
			ImGui::SliderFloat( "PosY", &pos.y, -max_pos, max_pos );
			ImGui::SliderFloat( "PosZ", &pos.z, -max_pos, max_pos );
			t->sound->SetPosition( pos );

			ImGui::End();

			AudioSystem().Apply3D();
		}};

		CHECK_CE( t->imgui.Draw( RenderCoro_Get(), surface, UpdateUI, Default ));
		co_return;
	}

/*
=================================================
	Init
=================================================
*/
	bool  AudioSample::Init (PipelinePackID pack, IApplicationTS) __NE___
	{
		CHECK( profiler.Initialize( null ));

		auto&	res_mngr	= GraphicsScheduler().GetResourceManager();
		auto	gfx_alloc	= res_mngr.CreateLinearGfxMemAllocator();
		auto	rtech		= res_mngr.LoadRenderTech( pack, RTech );

		CHECK_ERR( imgui.Initialize( gfx_alloc, rtech,
									 ImGuiRenderer::PipelineInfo_t{
										Tuple{ EPixelFormat::SwapchainColor, RTech.Main, RTech.Main.imgui }}
									));

		#ifdef AE_PLATFORM_ANDROID
			imgui.SetScale( 0.8f );
		#endif

		RC<RStream>		stream;
		CHECK_ERR( GetVFS().Open( OUT stream, VFS::FileName{"Sonic Revolution.mp3"} ));

		RC<IAudioData>	sound_data = AudioSystem().CreateData( stream, ESoundFlags::Enable3D );
		CHECK_ERR( sound_data );

		sound = AudioSystem().CreateOutput( sound_data );
		CHECK_ERR( sound );

		sound->SetLooping( true );
		sound->Play();


		recorder = AudioSystem().CreateInput( Default );
		CHECK_ERR( recorder );

		inStream = MakeRC<ArrayWStream>();

		return true;
	}

/*
=================================================
	Update
=================================================
*/
	AsyncTask  AudioSample::Update (const IInputActions::ActionQueueReader &reader, ArrayView<AsyncTask> deps) __NE___
	{
		return Scheduler().Run(
					ETaskQueue::PerFrame,
					_ProcessInputTask( GetRC<AudioSample>(), reader ),
					Tuple{deps},
					"Audio::ProcessInput"
				);
	}

/*
=================================================
	_ProcessInputTask
=================================================
*/
	AsyncCoro  AudioSample::_ProcessInputTask (RC<AudioSample> t, ActionQueueReader reader) __NE___
	{
		t->imgui.mouseBtnDown	= {};
		t->imgui.mouseWheel		= {};

		ActionQueueReader::Header	hdr;
		for (; reader.ReadHeader( OUT hdr );)
		{
			switch_IA2( IA.Desktop, hdr.name )
			{
				case IA.Desktop.MousePos :
					t->imgui.mousePos = reader.Data<packed_float2>( hdr.offset );	break;

				case IA.Desktop.MouseWheel :
					t->imgui.mouseWheel = reader.Data<packed_float2>( hdr.offset );	break;

				case IA.Desktop.MouseLBDown :
					t->imgui.mouseBtnDown[0] = true;								break;

				case IA.Desktop.UI_Char :
					break;
			}
			switch_end
			switch_IA( hdr.name )
			{
				case IA.Touch_Move :
					t->imgui.mousePos    = reader.Data<packed_float2>( hdr.offset );
					t->imgui.touchActive = hdr.state != EGestureState::End;			break;

				case IA.Touch_Click :
					t->imgui.mousePos    = reader.Data<packed_float2>( hdr.offset );
					t->imgui.mouseBtnDown[0] = true;								break;
			}
			switch_end
		}
		co_return;
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
		auto	batch = rg.Render( "Audio UI pass" );
		CHECK_ERR( batch );

		auto	surf_acquire = rg.BeginOnSurface( batch, deps );
		CHECK_ERR( surf_acquire );

		return batch->Run( _DrawTask( GetRC<AudioSample>(), rg.GetSurface() ), Tuple{surf_acquire}, True{"Last"}, Default );
	}


} // AE::Samples::Demo

#endif // AE_ENABLE_IMGUI and AE_ENABLE_AUDIO
