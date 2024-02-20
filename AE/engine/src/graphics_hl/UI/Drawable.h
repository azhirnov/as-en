// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#ifndef AE_UI_NO_GRAPHICS
# include "graphics_hl/Canvas/Canvas.h"
# include "graphics_hl/Canvas/Primitives.h"
#endif

#include "graphics_hl/UI/LayoutEnums.h"
#include "graphics_hl/UI/StyleCollection.h"

namespace AE::UI
{

# ifndef AE_UI_NO_GRAPHICS
    using AE::Graphics::Canvas;
    using AE::Graphics::SurfaceDimensions;
    using TransferContext_t = Graphics::DirectCtx::Transfer;
    using DrawContext_t     = Graphics::DirectCtx::Draw;
# else
    struct Canvas;
    struct TransferContext_t;
    struct DrawContext_t;
# endif



    //
    // Drawable interface
    //
    class IDrawable :
        public EnableRC<IDrawable>,
        public NonAllocatable
    {
    // types
    public:
        struct Material;
        struct GlobalMaterial;
        struct PreDrawParams;
        struct DrawParams;

        using PreDrawFn_t   = void (*) (INOUT void* &data, TransferContext_t &ctx)                                          __Th___;
        using DrawFn_t      = void (*) (INOUT void* &data, const DrawParams &params, Canvas &canvas, DrawContext_t &ctx)    __Th___;

        enum class EType : ubyte
        {
            Rectangle,
            Image,
            NinePatch,
            Text,
            _Count,
            Unknown     = 0xFF
        };

    protected:
        using ColorStyle    = StyleCollection::ColorStyle;
        using ImageStyle    = StyleCollection::ImageStyle;
        using FontStyle     = StyleCollection::FontStyle;


    // interface
    public:
        //virtual void  Init () = 0;    // TODO

        virtual void  PreDraw (const PreDrawParams &params, TransferContext_t &ctx)         __Th___ { Unused( params, ctx ); }
        virtual void  Draw (const DrawParams &params, Canvas &canvas, DrawContext_t &ctx)   __Th___ = 0;

        ND_ virtual EType  GetType ()                                                       C_NE___ = 0;

        // serializing
        ND_ virtual bool  Serialize (Serializing::Serializer &)                             C_NE___ = 0;
        ND_ virtual bool  Deserialize (Serializing::Deserializer &)                         __NE___ = 0;

        // utils
        ND_ static bool  RegisterDrawables (Serializing::ObjectFactory &)                   __NE___;

        ND_ static void*  operator new (usize, void* where)                                 __NE___ { return where; }
        //  static void   operator delete (void*, void*)                                    __NE___ {}

    protected:
        ND_ float  _GetColorAnimSpeed ()                                                    C_NE___;

        ND_ static Serializing::SerializedID  _GetDrawableID (EType type)                   __NE___;
    };


    //
    // Material
    //
    struct IDrawable::Material
    {
    // variables
        uint                    stencilRef      = 0;
        uint                    mtrDynOffset    = 0;
        GraphicsPipelineID      ppln;

        // all UI pipelines must:
        //  - use same descriptor sets
        //  - have dynamic stencil reference

    // methods
        ND_ bool  operator == (const Material &rhs)     C_NE___;
        ND_ bool  operator != (const Material &rhs)     C_NE___ { return not (*this == rhs); }

        ND_ bool  IsDefined ()                          C_NE___ { return ppln != Default; }
    };


    //
    // Global Material
    //
    struct IDrawable::GlobalMaterial
    {
    // variables
        mutable Material                mtr;            // inout
        DescriptorSetID                 ds;
        uint                            globalDynOffset = 0;

        static constexpr DescSetBinding dsIndex         {0};
        static constexpr EPrimitive     topology        = EPrimitive::TriangleList;

    // methods
            void  operator =  (const Material &rhs)     C_NE___ { mtr = rhs; }

        ND_ bool  operator == (const Material &rhs)     C_NE___ { return mtr == rhs; }
        ND_ bool  operator != (const Material &rhs)     C_NE___ { return mtr != rhs; }

        ND_ bool  IsDefined ()                          C_NE___ { return mtr.IsDefined(); }

        ND_ Material*  operator -> ()                   C_NE___ { return &mtr; }
    };


    //
    // Draw Params
    //
    struct IDrawable::DrawParams
    {
        RectF           globalRect;
        RectF           clipRect;
        EStyleState     style       = Default;
        float           dt;
        GlobalMaterial  mtr;        // inout
    };


    //
    // Pre Draw Params
    //
    struct IDrawable::PreDrawParams
    {
        float           dt;
    };



    //
    // Rectangle Shape Drawable
    //
    class RectangleDrawable final : public IDrawable
    {
    // types
    public:
        struct Data
        {
            Material                mtr;
            RGBA8u                  prevColor;
            RGBA8u                  currColor;
            EStyleState             currStyle       = Default;
            float                   factor          = 2.0f;
            Ptr<const ColorStyle>   stylePtr;
        };

    // variables
    private:
        StyleName::Optimized_t  _style;
        Data                    _data;

    // methods
    public:
        explicit RectangleDrawable (Ptr<IAllocator>)                                __NE___ {}

        ND_ bool  SetStyle (StyleName::Ref name)                                    __NE___;

        // IDrawable //
        void    Draw (const DrawParams &params, Canvas &canvas, DrawContext_t &ctx) __Th_OV;
        EType   GetType ()                                                          C_NE_OV { return EType::Rectangle; }

        // serializing
        bool    Serialize (Serializing::Serializer &)                               C_NE_OV;
        bool    Deserialize (Serializing::Deserializer &)                           __NE_OV;
    };



    //
    // Image Drawable
    //
    class ImageDrawable final : public IDrawable
    {
    // types
    public:
        struct Data
        {
            Material        mtr;
        };

    // variables
    private:
        StyleName::Optimized_t  _style;
        Data                    _data;

    // methods
    public:
        explicit ImageDrawable (Ptr<IAllocator>)                                    __NE___ {}

        ND_ bool  SetStyle (StyleName::Ref name)                                    __NE___;

        // IDrawable //
        void    Draw (const DrawParams &params, Canvas &canvas, DrawContext_t &ctx) __Th_OV;
        EType   GetType ()                                                          C_NE_OV { return EType::Image; }

        // serializing
        bool    Serialize (Serializing::Serializer &)                               C_NE_OV;
        bool    Deserialize (Serializing::Deserializer &)                           __NE_OV;
    };



    //
    // Nine Patch Drawable
    //
    class NinePatchDrawable final : public IDrawable
    {
    // types
    public:
        struct Data
        {
            Material        mtr;
        };

    // variables
    private:
        StyleName::Optimized_t  _style;
        Data                    _data;

    // methods
    public:
        explicit NinePatchDrawable (Ptr<IAllocator>)                                __NE___ {}

        ND_ bool  SetStyle (StyleName::Ref name)                                    __NE___;

        // IDrawable //
        void    Draw (const DrawParams &params, Canvas &canvas, DrawContext_t &ctx) __Th_OV;
        EType   GetType ()                                                          C_NE_OV { return EType::NinePatch; }

        // serializing
        bool    Serialize (Serializing::Serializer &)                               C_NE_OV;
        bool    Deserialize (Serializing::Deserializer &)                           __NE_OV;
    };



    //
    // Text Drawable
    //
    class TextDrawable final : public IDrawable
    {
    // types
    public:
        struct Data
        {
            Material        mtr;
        };

    // variables
    private:
        StyleName::Optimized_t  _style;
        Data                    _data;

    // methods
    public:
        explicit TextDrawable (Ptr<IAllocator>)                                     __NE___ {}

        ND_ bool  SetStyle (StyleName::Ref name)                                    __NE___;

        // IDrawable //
        void    Draw (const DrawParams &params, Canvas &canvas, DrawContext_t &ctx) __Th_OV;
        EType   GetType ()                                                          C_NE_OV { return EType::Text; }

        // serializing
        bool    Serialize (Serializing::Serializer &)                               C_NE_OV;
        bool    Deserialize (Serializing::Deserializer &)                           __NE_OV;
    };


} // AE::UI
