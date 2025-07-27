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
	
	static void F6 (double, int) noexcept;
};


extern void UnitTest_FunctionInfo ()
{
	{
		using fi = FunctionInfo< decltype(&Cl::F1) >;
		StaticAssert( fi::is_const );
		StaticAssert( not fi::is_volatile );
		StaticAssert( not fi::is_noexcept );
		StaticAssert( IsSame< Cl, fi::clazz > );
		StaticAssert( IsSame< fi::type, void (Cl::*)(int) const > );
	}{
		using fi = FunctionInfo< decltype(&Cl::F2) >;
		StaticAssert( not fi::is_const );
		StaticAssert( fi::is_volatile );
		StaticAssert( not fi::is_noexcept );
		StaticAssert( IsSame< Cl, fi::clazz > );
		StaticAssert( IsSame< fi::type, bool (Cl::*)(float, int) volatile > );
	}{
		using fi = FunctionInfo< decltype(&Cl::F3) >;
		StaticAssert( fi::is_const );
		StaticAssert( fi::is_volatile );
		StaticAssert( not fi::is_noexcept );
		StaticAssert( IsSame< Cl, fi::clazz > );
		StaticAssert( IsSame< fi::type, int (Cl::*)() const volatile > );
	}{
		using fi = FunctionInfo< decltype(Cl::F4) >;
		StaticAssert( not fi::is_const );
		StaticAssert( not fi::is_volatile );
		StaticAssert( not fi::is_noexcept );
		StaticAssert( IsSame< void, fi::clazz > );
		StaticAssert( IsSame< fi::type, void (*)(double, int) > );
		StaticAssert( IsGlobalFunction< decltype(Cl::F4) >);
		StaticAssert( IsGlobalFunction< decltype(&Cl::F4) >);
	}{
		using fi = FunctionInfo< decltype(&Cl::F5) >;
		StaticAssert( not fi::is_const );
		StaticAssert( not fi::is_volatile );
		StaticAssert( fi::is_noexcept );
	}{
		using fi = FunctionInfo< decltype(&Cl::F6) >;
		StaticAssert( fi::is_noexcept );
		StaticAssert( IsGlobalFunction< decltype(Cl::F6) >);
		StaticAssert( IsGlobalFunction< decltype(&Cl::F6) >);
	}{
		auto	lambda = [i = 0u] (int, float) { Unused(i); return true; };
		using fi = FunctionInfo< decltype(lambda) >;
		StaticAssert( fi::is_const );
		StaticAssert( not IsVoid< fi::clazz >);
		StaticAssert( IsSame< fi::args, TypeList<int, float> >);
		StaticAssert( IsSame< fi::result, bool >);
	}{
		auto	lambda = [i = 0u] (int, float) { Unused(i); return true; };
		using T = decltype(lambda);
		using fi = FunctionInfo< decltype(&T::operator ()) >;
		StaticAssert( fi::is_const );
		StaticAssert( not IsVoid< fi::clazz >);
		StaticAssert( IsSame< fi::args, TypeList<int, float> >);
		StaticAssert( IsSame< fi::result, bool >);
	}{
		auto	lambda = [] (int, auto&&) { return true; };
		using T = decltype(lambda);
		using fi = FunctionInfo< decltype(&T::operator()<String&>) >;
		StaticAssert( fi::is_const );
		StaticAssert( not IsVoid< fi::clazz >);
		StaticAssert( IsSame< fi::args, TypeList<int, String&> >);
		StaticAssert( IsSame< fi::result, bool >);

		StaticAssert( not IsNotTemplateCallOperator<T> );
		StaticAssert( IsCallOperatorSpecialization< T, int >);
		StaticAssert( not IsCallOperatorSpecialization< T, int, float >);
		StaticAssert( IsCallOperatorSpecializationFromTypeList< T, TypeList<int> >);
	}{
		auto	lambda = [] (int, auto&&) { return true; };
		using T = decltype(lambda);

		using fi = TemplateFunctionInfo< T, TypeList<float> >;
		StaticAssert( fi::is_const );
		StaticAssert( not IsVoid< fi::clazz >);
		StaticAssert( IsSame< fi::args::Get<0>, int >);
		StaticAssert( IsSame< fi::args::Get<1>, float&& >);
		StaticAssert( IsSame< fi::result, bool >);
	}

	TEST_PASSED();
}
