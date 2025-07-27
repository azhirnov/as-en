// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Common.h"

namespace AE::UIEditor
{

	//
	// UI Screen
	//

	class UIScreen final : public EnableRC<UIScreen>
	{
	// types
	public:
		struct Ctor
		{
			RTechInfo		rtech;
			RC<RStream>		styleStream;
			RC<RStream>		widgetStream;

			ND_ RC<UIScreen>  Create (ResourceUploadManager&, ImageViewID);
		};

	private:
		class DrawTask;


	// variables
	private:
		RTechInfo					_rtech;
		ResourceCache				_resCache;

		UI::Widget::Allocator_t		_alloc;
		UI::WidgetPtr				_widget;
		UI::ScreenPtr				_screen;
		UI::SystemScope				_system;
		UI::IController::ActionMap	_actionMap;

		Serializing::ObjectFactory	_objFactory;

		DRC_ONLY( RWDataRaceCheck	_drCheck;)


	// methods
	public:
		UIScreen ()		__NE___;
		~UIScreen ()	__NE___;

		ND_ bool		Initialize (RTechInfo rtech, RC<RStream> styleStream, RC<RStream> widgetStream,
									ResourceUploadManager &uploadMngr, ImageViewID dummyView);
			void		Deinitialize ();

			void		ProcessInput (ActionQueueReader reader, OUT bool &switchMode);
		ND_ AsyncTask	Draw (RG::CommandBatchPtr &batch, Ptr<IOutputSurface> output, ArrayView<AsyncTask> deps);

		ND_ UI::StyleCollection const*	GetStyleCollection ()		const	{ return _system.IsCreated() ? &_system->GetStyleCollection() : null; }
		ND_ UI::StyleCollection*		GetStyleCollection ()				{ return _system.IsCreated() ? ConstCast( &_system->GetStyleCollection() ) : null; }

	private:
		static void		_DummyCb (UI::ILayout::LayoutState const&, AnyTypeRef);

		ND_ static RenderCoro  _DrawTask (RC<UIScreen> self, IOutputSurface &surface);
	};


} // AE::UIEditor
