// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

namespace AE::AssetPacker
{
    using ELayoutType = UI::ILayout::EType;
    using UI::ELayoutAlign;
    using UI::EStackOrigin;


    //
    // Base Layout
    //
    class ScriptBaseLayout : public EnableScriptRC
    {
    // variables
    protected:
        const ELayoutType                       _type   = Default;

        Array< ScriptRC<ScriptBaseLayout> >     _childs;
        ScriptUIDrawablePtr                     _drawable;
        ScriptUIControllerPtr                   _controller;


    // methods
    public:
        explicit ScriptBaseLayout (ELayoutType type)                        : _type{type} {}

        static void  Bind (const ScriptEnginePtr &se)                       __Th___;

        static void  Convert (UI::Widget &,
                              const ScriptBaseLayout &root,
                              OUT UI::IController::ActionMap &actionMap)    __Th___;

    protected:
        template <typename B>
        static void  _BindBase (B &binder)                                  __Th___;

        void  _AddChild (const ScriptRC<ScriptBaseLayout> &)                __Th___;
        void  _SetDrawable (const ScriptUIDrawablePtr &)                    __Th___;
        void  _SetController (const ScriptUIControllerPtr &)                __Th___;

        virtual RC<UI::ILayout> _ConvertPass1 (UI::Widget &)                C_Th___ = 0;
        virtual void            _ConvertPass2 (UI::ILayout &)               C_Th___ = 0;
    };



    //
    // Fixed Layout
    //
    class ScriptFixedLayout final : public ScriptBaseLayout
    {
    // variables
    private:
        RectF       _region;

    // methods
    public:
        ScriptFixedLayout ()                                    __Th___ : ScriptBaseLayout{ ELayoutType::FixedLayoutPx } {}
        explicit ScriptFixedLayout (ELayoutType type)           __Th___;

        void  SetRegion1 (const RectF &value)                   __Th___;

        static void  Bind (const ScriptEnginePtr &se)           __Th___;

    private:
        RC<UI::ILayout> _ConvertPass1 (UI::Widget &)            C_Th_OV;
        void            _ConvertPass2 (UI::ILayout &)           C_Th_OV;
    };
    using ScriptFixedLayoutPtr = ScriptRC< ScriptFixedLayout >;



    //
    // Padding Layout
    //
    class ScriptPaddingLayout final : public ScriptBaseLayout
    {
    // variables
    private:
        float2      _x;
        float2      _y;

    // methods
    public:
        ScriptPaddingLayout ()                                  __Th___ : ScriptBaseLayout{ ELayoutType::PaddingLayoutPx } {}
        explicit ScriptPaddingLayout (ELayoutType type)         __Th___;

        void  SetPadding (float value)                          __Th___ { SetPaddingX( value, value );  SetPaddingY( value, value ); }
        void  SetPaddingX (float left, float right)             __Th___;
        void  SetPaddingY (float bottom, float top)             __Th___;

        static void  Bind (const ScriptEnginePtr &se)           __Th___;

    private:
        RC<UI::ILayout> _ConvertPass1 (UI::Widget &)            C_Th_OV;
        void            _ConvertPass2 (UI::ILayout &)           C_Th_OV;
    };
    using ScriptPaddingLayoutPtr = ScriptRC< ScriptPaddingLayout >;



    //
    // Aligned Layout
    //
    class ScriptAlignedLayout final : public ScriptBaseLayout
    {
    // variables
    private:
        float2          _size;
        ELayoutAlign    _align  = Default;


    // methods
    public:
        ScriptAlignedLayout ()                                  __Th___ : ScriptBaseLayout{ ELayoutType::AlignedLayoutPx } {}
        explicit ScriptAlignedLayout (ELayoutType type)         __Th___;

        void  SetSize (const packed_float2 &value)              __Th___;
        void  SetAlign (ELayoutAlign value)                     __Th___;

        static void  Bind (const ScriptEnginePtr &se)           __Th___;

    private:
        RC<UI::ILayout> _ConvertPass1 (UI::Widget &)            C_Th_OV;
        void            _ConvertPass2 (UI::ILayout &)           C_Th_OV;
    };
    using ScriptAlignedLayoutPtr = ScriptRC< ScriptAlignedLayout >;



    //
    // Fill Stack Layout
    //
    class ScriptFillStackLayout final : public ScriptBaseLayout
    {
    // variables
    private:
        EStackOrigin    _origin     = EStackOrigin::Left;

    // methods
    public:
        ScriptFillStackLayout ()                                __Th___ : ScriptBaseLayout{ ELayoutType::FillStackLayout } {}

        void  SetOrigin (EStackOrigin value)                    __Th___;

        static void  Bind (const ScriptEnginePtr &se)           __Th___;

    private:
        RC<UI::ILayout> _ConvertPass1 (UI::Widget &)            C_Th_OV;
        void            _ConvertPass2 (UI::ILayout &)           C_Th_OV;
    };
    using ScriptFillStackLayoutPtr = ScriptRC< ScriptFillStackLayout >;
//-----------------------------------------------------------------------------



/*
=================================================
    ScriptBaseLayout_ToBase
=================================================
*/
    template <typename T>
    static ScriptBaseLayout*  ScriptBaseLayout_ToBase (T* ptr)
    {
        StaticAssert( IsBaseOf< ScriptBaseLayout, T >);

        ScriptRC<ScriptBaseLayout>  result{ ptr };
        return result.Detach();
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptBaseLayout::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptBaseLayout>    binder{ se };
        binder.CreateRef( 0, False{"no ctor"} );
    }

    template <typename B>
    void  ScriptBaseLayout::_BindBase (B &binder) __Th___
    {
        using T = typename B::Class_t;
        binder.Operators().ImplCast( &ScriptBaseLayout_ToBase<T> );

        binder.AddMethod( &ScriptBaseLayout::_AddChild,         "AddChild",         {} );
        binder.AddMethod( &ScriptBaseLayout::_SetDrawable,      "SetDrawable",      {} );
        binder.AddMethod( &ScriptBaseLayout::_SetController,    "SetController",    {} );
    }

/*
=================================================
    _AddChild
=================================================
*/
    void  ScriptBaseLayout::_AddChild (const ScriptRC<ScriptBaseLayout> &child) __Th___
    {
        CHECK_THROW_MSG( child != null );
        CHECK_THROW_MSG( child.Get() != this );

        _childs.push_back( child );
    }

/*
=================================================
    _SetDrawable
=================================================
*/
    void  ScriptBaseLayout::_SetDrawable (const ScriptUIDrawablePtr &drawable) __Th___
    {
        _drawable = drawable;
    }

/*
=================================================
    _SetController
=================================================
*/
    void  ScriptBaseLayout::_SetController (const ScriptUIControllerPtr &controller) __Th___
    {
        _controller = controller;
    }

/*
=================================================
    Convert
=================================================
*/
    void  ScriptBaseLayout::Convert (UI::Widget &widget, const ScriptBaseLayout &src_root, OUT UI::IController::ActionMap &actionMap) __Th___
    {
        UI::IController::ActionMapBuilder   builder;

        const auto  ConvertPass1 = [&widget, &builder](const ScriptBaseLayout &src)
        {{
            auto    dst = src._ConvertPass1( widget );  // throw
            CHECK_THROW( dst );

            if ( src._drawable )
            {
                auto    dst_draw = src._drawable->Convert( widget );
                CHECK_THROW( dst_draw );
                dst->SetDrawable( RVRef(dst_draw) );
            }

            if ( src._controller )
            {
                auto    dst_cont = src._controller->Convert( widget, INOUT builder );
                CHECK_THROW( dst_cont );
                dst->SetController( RVRef(dst_cont) );
            }
            return dst;
        }};

        auto    dst_root = ConvertPass1( src_root );  // throw

        RingBuffer< Pair< ScriptBaseLayout const*, RC<UI::ILayout> >>   queue;
        queue.emplace_back( &src_root, dst_root );

        Array< Pair< ScriptBaseLayout const*, RC<UI::ILayout> >>        converted;
        converted.emplace_back( &src_root, dst_root );

        for (; not queue.empty();)
        {
            auto [src_parent, dst_parent] = queue.ExtractFront();

            for (auto& src_child : src_parent->_childs)
            {
                auto    dst_child = ConvertPass1( *src_child );  // throw

                queue.emplace_back( src_child.Get(), dst_child );
                converted.emplace_back( src_child.Get(), dst_child );

                dst_parent->AddChild( dst_child );
            }
        }

        CHECK_THROW( widget.Initialize( dst_root ));

        for (auto& [src, dst] : converted)
        {
            src->_ConvertPass2( *dst );  // throw
        }

        actionMap = builder.Build();
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ScriptFixedLayout::ScriptFixedLayout (ELayoutType type) __Th___ :
        ScriptBaseLayout{ type }
    {
        CHECK_THROW_MSG( type == ELayoutType::FixedLayoutPx or
                         type == ELayoutType::FixedLayoutMm );
    }

/*
=================================================
    SetRegion1
=================================================
*/
    void  ScriptFixedLayout::SetRegion1 (const RectF &value) __Th___
    {
        CHECK_THROW_MSG( value.IsValid() );

        _region = value;
    }

/*
=================================================
    _ConvertPass1
=================================================
*/
    RC<UI::ILayout>  ScriptFixedLayout::_ConvertPass1 (UI::Widget &widget) C_Th___
    {
        switch ( _type )
        {
            case ELayoutType::FixedLayoutPx :
                return widget.Create< UI::FixedLayoutPx >();

            case ELayoutType::FixedLayoutMm :
                return widget.Create< UI::FixedLayoutMm >();
        }
        CHECK_THROW_MSG( false, "invalid layout type" );
    }

/*
=================================================
    _ConvertPass2
=================================================
*/
    void  ScriptFixedLayout::_ConvertPass2 (UI::ILayout &converted) C_Th___
    {
        const auto  Convert2 = [this] (auto& layout)
        {{
            layout.SetRegion( _region );
        }};

        switch ( _type )
        {
            case ELayoutType::FixedLayoutPx :
                return Convert2( RefCast< UI::FixedLayoutPx >( converted ));

            case ELayoutType::FixedLayoutMm :
                return Convert2( RefCast< UI::FixedLayoutMm >( converted ));
        }
        CHECK_THROW_MSG( false, "invalid layout type" );
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptFixedLayout::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptFixedLayout>   binder{ se };
        binder.CreateRef();
        binder.AddFactoryCtor< ELayoutType >();
        binder.AddMethod( &ScriptFixedLayout::SetRegion1,   "Region" );
        _BindBase( binder );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ScriptPaddingLayout::ScriptPaddingLayout (ELayoutType type) __Th___ :
        ScriptBaseLayout{ type }
    {
        CHECK_THROW_MSG( type == ELayoutType::PaddingLayoutPx or
                         type == ELayoutType::PaddingLayoutMm or
                         type == ELayoutType::PaddingLayoutRel );
    }

/*
=================================================
    SetPaddingX / SetPaddingY
=================================================
*/
    void  ScriptPaddingLayout::SetPaddingX (float left, float right) __Th___
    {
        _x = float2{ left, right };
    }

    void  ScriptPaddingLayout::SetPaddingY (float bottom, float top) __Th___
    {
        _y = float2{ bottom, top };
    }

/*
=================================================
    _ConvertPass1
=================================================
*/
    RC<UI::ILayout>  ScriptPaddingLayout::_ConvertPass1 (UI::Widget &widget) C_Th___
    {
        switch ( _type )
        {
            case ELayoutType::PaddingLayoutPx :
                return widget.Create< UI::PaddingLayoutPx >();

            case ELayoutType::PaddingLayoutMm :
                return widget.Create< UI::PaddingLayoutMm >();

            case ELayoutType::PaddingLayoutRel :
                return widget.Create< UI::PaddingLayoutRel >();
        }
        CHECK_THROW_MSG( false, "invalid layout type" );
    }

/*
=================================================
    _ConvertPass2
=================================================
*/
    void  ScriptPaddingLayout::_ConvertPass2 (UI::ILayout &converted) C_Th___
    {
        const auto  Convert2 = [this] (auto& layout) __Th___
        {{
            layout.SetPaddingX( _x[0], _x[1] );
            layout.SetPaddingY( _y[0], _y[1] );
        }};

        switch ( _type )
        {
            case ELayoutType::PaddingLayoutPx :
                return Convert2( RefCast< UI::PaddingLayoutPx >( converted ));

            case ELayoutType::PaddingLayoutMm :
                return Convert2( RefCast< UI::PaddingLayoutMm >( converted ));

            case ELayoutType::PaddingLayoutRel :
                return Convert2( RefCast< UI::PaddingLayoutRel >( converted ));
        }
        CHECK_THROW_MSG( false, "invalid layout type" );
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptPaddingLayout::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptPaddingLayout> binder{ se };
        binder.CreateRef();
        binder.AddFactoryCtor< ELayoutType >();
        binder.AddMethod( &ScriptPaddingLayout::SetPaddingX,    "PaddingX" );
        binder.AddMethod( &ScriptPaddingLayout::SetPaddingY,    "PaddingY" );
        binder.AddMethod( &ScriptPaddingLayout::SetPadding,     "Padding"  );
        _BindBase( binder );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    constructor
=================================================
*/
    ScriptAlignedLayout::ScriptAlignedLayout (ELayoutType type) __Th___ :
        ScriptBaseLayout{ type }
    {
        CHECK_THROW_MSG( type == ELayoutType::AlignedLayoutPx or
                         type == ELayoutType::AlignedLayoutMm or
                         type == ELayoutType::AlignedLayoutRel );
    }

/*
=================================================
    SetSize / SetAlign
=================================================
*/
    void  ScriptAlignedLayout::SetSize (const packed_float2 &value) __Th___
    {
        _size = value;
    }

    void  ScriptAlignedLayout::SetAlign (ELayoutAlign value) __Th___
    {
        _align = value;
    }

/*
=================================================
    _ConvertPass1
=================================================
*/
    RC<UI::ILayout>  ScriptAlignedLayout::_ConvertPass1 (UI::Widget &widget) C_Th___
    {
        switch ( _type )
        {
            case ELayoutType::AlignedLayoutPx :
                return widget.Create< UI::AlignedLayoutPx >();

            case ELayoutType::AlignedLayoutMm :
                return widget.Create< UI::AlignedLayoutMm >();

            case ELayoutType::AlignedLayoutRel :
                return widget.Create< UI::AlignedLayoutRel >();
        }
        CHECK_THROW_MSG( false, "invalid layout type" );
    }

/*
=================================================
    _ConvertPass2
=================================================
*/
    void  ScriptAlignedLayout::_ConvertPass2 (UI::ILayout &converted) C_Th___
    {
        const auto  Convert2 = [this] (auto& layout) __Th___
        {{
            layout.SetSize( _size );
            layout.SetAlign( _align );
        }};

        switch ( _type )
        {
            case ELayoutType::AlignedLayoutPx :
                return Convert2( RefCast< UI::AlignedLayoutPx >( converted ));

            case ELayoutType::AlignedLayoutMm :
                return Convert2( RefCast< UI::AlignedLayoutMm >( converted ));

            case ELayoutType::AlignedLayoutRel :
                return Convert2( RefCast< UI::AlignedLayoutRel >( converted ));
        }
        CHECK_THROW_MSG( false, "invalid layout type" );
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptAlignedLayout::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptAlignedLayout> binder{ se };
        binder.CreateRef();
        binder.AddFactoryCtor< ELayoutType >();
        binder.AddMethod( &ScriptAlignedLayout::SetSize,    "Size" );
        binder.AddMethod( &ScriptAlignedLayout::SetAlign,   "Align" );
        _BindBase( binder );
    }
//-----------------------------------------------------------------------------



/*
=================================================
    SetOrigin
=================================================
*/
    void  ScriptFillStackLayout::SetOrigin (EStackOrigin value) __Th___
    {
        _origin = value;
    }

/*
=================================================
    _ConvertPass1
=================================================
*/
    RC<UI::ILayout>  ScriptFillStackLayout::_ConvertPass1 (UI::Widget &widget) C_Th___
    {
        return widget.Create< UI::FillStackLayout >();
    }

/*
=================================================
    _ConvertPass2
=================================================
*/
    void  ScriptFillStackLayout::_ConvertPass2 (UI::ILayout &converted) C_Th___
    {
        auto&   layout = RefCast< UI::FillStackLayout >( converted );

        layout.SetOrigin( _origin );
    }

/*
=================================================
    Bind
=================================================
*/
    void  ScriptFillStackLayout::Bind (const ScriptEnginePtr &se) __Th___
    {
        Scripting::ClassBinder<ScriptFillStackLayout>   binder{ se };
        binder.CreateRef();
        binder.AddMethod( &ScriptFillStackLayout::SetOrigin,    "Origin" );
        _BindBase( binder );
    }


} // AE::AssetPacker
