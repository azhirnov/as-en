// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Math.h"
#include "base/Math/Bytes.h"
#include "base/Containers/ArrayView.h"

namespace AE::Base
{

    //
    // Structure View
    //

    template <typename T>
    struct StructView
    {
    // types
    public:
        using Self  = StructView< T >;

        struct const_iterator
        {
            friend struct StructView;

        private:
            Self const&     _ref;
            usize           _index = 0;

            const_iterator (const Self &ref, usize idx)             __NE___ : _ref{ref}, _index{idx} {}

        public:
            const_iterator& operator ++ ()                          __NE___ { ++_index;  return *this; }

            ND_ T const&    operator * ()                           C_NE___ { return _ref[_index]; }
            ND_ bool        operator == (const const_iterator &rhs) C_NE___ { return &_ref == &rhs._ref and _index == rhs._index; }
            ND_ bool        operator != (const const_iterator &rhs) C_NE___ { return not (*this == rhs); }
        };


        struct large_iterator
        {
            friend struct StructView;

        private:
            Self const      _ref;
            usize           _index = 0;

            large_iterator (const Self &ref, usize idx)             __NE___ : _ref{ref}, _index{idx} {}

        public:
            large_iterator& operator ++ ()                          __NE___ { ++_index;  return *this; }

            ND_ T const&    operator * ()                           C_NE___ { return _ref[_index]; }
            ND_ bool        operator != (const large_iterator &rhs) C_NE___ { return not (*this == rhs); }

            ND_ bool        operator == (const large_iterator &rhs) C_NE___
            {
                return  (_ref._array  == rhs._ref._array)  & (_ref._count == rhs._ref._count) &
                        (_ref._stride == rhs._ref._stride) & (_index == rhs._index);
            }
        };


    private:
        static constexpr uint   DBG_VIEW_COUNT = 400;

        struct _IViewer
        {
            virtual ~_IViewer () {}

            ND_ virtual Unique<_IViewer>  Clone () = 0;
        };


        template <typename St, usize Padding>
        struct _ViewerWithPaddingUnaligned final : _IViewer
        {
        // types
            #pragma pack (push, 1)
            struct Element {
                T           value;
                ubyte       _padding [Padding];
            };
            #pragma pack (pop)
            using ElementsPtr_t = Element const (*) [DBG_VIEW_COUNT];

        // variables
            ElementsPtr_t const     elements;

        // methods
            explicit _ViewerWithPaddingUnaligned (const void *ptr) __NE___ : elements{ BitCast<ElementsPtr_t>(ptr) } {
                STATIC_ASSERT( sizeof(Element) == sizeof(St) );
            }

            Unique<_IViewer>  Clone () __Th_OV { return Unique<_IViewer>{new _ViewerWithPaddingUnaligned< St, Padding >{ elements }}; }
        };


        template <typename St, usize Padding>
        struct _ViewerWithPadding final : _IViewer
        {
        // types
            struct Element {
                T           value;
                ubyte       _padding [Padding];
            };
            using ElementsPtr_t = Element const (*) [DBG_VIEW_COUNT];

        // variables
            ElementsPtr_t const     elements;

        // methods
            explicit _ViewerWithPadding (const void *ptr) __NE___ : elements{ BitCast<ElementsPtr_t>(ptr) } {
                STATIC_ASSERT( sizeof(Element) == sizeof(St) );
            }

            Unique<_IViewer>  Clone () __Th_OV { return Unique<_IViewer>{new _ViewerWithPadding< St, Padding >{ elements }}; }
        };


        template <typename St>
        struct _ViewerImpl final : _IViewer
        {
        // types
            using ElementsPtr_t = T const (*) [DBG_VIEW_COUNT];

        // variables
            ElementsPtr_t const     elements;

        // methods
            explicit _ViewerImpl (const void *ptr) __NE___ : elements{ BitCast<ElementsPtr_t>(ptr) } {}

            Unique<_IViewer>  Clone () __Th_OV { return Unique<_IViewer>{new _ViewerImpl< St >{ elements }}; }
        };


    // variables
    private:
        void const *    _array      = null;
        uint            _count      = 0;
        uint            _stride     = 0;

        DEBUG_ONLY(
            Unique<_IViewer>    _dbgView;   
        )


    // methods
    public:
        StructView () __NE___
        {}

        StructView (ArrayView<T> arr) __NE___ :
            _array{ arr.data() }, _count{ CheckCast<uint>( arr.size() )}, _stride{ sizeof(T) }
        {
            DEBUG_ONLY( _dbgView = _CreateView<T, sizeof(T)>( _array ));
        }

        StructView (const Self &other) __NE___ :
            _array{other._array}, _count{other._count}, _stride{other._stride}
        {
            DEBUG_ONLY( if ( other._dbgView ) _dbgView = other._dbgView->Clone() );
        }

        StructView (Self &&other) __NE___ :
            _array{other._array}, _count{other._count}, _stride{other._stride}
        {
            DEBUG_ONLY( std::swap( _dbgView, other._dbgView ));
        }

        template <typename Class>
        StructView (ArrayView<Class> arr, T (Class::*member)) __NE___ :
            _array{ arr.data() + OffsetOf(member) }, _count{ CheckCast<uint>( arr.size() )}, _stride{ sizeof(Class) }
        {
            DEBUG_ONLY( _dbgView = _CreateView<Class, sizeof(Class)>( _array ));
        }

        StructView (const T* ptr, usize count) __NE___ :
            _array{ ptr }, _count{ CheckCast<uint>( count )}, _stride{ sizeof(T) }
        {
            DEBUG_ONLY( _dbgView = _CreateView<T, sizeof(T)>( _array ));
        }

        StructView (const void *ptr, usize count, uint stride) __NE___ :
            _array{ptr}, _count{ CheckCast<uint>( count )}, _stride{stride}
        {}

        ~StructView () __NE___
        {}


        Self&  operator = (const Self &rhs) __NE___
        {
            _array  = rhs._array;
            _count  = rhs._count;
            _stride = rhs._stride;
            DEBUG_ONLY( if ( rhs._dbgView ) _dbgView = rhs._dbgView->Clone() );
            return *this;
        }

        Self&  operator = (Self && rhs) __NE___
        {
            _array  = rhs._array;
            _count  = rhs._count;
            _stride = rhs._stride;
            DEBUG_ONLY( std::swap( _dbgView, rhs._dbgView ));
            return *this;
        }


        ND_ usize       size ()                 C_NE___ { return _count; }
        ND_ bool        empty ()                C_NE___ { return _count == 0; }
        ND_ T const &   operator [] (usize i)   C_NE___ { ASSERT( i < size() );  return *static_cast<T const *>(_array + Bytes(i * _stride)); }

        ND_ T const&    front ()                C_NE___ { return operator[] (0); }
        ND_ T const&    back ()                 C_NE___ { return operator[] (_count-1); }

        ND_ const_iterator  begin ()            CrNE___ { return const_iterator{ *this, 0 }; }
        ND_ const_iterator  end ()              CrNE___ { return const_iterator{ *this, size() }; }

        ND_ large_iterator  begin ()            rvNE___ { return large_iterator{ *this, 0 }; }
        ND_ large_iterator  end ()              rvNE___ { return large_iterator{ *this, size() }; }

        ND_ const_iterator  begin ()            r_NE___ { return const_iterator{ *this, 0 }; }
        ND_ const_iterator  end ()              r_NE___ { return const_iterator{ *this, size() }; }


        ND_ bool  operator == (Self rhs)        C_NE___
        {
            if ( (_array == rhs._array) & (_count == rhs._count) & (_stride == rhs._stride) )
                return true;

            if ( size() != rhs.size() )
                return false;

            for (usize i = 0; i < size(); ++i)
            {
                if_unlikely( not ((*this)[i] == rhs[i]) )
                    return false;
            }
            return true;
        }


        ND_ Self  section (usize first, usize count) C_NE___
        {
            return first < size() ?
                    Self{ _array + Bytes(first * _stride), Min(size() - first, count) } :
                    Self{};
        }

        ND_ explicit operator Array<T> () C_NE___
        {
            Array<T>    result;
            result.resize( size() );

            for (usize i = 0; i < result.size(); ++i) {
                result[i] = (*this)[i];
            }
            return result;
        }


    private:
        template <typename Class, usize Stride>
        ND_ static Unique<_IViewer>  _CreateView (const void *ptr)
        {
            STATIC_ASSERT( Stride >= sizeof(T) );
            const usize padding = Stride - sizeof(T);

            if constexpr( padding == 0 )
                return MakeUnique< _ViewerImpl< Class >>( ptr );
            else
            if constexpr( padding % alignof(T) == 0 )
                return MakeUnique< _ViewerWithPadding< Class, padding >>( ptr );
            else
                return MakeUnique< _ViewerWithPaddingUnaligned< Class, padding >>( ptr );
        }
    };



    //
    // Structure View Transform
    //

    template <typename SrcType, typename DstType>
    struct StructViewTransform_CastConverter
    {
        STATIC_ASSERT( std::is_convertible_v< SrcType, DstType >);

        ND_ DstType  operator () (SrcType src) C_NE___
        {
            return DstType{src};
        }
    };

    template <typename SrcType, typename DstType>
    struct StructViewTransform_BitCastConverter
    {
        ND_ DstType  operator () (const SrcType &src) C_NE___
        {
            return BitCast<DstType>( src );
        }
    };


    template <typename SrcType,
              typename DstType,
              typename Converter = StructViewTransform_CastConverter< SrcType, DstType >>
    struct StructViewTransform
    {
    // types
    public:
        using Self          = StructViewTransform< SrcType, DstType, Converter >;
        using View_t        = StructView< SrcType >;
        using SrcValue_t    = SrcType;
        using DstValue_t    = DstType;
        using Converter_t   = Converter;

        struct const_iterator
        {
            friend struct StructViewTransform;

        private:
            Self const&     _ref;
            usize           _index = 0;

            const_iterator (const Self &ref, usize idx)                 __NE___ : _ref{ref}, _index{idx} {}

        public:
            const_iterator&     operator ++ ()                          __NE___ { ++_index;  return *this; }
            ND_ decltype(auto)  operator * ()                           C_NE___ { return _ref[_index]; }
            ND_ bool            operator == (const const_iterator &rhs) C_NE___ { return &_ref == &rhs._ref and _index == rhs._index; }
            ND_ bool            operator != (const const_iterator &rhs) C_NE___ { return not (*this == rhs); }
        };


        struct large_iterator
        {
            friend struct StructViewTransform;

        private:
            Self const      _ref;
            usize           _index = 0;

            large_iterator (const Self &ref, usize idx)                 __NE___ : _ref{ref}, _index{idx} {}

        public:
            large_iterator&     operator ++ ()                          __NE___ { ++_index;  return *this; }
            ND_ decltype(auto)  operator * ()                           C_NE___ { return _ref[_index]; }
            ND_ bool            operator != (const large_iterator &rhs) C_NE___ { return not (*this == rhs); }
            ND_ bool            operator == (const large_iterator &rhs) C_NE___ { return (_ref._view  == rhs._ref._view) & (_index == rhs._index); }
        };


    // variables
    private:
        View_t      _view;


    // methods
    public:
        StructViewTransform ()                      __NE___ {}
        StructViewTransform (const Self &)          __NE___ = default;
        StructViewTransform (Self &&)               __NE___ = default;

        explicit StructViewTransform (View_t view)  __NE___ : _view{RVRef(view)} {}

        Self&  operator = (const Self &)            __NE___ = default;
        Self&  operator = (Self &&)                 __NE___ = default;

        Self&  operator = (View_t rhs)              __NE___ { _view = RVRef(rhs);  return *this; }

        ND_ usize           size ()                 C_NE___ { return _view.size(); }
        ND_ bool            empty ()                C_NE___ { return _view.empty(); }
        ND_ decltype(auto)  operator [] (usize i)   C_NE___ { return Converter_t{}( _view[i] ); }

        ND_ decltype(auto)  front ()                C_NE___ { return operator[] (0); }
        ND_ decltype(auto)  back ()                 C_NE___ { return operator[] (size()-1); }

        ND_ const_iterator  begin ()                CrNE___ { return const_iterator{ *this, 0 }; }
        ND_ const_iterator  end ()                  CrNE___ { return const_iterator{ *this, size() }; }

        ND_ large_iterator  begin ()                rvNE___ { return large_iterator{ *this, 0 }; }
        ND_ large_iterator  end ()                  rvNE___ { return large_iterator{ *this, size() }; }

        ND_ const_iterator  begin ()                r_NE___ { return const_iterator{ *this, 0 }; }
        ND_ const_iterator  end ()                  r_NE___ { return const_iterator{ *this, size() }; }


        ND_ Self  section (usize first, usize count) C_NE___
        {
            return Self{ _view.section( first, count )};
        }

        ND_ explicit operator Array<DstType> ()     C_NE___
        {
            Array<DstType>  result;
            result.resize( size() );

            for (usize i = 0; i < result.size(); ++i) {
                result[i] = (*this)[i];
            }
            return result;
        }
    };


} // AE::Base
