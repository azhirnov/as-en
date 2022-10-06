// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "base/Stream/MemStream.h"
#include "UnitTest_Common.h"

namespace
{
	struct SerObj
	{
		int		i;
		float	f;
		int2	v;

		SerObj ()	{}
		~SerObj ()	{}
	};
	STATIC_ASSERT( not IsTrivialySerializable< SerObj >);


	static bool  SerObj_Serialize (Serializer &ser, const void *ptr)
	{
		auto*	self = Cast<SerObj>(ptr);

		ser( self->i, self->f, self->v );
		return true;
	}

	static bool  SerObj_Deserialize (Deserializer &deser, OUT void *ptr, bool create)
	{
		auto*	self = create ? PlacementNew<SerObj>(ptr) : Cast<SerObj>(ptr);
	
		deser( self->i, self->f, self->v );
		return true;
	}
	

	using TestID = NamedID< 32, 0x1111, true, UMax >;


	static void  Serialization_Test1 ()
	{
		ObjectFactory	factory;
		TEST( factory.Register<SerObj>( SerializedID{"Test1"}, SerObj_Serialize, SerObj_Deserialize ));

		const String			s1 = "sdkjfnbswdkjfbqkjhswkd;aklnskjsdhbnkjsfdn,mk";
		const FixedString<32>	f1 = "563ksdjcfn-";
		const TestID			i1{ "49qkjand" };

		SerObj	a;
		auto	stream = MakeRC<MemWStream>();

		a.i = 1111;
		a.f = 2.5463434f;
		a.v = { 3, 11 };

		{
			Serializer	ser{ stream };
			ser.factory	= &factory;
			TEST( ser( a, s1, f1, i1 ));
		}

		SerObj			b, c;
		String			s2;
		FixedString<32>	f2;
		TestID			i2;
		{
			auto	rstream = MakeRC<MemRefRStream>( stream->GetData() );

			Deserializer	deser1{ rstream };
			deser1.factory	= &factory;
			TEST( deser1( b, s2, f2, i2 ));
			TEST( deser1.IsEnd() );
	
			rstream = MakeRC<MemRefRStream>( stream->GetData() );
			
			Deserializer	deser2{ rstream };
			deser2.factory	= &factory;
			TEST( deser2( static_cast<void *>(&c) ));
		}

		TEST( a.i == b.i );
		TEST( a.f == b.f );
		TEST( All( a.v == b.v ));

		TEST( a.i == c.i );
		TEST( a.f == c.f );
		TEST( All( a.v == c.v ));

		TEST( s1 == s2 );
		TEST( f1 == f2 );
		TEST( i1 == i2 );
	}


	static void  SerializationTraits ()
	{
		STATIC_ASSERT( IsTrivialySerializable< int >);
		STATIC_ASSERT( IsTrivialySerializable< float >);
		STATIC_ASSERT( IsTrivialySerializable< int2 >);
		
		STATIC_ASSERT( not IsTrivialySerializable< String >);
	}
}


extern void UnitTest_Serialization ()
{
	SerializationTraits();
	Serialization_Test1();

	TEST_PASSED();
}
