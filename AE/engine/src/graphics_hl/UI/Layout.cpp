// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/UI/Layout.h"
#include "graphics_hl/UI/Widget.h"

namespace AE::UI
{
    using namespace AE::Serializing;

    AE_BIT_OPERATORS( ILayout::_EFlags );

/*
=================================================
    constructor
=================================================
*/
    ILayout::LayoutState::LayoutState (const RectF &localRect, EStyleState style) __NE___
    {
        _local      = localRect;
        _globalPos  = localRect.LeftTop();
        _style      = style;
    }

/*
=================================================
    Update
=================================================
*/
    void  ILayout::LayoutState::Update (const LayoutState &parentState, const RectF &localRect) __NE___
    {
        // inherit some states from parent
        _style      |= (parentState.StyleFlags() & EStyleState::_Inherited);

        _local      = localRect;
        _globalPos  = _local.LeftTop() + parentState.GlobalRect().LeftTop();
    }

/*
=================================================
    UpdateAndFitParent
=================================================
*/
    void  ILayout::LayoutState::UpdateAndFitParent (const LayoutState &parentState, RectF localRect) __NE___
    {
        const float2    off = parentState.LocalRect().LeftTop();

        localRect = Crop( parentState.LocalRect(), localRect + off ) - off;

        return Update( parentState, localRect );
    }

/*
=================================================
    UpdateAndFillParent
=================================================
*/
    void  ILayout::LayoutState::UpdateAndFillParent (const LayoutState &parentState) __NE___
    {
        return Update( parentState, RectF{parentState.LocalRect().Size()} );
    }

/*
=================================================
    Resize
=================================================
*/
    void  ILayout::LayoutState::Resize (const RectF &localRect) __NE___
    {
        const float2    global_off  = _globalPos - _local.LeftTop();

        _local      = localRect;
        _globalPos  = _local.LeftTop() + global_off;
    }

/*
=================================================
    ResizeAndFitPrevious
=================================================
*/
    void  ILayout::LayoutState::ResizeAndFitPrevious (RectF localRect) __NE___
    {
        const float2    global_off  = _globalPos - _local.LeftTop();

        _local      = Crop( _local, localRect );
        _globalPos  = _local.LeftTop() + global_off;
    }

/*
=================================================
    SetStyle
=================================================
*/
    void  ILayout::LayoutState::SetStyle (EStyleState value) __NE___
    {
        _style = value;
    }
//-----------------------------------------------------------------------------



/*
=================================================
    LayoutData::Set
=================================================
*/
    void  ILayout::LayoutData::Set (void* ptr,
                                    void* dataBegin, void* dataEnd,
                                    RectF* clipRects,
                                    EType* types,
                                    LayoutState* states,
                                    Index_t* parentIdx,
                                    usize count) __NE___
    {
        _ptr        = ptr;
        _dataBegin  = Offset_t( (Bytes{dataBegin}   - Bytes{ptr}) / BaseAlign );
        _dataEnd    = Offset_t( (Bytes{dataEnd}     - Bytes{ptr} + BaseAlign - 1) / BaseAlign );
        _clipRects  = Offset_t( (Bytes{clipRects}   - Bytes{ptr}) / BaseAlign );
        _types      = Offset_t( (Bytes{types}       - Bytes{ptr}) / BaseAlign );
        _states     = Offset_t( (Bytes{states}      - Bytes{ptr}) / BaseAlign );
        _parentIdx  = Offset_t( (Bytes{parentIdx}   - Bytes{ptr}) / BaseAlign );
        _count      = Index_t(count);

        ASSERT( _count == count );
        ASSERT( DataBegin() == dataBegin );
        ASSERT( DataEnd()   >= dataEnd );
        ASSERT( ClipRects() == clipRects );
        ASSERT( Types()     == types );
        ASSERT( States()    == states );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    LayoutStateAccess
=================================================
*/
    void  ILayout::LayoutStateAccess::Set (ILayout &self, const LayoutState* state) __NE___ {
        self._statePtr = state;
    }

    ILayout::LayoutState const*  ILayout::LayoutStateAccess::Get (ILayout &self) __NE___ {
        return self._statePtr.get();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    Serialize1
=================================================
*/
    bool  ILayout::Serialize1 (Serializing::Serializer &ser) C_NE___
    {
        const _EFlags   flags   = (_drawable        ? _EFlags::Drawable     : Default) |
                                  (_controller      ? _EFlags::Controller   : Default) |
                                  (_childs.empty()  ? Default               : _EFlags::Childs);
        const EType     type    = GetType();

        CHECK_ERR( ser( _GetLayoutID(type), type, flags ));

        if ( _drawable )
            CHECK_ERR( _drawable->Serialize( ser ));

        if ( _controller )
            CHECK_ERR( _controller->Serialize( ser ));

        return true;
    }

/*
=================================================
    Deserialize1
=================================================
*/
    bool  ILayout::Deserialize1 (Deserializer &des) __NE___
    {
        _EFlags flags;
        EType   type;

        CHECK_ERR( des( OUT type, OUT flags ) and type == GetType() );

        if ( AllBits( flags, _EFlags::Drawable ))
            CHECK_ERR( des( OUT _drawable ));

        if ( AllBits( flags, _EFlags::Controller ))
            CHECK_ERR( des( OUT _controller ));

        return true;
    }

/*
=================================================
    _Default_PreInit
=================================================
*/
    bool  ILayout::_Default_PreInit (const PreInitParams &p) C_NE___
    {
        for (auto& c : GetChilds())
        {
            p.queue.emplace_back( c.get(), this );
        }
        return true;
    }

    template <typename DataType>
    bool  ILayout::_Default_PreInit (const PreInitParams &p) C_NE___
    {
        Unused( p.mem.Reserve( SizeAndAlignOf<DataType> ));

        return _Default_PreInit( p );
    }

/*
=================================================
    _Default_Init
=================================================
*/
    bool  ILayout::_Default_Init (const InitParams &) __NE___
    {
        _maxChilds = 0; // disable 'AddChild()'

        return true;
    }

    template <typename DataType>
    bool  ILayout::_Default_Init (const InitParams &p, OUT Ptr<DataType> &outPtr) __NE___
    {
        _maxChilds = 0; // disable 'AddChild()'

        void*   ptr = p.mem.Reserve( SizeAndAlignOf<DataType> );

        PlacementNew<DataType>( OUT ptr );

        outPtr = Cast<DataType>(ptr);
        return true;
    }
//-----------------------------------------------------------------------------



    template class FixedLayoutTmpl< ILayout::EType::FixedLayoutPx >;
    template class FixedLayoutTmpl< ILayout::EType::FixedLayoutMm >;

/*
=================================================
    PreInit / Init
=================================================
*/
    template <ILayout::EType FLType>
    bool  FixedLayoutTmpl<FLType>::PreInit (const PreInitParams &p) C_NE___
    {
        return _Default_PreInit<Data>( p );
    }

    template <ILayout::EType FLType>
    bool  FixedLayoutTmpl<FLType>::Init (const InitParams &p) __NE___
    {
        return _Default_Init<Data>( p, OUT _data );
    }

/*
=================================================
    Serialize2 / Deserialize2
=================================================
*/
    template <ILayout::EType FLType>
    bool  FixedLayoutTmpl<FLType>::Serialize2 (Serializer &ser) C_NE___
    {
        CHECK_ERR( _data );
        return  ser( _data->region );
    }

    template <ILayout::EType FLType>
    bool  FixedLayoutTmpl<FLType>::Deserialize2 (Deserializer &des) __NE___
    {
        CHECK_ERR( _data );
        return  des( OUT _data->region );
    }
//-----------------------------------------------------------------------------



    template class PaddingLayoutTmpl< ILayout::EType::PaddingLayoutPx >;
    template class PaddingLayoutTmpl< ILayout::EType::PaddingLayoutMm >;
    template class PaddingLayoutTmpl< ILayout::EType::PaddingLayoutRel >;

/*
=================================================
    PreInit / Init
=================================================
*/
    template <ILayout::EType PLType>
    bool  PaddingLayoutTmpl<PLType>::PreInit (const PreInitParams &p) C_NE___
    {
        return _Default_PreInit<Data>( p );
    }

    template <ILayout::EType PLType>
    bool  PaddingLayoutTmpl<PLType>::Init (const InitParams &p) __NE___
    {
        return _Default_Init<Data>( p, OUT _data );
    }

/*
=================================================
    Serialize2 / Deserialize2
=================================================
*/
    template <ILayout::EType PLType>
    bool  PaddingLayoutTmpl<PLType>::Serialize2 (Serializer &ser) C_NE___
    {
        CHECK_ERR( _data );
        return  ser( _data->x, _data->y );
    }

    template <ILayout::EType PLType>
    bool  PaddingLayoutTmpl<PLType>::Deserialize2 (Deserializer &des) __NE___
    {
        CHECK_ERR( _data );
        return  des( OUT _data->x, OUT _data->y );
    }
//-----------------------------------------------------------------------------



    template class AlignedLayoutTmpl< ILayout::EType::AlignedLayoutPx >;
    template class AlignedLayoutTmpl< ILayout::EType::AlignedLayoutMm >;
    template class AlignedLayoutTmpl< ILayout::EType::AlignedLayoutRel >;

/*
=================================================
    PreInit / Init
=================================================
*/
    template <ILayout::EType ALType>
    bool  AlignedLayoutTmpl<ALType>::PreInit (const PreInitParams &p) C_NE___
    {
        return _Default_PreInit<Data>( p );
    }

    template <ILayout::EType ALType>
    bool  AlignedLayoutTmpl<ALType>::Init (const InitParams &p) __NE___
    {
        return _Default_Init<Data>( p, OUT _data );
    }

/*
=================================================
    Serialize2 / Deserialize2
=================================================
*/
    template <ILayout::EType ALType>
    bool  AlignedLayoutTmpl<ALType>::Serialize2 (Serializer &ser) C_NE___
    {
        CHECK_ERR( _data );
        return  ser( _data->size, _data->align );
    }

    template <ILayout::EType ALType>
    bool  AlignedLayoutTmpl<ALType>::Deserialize2 (Deserializer &des) __NE___
    {
        CHECK_ERR( _data );
        return  des( OUT _data->size, OUT _data->align );
    }
//-----------------------------------------------------------------------------



    //
    // Fill Stack Cell Layout
    //
    class FillStackLayout::FSL_CellLayout final : public ILayout
    {
        friend class FillStackLayout;

    public:
        struct Data
        {
            float2          range       {1.f, 2.f};
            EStackOrigin    origin      = Default;
            // TODO: padding

            Data () __NE___ {}
        };

    private:
        Ptr<Data>       _data;      // valid after 'Init()'

    public:
        explicit FSL_CellLayout (Ptr<IAllocator> alloc) __NE___ : ILayout{ alloc } {}

        // ILayout //
        bool    PreInit (const PreInitParams &)         C_NE_OV;
        bool    Init (const InitParams &)               __NE_OV;
        EType   GetType ()                              C_NE_OV { return EType::FillStackLayout_Cell; }

        bool    Serialize2 (Serializer &)               C_NE_OV { return true; }
        bool    Deserialize2 (Deserializer &)           __NE_OV { return true; }
    };

/*
=================================================
    PreInit / Init
=================================================
*/
    bool  FillStackLayout::FSL_CellLayout::PreInit (const PreInitParams &p) C_NE___
    {
        return _Default_PreInit<Data>( p );
    }

    bool  FillStackLayout::FSL_CellLayout::Init (const InitParams &p) __NE___
    {
        return _Default_Init<Data>( p, OUT _data );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    AddChild
=================================================
*/
    void  FillStackLayout::AddChild (ChildPtr child) __NE___
    {
        CHECK_ERRV( child );
        CHECK_ERRV( _childs.size() < MaxChilds() );

        if ( child->GetType() != EType::FillStackLayout_Cell )
        {
            auto*   ptr = _allocator->Allocate< FSL_CellLayout >(); // TODO: choose allocator
            CHECK_ERRV( ptr != null );

            RC<FSL_CellLayout>  cell {PlacementNew<FSL_CellLayout>( OUT ptr, _allocator )};

            cell->AddChild( RVRef(child) );

            _childs.push_back( RVRef(cell) );
        }
        else
        {
            _childs.push_back( RVRef(child) );
        }

        if ( _data )
            _data->arranged = false;
    }

/*
=================================================
    PreInit / Init
=================================================
*/
    bool  FillStackLayout::PreInit (const PreInitParams &p) C_NE___
    {
        CHECK_ERR( _Default_PreInit<Data>( p ));

        p.mem.AlignTo( AlignOf<FSL_CellLayout::Data> );
        return true;
    }

    bool  FillStackLayout::Init (const InitParams &p) __NE___
    {
        CHECK_ERR( _Default_Init<Data>( p, OUT _data ));

        _data->offset       = Bytes{p.mem.Reserve( 0_b, AlignOf<FSL_CellLayout::Data> )} - Bytes{_data};
        _data->childCount   = ushort(_childs.size());

        return true;
    }

/*
=================================================
    SetOrigin
=================================================
*
    void  FillStackLayout::SetOrigin (EStackOrigin value) __NE___
    {
        CHECK( not _childs.empty() );

        _origin = value;

        usize   count = 0;

        for (auto& c : _childs)
        {
            auto*   cell = Cast<FSL_CellLayout>( c.get() );
            ASSERT( CastAllowed<FSL_CellLayout>( c.get() ));

            // if initialized
            if ( cell->_data != null )
            {
                count += usize( not AllBits( cell->StyleFlags(), EStyleState::Invisible ));
            }
            else
                ++count;
        }

        if ( count > 0 )
        {
            const float step    = 1.f / float(count);
            usize       idx     = 0;

            for (auto& c : _childs)
            {
                auto*   cell = Cast<FSL_CellLayout>( c.get() );
                auto&   tmp  = cell->_temp;

                tmp.origin  = value;
                tmp.range.x = step * idx;
                tmp.range.y = step * (idx+1);

                if ( cell->_data != null )
                {
                    *cell->_data = tmp;

                    if_unlikely( AllBits( cell->StyleFlags(), EStyleState::Invisible ))
                        continue;
                }

                ++idx;
            }

            _arranged = true;
        }
    }

/*
=================================================
    Serialize2 / Deserialize2
=================================================
*/
    bool  FillStackLayout::Serialize2 (Serializer &ser) C_NE___
    {
        CHECK_ERR( _data );
        return  ser( _data->origin );
    }

    bool  FillStackLayout::Deserialize2 (Deserializer &des) __NE___
    {
        CHECK_ERR( _data );
        return  des( OUT _data->origin );
    }
//-----------------------------------------------------------------------------



namespace
{
    using LayoutState_t = ILayout::LayoutState;

/*
=================================================
    CastData
=================================================
*/
    template <typename LayoutType>
    ND_ static auto&  CastData (INOUT void* &data) __NE___
    {
        using T = typename LayoutType::Data;

                data = AlignUp( data, AlignOf<T> );
        auto*   ptr = Cast< typename LayoutType::Data >( data );

        data += SizeOf< typename LayoutType::Data >;
        return *ptr;
    }

/*
=================================================
    Update_FixedLayout*
=================================================
*/
    static void  Update_FixedLayoutPx (INOUT void* &data, const LayoutState_t &parentState, const float, INOUT LayoutState_t &state) __NE___
    {
        const auto& region = CastData<FixedLayoutPx>(data).region;

        state.Update( parentState, region );
    }

    static void  Update_FixedLayoutMm (INOUT void* &data, const LayoutState_t &parentState, const float mmToPx, INOUT LayoutState_t &state) __NE___
    {
        const auto  region = CastData<FixedLayoutMm>(data).region * mmToPx;

        state.Update( parentState, region );
    }

/*
=================================================
    Update_PaddingLayout*
=================================================
*/
    static void  Update_PaddingLayoutPx (INOUT void* &data, const LayoutState_t &parentState, const float, INOUT LayoutState_t &state) __NE___
    {
        const auto& padding = CastData<PaddingLayoutPx>(data);

        RectF   region{ parentState.LocalRect().Size() };
        region.left     += padding.x[0];
        region.right    -= padding.x[1];
        region.top      += padding.y[0];
        region.bottom   -= padding.y[1];

        // if not enough space for child with padding
        if_unlikely( Any( region.RightBottom() < region.LeftTop() ))
        {
            region.RightBottom( region.LeftTop() );
        }

        state.Update( parentState, region );
    }

    static void  Update_PaddingLayoutMm (INOUT void* &data, const LayoutState_t &parentState, const float mmToPx, INOUT LayoutState_t &state) __NE___
    {
        const auto& padding = CastData<PaddingLayoutMm>(data);

        RectF   region{ parentState.LocalRect().Size() };
        region.left     += padding.x[0] * mmToPx;
        region.right    -= padding.x[1] * mmToPx;
        region.top      += padding.y[0] * mmToPx;
        region.bottom   -= padding.y[1] * mmToPx;

        // if not enough space for child with padding
        if_unlikely( Any( region.RightBottom() < region.LeftTop() ))
        {
            region.RightBottom( region.LeftTop() );
        }

        state.Update( parentState, region );
    }

    static void  Update_PaddingLayoutRel (INOUT void* &data, const LayoutState_t &parentState, const float, INOUT LayoutState_t &state) __NE___
    {
        const auto& padding = CastData<PaddingLayoutRel>(data);

        const float2    size = parentState.LocalRect().Size();
        RectF           region{ size };
        region.left     += padding.x[0] * size.x;
        region.right    -= padding.x[1] * size.x;
        region.top      += padding.y[0] * size.y;
        region.bottom   -= padding.y[1] * size.y;

        // if not enough space for child with padding
        if_unlikely( Any( region.RightBottom() < region.LeftTop() ))
        {
            region.RightBottom( region.LeftTop() );
        }

        state.Update( parentState, region );
    }

/*
=================================================
    Update_AlignedLayout*
=================================================
*/
    static void  AlignedLayoutImpl (const float2 &size, ELayoutAlign align, const LayoutState_t &parentState, INOUT LayoutState_t &state) __NE___
    {
        RectF const parent_region { parentState.LocalRect().Size() };
        RectF       region;

        // horizontal alignment
        if ( AllBits( align, ELayoutAlign::CenterX ))
        {
            region.left     = parent_region.Center().x - size.x * 0.5f;
            region.right    = region.left + size.x;
        }
        else
        if ( AllBits( align, ELayoutAlign::Left | ELayoutAlign::Right ))
        {
            region.left     = parent_region.left;
            region.right    = parent_region.right;
        }
        else
        if ( AllBits( align, ELayoutAlign::Left ))
        {
            region.left     = parent_region.left;
            region.right    = region.left + size.x;
        }
        else
        if ( AllBits( align, ELayoutAlign::Right ))
        {
            region.right    = parent_region.right;
            region.left     = region.right - size.x;
        }
        else
        {
            // alignment is undefined
            region.left     = 0.0f;
            region.right    = 0.0f;
        }


        // vertical alignment
        if ( AllBits( align, ELayoutAlign::CenterY ))
        {
            region.top      = parent_region.Center().y - size.y * 0.5f;
            region.bottom   = region.top + size.y;
        }
        else
        if ( AllBits( align, ELayoutAlign::Bottom | ELayoutAlign::Top ))
        {
            region.top      = parent_region.top;
            region.bottom   = parent_region.bottom;
        }
        else
        if ( AllBits( align, ELayoutAlign::Bottom ))
        {
            region.top      = parent_region.top;
            region.bottom   = region.top + size.y;
        }
        else
        if ( AllBits( align, ELayoutAlign::Top ))
        {
            region.bottom   = parent_region.bottom;
            region.top      = region.bottom - size.y;
        }
        else
        {
            // alignment is undefined
            region.bottom   = 0.0f;
            region.top      = 0.0f;
        }

        region = Crop( region, parent_region );

        state.Update( parentState, region );
    }

    static void  Update_AlignedLayoutPx (INOUT void* &data, const LayoutState_t &parentState, const float, INOUT LayoutState_t &state) __NE___
    {
        const auto& info = CastData<AlignedLayoutPx>(data);

        AlignedLayoutImpl( info.size, info.align, parentState, INOUT state );
    }

    static void  Update_AlignedLayoutMm (INOUT void* &data, const LayoutState_t &parentState, const float mmToPx, INOUT LayoutState_t &state) __NE___
    {
        const auto& info = CastData<AlignedLayoutMm>(data);

        AlignedLayoutImpl( info.size * mmToPx, info.align, parentState, INOUT state );
    }

    static void  Update_AlignedLayoutRel (INOUT void* &data, const LayoutState_t &parentState, const float, INOUT LayoutState_t &state) __NE___
    {
        const auto& info = CastData<AlignedLayoutRel>(data);

        AlignedLayoutImpl( info.size * parentState.LocalRect().Size(), info.align, parentState, INOUT state );
    }

/*
=================================================
    Update_StackLayout*
=================================================
*/
    static void  Update_StackLayoutL (INOUT void* &data, const LayoutState_t &parentState, const float, INOUT LayoutState_t &state) __NE___
    {
        const auto& info = CastData<StackLayout>(data);
        Unused( info, parentState, state );
    }

    static void  Update_StackLayoutR (INOUT void* &data, const LayoutState_t &parentState, const float, INOUT LayoutState_t &state) __NE___
    {
        const auto& info = CastData<StackLayout>(data);
        Unused( info, parentState, state );
    }

    static void  Update_StackLayoutB (INOUT void* &data, const LayoutState_t &parentState, const float, INOUT LayoutState_t &state) __NE___
    {
        const auto& info = CastData<StackLayout>(data);
        Unused( info, parentState, state );
    }

    static void  Update_StackLayoutT (INOUT void* &data, const LayoutState_t &parentState, const float, INOUT LayoutState_t &state) __NE___
    {
        const auto& info = CastData<StackLayout>(data);
        Unused( info, parentState, state );
    }

/*
=================================================
    Update_FillStackLayout*
=================================================
*/
    static void  Update_FillStackLayout (INOUT void* &data, const LayoutState_t &parentState, const float, INOUT LayoutState_t &state) __NE___
    {
        auto&   info = CastData<FillStackLayout>(data);

        state.UpdateAndFillParent( parentState );

        if_unlikely( not info.arranged )
        {
            info.arranged = true;

            auto    child_data      = MutableArrayView{ Cast< FillStackLayout::FSL_CellLayout::Data >( &info + info.offset ), info.childCount };
            auto    child_state     = ArrayView{ &state + 1, info.childCount };
            usize   active_count    = 0;

            for (auto& cs : child_state)
            {
                active_count += usize{ not AllBits( cs.StyleFlags(), EStyleState::Invisible )};
            }

            if ( active_count == 0 )
                return;

            const float step    = 1.f / float(active_count);
            usize       idx     = 0;

            for (usize i : IndicesOnly( child_data ))
            {
                auto&   cd  = child_data[i];
                auto&   cs  = child_state[i];

                cd.origin   = info.origin;
                cd.range.x  = step * idx;
                cd.range.y  = step * (idx+1);

                idx += usize{ not AllBits( cs.StyleFlags(), EStyleState::Invisible )};
            }
            ASSERT( active_count == idx );
        }
    }

    static void  Update_FillStackLayout_Cell (INOUT void* &data, const LayoutState_t &parentState, const float, INOUT LayoutState_t &state) __NE___
    {
        const auto&     info        = CastData<FillStackLayout::FSL_CellLayout>(data);
        float2 const    parent_size = parentState.LocalRect().Size();
        RectF           region;

        if ( AnyEqual( info.origin, EStackOrigin::Left, EStackOrigin::Right ))
        {
            bool const  invert  = (info.origin == EStackOrigin::Right);

            region.left     = parent_size.x * (invert ? 1.0f - info.range[1] : info.range[0]);
            region.top      = 0.f;
            region.right    = parent_size.x * (invert ? 1.0f - info.range[0] : info.range[1]);
            region.bottom   = parent_size.y;
        }
        else
        {
            bool const  invert  = (info.origin == EStackOrigin::Top);

            region.left     = 0.f;
            region.top      = parent_size.y * (invert ? 1.0f - info.range[1] : info.range[0]);
            region.right    = parent_size.x;
            region.bottom   = parent_size.y * (invert ? 1.0f - info.range[0] : info.range[1]);
        }

        state.Update( parentState, region );
    }

/*
=================================================
    s_LayoutUpdateFns
=================================================
*/
    static constexpr ILayout::LayoutUpdateFn_t  s_LayoutUpdateFns [] = {
        null,
        // FixedLayout
        &Update_FixedLayoutPx,
        &Update_FixedLayoutMm,
        // PaddingLayout
        &Update_PaddingLayoutPx,
        &Update_PaddingLayoutMm,
        &Update_PaddingLayoutRel,
        // AlignedLayout
        &Update_AlignedLayoutPx,
        &Update_AlignedLayoutMm,
        &Update_AlignedLayoutRel,
        // StackLayout
        &Update_StackLayoutL,
        &Update_StackLayoutR,
        &Update_StackLayoutB,
        &Update_StackLayoutT,
        // FillStackLayout
        &Update_FillStackLayout,
        &Update_FillStackLayout_Cell,
        null
    };

    StaticAssert( CountOf(s_LayoutUpdateFns) == usize(ILayout::EType::_Count) );
    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::FixedLayoutPx)] == &Update_FixedLayoutPx );
    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::FixedLayoutMm)] == &Update_FixedLayoutMm );

    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::PaddingLayoutPx)] == &Update_PaddingLayoutPx );
    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::PaddingLayoutMm)] == &Update_PaddingLayoutMm );
    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::PaddingLayoutRel)] == &Update_PaddingLayoutRel );

    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::AlignedLayoutPx)] == &Update_AlignedLayoutPx );
    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::AlignedLayoutMm)] == &Update_AlignedLayoutMm );
    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::AlignedLayoutRel)] == &Update_AlignedLayoutRel );

    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::StackLayoutL)] == &Update_StackLayoutL );
    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::StackLayoutR)] == &Update_StackLayoutR );
    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::StackLayoutB)] == &Update_StackLayoutB );
    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::StackLayoutT)] == &Update_StackLayoutT );

    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::FillStackLayout)] == &Update_FillStackLayout );
    StaticAssert( s_LayoutUpdateFns[usize(ILayout::EType::FillStackLayout_Cell)] == &Update_FillStackLayout_Cell );

/*
=================================================
    s_LayoutIDs
=================================================
*/
    static constexpr SerializedID   s_LayoutIDs [] = {
        Default,
        // FixedLayout
        SerializedID{"FixedLayoutPx"},
        SerializedID{"FixedLayoutMm"},
        // PaddingLayout
        SerializedID{"PaddingLayoutPx"},
        SerializedID{"PaddingLayoutMm"},
        SerializedID{"PaddingLayoutRel"},
        // AlignedLayout
        SerializedID{"AlignedLayoutPx"},
        SerializedID{"AlignedLayoutMm"},
        SerializedID{"AlignedLayoutRel"},
        // StackLayout
        SerializedID{"StackLayoutL"},
        SerializedID{"StackLayoutR"},
        SerializedID{"StackLayoutB"},
        SerializedID{"StackLayoutT"},
        // FillStackLayout
        SerializedID{"FillStackLayout"},
        SerializedID{"FillStackLayout_Cell"},
        Default
    };
    StaticAssert( CountOf(s_LayoutIDs) == usize(ILayout::EType::_Count) );

} // namespace
//-----------------------------------------------------------------------------


/*
=================================================
    CallUpdateFn
=================================================
*/
    void  ILayout::CallUpdateFn (EType type, INOUT void* &data, const LayoutState &parentState, const float mmToPx, INOUT LayoutState &state) __NE___
    {
        return s_LayoutUpdateFns[usize(type)]( INOUT data, parentState, mmToPx, INOUT state );
    }

/*
=================================================
    _GetLayoutID
=================================================
*/
    SerializedID  ILayout::_GetLayoutID (EType type) __NE___
    {
        return s_LayoutIDs[ usize(type) ];
    }

/*
=================================================
    LayoutSerializer
=================================================
*/
namespace
{
    template <typename T>
    struct LayoutSerializer
    {
        static bool  Serialize (Serializer &, const void*) __NE___ { return false; }

        static bool  Deserialize (Deserializer &des, INOUT void* &ptr, Ptr<IAllocator> alloc) __NE___
        {
            CHECK_ERR( ptr == null and alloc != null );

            ptr = alloc->Allocate<T>();
            CHECK_ERR( ptr != null );

            return PlacementNew<T>( OUT ptr, alloc )->Deserialize1( des );
        }
    };
}
/*
=================================================
    RegisterLayouts
=================================================
*/
    bool  ILayout::RegisterLayouts (ObjectFactory &factory) __NE___
    {
        switch_enum( EType::Unknown )
        {
            case EType::Unknown :
            case EType::StackLayoutL :
            case EType::StackLayoutR :
            case EType::StackLayoutB :
            case EType::StackLayoutT :
            case EType::_Begin_AutoSize :
            case EType::_Count :

            #define REG( _name_ )                                               \
                case EType::_name_ :                                            \
                    CHECK_ERR( factory.Register< _name_ >(                      \
                                    _GetLayoutID( EType::_name_ ),              \
                                    &LayoutSerializer<_name_>::Serialize,       \
                                    &LayoutSerializer<_name_>::Deserialize ));
            // FixedLayout
            REG( FixedLayoutPx )
            REG( FixedLayoutMm )
            // PaddingLayout
            REG( PaddingLayoutPx )
            REG( PaddingLayoutMm )
            REG( PaddingLayoutRel )
            // AlignedLayout
            REG( AlignedLayoutPx )
            REG( AlignedLayoutMm )
            REG( AlignedLayoutRel )
            // FillStackLayout
            REG( FillStackLayout )
            // StackLayout
            #undef REG

            case EType::FillStackLayout_Cell :
                CHECK_ERR( factory.Register< FillStackLayout::FSL_CellLayout >(
                                _GetLayoutID( EType::FillStackLayout_Cell ),
                                &LayoutSerializer< FillStackLayout::FSL_CellLayout >::Serialize,
                                &LayoutSerializer< FillStackLayout::FSL_CellLayout >::Deserialize ));

            default : break;
        }
        switch_end
        return true;
    }

} // AE::UI
