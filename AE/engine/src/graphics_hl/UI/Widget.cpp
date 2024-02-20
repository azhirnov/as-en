// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/UI/Widget.h"

#ifndef AE_UI_NO_GRAPHICS
# include "graphics_hl/Canvas/Primitives.h"
# include "graphics_hl/UI/System.h"
#endif

namespace AE::UI
{
    static constexpr usize  c_ExpectedLayoutCount = 128;

/*
=================================================
    New
=================================================
*/
    RC<Widget>  Widget::New (Allocator_t &alloc, TempAllocator_t &tempAlloc) __NE___
    {
        ASSERT( &alloc != &tempAlloc );

        auto*   ptr = alloc.Allocate<Widget>();
        CHECK_ERR( ptr != null );

        return RC<Widget>{ new(ptr) Widget{ alloc, tempAlloc }};
    }

/*
=================================================
    destructor
=================================================
*/
    Widget::~Widget () __NE___
    {
        // not needed for linear allocator
        if ( void* ptr = _layoutData.ReleasePtr() )
            _allocator.Deallocate( ptr );
    }

/*
=================================================
    Initialize
=================================================
*/
    bool  Widget::Initialize (RC<ILayout> root) __NE___
    {
        CHECK_ERR( not _root );
        CHECK_ERR( root );
        CHECK_ERR( root->GetType() < ILayout::EType::_Begin_AutoSize );

        _root = RVRef(root);

        // TODO: use temp allocator

        ILayout::LayoutQueue_t  sorted_views;

        // reserve space
        Bytes   req_size;
        uint    view_count          = 0;
        uint    drawable_count      = 0;
        uint    controller_count    = 0;
        {
            ILayout::LayoutQueue_t  queue;
            queue.reserve( c_ExpectedLayoutCount );
            queue.emplace_back( _root.get(), null );

            MemWriter               mem     {LayoutData_t::BaseAlign};
            ILayout::PreInitParams  params  { mem, queue };

            // _dataBegin, _dataEnd
            for (; not queue.empty();)
            {
                auto [layout, parent] = queue.ExtractFront();
                layout->PreInit( params );
                ++view_count;

                drawable_count   += uint(layout->GetDrawable() != null);
                controller_count += uint(layout->GetController() != null);

                sorted_views.emplace_back( layout, parent );
            }

            mem.AlignTo( LayoutData_t::BaseAlign );
            Unused( mem.Reserve( SizeOf<RectF>                  * view_count, LayoutData_t::BaseAlign ));   // _clipRects
            Unused( mem.Reserve( SizeOf<LayoutType_t>           * view_count, LayoutData_t::BaseAlign ));   // _types
            Unused( mem.Reserve( SizeOf<LayoutState_t>          * view_count, LayoutData_t::BaseAlign ));   // _states
            Unused( mem.Reserve( SizeOf<LayoutData_t::Index_t>  * view_count, LayoutData_t::BaseAlign ));   // _parentIdx
            mem.AlignTo( LayoutData_t::BaseAlign );

            req_size = mem.AllocatedSize();
        }
        CHECK_ERR( view_count > 0 );

        // allocate & initialize
        void*   ptr = _allocator.Allocate( SizeAndAlign{ req_size, LayoutData_t::BaseAlign });
        CHECK_ERR( ptr != null );

        MemWriter   mem2        { ptr, req_size };
        auto*       rect_ptr    = Cast<RectF>(                  mem2.Reserve( SizeOf<RectF>                 * view_count, LayoutData_t::BaseAlign ));
        auto*       type_ptr    = Cast<LayoutType_t>(           mem2.Reserve( SizeOf<LayoutType_t>          * view_count, LayoutData_t::BaseAlign ));
        auto*       state_ptr   = Cast<LayoutState_t>(          mem2.Reserve( SizeOf<LayoutState_t>         * view_count, LayoutData_t::BaseAlign ));
        auto*       parent_ptr  = Cast<LayoutData_t::Index_t>(  mem2.Reserve( SizeOf<LayoutData_t::Index_t> * view_count, LayoutData_t::BaseAlign ));
        void*       data_ptr    = mem2.Reserve( 0_b, LayoutData_t::BaseAlign );

        CHECK_ERR( mem2.AllocatedSize() < mem2.MaxSize() );

        _drawables.reserve( drawable_count );
        _controllers.reserve( controller_count );

        {
            struct ViewInfo
            {
                uint    index   = UMax;
                int     depth   = -1;
            };
            HashMap< const ILayout*, ViewInfo >     view_idx_map;
            view_idx_map.reserve( view_count );

            uint    view_idx = 0;
            view_idx_map[null] = ViewInfo{};

            ILayout::InitParams     params { mem2 };

            for (auto [layout, parent] : sorted_views)
            {
                const auto  parent_info = view_idx_map[parent];
                auto&       view_info   = view_idx_map[layout];

                layout->Init( params );
                ILayout::LayoutStateAccess::Set( *layout, &state_ptr[view_idx] );

                PlacementNew<LayoutState_t>( OUT &state_ptr[view_idx] );
                view_info = ViewInfo{ view_idx, parent_info.depth+1 };

                rect_ptr[view_idx]      = RectF{};
                type_ptr[view_idx]      = layout->GetType();
                parent_ptr[view_idx]    = LayoutData_t::Index_t( parent_info.index );

                ++view_idx;
                ASSERT( view_idx <= view_count );

                if ( auto* drawable = layout->GetDrawable() )
                {
                    CHECK( view_info.depth >= 0 and view_info.depth <= _MaxStencilRef );

                    DrawableInfo&   info = _drawables.emplace_back();
                    info.ptr        = drawable;
                    info.stencilRef = uint(view_info.depth);
                    info.layoutIdx  = view_info.index;
                }

                if ( auto* controller = layout->GetController() )
                {
                    ControllerInfo& info = _controllers.emplace_back();
                    info.ptr        = controller;
                    info.layoutIdx  = view_info.index;
                }
            }

            ASSERT( view_idx == view_count );
            ASSERT( _drawables.size() == drawable_count );
            ASSERT( _controllers.size() == controller_count );

            // TODO: reverse '_controllers'
            // TODO: sort '_drawables'
        }

        CHECK_ERR( mem2.AllocatedSize() <= mem2.MaxSize() );

        _layoutData.Set(
            mem2.Data(),
            data_ptr, mem2.Data() + mem2.AllocatedSize(),
            rect_ptr, type_ptr, state_ptr, parent_ptr,
            view_count );

        _requireUpdate = true;

        _tempAllocator->Release();
        _tempAllocator  = null;

        return true;
    }

/*
=================================================
    SetActionBindings
=================================================
*/
    void  Widget::SetActionBindings (const ActionMap_t &actions) __NE___
    {
        _actionMap = &actions;
    }

/*
=================================================
    Serialize
=================================================
*/
    bool  Widget::Serialize (Serializing::Serializer &ser) C_NE___
    {
        CHECK_ERR( ser.factory );
        CHECK_ERR( _root );

        RingBuffer< ILayout const* >    queue;
        queue.reserve( c_ExpectedLayoutCount );

        queue.push_back( _root.get() );
        CHECK_ERR( _root->Serialize1( ser ));

        for (usize i = 0; i < queue.size(); ++i)
        {
            auto*   parent = queue[i];

            CHECK_ERR( ser( ushort(parent->GetChilds().size()) ));

            for (auto& c : parent->GetChilds())
            {
                queue.push_back( c.get() );
                CHECK_ERR( c->Serialize1( ser ));
            }
        }

        for (usize i = 0; i < queue.size(); ++i)
        {
            CHECK_ERR( queue[i]->Serialize2( ser ));
        }

        return true;
    }

/*
=================================================
    Deserialize
=================================================
*/
    bool  Widget::Deserialize (Serializing::Deserializer &des) __NE___
    {
        CHECK_ERR( des.factory );
        CHECK_ERR( des.allocator == null );

        des.allocator = &_allocator;

        RC<ILayout>                 root;
        RingBuffer< RC<ILayout> >   queue;
        queue.reserve( c_ExpectedLayoutCount );

        CHECK_ERR( des( OUT root ));
        queue.push_back( root );

        for (usize i = 0; i < queue.size(); ++i)
        {
            auto*   parent = queue[i].get();

            ushort  child_count = 0;
            CHECK_ERR( des( OUT child_count ));

            for (usize j : IndicesOnly( child_count ))
            {
                Unused( j );

                RC<ILayout> child;
                CHECK_ERR( des( OUT child ));

                parent->AddChild( child );
                queue.push_back( RVRef(child) );
            }
        }

        CHECK_ERR( Initialize( RVRef(root) ));

        for (usize i = 0; i < queue.size(); ++i)
        {
            CHECK_ERR( queue[i]->Deserialize2( des ));
        }

        des.allocator = null;
        return true;
    }
//-----------------------------------------------------------------------------


#ifndef AE_UI_NO_GRAPHICS
/*
=================================================
    Update
=================================================
*/
    void  Widget::Update (const float2 &surfSize, const float mmToPx, const IController::InputState &input) __NE___
    {
        CHECK_ERRV( _actionMap != null );

        LayoutData_t const&     ldata   = _layoutData;
        const uint              cnt     = ldata.Count();

        // update controllers for previous layouts
        {
            for (auto& cont : _controllers)
            {
                IController::UpdateParams   params{ input, *_actionMap, ldata.State( cont.layoutIdx ) };

                cont.ptr->Update( params );
            }
        }

    //  if ( not _requireUpdate )
    //      return;

        _requireUpdate = false;

        // update layouts
        {
            void*   data_ptr = ldata.DataBegin();
            {
                LayoutState_t   parent_state{ RectF{surfSize}, EStyleState::Unknown };
                ILayout::CallUpdateFn( ldata.Type(0), INOUT data_ptr, parent_state, mmToPx, INOUT ldata.State(0) );
            }

            for (uint idx = 1; idx < cnt; ++idx)
            {
                // TODO: animation

                ASSERT( data_ptr < ldata.DataEnd() );
                ILayout::CallUpdateFn( ldata.Type(idx), INOUT data_ptr, ldata.ParentState(idx), mmToPx, INOUT ldata.State(idx) );
            }
        }

        // update clip rects
        {
            ldata.ClipRect(0) = ldata.State(0).GlobalRect();

            for (uint idx = 1; idx < cnt; ++idx)
            {
                ldata.ClipRect(idx) = Crop( ldata.ParentClipRect(idx), ldata.State(idx).GlobalRect() );
            }
        }
    }

/*
=================================================
    PreDraw
=================================================
*/
    void  Widget::PreDraw (const PreDrawParams &params, TransferContext_t &ctx) __Th___
    {
        for (auto& drawable : _drawables)
        {
            drawable.ptr->PreDraw( params, ctx );
        }
    }

/*
=================================================
    DrawPass1
=================================================
*/
    void  Widget::DrawPass1 (const Draw1Params &params, Canvas &canvas, DrawContext_t &ctx) __Th___
    {
        Unused( params, canvas, ctx );
        // TODO
    }

/*
=================================================
    DrawPass2
=================================================
*/
    void  Widget::DrawPass2 (const Draw2Params &inParams, Canvas &canvas, DrawContext_t &ctx) __Th___
    {
        LayoutData_t const&     ldata   = _layoutData;
        IDrawable::DrawParams   params;

        params.mtr  = inParams.mtr;
        params.dt   = inParams.dt;

        for (auto& drawable : _drawables)
        {
            params.mtr->stencilRef  = drawable.stencilRef + inParams.baseStencilRef;
            params.clipRect         = ldata.ClipRect( drawable.layoutIdx );
            params.globalRect       = ldata.State( drawable.layoutIdx ).GlobalRect();
            params.style            = ldata.State( drawable.layoutIdx ).StyleFlags();

            ASSERT( params.mtr->stencilRef <= _MaxStencilRef );

            drawable.ptr->Draw( params, canvas, ctx );
        }

        // update material
        inParams.mtr.mtr = params.mtr.mtr;
    }

/*
=================================================
    DbgDrawLayouts
=================================================
*/
    void  Widget::DbgDrawLayouts (Canvas &canvas) __NE___
    {
        LayoutData_t const&     ldata   = _layoutData;
        const uint              cnt     = ldata.Count();

        for (uint idx = 1; idx < cnt; ++idx)
        {
            const RGBA8u    col {RainbowWrap( idx * 0.2f )};

            canvas.Draw( Graphics::Rectangle2D{ ldata.ClipRect(idx), col });
        }
    }

#endif // AE_UI_NO_GRAPHICS

} // AE::UI
