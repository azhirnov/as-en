// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	template <typename T>
	using ScriptRC = AngelScriptHelper::SharedPtr<T>;


	// class
	struct Test1_CL : AngelScriptHelper::SimpleRefCounter
	{
		int i;

		Test1_CL () : i(3)
		{}

		Test1_CL (int i): i(i)
		{}

		int F () {
			return i;
		}
	};


	// value class
	struct Test2_Value
	{
		int i;

		Test2_Value () : i(4) {}
		Test2_Value (int i) : i(i) {}

		//void operator = (const Test2_Value &) {}

		int  F () const {
			return i;
		}

		static void  Ctor (OUT void* mem, int i) {
			PlacementNew<Test2_Value>( OUT mem, i );
		}

		static int  Append (Test2_Value &self, int x) {
			self.i += x;
			return self.i;
		}

		void  Add (const Test2_Value &rhs) {
			i += rhs.i;
		}
	};


	// enum
	enum class EEnum : uint
	{
		Value1 = 1,
		Value2,
		Value3,
		Value4,
		_Count
	};

	// enum bits
	enum class EEnumBit : uint
	{
		Value1	= 1 << 0,
		Value2	= 1 << 2,
		Value3	= 1 << 3,
		Value4	= 1 << 5
	};
	AE_BIT_OPERATORS( EEnumBit );


	struct Test5_CL : AngelScriptHelper::SimpleRefCounter
	{
		int i;

		Test5_CL () : i(3)
		{}

		Test5_CL (int i): i(i)
		{}

		int F () {
			return i;
		}
	};


	struct Test6_CL : AngelScriptHelper::SimpleRefCounter
	{
		int i;

		Test6_CL () : i(0)
		{}

		void Set (int v) {
			this->i = v;
		}
	};


	// value class
	struct Test7_Value
	{
		int i;

		Test7_Value () : i(4) {}
		Test7_Value (int i) : i(i) {}

		int F () const {
			return i;
		}

		static int Append (Test7_Value &self, int x) {
			self.i += x;
			return self.i;
		}
	};


	struct Test8_CL : AngelScriptHelper::SimpleRefCounter
	{
		int i;

		Test8_CL () : i(0)
		{}

		void Set (int v) {
			this->i = v;
		}
	};


	struct Test9_1_CL : AngelScriptHelper::SimpleRefCounter
	{
		int i;

		Test9_1_CL () : i(0) {}

		Test9_1_CL (int i): i{i} {}

		static Test9_1_CL*  Ctor (int i) {
			return ScriptRC<Test9_1_CL>{ new Test9_1_CL{ i }}.Detach();
		}
	};

	struct Test9_2_CL : AngelScriptHelper::SimpleRefCounter
	{
		using Ptr = ScriptRC<Test9_1_CL>;

		Array<Ptr>	arr;

		Test9_2_CL () {}

		void  Add (const Ptr &p)	{
			arr.push_back( p );
		}

		Test9_1_CL*  Add2 (const String &s) {
			return arr.emplace_back( new Test9_1_CL{ int(s.length()) }).Retain();
		}
	};

	struct Test10_CL : AngelScriptHelper::SimpleRefCounter
	{
		static void  Gen1 (ScriptArgList args)
		{
			if ( args.Is< uint (Test10_CL::*)(int) >())
			{
				TEST_Eq( args.Arg<int>(0), 1 );
				args.Return( 111u );
			}
			else
				throw AE::Exception{""};
		}
	};

} // namespace


AE_DECL_SCRIPT_OBJ_RC(	Test1_CL,		"Test1_CL"		);
AE_DECL_SCRIPT_OBJ(		Test2_Value,	"Test2_Value"	);
AE_DECL_SCRIPT_TYPE(	EEnum,			"EEnum"			);
AE_DECL_SCRIPT_TYPE(	EEnumBit,		"EEnumBit"		);
AE_DECL_SCRIPT_OBJ_RC(	Test5_CL,		"Test5_CL"		);
AE_DECL_SCRIPT_OBJ_RC(	Test6_CL,		"Test6_CL"		);
AE_DECL_SCRIPT_OBJ(		Test7_Value,	"Test7_Value"	);
AE_DECL_SCRIPT_OBJ_RC(	Test8_CL,		"Test8_CL"		);
AE_DECL_SCRIPT_OBJ_RC(	Test9_1_CL,		"Test9_1_CL"	);
AE_DECL_SCRIPT_OBJ_RC(	Test9_2_CL,		"Test9_2_CL"	);
AE_DECL_SCRIPT_OBJ_RC(	Test10_CL,		"Test10_CL"		);


namespace
{
	static void  ScriptClass_Test1 (const ScriptEnginePtr &se)
	{
		TEST_NOTHROW(
			ClassBinder<Test1_CL>	binder( se );
			binder.CreateRef();
			binder.AddMethod( &Test1_CL::F, "F" );
		)

		static const int  line		= __LINE__ + 1;
		static const char script[]	= R"#(
			int ASmain (int i) {
				Test1_CL@ c = Test1_CL();
				return c.F() + i;
			}
		)#";

		int	res = 0;
		TEST( Run< int (int) >( se, script, "ASmain", SourceLoc{__FILE__, line}, OUT res, 1 ));
		TEST_Eq( res, 3+1 );
	}


	static void  ScriptClass_Test2 (const ScriptEnginePtr &se)
	{
		TEST_NOTHROW(
			ClassBinder<Test2_Value>	binder{ se };
			binder.CreateClassValue();
			binder.AddConstructor( &Test2_Value::Ctor );
			binder.AddMethod( &Test2_Value::F, "F" );
			binder.AddProperty( &Test2_Value::i, "i" );
			binder.AddMethod( &Test2_Value::Add, "Add" );
			binder.AddMethodFromGlobal( &Test2_Value::Append, "Append" );
		)

		static const int  line		= __LINE__ + 1;
		static const char script[]	= R"#(
			int ASmain (int i) {
				Test2_Value p1 = Test2_Value();
				Test2_Value p2 = Test2_Value( 5 );
				p1.i *= 10;
				p1.Append( i );
				p2.Add( p1 );
				return p2.F();
			}
		)#";

		int	res = 0;
		TEST( Run< int (int) >( se, script, "ASmain", SourceLoc{__FILE__, line}, OUT res, 2 ));
		TEST_Eq( res, 5 + ((4*10)+2) );
	}


	static void  ScriptClass_Test3 (const ScriptEnginePtr &se)
	{
		TEST_NOTHROW(
			EnumBinder<EEnum>	binder{ se };
			binder.Create();
			binder.AddValue( "Value1", EEnum::Value1 );
			binder.AddValue( "Value2", EEnum::Value2 );
			binder.AddValue( "Value3", EEnum::Value3 );
			binder.AddValue( "Value4", EEnum::Value4 );
		)

		static const int  line		= __LINE__ + 1;
		static const char script[]	= R"#(
			uint ASmain () {
				EEnum e = EEnum_Value1;
				return e + EEnum_Value2;
			}
		)#";

		uint	res = 0;
		TEST( Run< uint () >( se, script, "ASmain", SourceLoc{__FILE__, line}, OUT res ));
		TEST_Eq( res, uint(EEnum::Value1) + uint(EEnum::Value2) );
	}


	// script in script
	class ScriptCl
	{
	public:
		static ScriptEnginePtr	engine;

		int Run (int value)
		{
			static const int  line		= __LINE__ + 1;
			static const char script[]	= R"#(
				int ASmain (int x) {
					return 10 + x;
				}
			)#";

			int	res = 0;
			TEST( ::Run< int (int) >( engine, script, "ASmain", SourceLoc{__FILE__, line}, OUT res, value ));

			return res;
		}
	};
	ScriptEnginePtr	ScriptCl::engine;

	static void  ScriptClass_Test4 (const ScriptEnginePtr &se)
	{
		static const int  line		= __LINE__ + 1;
		static const char script[]	= R"#(
			int ASmain ()
			{
				Script sc;
				return sc.Run( 1 );
			}
		)#";

		ScriptCl::engine = se;

		TEST_NOTHROW(
			ClassBinder<ScriptCl>	binder{ se, "Script" };
			binder.CreateClassValue();
			binder.AddMethod( &ScriptCl::Run, "Run" );
		)

		int	res = 0;
		TEST( Run< int() >( se, script, "ASmain", SourceLoc{__FILE__, line}, OUT res ));
		TEST_Eq( res, 11 );

		ScriptCl::engine = null;
	}


	static void  ScriptClass_Test5 (const ScriptEnginePtr &se)
	{
		TEST_NOTHROW(
			ClassBinder<Test5_CL> binder( se );
			binder.CreateRef();
			binder.AddMethod( &Test5_CL::F, "F" );
		)

		static const int  line		= __LINE__ + 1;
		static const char script[]	= R"#(
			int ASmain (Test5_CL@ c) {
				return c.F() + 22;
			}
		)#";

		ScriptRC<Test5_CL>	arg{ new Test5_CL{11} };
		int					res = 0;

		TEST( Run< int(Test5_CL*) >( se, script, "ASmain", SourceLoc{__FILE__, line}, OUT res, arg ));
		TEST_Eq( res, 11+22 );
		TEST_Eq( arg->__Counter(), 1 );
	}


	static void  ScriptClass_Test6 (const ScriptEnginePtr &se)
	{
		TEST_NOTHROW(
			ClassBinder<Test6_CL> binder( se );
			binder.CreateRef();
			binder.AddMethod( &Test6_CL::Set, "Set" );
		)

		static const int  line		= __LINE__ + 1;
		static const char script[]	= R"#(
			Test6_CL@ ASmain () {
				Test6_CL@ c = Test6_CL();
				c.Set( 11 );
				return c;
			}
		)#";

		Test6_CL*	res = null;
		TEST( Run< Test6_CL*() >( se, script, "ASmain", SourceLoc{__FILE__, line}, OUT res ));
		TEST( res != null );
		TEST_Eq( res->__Counter(), 1 );
		TEST_Eq( res->i, 11 );
		res->__Release();
	}


	static void  ScriptClass_Test7 (const ScriptEnginePtr &se)
	{
		TEST_NOTHROW(
			ClassBinder<Test7_Value>	binder{ se };
			binder.CreateRef( &AngelScriptHelper::FactoryCreate<Test7_Value>, null, null, 0 );
			binder.AddMethod( &Test7_Value::F, "F" );
			binder.AddMethodFromGlobal( &Test7_Value::Append, "Append" );
		)

		static const int  line		= __LINE__ + 1;
		static const char script[]	= R"#(
			int ASmain (Test7_Value& p) {
				p.Append( 5 );
				return p.F();
			}
		)#";

		Test7_Value	val{3};
		int			res = 0;
		TEST( Run< int (Test7_Value*) >( se, script, "ASmain", SourceLoc{__FILE__, line}, OUT res, &val ));
		TEST_Eq( res, 3+5 );
	}


	static void  ScriptClass_Test8 (const ScriptEnginePtr &se)
	{
		using Test8_Ptr = ScriptRC<Test8_CL>;

		TEST_NOTHROW(
			ClassBinder<Test8_CL> binder( se );
			binder.CreateRef();
			binder.AddMethod( &Test8_CL::Set, "Set" );
		)

		static const int  line		= __LINE__ + 1;
		static const char script[]	= R"#(
			Test8_CL@ ASmain (Test8_CL@ c) {
				c.Set( 11 );
				Test8_CL@ a = Test8_CL();
				a.Set( 22 );
				return a;
			}
		)#";

		Test8_Ptr	arg{ new Test8_CL{} };
		Test8_Ptr	res;

		TEST( Run< Test8_CL* (Test8_CL*) >( se, script, "ASmain", SourceLoc{__FILE__, line}, OUT res, arg.Get() ));
		TEST( res );
		TEST_Eq( arg->__Counter(), 1 );
		TEST_Eq( arg->i, 11 );
		TEST_Eq( res->__Counter(), 1 );
		TEST_Eq( res->i, 22 );
	}


	static void  ScriptClass_Test9 (const ScriptEnginePtr &se)
	{
		using Test9_2_Ptr = ScriptRC<Test9_2_CL>;
		TEST_NOTHROW(
			ClassBinder<Test9_1_CL> binder( se );
			binder.CreateRef();
			binder.AddFactoryCtor( &Test9_1_CL::Ctor );
		)
		TEST_NOTHROW(
			ClassBinder<Test9_2_CL> binder( se );
			binder.CreateRef();
			binder.AddMethod( &Test9_2_CL::Add,  "Add"  );
			binder.AddMethod( &Test9_2_CL::Add2, "Add2" );
		)
		static const int  line		= __LINE__ + 1;
		static const char script[]	= R"#(
			Test9_2_CL@ ASmain () {
				Test9_2_CL@ a = Test9_2_CL();
				a.Add( Test9_1_CL( 1 ));
				a.Add( Test9_1_CL( 2 ));
				Test9_1_CL@ b = a.Add2( "aaabbccc" );
				a.Add( b );
				return a;
			}
		)#";

		Test9_2_Ptr	res;

		TEST( Run< Test9_2_CL*() >( se, script, "ASmain", SourceLoc{__FILE__, line}, OUT res ));
		TEST( res );
		TEST_Eq( res->__Counter(), 1 );
		TEST_Eq( res->arr.size(), 4 );

		TEST( res->arr[0] );
		TEST_Eq( res->arr[0]->__Counter(), 1 );
		TEST_Eq( res->arr[0]->i, 1 );

		TEST( res->arr[1] );
		TEST_Eq( res->arr[1]->__Counter(), 1 );
		TEST_Eq( res->arr[1]->i, 2 );

		TEST( res->arr[2] );
		TEST_Eq( res->arr[2]->__Counter(), 2 );
		TEST_Eq( res->arr[2]->i, 8 );

		TEST( res->arr[2] == res->arr[3] );
	}


	static void  ScriptClass_Test10 (const ScriptEnginePtr &se)
	{
		TEST_NOTHROW(
			EnumBinder<EEnumBit>	binder{ se };
			binder.Create();
			binder.AddValue( "Value1", EEnumBit::Value1 );
			binder.AddValue( "Value2", EEnumBit::Value2 );
			binder.AddValue( "Value3", EEnumBit::Value3 );
			binder.AddValue( "Value4", EEnumBit::Value4 );
		)

		static const int  line		= __LINE__ + 1;
		static const char script[]	= R"#(
			uint ASmain () {
				EEnumBit e = EEnumBit(EEnumBit_Value1 | EEnumBit_Value3);
				return e;
			}
		)#";

		uint	res = 0;
		TEST( Run< uint () >( se, script, "ASmain", SourceLoc{__FILE__, line}, OUT res ));
		TEST_Eq( res, uint(EEnumBit::Value1 | EEnumBit::Value3) );
	}


	static void  ScriptClass_Test11 (const ScriptEnginePtr &se)
	{
		TEST_NOTHROW(
			ClassBinder<Test10_CL>	binder{ se };
			binder.CreateRef();
			binder.AddGenericMethod< uint(int) >( &Test10_CL::Gen1, "gen" );
		)

		StaticAssert( ScriptTypeInfo<Test10_CL>::is_object );
		StaticAssert( ScriptTypeInfo<Test10_CL>::is_ref_counted );

		static const int  line		= __LINE__ + 1;
		static const char script[]	= R"#(
			uint ASmain () {
				Test10_CL@ a = Test10_CL();
				return a.gen( 1 );
			}
		)#";

		uint	res = 0;
		TEST( Run< uint () >( se, script, "ASmain", SourceLoc{__FILE__, line}, OUT res ));
		TEST_Eq( res, 111u );
	}
}


extern void UnitTest_Class ()
{
	TEST_NOTHROW(
		auto	se = MakeRC<ScriptEngine>();
		TEST( se->Create() );

		CoreBindings::BindString( se );

		ScriptClass_Test1( se );
		ScriptClass_Test2( se );
		ScriptClass_Test3( se );
		ScriptClass_Test4( se );
		ScriptClass_Test5( se );
		ScriptClass_Test6( se );
		ScriptClass_Test7( se );
		ScriptClass_Test8( se );
		ScriptClass_Test9( se );
		ScriptClass_Test10( se );
		ScriptClass_Test11( se );

		TEST_PASSED();
	)
}
