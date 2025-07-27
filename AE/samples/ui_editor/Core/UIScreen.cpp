// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Core/UIScreen.h"
#include "Core/EditorCore.h"

#include "_data/cpp/types.h"
#include "_ui_data/cpp/types.h"

namespace AE::UIEditor
{
	static constexpr auto&	IA	= InputActions::Screen_UI;

/*
=================================================
	_DrawTask
=================================================
*/
	RenderCoro  UIScreen::_DrawTask (RC<UIScreen> self, IOutputSurface &surface)
	{
		const secondsf	dt = GraphicsScheduler().GetFrameTimeDelta();

		IOutputSurface::RenderTargets_t		targets;
		CHECK_CE( surface.GetTargets( OUT targets ));

		auto&	rt		= targets[0];
		auto&	screen	= *self->_screen;
		auto&	config	= UIEditorCore::config;

		Canvas	canvas;
		canvas.SetDimensions( rt );

		screen.SetDimensions( canvas.Dimensions() );
		screen.Update();

		UI::TransferContext_t	tfr_ctx{ RenderCoro_Get() };

		{
			UI::Screen::PreDrawParams	params;
			params.dt	= dt;

			screen.PreDraw( params, tfr_ctx );
		}

		DirectCtx::Graphics		gfx_ctx{ RenderCoro_Get(), tfr_ctx.ReleaseCommandBuffer() };

		// draw
		{
			canvas.NextFrame( RenderCoro_Get().FrameId() );

			gfx_ctx.AddSurfaceTargets( targets );

			auto	draw_ctx = gfx_ctx.BeginRenderPass( RenderPassDesc{ *self->_rtech.rtech, RenderTechPassName{"Main"}, rt.RegionSize() }
										.AddViewport( rt.RegionSize() )
										.AddTarget( AttachmentName{"Color"}, rt.viewId, rt.initialState | EResourceState::Invalidate, rt.finalState ));

			draw_ctx.ClearAttachment( AttachmentName{"Color"}, RGBA32f{HtmlColor::Black}, rt.region );

			if ( not config.dbgLayouts.load() )
			{
				// pass 1
				/*{
					UI::Screen::Draw1Params	params;
					params.baseStencilRef	= 0;

					screen.DrawPass1( params, canvas, draw_ctx.GetBaseContext() );
				}*/

				// pass 2
				{
					UI::Screen::Draw2Params	params;
					params.baseStencilRef	= 0;
					params.dt				= dt;

					screen.DrawPass2( params, canvas, draw_ctx.GetBaseContext() );
				}
			}
			else
			{
				screen.DbgDrawLayouts( canvas, draw_ctx.GetBaseContext() );
			}

			gfx_ctx.EndRenderPass( draw_ctx );
		}

		RenderCoro_Execute( gfx_ctx );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	UIScreen::UIScreen () __NE___
	{}

	UIScreen::~UIScreen () __NE___
	{
		Deinitialize();
	}

/*
=================================================
	Initialize
----
	main thread
=================================================
*/
	bool  UIScreen::Initialize (RTechInfo rtech, RC<RStream> styleStream, RC<RStream> widgetStream,
								ResourceUploadManager &uploadMngr, ImageViewID dummyView)
	{
		using namespace AE::UI;

		DRC_EXLOCK( _drCheck );
		CHECK_ERR( rtech );

		_rtech = RVRef(rtech);

		CHECK_ERR( _system.Create( StyleCollection::CreateInfo{
						_rtech.rtech,
						RVRef(styleStream),
						Ref{_resCache},
						Ref{uploadMngr},
						SizeOf<ShaderTypes::ui_global_ublock>,
						dummyView
					}));

		UI::Widget::TempAllocator_t			temp_alloc;
		UI::IController::ActionMapBuilder	builder;

		_screen	= UISystem().CreateScreen();
		_widget	= Widget::New( _alloc, temp_alloc );

		CHECK_ERR( ILayout::RegisterLayouts( _objFactory ));
		CHECK_ERR( IDrawable::RegisterDrawables( _objFactory ));
		CHECK_ERR( IController::RegisterControllers( _objFactory ));

		{
			Serializing::Deserializer	des	{RVRef(widgetStream)};
			des.factory	= &_objFactory;

			CHECK_ERR( _widget->Deserialize( des ));
			CHECK_ERR( builder.Deserialize( des ));
		}

		builder.BindAll(
			[] (ActionName::Ref, OUT UI::IController::ActionMap::Callback_t &cb)
			{
				cb = _DummyCb;
			});

		_actionMap = builder.Build();
		_widget->SetActionBindings( _actionMap );

		_screen->Add( _widget );

		return true;
	}

/*
=================================================
	Deinitialize
----
	main thread
=================================================
*/
	void  UIScreen::Deinitialize ()
	{
		DRC_EXLOCK( _drCheck );

		_widget	= null;
		_screen = null;
		_system.Destroy();
	}

/*
=================================================
	ProcessInput
----
	async
=================================================
*/
	void  UIScreen::ProcessInput (ActionQueueReader reader, OUT bool &switchMode)
	{
		DRC_SHAREDLOCK( _drCheck );

		switchMode = false;

		_screen->ProcessInput( reader );

		ActionQueueReader::Header	hdr;
		for (; reader.ReadHeader( OUT hdr );)
		{
			if_unlikely( uint{hdr.name} == IA.SwitchInputMode )
				switchMode = true;
		}
	}

/*
=================================================
	Draw
----
	main thread
=================================================
*/
	AsyncTask  UIScreen::Draw (RG::CommandBatchPtr &batch, Ptr<IOutputSurface> output, ArrayView<AsyncTask> deps)
	{
		DRC_SHAREDLOCK( _drCheck );

		return batch.Task( _DrawTask( GetRC<UIScreen>(), *output ), {"UIScreen pass"} )
						.Run( Tuple{deps} );
	}

/*
=================================================
	Ctor::Create
=================================================
*/
	RC<UIScreen>  UIScreen::Ctor::Create (ResourceUploadManager &uploadMngr, ImageViewID dummyView)
	{
		auto	result = MakeRC<UIScreen>();
		CHECK_ERR( result->Initialize( RVRef(rtech), RVRef(styleStream), RVRef(widgetStream), uploadMngr, dummyView ));
		return result;
	}

/*
=================================================
	_DummyCb
=================================================
*/
	void  UIScreen::_DummyCb (UI::ILayout::LayoutState const&, AnyTypeRef)
	{
		AE_LOGI( "ui.callback" );
	}


} // AE::UIEditor
