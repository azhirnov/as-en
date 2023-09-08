// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

class Cl
{
public:
    void F1 (int) const;
    bool F2 (float, int) volatile;
    int F3 () const volatile;

    static void F4 (double, int);

    float F5 () noexcept;
};


extern void UnitTest_FunctionInfo ()
{
    {
        using fi = FunctionInfo< decltype(&Cl::F1) >;
        STATIC_ASSERT( fi::is_const );
        STATIC_ASSERT( not fi::is_volatile );
        STATIC_ASSERT( not fi::is_noexcept );
        STATIC_ASSERT( IsSameTypes< Cl, fi::clazz > );
        STATIC_ASSERT( IsSameTypes< fi::type, void (Cl::*)(int) const > );
    }{
        using fi = FunctionInfo< decltype(&Cl::F2) >;
        STATIC_ASSERT( not fi::is_const );
        STATIC_ASSERT( fi::is_volatile );
        STATIC_ASSERT( not fi::is_noexcept );
        STATIC_ASSERT( IsSameTypes< Cl, fi::clazz > );
        STATIC_ASSERT( IsSameTypes< fi::type, bool (Cl::*)(float, int) volatile > );
    }{
        using fi = FunctionInfo< decltype(&Cl::F3) >;
        STATIC_ASSERT( fi::is_const );
        STATIC_ASSERT( fi::is_volatile );
        STATIC_ASSERT( not fi::is_noexcept );
        STATIC_ASSERT( IsSameTypes< Cl, fi::clazz > );
        STATIC_ASSERT( IsSameTypes< fi::type, int (Cl::*)() const volatile > );
    }{
        using fi = FunctionInfo< decltype(&Cl::F4) >;
        STATIC_ASSERT( not fi::is_const );
        STATIC_ASSERT( not fi::is_volatile );
        STATIC_ASSERT( not fi::is_noexcept );
        STATIC_ASSERT( IsSameTypes< void, fi::clazz > );
        STATIC_ASSERT( IsSameTypes< fi::type, void (*)(double, int) > );
    }{
        using fi = FunctionInfo< decltype(&Cl::F5) >;
        STATIC_ASSERT( not fi::is_const );
        STATIC_ASSERT( not fi::is_volatile );
        STATIC_ASSERT( fi::is_noexcept );
    }

    TEST_PASSED();
}
