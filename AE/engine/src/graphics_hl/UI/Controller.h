// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_hl/UI/Layout.h"

namespace AE::UI
{

    //
    // Controller interface
    //
    class IController :
        public EnableRC<IController>,
        public NonAllocatable
    {
    // types
    public:
        struct InputState;
        struct ActionMap;
        struct ActionMapBuilder;

        struct UpdateParams
        {
            InputState const &          input;
            ActionMap const &           actionMap;
            ILayout::LayoutState &      state;

            UpdateParams (const InputState      &input,
                          const ActionMap       &actionMap,
                          ILayout::LayoutState  &state) __NE___ :
                input{input}, actionMap{actionMap}, state{state}
            {}
        };

        enum class EType : ubyte
        {
            Button,
            Selection,
            Draggable,
            Resizable,
            _Count,
            Unknown     = 0xFF
        };


    // interface
    public:
            virtual void  Update (const UpdateParams &)                     __NE___ = 0;

        ND_ virtual EType  GetType ()                                       C_NE___ = 0;

        // serializing
        ND_ virtual bool  Serialize (Serializing::Serializer &)             C_NE___ = 0;
        ND_ virtual bool  Deserialize (Serializing::Deserializer &)         __NE___ = 0;

        // utils
        ND_ static bool  RegisterControllers (Serializing::ObjectFactory &) __NE___;

        ND_ static void*  operator new (usize, void* where)                 __NE___ { return where; }
        //  static void   operator delete (void*, void*)                    __NE___ {}

    protected:
        ND_ static Serializing::SerializedID  _GetControllerID (EType)      __NE___;
    };



    //
    // Input State
    //
    struct IController::InputState
    {
        friend class Screen;

    // types
    public:
        enum class EGesture
        {
            Down,           // just on pressing
            Hold,           // continuous pressing
            Click,          // just after pressing
            DoubleClick,
            Move,           // continuous
            LongPress,
            LongPress_Move, // continuous
            _Count
        };
        using EGestureBits = EnumSet< EGesture >;

    private:
        static constexpr EGestureBits   _touchDownBits { EGesture::Down, EGesture::Hold, EGesture::Move, EGesture::LongPress };

        struct CursorData
        {
            friend struct InputState;

        // variables
        private:
            mutable IController*    _focused        = null;
            float2                  _position;              // px
            float2                  _wheel;
            float4                  _scaleRotate;           // { scale_delta, rotation_delta, scale_absolute, rotation_absolute}
            float                   _longPressTime  = 0.f;
            EGestureBits            _bits;
            mutable bool            _resetFocus     = false;

        // methods
        public:
                void            ResetFocus ()       C_NE___ { _resetFocus = true; }
            ND_ float2 const&   Position ()         C_NE___ { return _position; }
            ND_ float2          Scale ()            C_NE___ { return float2{ _scaleRotate.x, _scaleRotate.y }; }
            ND_ float2          Rotate ()           C_NE___ { return float2{ _scaleRotate.z, _scaleRotate.w }; }
            ND_ float           LongPressTime ()    C_NE___ { ASSERT( Is( EGesture::LongPress ));  return _longPressTime; }
            ND_ bool            Is (EGesture g)     C_NE___ { return _bits.contains( g ); }
            ND_ bool            IsTouchDown ()      C_NE___ { return (_bits & _touchDownBits).Any(); }
        };


    // variables
    private:
        CursorData      _cursor;


    // methods
    public:

        // for mouse and touches
        ND_ CursorData const*   TryToCaptureCursor (IController *, const RectF &, bool hasActiveEvents) C_NE___;
            bool                ReleaseCursor (IController *)                                           C_NE___;

    private:
        void  _SetCursorState (const float2 &pos, const float2 &wheel, float longPressTime,
                               const float4 &scaleRotate, EGestureBits bits)                            __NE___;
    };



    //
    // Action Map
    //
    struct IController::ActionMap
    {
    // types
    public:
        using Callback_t    = Function< void (ILayout::LayoutState const&, AnyTypeRef) >;
    private:
        using Map_t         = FlatHashMap< ActionName::Optimized_t, Callback_t >;


    // variables
    private:
        Map_t       _actions;


    // methods
    public:
        ActionMap ()                                                __NE___ {}
        ActionMap (ActionMap &&)                                    __NE___ = default;
        ActionMap&  operator = (ActionMap &&)                       __NE___ = default;

        void  Call (ActionName::Ref             act,
                    ILayout::LayoutState const& state,
                    AnyTypeRef                  data    = Default)  C_NE___;

        ND_ bool  Serialize (Serializing::Serializer &ser)          C_NE___;

    private:
        friend struct IController::ActionMapBuilder;
        explicit ActionMap (Map_t actions)                          __NE___ : _actions{actions} {}
    };



    //
    // Action Map Builder
    //
    struct IController::ActionMapBuilder
    {
    // types
    private:
        using Callback_t    = ActionMap::Callback_t;
        using Map_t         = ActionMap::Map_t;

    // variables
    private:
        Map_t       _actions;

    // methods
    public:
        ActionMapBuilder ()                                         __NE___ {}

            template <typename FN>
            void  BindAll (const FN &fn)                            __NE___;

            bool  Bind (ActionName::Ref act, Callback_t cb)         __NE___;

        ND_ bool  Deserialize (Serializing::Deserializer &)         __NE___;

        ND_ bool  IsAllBound ()                                     C_NE___;
        ND_ bool  Merge (ActionMapBuilder &from)                    __NE___;

        ND_ ActionMap   Build ()                                    __NE___ { ASSERT( IsAllBound() );  return ActionMap{ RVRef(_actions) }; }
    };



    //
    // Button Controller
    //
    class ButtonController final : public IController
    {
    // variables
    private:
        ActionName      _onClick;
        ActionName      _onDoubleClick;
        ActionName      _onLongPress;


    // methods
    public:
        explicit ButtonController (Ptr<IAllocator>)         __NE___ {}

        void    SetActions (ActionName::Ref onClick,
                            ActionName::Ref onDoubleClick,
                            ActionName::Ref onLongPress);

        // IController //
        void    Update (const UpdateParams &)               __NE_OV;

        EType   GetType ()                                  C_NE_OV { return EType::Button; }

        bool    Serialize (Serializing::Serializer &)       C_NE_OV;
        bool    Deserialize (Serializing::Deserializer &)   __NE_OV;
    };



    //
    // Selection Controller
    //
    class SelectionController final : public IController
    {
    // variables
    private:
        ActionName      _onSelectionChanged;
        bool            _selected               = false;


    // methods
    public:
        explicit SelectionController (Ptr<IAllocator>)      __NE___ {}

        // IController //
        void    Update (const UpdateParams &)               __NE_OV;

        EType   GetType ()                                  C_NE_OV { return EType::Selection; }

        bool    Serialize (Serializing::Serializer &)       C_NE_OV;
        bool    Deserialize (Serializing::Deserializer &)   __NE_OV;
    };



    //
    // Draggable Controller
    //
    class DraggableController final : public IController
    {
    // variables
    private:
        bool        _isDragging     = false;


    // methods
    public:
        explicit DraggableController (Ptr<IAllocator>)      __NE___ {}

        // IController //
        void    Update (const UpdateParams &)               __NE_OV;

        EType   GetType ()                                  C_NE_OV { return EType::Draggable; }

        bool    Serialize (Serializing::Serializer &)       C_NE_OV;
        bool    Deserialize (Serializing::Deserializer &)   __NE_OV;
    };



    //
    // Resizable Controller
    //
    class ResizableController final : public IController
    {
    // variables
    private:
        ELayoutAlign    _direction  = Default;


    // methods
    public:
        explicit ResizableController (Ptr<IAllocator>)      __NE___ {}

        // IController //
        void    Update (const UpdateParams &)               __NE_OV;

        EType   GetType ()                                  C_NE_OV { return EType::Resizable; }

        bool    Serialize (Serializing::Serializer &)       C_NE_OV;
        bool    Deserialize (Serializing::Deserializer &)   __NE_OV;
    };
//-----------------------------------------------------------------------------



/*
=================================================
    BindAll
=================================================
*/
    template <typename FN>
    void  IController::ActionMapBuilder::BindAll (const FN &fn) __NE___
    {
        for (auto& [act, cb] : _actions)
        {
            fn( ActionName{act}, OUT cb );
        }
    }


} // AE::UI
