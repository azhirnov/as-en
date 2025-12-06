// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "Examples/ImGuiSample.h"
# include "imgui.h"

namespace AE::Samples::Demo
{
	INTERNAL_LINKAGE( constexpr auto&	RTech	= RenderTechs::ImGui_RTech );
	INTERNAL_LINKAGE( constexpr auto&	IA		= InputActions::imGUI );


/*
=================================================
	_DrawTask
=================================================
*/
	RenderCoro  ImGuiSample::_DrawTask (RC<ImGuiSample> t, IOutputSurface &surface) __NE___
	{
		static bool		show_demo_window	= true;
		static bool		show_another_window	= false;
		static ImVec4	clear_color;

		const auto	UpdateUI = [t] ()
		{{
			// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
			if ( show_demo_window )
				ImGui::ShowDemoWindow( OUT &show_demo_window );

			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
			{
				static float	f		= 0.0f;
				static int		counter	= 0;

				ImGui::Begin( "Hello, world!" );							// Create a window called "Hello, world!" and append into it.

				ImGui::Text( "This is some useful text." );					// Display some text (you can use a format strings too)
				ImGui::Checkbox( "Demo Window", &show_demo_window );		// Edit bools storing our window open/close state
				ImGui::Checkbox( "Another Window", OUT &show_another_window );

				ImGui::SliderFloat( "float", &f, 0.0f, 1.0f );				// Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3( "clear color", &clear_color.x );			// Edit 3 floats representing a color

				if ( ImGui::Button( "Button" ))								// Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;

				ImGui::SameLine();
				ImGui::Text( "counter = %d", counter );

				ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", double(1000.0f / ImGui::GetIO().Framerate), double(ImGui::GetIO().Framerate) );
				ImGui::End();
			}

			// 3. Show another simple window.
			if ( show_another_window )
			{
				ImGui::Begin( "Another Window", &show_another_window );		// Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
				ImGui::Text( "Hello from another window!" );
				if ( ImGui::Button( "Close Me" ))
					show_another_window = false;
				ImGui::End();
			}

			t->profiler.DrawImGUI();
		}};

		CHECK_CE( t->imgui.Draw( RenderCoro_Get(), surface, UpdateUI, Default ));
		co_return;
	}

/*
=================================================
	Init
=================================================
*/
	bool  ImGuiSample::Init (PipelinePackID pack, IApplicationTS) __NE___
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
		return true;
	}

/*
=================================================
	Update
=================================================
*/
	AsyncTask  ImGuiSample::Update (const IInputActions::ActionQueueReader &reader, ArrayView<AsyncTask> deps) __NE___
	{
		return Scheduler().Run(
					ETaskQueue::PerFrame,
					_ProcessInputTask( GetRC<ImGuiSample>(), reader ),
					Tuple{deps},
					"ImGui::ProcessInput"
				);
	}

/*
=================================================
	_ProcessInputTask
=================================================
*/
	AsyncCoro  ImGuiSample::_ProcessInputTask (RC<ImGuiSample> t, ActionQueueReader reader) __NE___
	{
		t->imgui.mouseLBDown	= false;
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
					t->imgui.mouseLBDown = true;									break;
			}
			switch_end
			switch_IA( hdr.name )
			{
				case IA.Touch_Move :
					t->imgui.mousePos    = reader.Data<packed_float2>( hdr.offset );
					t->imgui.touchActive = hdr.state != EGestureState::End;			break;

				case IA.Touch_Click :
					t->imgui.mousePos    = reader.Data<packed_float2>( hdr.offset );
					t->imgui.mouseLBDown = true;									break;
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
	InputModeName  ImGuiSample::GetInputMode () C_NE___
	{
		return IA;
	}

/*
=================================================
	Draw
=================================================
*/
	AsyncTask  ImGuiSample::Draw (RenderGraph &rg, ArrayView<AsyncTask> deps) __NE___
	{
		auto	batch = rg.Render( "ImGui pass" );
		CHECK_ERR( batch );

		auto	surf_acquire = rg.BeginOnSurface( batch, deps );
		CHECK_ERR( surf_acquire );

		return batch->Run( _DrawTask( GetRC<ImGuiSample>(), rg.GetSurface() ), Tuple{surf_acquire}, True{"Last"}, Default );
	}


} // AE::Samples::Demo

#endif // AE_ENABLE_IMGUI
