// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/BufferMemView.h"
#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/ImageUtils.h"
#include "graphics/Public/ImageSwizzle.h"

namespace AE::Graphics
{

    //
    // Image Memory View
    //

    struct ImageMemView
    {
    // types
    public:
        using Pixel_t   = BufferMemView::ConstData;
        using Row_t     = BufferMemView::Data;
        using CRow_t    = BufferMemView::ConstData;
        using Slice_t   = BufferMemView;

        static constexpr uint   Count = BufferMemView::Count;


    // variables
    protected:
        BufferMemView       _content;
        Bytes32u            _rowPitch;
        Bytes32u            _slicePitch;
        ushort3             _offset;
        ushort3             _dimension;
        ushort              _bitsPerBlock   = 0;
        ubyte2              _texBlockDim;
        EPixelFormat        _format         = Default;
        EImageAspect        _aspect         = Default;


    // methods
    public:
        ImageMemView () __NE___ {}
        ImageMemView (const BufferMemView& content, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect) __NE___;
        ImageMemView (void *content, Bytes contentSize, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect) __NE___;

        template <typename T>
        ImageMemView (Array<T> &content, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect) __NE___ :
            ImageMemView{ BufferMemView{content}, off, dim, rowPitch, slicePitch, format, aspect } {}

        ImageMemView (const ImageMemView &)                 __NE___ = default;
        ImageMemView (ImageMemView &&)                      __NE___ = default;

        ImageMemView&  operator = (const ImageMemView &)    __NE___ = default;
        ImageMemView&  operator = (ImageMemView &&)         __NE___ = default;

        ND_ uint3           Offset ()                       C_NE___ { return uint3{ _offset }; }
        ND_ uint3           Dimension ()                    C_NE___ { return uint3{ _dimension }; }
        ND_ Bytes           RowPitch ()                     C_NE___ { return Bytes(_rowPitch); }
        ND_ Bytes           SlicePitch ()                   C_NE___ { return Bytes(_slicePitch); }
        ND_ uint            BitsPerBlock ()                 C_NE___ { return _bitsPerBlock; }
        ND_ Bytes           BytesPerBlock ()                C_NE___ { return ImageUtils::BytesPerBlock( _bitsPerBlock, TexBlockDim() ); }
        ND_ Bytes           MinRowSize ()                   C_NE___ { return ImageUtils::RowSize( _dimension.x, _bitsPerBlock, TexBlockDim() ); };
        ND_ Bytes           MinSliceSize ()                 C_NE___ { return ImageUtils::SliceSize( _dimension.y, RowPitch(), TexBlockDim() ); }
        ND_ Bytes           Image2DSize ()                  C_NE___ { return RowPitch() * _dimension.y; }
        ND_ Bytes           Image3DSize ()                  C_NE___ { return SlicePitch() * _dimension.z; }
        ND_ Bytes           ContentSize ()                  C_NE___ { return _content.DataSize(); }
        ND_ EPixelFormat    Format ()                       C_NE___ { return _format; }
        ND_ EImageAspect    Aspect ()                       C_NE___ { return _aspect; }
        ND_ auto            Parts ()                        __NE___ { return _content.Parts(); }
        ND_ auto            Parts ()                        C_NE___ { return _content.Parts(); }
        ND_ bool            Empty ()                        C_NE___ { return _content.Empty(); }
        ND_ uint2           TexBlockDim ()                  C_NE___ { return uint2{_texBlockDim}; }
        ND_ uint2           TexelBlocks ()                  C_NE___ { return uint2{_dimension.x, _dimension.y} / TexBlockDim(); }

            bool    PushBack (void *ptr, Bytes size)        __NE___ { return _content.PushBack( ptr, size ); }

        ND_ Row_t   GetRow (uint y, uint z = 0)             __NE___;
        ND_ CRow_t  GetRow (uint y, uint z = 0)             C_NE___;

        ND_ Slice_t GetSlice (uint z)                       __NE___;
        ND_ Slice_t GetSlice (uint z)                       C_NE___;

        ND_ Pixel_t GetPixel (const uint3 &point)           C_NE___;

        ND_ bool    operator == (const ImageMemView &rhs)   C_NE___ { return Compare( rhs ) == 0_b; }


        // 'dataSize' - number of copied bytes.
        // Returns 'true' if all required data are copied.
            bool  CopyFrom (const ImageMemView &src, OUT Bytes &dataSize)                                                                           __NE___;
            bool  CopyFrom (const uint3 &dstOffset, const uint3 &srcOffset, const ImageMemView &srcImage, const uint3 &dim, OUT Bytes &dataSize)    __NE___;
        ND_ bool  CopyFrom (const ImageMemView &src)                                                                                                __NE___;
        ND_ bool  CopyFrom (const uint3 &dstOffset, const uint3 &srcOffset, const ImageMemView &srcImage, const uint3 &dim)                         __NE___;

        // Returns 'true' if all required data are copied.
        ND_ bool  CopyTo (OUT void* data, Bytes size)       C_NE___;

        // Returns how much bytes are different.
        ND_ Bytes  Compare (const ImageMemView &rhs)                                                                        C_NE___;
        ND_ Bytes  Compare (const uint3 &lhsOffset, const uint3 &rhsOffset, const ImageMemView &rhsImage, const uint3 &dim) C_NE___;
    };



    //
    // Read/Write Image Memory View
    //
    struct RWImageMemView : public ImageMemView
    {
    // types
    public:
        template <typename T>   using LoadPixelTFn_t    = void (*) (const CRow_t &, uint x, OUT T &);
        template <typename T>   using StorePixelTFn_t   = void (*) (OUT Row_t, uint x, const T &);

        using PixStorage_t      = StaticArray<uint,4>;

        using LoadPixelFn_t     = LoadPixelTFn_t< PixStorage_t >;
        using LoadRGBA32fFn_t   = LoadPixelTFn_t< RGBA32f >;
        using LoadRGBA32uFn_t   = LoadPixelTFn_t< RGBA32u >;
        using LoadRGBA32iFn_t   = LoadPixelTFn_t< RGBA32i >;
        using LoadDSFn_t        = LoadPixelTFn_t< DepthStencil >;

        using StorePixelFn_t    = StorePixelTFn_t< PixStorage_t >;
        using StoreRGBA32fFn_t  = StorePixelTFn_t< RGBA32f >;
        using StoreRGBA32uFn_t  = StorePixelTFn_t< RGBA32u >;
        using StoreRGBA32iFn_t  = StorePixelTFn_t< RGBA32i >;
        using StoreDSFn_t       = StorePixelTFn_t< DepthStencil >;

        struct Swizzle
        {
            uint4   _value;

            Swizzle (VecSwizzle src0, VecSwizzle src1)      __NE___;
            Swizzle (ImageSwizzle src0, ImageSwizzle src1)  __NE___;
            Swizzle (const uint4 &src0, const uint4 & src1) __NE___;

            template <typename T>
            ND_ RGBAColor<T>  Transform (const RGBAColor<T> &src0, const RGBAColor<T> &src1) C_NE___;
        };


    // variables
    private:
        LoadRGBA32fFn_t     _loadF4     = null;
        LoadRGBA32uFn_t     _loadU4     = null;
        LoadRGBA32iFn_t     _loadI4     = null;
        LoadDSFn_t          _loadDS     = null;

        StoreRGBA32fFn_t    _storeF4    = null;
        StoreRGBA32uFn_t    _storeU4    = null;
        StoreRGBA32iFn_t    _storeI4    = null;
        StoreDSFn_t         _storeDS    = null;


    // methods
    public:
        RWImageMemView ()                                           __NE___;
        RWImageMemView (const ImageMemView& other)                  __NE___;
        RWImageMemView (const BufferMemView& content, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect) __NE___;
        RWImageMemView (void *content, Bytes contentSize, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect) __NE___;

        template <typename T>
        RWImageMemView (Array<T> &content, const uint3 &off, const uint3 &dim, Bytes rowPitch, Bytes slicePitch, EPixelFormat format, EImageAspect aspect) __NE___ :
            RWImageMemView{ BufferMemView{content}, off, dim, rowPitch, slicePitch, format, aspect } {}

        void  Load (const uint3 &point, OUT RGBA32f &col)           C_NE___ { ASSERT( _loadF4 != null );    _loadF4( GetRow( point.y, point.z ), point.x, OUT col ); }
        void  Load (const uint3 &point, OUT RGBA32u &col)           C_NE___ { ASSERT( _loadU4 != null );    _loadU4( GetRow( point.y, point.z ), point.x, OUT col ); }
        void  Load (const uint3 &point, OUT RGBA32i &col)           C_NE___ { ASSERT( _loadI4 != null );    _loadI4( GetRow( point.y, point.z ), point.x, OUT col ); }
        void  Load (const uint3 &point, OUT DepthStencil &ds)       C_NE___ { ASSERT( _loadDS != null );    _loadDS( GetRow( point.y, point.z ), point.x, OUT ds ); }

        void  Store (const uint3 &point, const RGBA32f &col)        __NE___ { ASSERT( _storeF4 != null );   _storeF4( GetRow( point.y, point.z ), point.x, col ); }
        void  Store (const uint3 &point, const RGBA32u &col)        __NE___ { ASSERT( _storeU4 != null );   _storeU4( GetRow( point.y, point.z ), point.x, col ); }
        void  Store (const uint3 &point, const RGBA32i &col)        __NE___ { ASSERT( _storeI4 != null );   _storeI4( GetRow( point.y, point.z ), point.x, col ); }
        void  Store (const uint3 &point, const DepthStencil &ds)    __NE___ { ASSERT( _storeDS != null );   _storeDS( GetRow( point.y, point.z ), point.x, ds ); }

        // convert between different formats without filtering
        ND_ bool  Blit (const RWImageMemView &src)                                                                          __NE___;
        ND_ bool  Blit (const RWImageMemView &src, OUT Bytes &readn, OUT Bytes &written)                                    __NE___;
        ND_ bool  Blit (const uint3 &dstOffset, const uint3 &srcOffset, const RWImageMemView &srcImage, const uint3 &dim)   __NE___;
        ND_ bool  Blit (const uint3 &dstOffset, const uint3 &srcOffset, const RWImageMemView &srcImage, const uint3 &dim,
                        OUT Bytes &readn, OUT Bytes &written)                                                               __NE___;

        // with component swizzle
        ND_ bool  Blit (const RWImageMemView &src, const Swizzle &swizzle)                                                  __NE___;
        ND_ bool  Blit (const RWImageMemView &src, const Swizzle &swizzle, OUT Bytes &readn, OUT Bytes &written)            __NE___;
        ND_ bool  Blit (const uint3 &dstOffset, const uint3 &srcOffset, const RWImageMemView &srcImage,
                        const uint3 &dim, const Swizzle &swizzle)                                                           __NE___;
        ND_ bool  Blit (const uint3 &dstOffset, const uint3 &srcOffset, const RWImageMemView &srcImage,
                        const uint3 &dim, const Swizzle &swizzle, OUT Bytes &readn, OUT Bytes &written)                     __NE___;


        ND_ bool  Fill (const RGBA32f &col)                                         __NE___ { return Fill( col, uint3{}, Dimension() ); }
        ND_ bool  Fill (const RGBA32u &col)                                         __NE___ { return Fill( col, uint3{}, Dimension() ); }
        ND_ bool  Fill (const RGBA32i &col)                                         __NE___ { return Fill( col, uint3{}, Dimension() ); }

        ND_ bool  Fill (const RGBA32f &col, const uint3 &offset, const uint3 &dim)  __NE___;
        ND_ bool  Fill (const RGBA32u &col, const uint3 &offset, const uint3 &dim)  __NE___;
        ND_ bool  Fill (const RGBA32i &col, const uint3 &offset, const uint3 &dim)  __NE___;


    private:
        ND_ bool  _Fill (const CRow_t &data, const uint3 &offset, const uint3 &dim) __NE___;

        template <typename T>
        ND_ bool  _Blit (const uint3 &dstOffset, const uint3 &srcOffset, const RWImageMemView &srcImage,
                         const uint3 &dim, const Swizzle &swizzle, LoadPixelTFn_t<T> loadSrc, LoadPixelTFn_t<T> loadDst,
                         StorePixelTFn_t<T> store, OUT Bytes &readn, OUT Bytes &written) __NE___;
    };


} // AE::Graphics
