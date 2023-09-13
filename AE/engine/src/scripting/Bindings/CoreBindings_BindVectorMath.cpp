// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "scripting/Bindings/CoreBindings.h"
#include "scripting/Impl/ClassBinder.h"
#include "scripting/Impl/ScriptEngine.inl.h"

namespace AE::Scripting
{
namespace
{

    template <typename T>
    struct InitVecFields
    {};

/*
=================================================
    InitVecFields (Vec2)
=================================================
*/
    template <typename T>
    struct InitVecFields< PackedVec<T,2> >
    {
    private:
        static void  _Ctor1 (void *mem, const T value)
        {
            PlacementNew< PackedVec<T,2> >( OUT mem, value );
        }

        template <typename B>
        static void  _Ctor2 (void *mem, const PackedVec<B,2> &value)
        {
            PlacementNew< PackedVec<T,2> >( OUT mem, PackedVec<T,2>{value} );
        }

        template <typename B>
        static void  _Ctor3 (void *mem, const PackedVec<B,3> &value)
        {
            PlacementNew< PackedVec<T,2> >( OUT mem, PackedVec<T,2>{value} );
        }

        template <typename B>
        static void  _Ctor4 (void *mem, const PackedVec<B,4> &value)
        {
            PlacementNew< PackedVec<T,2> >( OUT mem, PackedVec<T,2>{value} );
        }

        static void  _CtorArg2 (void *mem, T x, T y)
        {
            PlacementNew< PackedVec<T,2> >( OUT mem, x, y );
        }

    public:
        static void  Init (ClassBinder< PackedVec<T,2> > &binder)
        {
            using Vec_t = PackedVec< T, 2 >;

            binder.AddProperty( &Vec_t::x, "x" );
            binder.AddProperty( &Vec_t::y, "y" );

            binder.AddConstructor( &_Ctor1,     {"v"} );
            binder.AddConstructor( &_Ctor3<T>,  {"v3"} );
            binder.AddConstructor( &_Ctor4<T>,  {"v4"} );
            binder.AddConstructor( &_CtorArg2,  {"x", "y"} );

            if constexpr( not IsSameTypes< T, short >)
            {
                binder.AddConstructor( &_Ctor2<short>,  {"v2"} );
                binder.AddConstructor( &_Ctor3<short>,  {"v3"} );
                binder.AddConstructor( &_Ctor4<short>,  {"v4"} );
            }
            if constexpr( not IsSameTypes< T, ushort >)
            {
                binder.AddConstructor( &_Ctor2<ushort>, {"v2"} );
                binder.AddConstructor( &_Ctor3<ushort>, {"v3"} );
                binder.AddConstructor( &_Ctor4<ushort>, {"v4"} );
            }
            if constexpr( not IsSameTypes< T, int >)
            {
                binder.AddConstructor( &_Ctor2<int>,    {"v2"} );
                binder.AddConstructor( &_Ctor3<int>,    {"v3"} );
                binder.AddConstructor( &_Ctor4<int>,    {"v4"} );
            }
            if constexpr( not IsSameTypes< T, uint >)
            {
                binder.AddConstructor( &_Ctor2<uint>,   {"v2"} );
                binder.AddConstructor( &_Ctor3<uint>,   {"v3"} );
                binder.AddConstructor( &_Ctor4<uint>,   {"v4"} );
            }
            if constexpr( not IsSameTypes< T, float >)
            {
                binder.AddConstructor( &_Ctor2<float>,  {"v2"} );
                binder.AddConstructor( &_Ctor3<float>,  {"v3"} );
                binder.AddConstructor( &_Ctor4<float>,  {"v4"} );
            }
        }
    };

/*
=================================================
    InitVecFields (Vec3)
=================================================
*/
    template <typename T>
    struct InitVecFields< PackedVec<T,3> >
    {
    private:
        static void  _Ctor1 (void *mem, const T value)
        {
            PlacementNew< PackedVec<T,3> >( OUT mem, value );
        }

        template <typename B>
        static void  _Ctor2 (void *mem, const PackedVec<B,2> &value)
        {
            PlacementNew< PackedVec<T,3> >( OUT mem, PackedVec<T,3>{value.x, value.y, B{0}} );
        }

        template <typename B>
        static void  _Ctor3 (void *mem, const PackedVec<B,3> &value)
        {
            PlacementNew< PackedVec<T,3> >( OUT mem, PackedVec<T,3>{value} );
        }

        template <typename B>
        static void  _Ctor4 (void *mem, const PackedVec<B,4> &value)
        {
            PlacementNew< PackedVec<T,3> >( OUT mem, PackedVec<T,3>{value.x, value.y, value.z} );
        }

        static void  _CtorArg2 (void *mem, const PackedVec<T,2> &xy, T z)
        {
            PlacementNew< PackedVec<T,3> >( OUT mem, xy, z );
        }

        static void  _CtorArg3 (void *mem, T x, T y, T z)
        {
            PlacementNew< PackedVec<T,3> >( OUT mem, x, y, z );
        }

    public:
        static void  Init (ClassBinder< PackedVec<T,3> > &binder)
        {
            using Vec_t = PackedVec< T, 3 >;

            binder.AddProperty( &Vec_t::x, "x" );
            binder.AddProperty( &Vec_t::y, "y" );
            binder.AddProperty( &Vec_t::z, "z" );

            binder.AddConstructor( &_Ctor1,     {"v"} );
            binder.AddConstructor( &_Ctor2<T>,  {"v2"} );
            binder.AddConstructor( &_Ctor4<T>,  {"v4"} );
            binder.AddConstructor( &_CtorArg2,  {"xy", "z"} );
            binder.AddConstructor( &_CtorArg3,  {"x", "y", "z"} );

            if constexpr( not IsSameTypes< T, short >)
            {
                binder.AddConstructor( &_Ctor2<short>,  {"v2"} );
                binder.AddConstructor( &_Ctor3<short>,  {"v3"} );
                binder.AddConstructor( &_Ctor4<short>,  {"v4"} );
            }
            if constexpr( not IsSameTypes< T, ushort >)
            {
                binder.AddConstructor( &_Ctor2<ushort>, {"v2"} );
                binder.AddConstructor( &_Ctor3<ushort>, {"v3"} );
                binder.AddConstructor( &_Ctor4<ushort>, {"v4"} );
            }
            if constexpr( not IsSameTypes< T, int >)
            {
                binder.AddConstructor( &_Ctor2<int>,    {"v2"} );
                binder.AddConstructor( &_Ctor3<int>,    {"v3"} );
                binder.AddConstructor( &_Ctor4<int>,    {"v4"} );
            }
            if constexpr( not IsSameTypes< T, uint >)
            {
                binder.AddConstructor( &_Ctor2<uint>,   {"v2"} );
                binder.AddConstructor( &_Ctor3<uint>,   {"v3"} );
                binder.AddConstructor( &_Ctor4<uint>,   {"v4"} );
            }
            if constexpr( not IsSameTypes< T, float >)
            {
                binder.AddConstructor( &_Ctor2<float>,  {"v2"} );
                binder.AddConstructor( &_Ctor3<float>,  {"v3"} );
                binder.AddConstructor( &_Ctor4<float>,  {"v4"} );
            }
        }
    };

/*
=================================================
    InitVecFields (Vec4)
=================================================
*/
    template <typename T>
    struct InitVecFields< PackedVec<T,4> >
    {
    private:
        static void  _Ctor1 (void *mem, const T value)
        {
            PlacementNew< PackedVec<T,4> >( OUT mem, value );
        }

        template <typename B>
        static void  _Ctor2 (void *mem, const PackedVec<B,2> &value)
        {
            PlacementNew< PackedVec<T,4> >( OUT mem, PackedVec<T,4>{value.x, value.y, B{0}, B{0}} );
        }

        template <typename B>
        static void  _Ctor3 (void *mem, const PackedVec<B,3> &value)
        {
            PlacementNew< PackedVec<T,4> >( OUT mem, PackedVec<T,4>{value.x, value.y, value.z, B{0}} );
        }

        template <typename B>
        static void  _Ctor4 (void *mem, const PackedVec<B,4> &value)
        {
            PlacementNew< PackedVec<T,4> >( OUT mem, PackedVec<T,4>{value} );
        }

        static void  _CtorArg2 (void *mem, const PackedVec<T,2> &xy, const PackedVec<T,2> &zw)
        {
            PlacementNew< PackedVec<T,4> >( OUT mem, xy, zw );
        }

        static void  _CtorArg3 (void *mem, const PackedVec<T,3> &xyz, T w)
        {
            PlacementNew< PackedVec<T,4> >( OUT mem, xyz, w );
        }

        static void  _CtorArg4 (void *mem, T x, T y, T z, T w)
        {
            PlacementNew< PackedVec<T,4> >( OUT mem, x, y, z, w );
        }

    public:
        static void  Init (ClassBinder< PackedVec<T,4> > &binder)
        {
            using Vec_t = PackedVec< T, 4 >;

            binder.AddProperty( &Vec_t::x, "x" );
            binder.AddProperty( &Vec_t::y, "y" );
            binder.AddProperty( &Vec_t::z, "z" );
            binder.AddProperty( &Vec_t::w, "w" );

            binder.AddConstructor( &_Ctor1,     {"v"} );
            binder.AddConstructor( &_Ctor2<T>,  {"v2"} );
            binder.AddConstructor( &_Ctor3<T>,  {"v3"} );
            binder.AddConstructor( &_CtorArg2,  {"xy", "zw"} );
            binder.AddConstructor( &_CtorArg3,  {"xyz", "w"} );
            binder.AddConstructor( &_CtorArg4,  {"x", "y", "z", "w"} );

            if constexpr( not IsSameTypes< T, short >)
            {
                binder.AddConstructor( &_Ctor2<short>,  {"v2"} );
                binder.AddConstructor( &_Ctor3<short>,  {"v3"} );
                binder.AddConstructor( &_Ctor4<short>,  {"v4"} );
            }
            if constexpr( not IsSameTypes< T, ushort >)
            {
                binder.AddConstructor( &_Ctor2<ushort>, {"v2"} );
                binder.AddConstructor( &_Ctor3<ushort>, {"v3"} );
                binder.AddConstructor( &_Ctor4<ushort>, {"v4"} );
            }
            if constexpr( not IsSameTypes< T, int >)
            {
                binder.AddConstructor( &_Ctor2<int>,    {"v2"} );
                binder.AddConstructor( &_Ctor3<int>,    {"v3"} );
                binder.AddConstructor( &_Ctor4<int>,    {"v4"} );
            }
            if constexpr( not IsSameTypes< T, uint >)
            {
                binder.AddConstructor( &_Ctor2<uint>,   {"v2"} );
                binder.AddConstructor( &_Ctor3<uint>,   {"v3"} );
                binder.AddConstructor( &_Ctor4<uint>,   {"v4"} );
            }
            if constexpr( not IsSameTypes< T, float >)
            {
                binder.AddConstructor( &_Ctor2<float>,  {"v2"} );
                binder.AddConstructor( &_Ctor3<float>,  {"v3"} );
                binder.AddConstructor( &_Ctor4<float>,  {"v4"} );
            }
        }
    };

/*
=================================================
    VecFunc
=================================================
*/
    struct VecFunc
    {
        template <typename V, typename R>
        using FloatOnly = EnableIf<IsFloatPoint<typename V::value_type>, R>;

        template <typename V, typename R>
        using ExceptFloat = EnableIf<not IsFloatPoint<typename V::value_type>, R>;

        template <typename V>
        static bool  Equal (const V &lhs, const V &rhs) {
            return Math::All( lhs == rhs );
        }

        template <typename V>
        static int  Cmp (const V &lhs, const V &rhs) {
            for (uint i = 0; i < VecSize<V>; ++i) {
                if ( lhs[i] > rhs[i] )  return +1;
                if ( lhs[i] < rhs[i] )  return -1;
            }
            return 0;
        }

        template <typename V> static V   Neg (const V& v)                                                       { return -v; }

        template <typename V> static V&  Add_a_v (V& lhs, const V &rhs)                                         { return lhs += rhs; }
        template <typename V> static V&  Add_a_s (V& lhs, typename V::value_type rhs)                           { return lhs += rhs; }
        template <typename V> static V   Add_v_v (const V& lhs, const V &rhs)                                   { return lhs + rhs; }
        template <typename V> static V   Add_v_s (const V &lhs, typename V::value_type rhs)                     { return lhs + rhs; }
        template <typename V> static V   Add_s_v (typename V::value_type lhs, const V &rhs)                     { return lhs + rhs; }

        template <typename V> static V&  Sub_a_v (V& lhs, const V &rhs)                                         { return lhs -= rhs; }
        template <typename V> static V&  Sub_a_s (V& lhs, typename V::value_type rhs)                           { return lhs -= rhs; }
        template <typename V> static V   Sub_v_v (const V& lhs, const V &rhs)                                   { return lhs - rhs; }
        template <typename V> static V   Sub_v_s (const V &lhs, typename V::value_type rhs)                     { return lhs - rhs; }
        template <typename V> static V   Sub_s_v (typename V::value_type lhs, const V &rhs)                     { return lhs - rhs; }

        template <typename V> static V&  Mul_a_v (V& lhs, const V &rhs)                                         { return lhs *= rhs; }
        template <typename V> static V&  Mul_a_s (V& lhs, typename V::value_type rhs)                           { return lhs *= rhs; }
        template <typename V> static V   Mul_v_v (const V& lhs, const V &rhs)                                   { return lhs * rhs; }
        template <typename V> static V   Mul_v_s (const V &lhs, typename V::value_type rhs)                     { return lhs * rhs; }
        template <typename V> static V   Mul_s_v (typename V::value_type lhs, const V &rhs)                     { return lhs * rhs; }

        template <typename V> static V&  Div_a_v (V& lhs, const V &rhs)                                         { return lhs /= rhs; }
        template <typename V> static V&  Div_a_s (V& lhs, typename V::value_type rhs)                           { return lhs /= rhs; }
        template <typename V> static V   Div_v_v (const V& lhs, const V &rhs)                                   { return lhs / rhs; }
        template <typename V> static V   Div_v_s (const V &lhs, typename V::value_type rhs)                     { return lhs / rhs; }
        template <typename V> static V   Div_s_v (typename V::value_type lhs, const V &rhs)                     { return lhs / rhs; }

        template <typename V> static FloatOnly<V, V&>  Mod_a_v (V& lhs, const V &rhs)                           { return lhs = glm::mod( lhs, rhs ); }
        template <typename V> static FloatOnly<V, V&>  Mod_a_s (V& lhs, typename V::value_type rhs)             { return lhs = glm::mod( lhs, rhs ); }
        template <typename V> static FloatOnly<V, V>   Mod_v_v (const V& lhs, const V &rhs)                     { return glm::mod( lhs, rhs ); }
        template <typename V> static FloatOnly<V, V>   Mod_v_s (const V &lhs, typename V::value_type rhs)       { return glm::mod( lhs, rhs ); }
        template <typename V> static FloatOnly<V, V>   Mod_s_v (typename V::value_type lhs, const V &rhs)       { return glm::mod( V(lhs), rhs ); }

        template <typename V> static ExceptFloat<V, V&>  Mod_a_v (V& lhs, const V &rhs)                         { return lhs %= rhs; }
        template <typename V> static ExceptFloat<V, V&>  Mod_a_s (V& lhs, typename V::value_type rhs)           { return lhs %= rhs; }
        template <typename V> static ExceptFloat<V, V>   Mod_v_v (const V& lhs, const V &rhs)                   { return lhs % rhs; }
        template <typename V> static ExceptFloat<V, V>   Mod_v_s (const V &lhs, typename V::value_type rhs)     { return lhs % rhs; }
        template <typename V> static ExceptFloat<V, V>   Mod_s_v (typename V::value_type lhs, const V &rhs)     { return lhs % rhs; }

        template <typename V> static V&  And_a_v (V& lhs, const V &rhs)                                         { return lhs &= rhs; }
        template <typename V> static V&  And_a_s (V& lhs, typename V::value_type rhs)                           { return lhs &= rhs; }
        template <typename V> static V   And_v_v (const V& lhs, const V &rhs)                                   { return lhs & rhs; }
        template <typename V> static V   And_v_s (const V &lhs, typename V::value_type rhs)                     { return lhs & rhs; }
        template <typename V> static V   And_s_v (typename V::value_type lhs, const V &rhs)                     { return lhs & rhs; }

        template <typename V> static V&  Or_a_v (V& lhs, const V &rhs)                                          { return lhs |= rhs; }
        template <typename V> static V&  Or_a_s (V& lhs, typename V::value_type rhs)                            { return lhs |= rhs; }
        template <typename V> static V   Or_v_v (const V& lhs, const V &rhs)                                    { return lhs | rhs; }
        template <typename V> static V   Or_v_s (const V &lhs, typename V::value_type rhs)                      { return lhs | rhs; }
        template <typename V> static V   Or_s_v (typename V::value_type lhs, const V &rhs)                      { return lhs | rhs; }

        template <typename V> static V&  Xor_a_v (V& lhs, const V &rhs)                                         { return lhs ^= rhs; }
        template <typename V> static V&  Xor_a_s (V& lhs, typename V::value_type rhs)                           { return lhs ^= rhs; }
        template <typename V> static V   Xor_v_v (const V& lhs, const V &rhs)                                   { return lhs ^ rhs; }
        template <typename V> static V   Xor_v_s (const V &lhs, typename V::value_type rhs)                     { return lhs ^ rhs; }
        template <typename V> static V   Xor_s_v (typename V::value_type lhs, const V &rhs)                     { return lhs ^ rhs; }

        template <typename V> static V&  ShiftL_a_v (V& lhs, const V &rhs)                                      { return lhs <<= rhs; }
        template <typename V> static V&  ShiftL_a_s (V& lhs, typename V::value_type rhs)                        { return lhs <<= rhs; }
        template <typename V> static V   ShiftL_v_v (const V& lhs, const V &rhs)                                { return lhs << rhs; }
        template <typename V> static V   ShiftL_v_s (const V &lhs, typename V::value_type rhs)                  { return lhs << rhs; }
        template <typename V> static V   ShiftL_s_v (typename V::value_type lhs, const V &rhs)                  { return lhs << rhs; }

        template <typename V> static V&  ShiftR_a_v (V& lhs, const V &rhs)                                      { return lhs >>= rhs; }
        template <typename V> static V&  ShiftR_a_s (V& lhs, typename V::value_type rhs)                        { return lhs >>= rhs; }
        template <typename V> static V   ShiftR_v_v (const V& lhs, const V &rhs)                                { return lhs >> rhs; }
        template <typename V> static V   ShiftR_v_s (const V &lhs, typename V::value_type rhs)                  { return lhs >> rhs; }
        template <typename V> static V   ShiftR_s_v (typename V::value_type lhs, const V &rhs)                  { return lhs >> rhs; }

        template <typename V> static bool  All (const V &x)                                                     { return Math::All( x ); }
        template <typename V> static bool  Any (const V &x)                                                     { return Math::Any( x ); }

        template <typename V> static V  ToSNorm (const V& x)                                                    { return Math::ToSNorm( x ); }
        template <typename V> static V  ToUNorm (const V& x)                                                    { return Math::ToUNorm( x ); }

        template <typename V> static typename V::value_type  Sum (const V& x)                                   { return Math::Sum<typename V::value_type>( x ); }
        template <typename V> static typename V::value_type  Area (const V& x)                                  { return Math::Area( x ); }
        template <typename V> static typename V::value_type  Dot (const V& x, const V& y)                       { return Math::Dot( x, y ); }
        template <typename V> static typename V::value_type  Length (const V& x)                                { return Math::Length( x ); }
        template <typename V> static typename V::value_type  LengthSq (const V& x)                              { return Math::LengthSq( x ); }
        template <typename V> static typename V::value_type  Distance (const V& x, const V& y)                  { return Math::Distance( x, y ); }
        template <typename V> static typename V::value_type  DistanceSq (const V& x, const V& y)                { return Math::DistanceSq( x, y ); }
        template <typename V> static typename V::value_type  VecToLinear (const V& x, const V& y)               { return Math::VecToLinear( x, y ); }

        template <typename V> static V  Cross (const V &x, const V &y)                                          { return Math::Cross( x, y ); }
        template <typename V> static V  Min (const V &x, const V &y)                                            { return Math::Min( x, y ); }
        template <typename V> static V  Max (const V &x, const V &y)                                            { return Math::Max( x, y ); }
        template <typename V> static V  Clamp_1 (const V &x, const V &y, const V &z)                            { return Math::Clamp( x, y, z ); }
        template <typename V> static V  Clamp_2 (const V &x, typename V::value_type y, typename V::value_type z){ return Math::Clamp( x, y, z ); }
        template <typename V> static V  Wrap_1 (const V &x, const V &y, const V &z)                             { return Math::Wrap( x, y, z ); }
        template <typename V> static V  Wrap_2 (const V &x, typename V::value_type y, typename V::value_type z) { return Math::Wrap( x, y, z ); }
        template <typename V> static V  MirroredWrap_1 (const V &x, const V &y, const V &z)                     { return Math::MirroredWrap( x, y, z ); }
        template <typename V> static V  MirroredWrap_2 (const V &x, typename V::value_type y, typename V::value_type z) { return Math::MirroredWrap( x, y, z ); }

        template <typename V> static V  Normalize (const V& x)                                                  { return Math::Normalize( x ); }
        template <typename V> static V  Floor (const V& x)                                                      { return Math::Floor( x ); }
        template <typename V> static V  Ceil (const V& x)                                                       { return Math::Ceil( x ); }
        template <typename V> static V  Round (const V& x)                                                      { return Math::Round( x ); }
        template <typename V> static V  Fract (const V& x)                                                      { return Math::Fract( x ); }
        template <typename V> static V  Sqrt (const V& x)                                                       { return Math::Sqrt( x ); }
        template <typename V> static V  Abs (const V& x)                                                        { return Math::Abs( x ); }
        template <typename V> static V  Lerp (const V& x, const V& y, typename V::value_type f)                 { return Math::Lerp( x, y, f ); }
        template <typename V> static V  Ln (const V& x)                                                         { return Math::Ln( x ); }
        template <typename V> static V  Log2 (const V& x)                                                       { return Math::Log2( x ); }
        template <typename V> static V  Log_1 (const V& x, const V& y)                                          { return Math::Log( x, y ); }
        template <typename V> static V  Log_2 (const V& x, typename V::value_type y)                            { return Math::Log( x, y ); }
        template <typename V> static V  Pow_1 (const V& x, const V& y)                                          { return Math::Pow( x, y ); }
        template <typename V> static V  Pow_2 (const V& x, typename V::value_type y)                            { return Math::Pow( x, y ); }
        template <typename V> static V  Exp (const V& x)                                                        { return Math::Exp( x ); }
        template <typename V> static V  Exp2 (const V& x)                                                       { return Math::Exp2( x ); }
        template <typename V> static V  Exp10 (const V& x)                                                      { return Math::Exp10( x ); }
    };

/*
=================================================
    BindBoolVec
=================================================
*/
    template <typename T>
    static void  BindBoolVec (ClassBinder<T> &binder, const ScriptEnginePtr &se)
    {
        using Vec_t = T;

        binder.Operators()
            .Unary(     EUnaryOperator::Not,    static_cast<T (*)(const T&) >(&glm::operator !) )
            .Equals(                            &VecFunc::template Equal< Vec_t >)
            .Compare(                           &VecFunc::template Cmp< Vec_t >);

        se->AddFunction( &VecFunc::template All< Vec_t >,   "All",  {"x"} );
        se->AddFunction( &VecFunc::template Any< Vec_t >,   "Any",  {"x"} );
    }

/*
=================================================
    BindIntFloatVec
=================================================
*/
    template <typename T>
    static void  BindIntFloatVec (ClassBinder<T> &binder, const ScriptEnginePtr &se)
    {
        using Vec_t     = T;
        using Scalar_t  = typename T::value_type;

        if constexpr( IsSameTypes< Scalar_t, int >  or
                      IsSameTypes< Scalar_t, uint > or
                      IsSameTypes< Scalar_t, float >)
        {
            if constexpr( IsSignedInteger<Scalar_t> or IsFloatPoint<Scalar_t> )
            {
                se->AddFunction( &VecFunc::template Abs< Vec_t >,               "Abs",          {"x"} );
                se->AddFunction( &VecFunc::template MirroredWrap_1< Vec_t >,    "MirroredWrap", {"x", "min", "max"} );
                se->AddFunction( &VecFunc::template MirroredWrap_2< Vec_t >,    "MirroredWrap", {"x", "min", "max"} );

                binder.Operators()
                    .Unary( EUnaryOperator::Inverse, &VecFunc::template Neg< Vec_t >);
            }

            se->AddFunction( &VecFunc::template Sum< Vec_t >,               "Sum",          {"x"} );
            se->AddFunction( &VecFunc::template Area< Vec_t >,              "Area",         {"x"} );
            se->AddFunction( &VecFunc::template Min< Vec_t >,               "Min",          {"x", "y"} );
            se->AddFunction( &VecFunc::template Max< Vec_t >,               "Max",          {"x", "y"} );
            se->AddFunction( &VecFunc::template Clamp_1< Vec_t >,           "Clamp",        {"val", "min", "max"} );
            se->AddFunction( &VecFunc::template Clamp_2< Vec_t >,           "Clamp",        {"val", "min", "max"} );
            se->AddFunction( &VecFunc::template Wrap_1< Vec_t >,            "Wrap",         {"val", "min", "max"} );
            se->AddFunction( &VecFunc::template Wrap_2< Vec_t >,            "Wrap",         {"val", "min", "max"} );
            se->AddFunction( &VecFunc::template VecToLinear< Vec_t >,       "VecToLinear",  {"pos", "dim"} );
        }
    }

/*
=================================================
    BindIntVec
=================================================
*/
    template <typename T>
    static void  BindIntVec (ClassBinder<T> &binder, const ScriptEnginePtr &se)
    {
        using Vec_t = T;

        binder.Operators()
            .BinaryAssign(  EBinaryOperator::Add, &VecFunc::template Add_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::Add, &VecFunc::template Add_a_s< Vec_t >)
            .Binary(        EBinaryOperator::Add, &VecFunc::template Add_v_v< Vec_t >)
            .Binary(        EBinaryOperator::Add, &VecFunc::template Add_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::Add, &VecFunc::template Add_s_v< Vec_t >)

            .BinaryAssign(  EBinaryOperator::Sub, &VecFunc::template Sub_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::Sub, &VecFunc::template Sub_a_s< Vec_t >)
            .Binary(        EBinaryOperator::Sub, &VecFunc::template Sub_v_v< Vec_t >)
            .Binary(        EBinaryOperator::Sub, &VecFunc::template Sub_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::Sub, &VecFunc::template Sub_s_v< Vec_t >)

            .BinaryAssign(  EBinaryOperator::Mul, &VecFunc::template Mul_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::Mul, &VecFunc::template Mul_a_s< Vec_t >)
            .Binary(        EBinaryOperator::Mul, &VecFunc::template Mul_v_v< Vec_t >)
            .Binary(        EBinaryOperator::Mul, &VecFunc::template Mul_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::Mul, &VecFunc::template Mul_s_v< Vec_t >)

            .BinaryAssign(  EBinaryOperator::Div, &VecFunc::template Div_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::Div, &VecFunc::template Div_a_s< Vec_t >)
            .Binary(        EBinaryOperator::Div, &VecFunc::template Div_v_v< Vec_t >)
            .Binary(        EBinaryOperator::Div, &VecFunc::template Div_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::Div, &VecFunc::template Div_s_v< Vec_t >)

            .BinaryAssign(  EBinaryOperator::Mod, &VecFunc::template Mod_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::Mod, &VecFunc::template Mod_a_s< Vec_t >)
            .Binary(        EBinaryOperator::Mod, &VecFunc::template Mod_v_v< Vec_t >)
            .Binary(        EBinaryOperator::Mod, &VecFunc::template Mod_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::Mod, &VecFunc::template Mod_s_v< Vec_t >)

            .BinaryAssign(  EBinaryOperator::And, &VecFunc::template And_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::And, &VecFunc::template And_a_s< Vec_t >)
            .Binary(        EBinaryOperator::And, &VecFunc::template And_v_v< Vec_t >)
            .Binary(        EBinaryOperator::And, &VecFunc::template And_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::And, &VecFunc::template And_s_v< Vec_t >)

            .BinaryAssign(  EBinaryOperator::Or,  &VecFunc::template Or_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::Or,  &VecFunc::template Or_a_s< Vec_t >)
            .Binary(        EBinaryOperator::Or,  &VecFunc::template Or_v_v< Vec_t >)
            .Binary(        EBinaryOperator::Or,  &VecFunc::template Or_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::Or,  &VecFunc::template Or_s_v< Vec_t >)

            .BinaryAssign(  EBinaryOperator::Xor, &VecFunc::template Xor_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::Xor, &VecFunc::template Xor_a_s< Vec_t >)
            .Binary(        EBinaryOperator::Xor, &VecFunc::template Xor_v_v< Vec_t >)
            .Binary(        EBinaryOperator::Xor, &VecFunc::template Xor_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::Xor, &VecFunc::template Xor_s_v< Vec_t >)

            .BinaryAssign(  EBinaryOperator::ShiftLeft, &VecFunc::template ShiftL_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::ShiftLeft, &VecFunc::template ShiftL_a_s< Vec_t >)
            .Binary(        EBinaryOperator::ShiftLeft, &VecFunc::template ShiftL_v_v< Vec_t >)
            .Binary(        EBinaryOperator::ShiftLeft, &VecFunc::template ShiftL_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::ShiftLeft, &VecFunc::template ShiftL_s_v< Vec_t >)

            .BinaryAssign(  EBinaryOperator::ShiftRight, &VecFunc::template ShiftR_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::ShiftRight, &VecFunc::template ShiftR_a_s< Vec_t >)
            .Binary(        EBinaryOperator::ShiftRight, &VecFunc::template ShiftR_v_v< Vec_t >)
            .Binary(        EBinaryOperator::ShiftRight, &VecFunc::template ShiftR_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::ShiftRight, &VecFunc::template ShiftR_s_v< Vec_t >)

            .Equals( &VecFunc::template Equal< Vec_t >)
            .Compare( &VecFunc::template Cmp< Vec_t >);

        BindIntFloatVec( binder, se );
    }

/*
=================================================
    BindFloatVec
=================================================
*/
    template <typename T>
    static void  BindFloatVec (ClassBinder<T> &binder, const ScriptEnginePtr &se)
    {
        using Vec_t = T;

        binder.Operators()
            .BinaryAssign(  EBinaryOperator::Add, &VecFunc::template Add_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::Add, &VecFunc::template Add_a_s< Vec_t >)
            .Binary(        EBinaryOperator::Add, &VecFunc::template Add_v_v< Vec_t >)
            .Binary(        EBinaryOperator::Add, &VecFunc::template Add_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::Add, &VecFunc::template Add_s_v< Vec_t >)

            .BinaryAssign(  EBinaryOperator::Sub, &VecFunc::template Sub_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::Sub, &VecFunc::template Sub_a_s< Vec_t >)
            .Binary(        EBinaryOperator::Sub, &VecFunc::template Sub_v_v< Vec_t >)
            .Binary(        EBinaryOperator::Sub, &VecFunc::template Sub_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::Sub, &VecFunc::template Sub_s_v< Vec_t >)

            .BinaryAssign(  EBinaryOperator::Mul, &VecFunc::template Mul_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::Mul, &VecFunc::template Mul_a_s< Vec_t >)
            .Binary(        EBinaryOperator::Mul, &VecFunc::template Mul_v_v< Vec_t >)
            .Binary(        EBinaryOperator::Mul, &VecFunc::template Mul_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::Mul, &VecFunc::template Mul_s_v< Vec_t >)

            .BinaryAssign(  EBinaryOperator::Div, &VecFunc::template Div_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::Div, &VecFunc::template Div_a_s< Vec_t >)
            .Binary(        EBinaryOperator::Div, &VecFunc::template Div_v_v< Vec_t >)
            .Binary(        EBinaryOperator::Div, &VecFunc::template Div_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::Div, &VecFunc::template Div_s_v< Vec_t >)

            .BinaryAssign(  EBinaryOperator::Mod, &VecFunc::template Mod_a_v< Vec_t >)
            .BinaryAssign(  EBinaryOperator::Mod, &VecFunc::template Mod_a_s< Vec_t >)
            .Binary(        EBinaryOperator::Mod, &VecFunc::template Mod_v_v< Vec_t >)
            .Binary(        EBinaryOperator::Mod, &VecFunc::template Mod_v_s< Vec_t >)
            .BinaryRH(      EBinaryOperator::Mod, &VecFunc::template Mod_s_v< Vec_t >)

            .Equals( &VecFunc::template Equal< Vec_t >)
            .Compare( &VecFunc::template Cmp< Vec_t >);

        BindIntFloatVec( binder, se );

        if constexpr( VecSize<Vec_t> == 3 ) {
            se->AddFunction( &VecFunc::template Cross< Vec_t >,     "Cross",        {"x", "y"} );
        }

        se->AddFunction( &VecFunc::template ToSNorm< Vec_t >,       "ToSNorm",      {"x"} );
        se->AddFunction( &VecFunc::template ToUNorm< Vec_t >,       "ToUNorm",      {"x"} );

        se->AddFunction( &VecFunc::template Dot< Vec_t >,           "Dot",          {"x", "y"} );
        se->AddFunction( &VecFunc::template Lerp< Vec_t >,          "Lerp",         {"x", "y", "factor"} );
        se->AddFunction( &VecFunc::template Normalize< Vec_t >,     "Normalize",    {"x"} );
        se->AddFunction( &VecFunc::template Floor< Vec_t >,         "Floor",        {"x"} );
        se->AddFunction( &VecFunc::template Ceil< Vec_t >,          "Ceil",         {"x"} );
        se->AddFunction( &VecFunc::template Round< Vec_t >,         "Round",        {"x"} );
        se->AddFunction( &VecFunc::template Fract< Vec_t >,         "Fract",        {"x"} );
        se->AddFunction( &VecFunc::template Sqrt< Vec_t >,          "Sqrt",         {"x"} );
        se->AddFunction( &VecFunc::template Ln< Vec_t >,            "Ln",           {"x"} );
        se->AddFunction( &VecFunc::template Log2< Vec_t >,          "Log2",         {"x"} );
        se->AddFunction( &VecFunc::template Log_1< Vec_t >,         "Log",          {"x", "base"} );
        se->AddFunction( &VecFunc::template Log_2< Vec_t >,         "Log",          {"x", "base"} );
        se->AddFunction( &VecFunc::template Pow_1< Vec_t >,         "Pow",          {"x", "pow"} );
        se->AddFunction( &VecFunc::template Pow_2< Vec_t >,         "Pow",          {"x", "pow"} );
        se->AddFunction( &VecFunc::template Exp< Vec_t >,           "Exp",          {"x"} );
        se->AddFunction( &VecFunc::template Exp2< Vec_t >,          "Exp2",         {"x"} );
        se->AddFunction( &VecFunc::template Exp10< Vec_t >,         "Exp10",        {"x"} );
        se->AddFunction( &VecFunc::template Length< Vec_t >,        "Length",       {"x"} );
        se->AddFunction( &VecFunc::template LengthSq< Vec_t >,      "LengthSq", {"x"} );
        se->AddFunction( &VecFunc::template Distance< Vec_t >,      "Distance",     {"x", "y"} );
        se->AddFunction( &VecFunc::template DistanceSq< Vec_t >,    "DistanceSq",   {"x", "y"} );
    }

/*
=================================================
    DefineVector_Func
=================================================
*/
    struct DefineVector_Func
    {
        ScriptEnginePtr _se;

        explicit DefineVector_Func (const ScriptEnginePtr &se) : _se{se}
        {}

        template <typename T, usize Index>
        void  operator () ()
        {
            ClassBinder<T>  binder{ _se };

            binder.CreateClassValue();
        }
    };

/*
=================================================
    BindVector_Func
=================================================
*/
    struct BindVector_Func
    {
        ScriptEnginePtr _se;

        explicit BindVector_Func (const ScriptEnginePtr &se) : _se{se}
        {}

        template <typename T, usize Index>
        void  operator () ()
        {
            using Value_t = typename T::value_type;

            ClassBinder<T>  binder{ _se };

            InitVecFields<T>::Init( binder );

            if constexpr( IsSameTypes< Value_t, bool >) {
                BindBoolVec( binder, _se );
            } else
            if constexpr( IsInteger< Value_t >) {
                BindIntVec( binder, _se );
            } else
            if constexpr( IsFloatPoint< Value_t >) {
                BindFloatVec( binder, _se );
            }
        }
    };

} // namespace


/*
=================================================
    BindVectorMath
=================================================
*/
    void  CoreBindings::BindVectorMath (const ScriptEnginePtr &se) __Th___
    {
        using VecTypes = TypeList<
                            packed_bool2,   packed_bool3,   packed_bool4,
                            packed_sbyte2,  packed_sbyte3,  packed_sbyte4,
                            packed_ubyte2,  packed_ubyte3,  packed_ubyte4,
                            packed_short2,  packed_short3,  packed_short4,
                            packed_ushort2, packed_ushort3, packed_ushort4,
                            packed_int2,    packed_int3,    packed_int4,
                            packed_uint2,   packed_uint3,   packed_uint4,
                            packed_float2,  packed_float3,  packed_float4
                        >;

        CHECK_THROW( se and se->IsInitialized() );

        // declare
        {
            DefineVector_Func   func{ se };

            VecTypes::Visit( func );
        }

        // bind
        {
            BindVector_Func func{ se };

            VecTypes::Visit( func );
        }
    }


} // AE::Scripting
