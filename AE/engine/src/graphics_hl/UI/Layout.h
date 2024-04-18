// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	UI Layouts with data oriented design (DOD).
	- Layout data stored sequentially for cache friendly access.
	- Layout updated using pointer to static function - it is faster than virtual function call (2 cache misses vs 1).
*/

#pragma once

#include "graphics_hl/UI/LayoutEnums.h"

namespace AE::UI
{

	//
	// Layout interface
	//
	class ILayout :
		public EnableRC<ILayout>,
		public NonAllocatable
	{
	// types
	public:
		struct LayoutState;
		struct LayoutData;

		enum class EType : ubyte
		{
			Unknown			= 0,

			// FixedLayout
			FixedLayoutPx,
			FixedLayoutMm,

			// PaddingLayout
			PaddingLayoutPx,
			PaddingLayoutMm,
			PaddingLayoutRel,

			// AlignedLayout
			AlignedLayoutPx,
			AlignedLayoutMm,
			AlignedLayoutRel,

			// StackLayout
			StackLayoutL,
			StackLayoutR,
			StackLayoutB,
			StackLayoutT,

			// FillStackLayout
			FillStackLayout,
			FillStackLayout_Cell,

			_Begin_AutoSize,
			_Count
		};

		using LayoutUpdateFn_t	= void (*) (INOUT void* &data, const LayoutState &parentState, float mmToPx, INOUT LayoutState &state);
		using LayoutQueue_t		= RingBuffer< Pair< ILayout *, ILayout const* >>;	// { child, parent }

		struct LayoutStateAccess {
			friend class Widget;
			static void					Set (ILayout &, const LayoutState *)	__NE___;
			static LayoutState const*	Get (ILayout &)							__NE___;
		};

		using ChildPtr = RC< ILayout >;

		struct PreInitParams
		{
			MemWriter			& mem;
			LayoutQueue_t		& queue;

			PreInitParams (MemWriter		&mem,
						   LayoutQueue_t	&queue) __NE___ :
				mem{mem}, queue{queue}
			{}
		};

		struct InitParams
		{
			MemWriter			& mem;
			//ILayout const		* parent	= null;

			explicit InitParams (MemWriter &mem) __NE___ : mem{mem} {}
		};

		enum class _EFlags : ubyte
		{
			Unknown		= 0,
			Drawable	= 1 << 0,
			Controller	= 1 << 1,
			Childs		= 1 << 2,
		};


	// variables
	protected:
		ubyte					_maxChilds	= 1;

		RC< IDrawable >			_drawable;
		RC< IController >		_controller;
		// TODO:
		//	- layout animation
		//	- drawable animation ?

		Array< ChildPtr >		_childs;

		Ptr<const LayoutState>	_statePtr;		// valid after 'Init()'
		Ptr<IAllocator>			_allocator;


	// interface
	public:
			virtual bool	PreInit (const PreInitParams &)							C_NE___ = 0;
			virtual bool	Init (const InitParams &)								__NE___	= 0;
		ND_ virtual EType	GetType ()												C_NE___ = 0;

		ND_ usize			MaxChilds ()											C_NE___	{ return _maxChilds == UMax ? usize{UMax} : usize{_maxChilds}; }

		ND_ IDrawable*		GetDrawable ()											C_NE___	{ return _drawable.get(); }
		ND_ IController*	GetController ()										C_NE___	{ return _controller.get(); }

		ND_ ArrayView<ChildPtr>  GetChilds ()										C_NE___	{ return _childs; }

			void  SetDrawable (RC<IDrawable> ptr)									__NE___;
			void  SetController (RC<IController> ptr)								__NE___;

			virtual void  AddChild (ChildPtr ptr)									__NE___;


		// helpers
		ND_ RectF const&	LocalRect ()											C_NE___;
		ND_ RectF const		GlobalRect ()											C_NE___;
		ND_ EStyleState		StyleFlags ()											C_NE___;


		// serializing
		ND_			bool  Serialize1 (Serializing::Serializer &)					C_NE___;
		ND_ virtual bool  Serialize2 (Serializing::Serializer &)					C_NE___ = 0;
		ND_			bool  Deserialize1 (Serializing::Deserializer &)				__NE___;
		ND_ virtual bool  Deserialize2 (Serializing::Deserializer &)				__NE___ = 0;


		// utils
			static void  CallUpdateFn (EType type, INOUT void* &data,
									   const LayoutState &parentState,
									   float mmToPx, INOUT LayoutState &state)		__NE___;
		ND_ static bool  RegisterLayouts (Serializing::ObjectFactory &)				__NE___;

		ND_ static void*  operator new (usize, void* where)							__NE___	{ return where; }
		//	static void   operator delete (void*, void*)							__NE___	{}


	protected:
		explicit ILayout (Ptr<IAllocator> alloc, ubyte maxChilds = 1)				__NE___	: _maxChilds{maxChilds}, _allocator{alloc} {}

		template <typename DataType>
		ND_ bool  _Default_PreInit (const PreInitParams &)							C_NE___;
		ND_ bool  _Default_PreInit (const PreInitParams &)							C_NE___;

		template <typename DataType>
		ND_ bool  _Default_Init (const InitParams &, OUT Ptr<DataType> &ptr)		__NE___;
		ND_ bool  _Default_Init (const InitParams &)								__NE___;

	private:
		ND_ static Serializing::SerializedID  _GetLayoutID (EType type)				__NE___;
	};



	//
	// Layout State
	//
	struct ILayout::LayoutState
	{
	// variables
	private:
		float2			_globalPos;
		RectF			_local;			// TODO: local size, pos
		EStyleState		_style		= Default;


	// methods
	public:
		LayoutState ()																__NE___	{}
		LayoutState (const RectF &localRect, EStyleState style)						__NE___;

		ND_ RectF const		GlobalRect ()											C_NE___	{ return RectF{ _local.Size() } + _globalPos; }
		ND_ RectF const&	LocalRect ()											C_NE___	{ return _local; }
		ND_ EStyleState		StyleFlags ()											C_NE___	{ return _style; }

		void  Update (const LayoutState &parentState, const RectF &localRect)		__NE___;
		void  UpdateAndFitParent (const LayoutState &parentState, RectF localRect)	__NE___;
		void  UpdateAndFillParent (const LayoutState &parentState)					__NE___;

		void  Resize (const RectF &localRect)										__NE___;
		void  ResizeAndFitPrevious (RectF localRect)								__NE___;
		void  SetStyle (EStyleState value)											__NE___;
	};



	//
	// Layout Data
	//
	struct ILayout::LayoutData
	{
	// types
	public:
		using Offset_t	= ushort;
		using Index_t	= ushort;

		static constexpr Bytes	BaseAlign {AE_CACHE_LINE};

		StaticAssert( BaseAlign >= AlignOf<RectF> );
		StaticAssert( BaseAlign >= AlignOf<EType> );
		StaticAssert( BaseAlign >= AlignOf<LayoutState> );
		StaticAssert( BaseAlign >= AlignOf<Index_t> );

	// variables
	private:
		void *		_ptr			= null;
		Offset_t	_dataBegin		= UMax;
		Offset_t	_dataEnd		= UMax;
		Offset_t	_clipRects		= UMax;
		Offset_t	_types			= UMax;
		Offset_t	_states			= UMax;
		Offset_t	_parentIdx		= UMax;
		Index_t		_count			= UMax;


	// methods
	public:
		LayoutData ()												__NE___	{}
		LayoutData (const LayoutData &)								__NE___	= default;

		ND_ void *	ReleasePtr ()									__NE___ { void* ptr = _ptr;  _ptr = null;  return ptr; }

		// getter
		ND_ void *				DataBegin ()						C_NE___	{ return _ptr + _DataBeginOffset(); }
		ND_ void const*			DataEnd ()							C_NE___	{ return _ptr + _DataEndOffset(); }
		ND_ RectF *				ClipRects ()						C_NE___	{ return Cast<RectF>( _ptr + _ClipRectsOffset() ); }
		ND_ RectF &				ClipRect (usize idx)				C_NE___	{ ASSERT( idx < _count );  return ClipRects()[ idx ]; }
		ND_ EType const*		Types ()							C_NE___	{ return Cast<EType>( _ptr + _TypesOffset() ); }
		ND_ EType				Type (usize idx)					C_NE___	{ ASSERT( idx < _count );  return Types()[ idx ]; }
		ND_ LayoutState *		States ()							C_NE___	{ return Cast<LayoutState>( _ptr + _StatesOffset() ); }
		ND_ LayoutState &		State (usize idx)					C_NE___	{ ASSERT( idx < _count );  return States()[ idx ]; }
		ND_ uint				Count ()							C_NE___	{ return _count; }

		ND_ LayoutState const&	ParentState (usize idx)				C_NE___	{ ASSERT( idx < _count );  return State( _ParentIndex( idx )); }
		ND_ RectF const&		ParentClipRect (usize idx)			C_NE___	{ ASSERT( idx < _count );  return ClipRect( _ParentIndex( idx )); }

		// setter
		void  Set  (void*			ptr,
					void*			dataBegin, void* dataEnd,
					RectF*			clipRects,
					EType*			types,
					LayoutState*	states,
					Index_t*		parentIdx,
					usize			count)							__NE___;

	private:
		ND_ usize	_ParentIndex (usize idx)						C_NE___	{ return Cast<Index_t>( _ptr + _ParentIndexOffset() )[idx]; }

		ND_ Bytes	_DataBeginOffset ()								C_NE___	{ return _dataBegin	* BaseAlign; }
		ND_ Bytes	_DataEndOffset ()								C_NE___	{ return _dataEnd	* BaseAlign; }
		ND_ Bytes	_ClipRectsOffset ()								C_NE___	{ return _clipRects	* BaseAlign; }
		ND_ Bytes	_TypesOffset ()									C_NE___	{ return _types		* BaseAlign; }
		ND_ Bytes	_StatesOffset ()								C_NE___	{ return _states	* BaseAlign; }
		ND_ Bytes	_ParentIndexOffset ()							C_NE___	{ return _parentIdx	* BaseAlign; }
	};



	//
	// Fixed Layout in pixels / mm
	//
	template <ILayout::EType FLType>
	class FixedLayoutTmpl final : public ILayout
	{
	// types
	public:
		struct Data
		{
			RectF	region;

			Data ()	__NE___	{}
		};


	// variables
	private:
		Ptr<Data>	_data;		// valid after 'Init()'


	// methods
	public:
		explicit FixedLayoutTmpl (Ptr<IAllocator> alloc)			__NE___ : ILayout{alloc} {}

		ND_ RectF const&	GetRegion ()							C_NE___	{ return _data->region; }
			void			SetRegion (const RectF &value)			__NE___	{ _data->region = value; }
			void			Move (const float2 &delta)				__NE___	{ _data->region += delta; }

		// ILayout //
		bool	PreInit (const PreInitParams &)						C_NE_OV;
		bool	Init (const InitParams &)							__NE_OV;
		EType	GetType ()											C_NE_OV	{ return FLType; }

		bool	Serialize2 (Serializing::Serializer &)				C_NE_OV;
		bool	Deserialize2 (Serializing::Deserializer &)			__NE_OV;
	};

	using FixedLayoutPx	= FixedLayoutTmpl< ILayout::EType::FixedLayoutPx >;
	using FixedLayoutMm	= FixedLayoutTmpl< ILayout::EType::FixedLayoutMm >;



	//
	// Padding Layout in pixels / mm / relative
	//
	template <ILayout::EType PLType>
	class PaddingLayoutTmpl final : public ILayout
	{
	// types
	public:
		struct Data
		{
			float2		x;
			float2		y;

			Data ()	__NE___	{}
		};


	// variables
	private:
		Ptr<Data>	_data;		// valid after 'Init()'


	// methods
	public:
		explicit PaddingLayoutTmpl (Ptr<IAllocator> alloc)			__NE___ : ILayout{alloc} {}

			void			SetPadding (float value)				__NE___	{ SetPaddingX( value, value );  SetPaddingY( value, value ); }
			void			SetPaddingX (float left, float right)	__NE___	{ _data->x = float2{left, right}; }
			void			SetPaddingY (float bottom, float top)	__NE___	{ _data->y = float2{bottom, top}; }
		ND_ float2 const&	GetPaddingX ()							C_NE___	{ return _data->x; }
		ND_ float2 const&	GetPaddingY ()							C_NE___	{ return _data->y; }

		// ILayout //
		bool	PreInit (const PreInitParams &)						C_NE_OV;
		bool	Init (const InitParams &)							__NE_OV;
		EType	GetType ()											C_NE_OV	{ return PLType; }

		bool	Serialize2 (Serializing::Serializer &)				C_NE_OV;
		bool	Deserialize2 (Serializing::Deserializer &)			__NE_OV;
	};

	using PaddingLayoutPx	= PaddingLayoutTmpl< ILayout::EType::PaddingLayoutPx >;
	using PaddingLayoutMm	= PaddingLayoutTmpl< ILayout::EType::PaddingLayoutMm >;
	using PaddingLayoutRel	= PaddingLayoutTmpl< ILayout::EType::PaddingLayoutRel >;



	//
	// Aligned Layout in pixels / mm / relative
	//
	template <ILayout::EType ALType>
	class AlignedLayoutTmpl final : public ILayout
	{
	// types
	public:
		struct Data
		{
			float2			size;
			ELayoutAlign	align	= Default;

			Data ()	__NE___	{}
		};


	// variables
	private:
		Ptr<Data>		_data;		// valid after 'Init()'


	// methods
	public:
		explicit AlignedLayoutTmpl (Ptr<IAllocator> alloc)			__NE___ : ILayout{alloc} {}

			void			SetSize (const float2 &value)			__NE___	{ _data->size  = value; }
			void			SetAlign (ELayoutAlign value)			__NE___	{ _data->align = value; }
		ND_ float2 const&	GetSize ()								C_NE___	{ return _data->size; }
		ND_ ELayoutAlign	GetAlign ()								C_NE___	{ return _data->align; }

		// ILayout //
		bool	PreInit (const PreInitParams &)						C_NE_OV;
		bool	Init (const InitParams &)							__NE_OV;
		EType	GetType () 											C_NE_OV	{ return ALType; }

		bool	Serialize2 (Serializing::Serializer &)				C_NE_OV;
		bool	Deserialize2 (Serializing::Deserializer &)			__NE_OV;
	};

	using AlignedLayoutPx	= AlignedLayoutTmpl< ILayout::EType::AlignedLayoutPx >;
	using AlignedLayoutMm	= AlignedLayoutTmpl< ILayout::EType::AlignedLayoutMm >;
	using AlignedLayoutRel	= AlignedLayoutTmpl< ILayout::EType::AlignedLayoutRel >;



	//
	// Stack Layout
	//
	class StackLayout
	{
	// types
	public:
		struct Data {};


	// variables
	private:
		EStackOrigin	_origin	= Default;
	};



	//
	// Fill Stack Layout
	//
	class FillStackLayout final : public ILayout
	{
	// types
	public:
		class FSL_CellLayout;

		struct Data
		{
			ushort			childCount	= 0;
			Bytes16u		offset;
			EStackOrigin	origin		= EStackOrigin::Left;
			bool			arranged	= false;

			Data ()	__NE___	{}
		};


	// variables
	private:
		Ptr<Data>	_data;		// valid after 'Init()'


	// methods
	public:
		explicit FillStackLayout (Ptr<IAllocator> alloc)			__NE___	: ILayout{ alloc, UMax } {}

			void			SetOrigin (EStackOrigin value)			__NE___	{ _data->origin = value; }
		ND_ EStackOrigin	GetOrigin ()							C_NE___	{ return _data->origin; }

		// ILayout //
		bool	PreInit (const PreInitParams &)						C_NE_OV;
		bool	Init (const InitParams &)							__NE_OV;
		EType	GetType ()											C_NE_OV	{ return EType::FillStackLayout; }
		void	AddChild (ChildPtr ptr)								__NE_OV;

		bool	Serialize2 (Serializing::Serializer &)				C_NE_OV;
		bool	Deserialize2 (Serializing::Deserializer &)			__NE_OV;
	};



	//
	// Scrollable Layout
	//
	class ScrollableLayout
	{
	// variables
	private:
		EDirection		_scroll		= Default;
	};

//-----------------------------------------------------------------------------



/*
=================================================
	Set***
=================================================
*/
	inline void  ILayout::SetDrawable (RC<IDrawable> ptr) __NE___
	{
		CHECK_ERRV( not _drawable );
		_drawable = RVRef(ptr);
	}

	inline void  ILayout::SetController (RC<IController> ptr) __NE___
	{
		CHECK_ERRV( not _controller );
		_controller = RVRef(ptr);
	}

	inline void  ILayout::AddChild (ChildPtr ptr) __NE___
	{
		CHECK_ERRV( ptr );
		CHECK_ERRV( _childs.size() < MaxChilds() );

		_childs.push_back( RVRef(ptr) );
	}

/*
=================================================
	helper getters
=================================================
*/
	inline RectF const&	ILayout::LocalRect ()	C_NE___	{ return _statePtr->LocalRect(); }
	inline RectF const	ILayout::GlobalRect ()	C_NE___	{ return _statePtr->GlobalRect(); }
	inline EStyleState	ILayout::StyleFlags ()	C_NE___	{ return _statePtr->StyleFlags(); }

/*
=================================================
	Crop
----
	returns rectangle intersection or point of 'other'
=================================================
*/
	ND_ inline RectF  Crop (const RectF &self, const RectF &other) __NE___
	{
		ASSERT( self.IsNormalized() and other.IsNormalized() );

		RectF	res{ Max( self.left,   other.left ),
					 Max( self.top,    other.top ),
					 Min( self.right,  other.right ),
					 Min( self.bottom, other.bottom )};

		return res.IsNormalized() ? res : RectF{other.LeftTop(), other.LeftTop()};
	}


} // AE::UI
