// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Widget contains collection of Views.
	Layouts are sorted for best performance when updated (calculated location and size).
	Drawables are sorted for correct draw order and low state changes.
	Controllers are sorted for correct input processing order.
*/

#pragma once

#include "graphics_hl/UI/Layout.h"
#include "graphics_hl/UI/Drawable.h"
#include "graphics_hl/UI/Controller.h"

namespace AE::UI
{

	//
	// Widget
	//
	class Widget final :
		public EnableRC<Widget>,
		public NonAllocatable,
		public Serializing::ISerializable
	{
	// types
	public:
		using Allocator_t		= Base::LinearAllocator< UntypedAllocator, 8, false >;
		using TempAllocator_t	= Base::LinearAllocator< UntypedAllocator, 8, false >;

		using PreDrawParams		= IDrawable::PreDrawParams;

		struct Draw1Params
		{
			mutable ushort				baseStencilRef	= 0;
		};

		struct Draw2Params
		{
			IDrawable::GlobalMaterial	mtr;
			ushort						baseStencilRef;
			float						dt;
		};


	private:
		using LayoutType_t		= ILayout::EType;
		using LayoutState_t		= ILayout::LayoutState;
		using LayoutData_t		= ILayout::LayoutData;

		struct DrawableInfo
		{
			IDrawable*		ptr			= null;
			uint			stencilRef	= 0;
			uint			layoutIdx	= UMax;
		};

		struct ControllerInfo
		{
			IController*	ptr			= null;
			uint			layoutIdx	= UMax;
		};

		using DrawableArray_t	= Array< DrawableInfo >;		// TODO: MutableArrayView<>
		using ControllerArray_t	= Array< ControllerInfo >;
		using ActionMap_t		= IController::ActionMap;

		static constexpr int	_MaxStencilRef = MaxValue<ubyte>();


	// variables
	private:
		bool					_requireUpdate	= false;
		LayoutData_t			_layoutData;
		DrawableArray_t			_drawables;
		ControllerArray_t		_controllers;

		RC<ILayout>				_root;
		Allocator_t &			_allocator;
		Ptr<TempAllocator_t>	_tempAllocator;			// valid before 'Initialize()'
		ActionMap_t const*		_actionMap		= null;


	// methods
	private:
		Widget (Allocator_t &alloc, TempAllocator_t &tempAlloc)										__NE___ :
			_allocator{alloc}, _tempAllocator{&tempAlloc} {}

	public:
		~Widget ()																					__NE_OV;

		ND_ bool  Initialize (RC<ILayout> root)														__NE___;
			void  Update (const float2 &surfSize, float mmToPx, const IController::InputState &)	__NE___;
			void  PreDraw (const PreDrawParams &params, TransferContext_t &ctx)						__Th___;
			void  DrawPass1 (const Draw1Params &params, Canvas &canvas, DrawContext_t &ctx)			__Th___;	// write to stencil
			void  DrawPass2 (const Draw2Params &params, Canvas &canvas, DrawContext_t &ctx)			__Th___;	// draw content with stencil test
			void  DbgDrawLayouts (Canvas &canvas)													__NE___;

			void  SetActionBindings (const ActionMap_t &actions)									__NE___;

		ND_ ILayout*			GetRoot ()															__NE___	{ return _root.get(); }

		template <typename T, typename ...Types>
		ND_ RC<T>				Create (Types&& ...args)											__NE___;

		template <typename T, typename ...Types>
		ND_ RC<T>				CreateTemp (Types&& ...args)										__NE___;

		ND_ RectF const			GlobalRect ()														C_NE___;
		ND_ bool				IsOpaque ()															C_NE___	{ return true; }	// TODO

		ND_ Allocator_t&		GetAllocator ()														__NE___	{ return _allocator; }
		ND_ TempAllocator_t*	GetTempAllocator ()													__NE___	{ return _tempAllocator.get(); }

		// ISerializable //
		bool  Serialize (Serializing::Serializer &)													C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &)												__NE_OV;

		ND_ static RC<Widget>  New (Allocator_t &alloc, TempAllocator_t &tempAlloc)					__NE___;

	private:
		template <typename T, typename Alloc, typename ...Types>
		ND_ RC<T>  _Create (Alloc &alloc, Types&& ...args)											__NE___;

		ND_ static void*  operator new (usize, void* where)											__NE___	{ return where; }
		//	static void   operator delete (void*, void*)											__NE___	{}
	};


/*
=================================================
	Create
=================================================
*/
	template <typename T, typename Alloc, typename ...Types>
	RC<T>  Widget::_Create (Alloc &alloc, Types&& ...args) __NE___
	{
		StaticAssert(	(IsBaseOf< ILayout, T >)	or
						(IsBaseOf< IDrawable, T >)	or
						(IsBaseOf< IController, T >));

		void*	ptr = alloc.Allocate( SizeAndAlignOf<T> );
		CHECK_ERR( ptr != null );

		PlacementNew<T>( OUT ptr, &alloc, FwdArg<Types>( args )... );
		return RC<T>{ Cast<T>(ptr) };
	}

	template <typename T, typename ...Types>
	RC<T>  Widget::Create (Types&& ...args) __NE___
	{
		return _Create<T>( _allocator, FwdArg<Types>( args )... );
	}

	template <typename T, typename ...Types>
	RC<T>  Widget::CreateTemp (Types&& ...args) __NE___
	{
		CHECK_ERR( _tempAllocator );
		return _Create<T>( *_tempAllocator, FwdArg<Types>( args )... );
	}

/*
=================================================
	GlobalRect
=================================================
*/
	inline RectF const  Widget::GlobalRect () C_NE___
	{
		return _layoutData.State(0).GlobalRect();
	}


} // AE::UI
