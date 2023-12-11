// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "serializing/Basic/Serializer.h"

namespace AE::Serializing
{

    template <typename T>
    bool  Serializer::_SerializeObj (const T &obj)
    {
        if ( factory )
            return factory->Serialize( *this, obj );

        if constexpr( IsBaseOf< ISerializable, T >)
            return obj.Serialize( *this );
        else
        {
            DBG_WARNING( "unknown type" );
            return false;
        }
    }


    template <typename ...Args>
    bool  Serializer::operator () (const Args& ...args) __NE___
    {
        return _RecursiveSerialize( args... );
    }


    template <typename Arg0, typename ...Args>
    bool  Serializer::_RecursiveSerialize (const Arg0 &arg0, const Args& ...args)
    {
        #if AE_DEBUG_SERIALIZER
            ASSERT( stream.Write( _dbgCounter ));
            ++_dbgCounter;
        #endif

        bool    res = _Serialize( arg0 );

        if constexpr( CountOf<Args...>() > 0 )
            return res and _RecursiveSerialize( args... );
        else
            return res;
    }


    template <typename T>
    bool  Serializer::_Serialize (const T &value)
    {
        if constexpr( IsTriviallySerializable<T> )
            return stream.Write( value );
        else
            return _SerializeObj( value );
    }


    template <typename F, typename S>
    bool  Serializer::_Serialize (const Pair<F,S> &value)
    {
        return _Serialize( value.first ) and _Serialize( value.second );
    }


    template <usize N>
    bool  Serializer::_Serialize (const BitSet<N> &value)
    {
        StaticAssert( N <= 64 );

        if constexpr( N <= 32 )
            return _Serialize( value.to_ulong() );
        else
            return _Serialize( value.to_ullong() );
    }


    template <typename T>
    bool  Serializer::_Serialize (ArrayView<T> arr)
    {
        CHECK_ERR( arr.size() <= MaxArrayLength );

        bool    res = stream.Write( CheckCast<uint>(arr.size()) );

        if constexpr( IsTriviallySerializable<T> )
        {
            return res and (arr.empty() or stream.Write( arr.data(), SizeOf<T> * arr.size() ));
        }
        else
        {
            for (usize i = 0; res & (i < arr.size()); ++i) {
                res = _Serialize( arr[i] );
            }
            return res;
        }
    }


    inline bool  Serializer::_Serialize (StringView str)
    {
        CHECK_ERR( str.length() <= MaxStringLength );
        return  stream.Write( CheckCast<uint>(str.length()) ) and
                (str.empty() or stream.Write( str.data(), StringSizeOf(str) ));
    }


    template <typename T, int I, glm::qualifier Q>
    bool  Serializer::_Serialize (const TVec<T,I,Q> &vec)
    {
        return stream.Write( &vec.x, SizeOf<T>*I );
    }


    template <typename T>
    bool  Serializer::_Serialize (const Rectangle<T> &rect)
    {
        return stream.Write( rect.data(), SizeOf<T>*4 );
    }


    template <typename T>
    bool  Serializer::_Serialize (const RGBAColor<T> &col)
    {
        return stream.Write( col.data(), Sizeof(col) );
    }


    template <typename T>
    bool  Serializer::_Serialize (const HSVColor &col)
    {
        return stream.Write( col.data(), Sizeof(col) );
    }


    template <usize Size, uint UID, uint Seed>
    bool  Serializer::_Serialize (const NamedID<Size, UID, true, Seed> &id)
    {
        return stream.Write( CheckCast<uint>(usize(id.GetHash())) );
    }


    template <usize Size, uint UID, uint Seed>
    bool  Serializer::_Serialize (const NamedID<Size, UID, false, Seed> &id)
    {
    #if AE_SERIALIZE_HASH_ONLY
        return stream.Write( CheckCast<uint>(usize(id.GetHash())) );
    #else
        return _Serialize( id.GetName() );
    #endif
    }


    template <typename K, typename V, typename MapType>
    bool  Serializer::_SerializeMap (const MapType &map)
    {
        CHECK_ERR( map.size() <= MaxArrayLength );

        bool    res = stream.Write( CheckCast<uint>(map.size()) );

        for (auto iter = map.begin(); res & (iter != map.end()); ++iter)
        {
            res = (_Serialize( iter->first ) and _Serialize( iter->second ));
        }
        return res;
    }


    template <typename T, typename SetType>
    bool  Serializer::_SerializeSet (const SetType &set)
    {
        CHECK_ERR( set.size() <= MaxArrayLength );

        bool    res = stream.Write( CheckCast<uint>(set.size()) );

        for (auto iter = set.begin(); res & (iter != set.end()); ++iter)
        {
            res = _Serialize( *iter );
        }
        return res;
    }


    template <usize I, typename ...Types>
    bool  Serializer::_RecursiveSrializeTuple (const Tuple<Types...> &tuple)
    {
        bool    res = _Serialize( tuple.template Get<I>() );

        if constexpr( I+1 < CountOf<Types...>() )
            return res and _RecursiveSrializeTuple< I+1 >( tuple );
        else
            return res;
    }


    template <typename ...Types>
    bool  Serializer::_Serialize (const Tuple<Types...> &tuple)
    {
        return _RecursiveSrializeTuple<0>( tuple );
    }


    template <usize I, usize S, typename ...Types>
    bool  Serializer::_RecursiveSrializeTupleArray (const FixedTupleArray<S, Types...> &arr)
    {
        auto    view    = arr.template get<I>();
        bool    res     = true;

        for (usize i = 0; res & (i < view.size()); ++i) {
            res = _Serialize( view[i] );
        }

        if constexpr( I+1 < CountOf<Types...>() )
            return res and _RecursiveSrializeTupleArray< I+1 >( arr );
        else
            return res;
    }


    template <usize S, typename ...Types>
    bool  Serializer::_Serialize (const FixedTupleArray<S,Types...> &arr)
    {
        CHECK_ERR( arr.size() <= MaxArrayLength );
        return  stream.Write( CheckCast<uint>(arr.size()) ) and
                _RecursiveSrializeTupleArray<0>( arr );
    }


    template <typename T>
    bool  Serializer::_Serialize (const Optional<T> &value)
    {
        bool    res = stream.Write( value.has_value() );

        if ( value )
            return res and _Serialize( *value );

        return res;
    }


    template <typename ...Types>
    bool  Serializer::_Serialize (const Union<Types...> &un)
    {
        return  stream.Write( CheckCast<uint>(un.index()) ) and
                _RecursiveSrializeUnion< Types... >( un );
    }


    template <typename T, typename ...Args, typename ...Types>
    bool  Serializer::_RecursiveSrializeUnion (const Union<Types...> &un)
    {
        if ( auto* value = UnionGet<T>( un ))
            return _Serialize( *value );

        if constexpr( CountOf<Args...>() > 0 )
            return _RecursiveSrializeUnion< Args... >( un );
        else
            return false;
    }


    template <typename V, typename D, typename S>
    bool  Serializer::_Serialize (const PhysicalQuantity<V,D,S> &value)
    {
        return _Serialize( value.GetNonScaled() );
    }


    template <typename Qt, int I, glm::qualifier Ql>
    bool  Serializer::_Serialize (const PhysicalQuantityVec<Qt,I,Ql> &value)
    {
        return _Serialize( typename PhysicalQuantityVec<Qt,I,Ql>::ValVec_t{value} );
    }


} // AE::Serializing
