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
		StaticAssert( fi::is_const );
		StaticAssert( not fi::is_volatile );
		StaticAssert( not fi::is_noexcept );
		StaticAssert( IsSameTypes< Cl, fi::clazz > );
		StaticAssert( IsSameTypes< fi::type, void (Cl::*)(int) const > );
	}{
		using fi = FunctionInfo< decltype(&Cl::F2) >;
		StaticAssert( not fi::is_const );
		StaticAssert( fi::is_volatile );
		StaticAssert( not fi::is_noexcept );
		StaticAssert( IsSameTypes< Cl, fi::clazz > );
		StaticAssert( IsSameTypes< fi::type, bool (Cl::*)(float, int) volatile > );
	}{
		using fi = FunctionInfo< decltype(&Cl::F3) >;
		StaticAssert( fi::is_const );
		StaticAssert( fi::is_volatile );
		StaticAssert( not fi::is_noexcept );
		StaticAssert( IsSameTypes< Cl, fi::clazz > );
		StaticAssert( IsSameTypes< fi::type, int (Cl::*)() const volatile > );
	}{
		using fi = FunctionInfo< decltype(&Cl::F4) >;
		StaticAssert( not fi::is_const );
		StaticAssert( not fi::is_volatile );
		StaticAssert( not fi::is_noexcept );
		StaticAssert( IsSameTypes< void, fi::clazz > );
		StaticAssert( IsSameTypes< fi::type, void (*)(double, int) > );
	}{
		using fi = FunctionInfo< decltype(&Cl::F5) >;
		StaticAssert( not fi::is_const );
		StaticAssert( not fi::is_volatile );
		StaticAssert( fi::is_noexcept );
	}

	TEST_PASSED();
}
