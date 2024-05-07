// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/UI/Screen.h"
#include "graphics_hl/UI/System.h"

namespace AE::UI
{
	using namespace AE::Threading;
	using namespace AE::Graphics;
	using namespace AE::App;

/*
=================================================
	constructor
=================================================
*/
	Screen::Screen () __NE___
	{
		_stack.reserve( 16 );
		_drawQueue.reserve( 16 );
	}

/*
=================================================
	SetDimensions
=================================================
*/
	void  Screen::SetDimensions (const SurfaceDimensions &value) __NE___
	{
		DRC_EXLOCK( _drCheck );
		_surfDim = value;
	}

	void  Screen::SetDimensions (const App::IOutputSurface::RenderTarget &rt) __NE___
	{
		DRC_EXLOCK( _drCheck );
		_surfDim.SetDimensions( rt );
	}

/*
=================================================
	Add
=================================================
*/
	bool  Screen::Add (RC<Widget> ptr) __NE___
	{
		DRC_EXLOCK( _drCheck );
		ASSERT( not Remove( ptr ));

		_stack.push_back( ptr );
		return true;
	}

/*
=================================================
	Remove
=================================================
*/
	bool  Screen::Remove (RC<Widget> ptr) __NE___
	{
		DRC_EXLOCK( _drCheck );

		for (auto iter = _stack.begin(); iter != _stack.end(); ++iter)
		{
			if_unlikely( *iter == ptr )
			{
				_stack.erase( iter );
				return true;
			}
		}
		return false;
	}

/*
=================================================
	Update
=================================================
*/
	bool  Screen::Update () __NE___
	{
		DRC_EXLOCK( _drCheck );

		_drawQueue.clear();

		const float2	scr_size	= _surfDim.GetSurfaceSize();
		const float		mm_to_px	= _surfDim.GetMmToPixels();

		// update layouts
		for (auto iter = _stack.rbegin(); iter != _stack.rend(); ++iter)
		{
			auto&	w = **iter;

			w.Update( scr_size, mm_to_px, _inputState );

			_drawQueue.push_back( &w );

			if_unlikely( w.IsOpaque() and All( RectF{scr_size} == w.GlobalRect() ))
				break;
		}

		return true;
	}

/*
=================================================
	_GlobalUBufOffset
=================================================
*/
	uint  Screen::_GlobalUBufOffset () C_NE___
	{
		return (_frameId.Index() + uint{_indexInPool} * StyleCollection::MaxScreens) * uint{_ubufferSize};
	}

/*
=================================================
	PreDraw
=================================================
*/
	void  Screen::PreDraw (const PreDrawParams &params, TransferContext_t &ctx) __Th___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERRV( _ubuffer );

		_frameId = GraphicsScheduler().GetFrameId();

		for (auto* w : Reverse(_drawQueue))
		{
			w->PreDraw( params, ctx );  // throw
		}

		const float2	inv_size_2	= 2.f * _surfDim.GetInvSurfaceSize();	// in pixels
		const float		data[4]		= { inv_size_2.x, inv_size_2.y,			// scale
										-1.0f, -1.0f						// bias
									  };
		const Bytes		off			= Bytes{_GlobalUBufOffset()};

		CHECK( Sizeof(data) <= _ubufferSize );
		ctx.UpdateBuffer( _ubuffer, off, Sizeof(data), data );  // throw
	}

/*
=================================================
	DrawPass1
=================================================
*/
	void  Screen::DrawPass1 (const Draw1Params &params, Canvas &canvas, DrawContext_t &ctx) __Th___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERRV( _fillStencilPpln and _descSet );

		//params.baseStencilRef = ...		// TODO

		for (auto* w : Reverse(_drawQueue))
		{
			w->DrawPass1( params, canvas, ctx );  // throw
		}

		const uint	off = _GlobalUBufOffset();

		ctx.BindDescriptorSet( IDrawable::GlobalMaterial::dsIndex, _descSet, {&off,1} );	// throw
		ctx.BindPipeline( _fillStencilPpln );												// throw
		canvas.Flush( ctx, IDrawable::GlobalMaterial::topology );							// throw
	}

/*
=================================================
	DrawPass2
=================================================
*/
	void  Screen::DrawPass2 (const Draw2Params &inParams, Canvas &canvas, DrawContext_t &ctx) __Th___
	{
		DRC_EXLOCK( _drCheck );
		CHECK_ERRV( _descSet );

		Draw2Params		params		= inParams;

		params.mtr.ds				= _descSet;
		params.mtr.globalDynOffset	= _GlobalUBufOffset();
	//	params.baseStencilRef		= ...		// TODO

		for (auto* w : Reverse(_drawQueue))
		{
			w->DrawPass2( params, canvas, ctx );  // throw
		}

		if_unlikely( params.mtr.IsDefined() )
		{
			ctx.BindPipeline( params.mtr->ppln );																				// throw
			ctx.BindDescriptorSet( params.mtr.dsIndex, params.mtr.ds, {&params.mtr.globalDynOffset, 1/*, params.mtr->mtrDynOffset*/} );	// throw
		//	ctx.SetStencilReference( params.mtr->stencilRef );																	// throw

			canvas.Flush( ctx, params.mtr.topology );																			// throw
		}
	}

/*
=================================================
	DbgDrawLayouts
=================================================
*/
	void  Screen::DbgDrawLayouts (Canvas &canvas, DrawContext_t &ctx) __Th___
	{
		for (auto* w : _drawQueue)
		{
			w->DbgDrawLayouts( canvas );
		}

		const uint	off = _GlobalUBufOffset();

		ctx.BindPipeline( UIStyleCollection().GetDebugDrawPipeline() );						// throw
		ctx.BindDescriptorSet( IDrawable::GlobalMaterial::dsIndex, _descSet, {&off,1} );	// throw
		canvas.Flush( ctx, IDrawable::GlobalMaterial::topology );							// throw
	}

/*
=================================================
	_SetMaterial
=================================================
*/
	bool  Screen::_SetMaterial (GraphicsPipelineID ppln, DescriptorSetID descSet, BufferID ubuf, Bytes16u ubSize) __NE___
	{
		CHECK_ERR( _IsInitialized() );

		_fillStencilPpln	= ppln;
		_descSet			= descSet;
		_ubuffer			= ubuf;
		_ubufferSize		= ubSize;

		return true;
	}

/*
=================================================
	_ReleaseObject
=================================================
*/
	void  Screen::_ReleaseObject () __NE___
	{
		//MemoryBarrier( EMemoryOrder::Acquire );

		CHECK_ERRV( _IsInitialized() );

		//MemoryBarrier( EMemoryOrder::Release );
		SystemImpl::ScreenApi::Recycle( _indexInPool );
	}

/*
=================================================
	ProcessInputTask
=================================================
*/
	class Screen::ProcessInputTask final : public Threading::IAsyncTask
	{
	public:
		Screen&				scr;
		ActionQueueReader	reader;

		ProcessInputTask (Screen* scr, ActionQueueReader reader) __NE___ :
			IAsyncTask{ ETaskQueue::PerFrame },
			scr{ *scr },
			reader{ RVRef(reader) }
		{}

		void  Run () __Th_OV
		{
			scr.ProcessInput( reader );
		}

		StringView	DbgName ()	C_NE_OV	{ return "Screen::ProcessInput"; }
	};

/*
=================================================
	ProcessInputAsync
=================================================
*/
	AsyncTask  Screen::ProcessInputAsync (ActionQueueReader reader, ArrayView<AsyncTask> deps) __NE___
	{
		CHECK_ERR( _IsInitialized() );
		return Scheduler().Run< ProcessInputTask >( Tuple{ this, reader }, Tuple{deps} );
	}

/*
=================================================
	ProcessInput
=================================================
*/
	void  Screen::ProcessInput (ActionQueueReader reader) __NE___
	{
		using EGesture		= IController::InputState::EGesture;
		using EGestureBits	= IController::InputState::EGestureBits;

		DRC_EXLOCK( _drCheck );

		float2			pos				{-1000.f};
		float2			wheel;
		float4			scale_rotate;
		float			long_press		= 0.f;
		EGestureBits	bits;

		ActionQueueReader::Header	hdr;
		for (; reader.ReadHeader( OUT hdr );)
		{
			switch ( uint{hdr.name} )
			{
				case uint{InputActionName{"ui.MousePos"}} :
					pos = reader.DataCopy<float2>( hdr.offset );			break;

				case uint{InputActionName{"ui.MouseWheel"}} :
					wheel = reader.DataCopy<float2>( hdr.offset );			break;

				case uint{InputActionName{"ui.TouchMove"}} :
					pos = reader.DataCopy<float2>( hdr.offset );
					bits.insert( EGesture::Move );							break;

				case uint{InputActionName{"ui.TouchDown"}} :
					pos = reader.DataCopy<float2>( hdr.offset );
					bits.insert( EGesture::Down );							break;

				case uint{InputActionName{"ui.TouchPress"}} :
					long_press	= reader.Data<float>( hdr.offset + sizeof(float2) );
					pos			= reader.DataCopy<float2>( hdr.offset );
					bits.insert( EGesture::Hold );							break;

				case uint{InputActionName{"ui.TouchClick"}} :
					pos = reader.DataCopy<float2>( hdr.offset );
					bits.insert( EGesture::Click );							break;

				case uint{InputActionName{"ui.TouchDoubleClick"}} :
					pos = reader.DataCopy<float2>( hdr.offset );
					bits.insert( EGesture::DoubleClick );					break;

				case uint{InputActionName{"ui.TouchLongPressMove"}} :
					pos = reader.DataCopy<float2>( hdr.offset );
					bits.insert( EGesture::LongPress );						break;

				case uint{InputActionName{"ui.ScaleRotate2D"}} :
					scale_rotate = reader.DataCopy<float4>( hdr.offset );	break;
			}
		}

		_inputState._SetCursorState( pos, wheel, long_press, scale_rotate, bits );
	}


} // AE::UI
