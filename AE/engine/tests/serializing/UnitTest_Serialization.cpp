// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	struct SerObj
	{
		int		i;
		float	f;
		int2	v;

		SerObj ()	__NE___ {}
		~SerObj ()	__NE___ {}
	};
	StaticAssert( not IsTriviallySerializable< SerObj >);


	struct SerObj2 : ISerializable
	{
		int		i;
		float	f;
		int2	v;

		bool  Serialize (Serializer &ser) C_NE_OV
		{
			return ser( v, i, f );
		}

		bool  Deserialize (Deserializer &des) __NE_OV
		{
			return des( v, i, f );
		}
	};


	static bool  SerObj_Serialize (Serializer &ser, const void* ptr) __NE___
	{
		auto*	self = Cast<SerObj>(ptr);

		return ser( self->i, self->f, self->v );
	}

	static bool  SerObj_Deserialize (Deserializer &deser, INOUT void* &ptr, Ptr<IAllocator>) __NE___
	{
		if ( ptr == null )
			ptr = new SerObj{};

		auto*	self = Cast<SerObj>( ptr );

		return deser( OUT self->i, OUT self->f, OUT self->v );
	}


	using TestID = NamedID< 32, 0x1111, true, UMax >;


	static void  Serialization_Test1 ()
	{
		ObjectFactory	factory;
		TEST( factory.Register<SerObj>( SerializedID{"Test1"}, SerObj_Serialize, SerObj_Deserialize ));

		const String			s1 = "sdkjfnbswdkjfbqkjhswkd;aklnskjsdhbnkjsfdn,mk";
		const FixedString<32>	f1 = "563ksdjcfn-";
		const TestID			i1{ "49qkjand" };

		SerObj	a1;
		SerObj2	a2;
		auto	stream = MakeRC<ArrayWStream>();

		a1.i = a2.i = 1111;
		a1.f = a2.f = 2.5463434f;
		a1.v = a2.v = { 3, 11 };

		{
			Serializer	ser{ stream };
			ser.factory	= &factory;
			TEST( ser( &a1, a2, s1, f1, i1 ));
		}

		Unique<SerObj>	b1;
		SerObj2			b2;
		String			s2;
		FixedString<32>	f2;
		TestID			i2;
		Unique<SerObj>	c;

		{
			auto	rstream = MakeRC<MemRefRStream>( stream->GetData() );

			Deserializer	deser1{ rstream };
			deser1.factory	= &factory;
			TEST( deser1( b1, b2, s2, f2, i2 ));
			TEST( deser1.IsEnd() );

			rstream = MakeRC<MemRefRStream>( stream->GetData() );

			Deserializer	deser2{ rstream };
			deser2.factory	= &factory;
			TEST( deser2( c ));
		}

		TEST( b1 != null );
		TEST( a1.i == b1->i );
		TEST( a1.f == b1->f );
		TEST( All( a1.v == b1->v ));

		TEST( a2.i == b2.i );
		TEST( a2.f == b2.f );
		TEST( All( a2.v == b2.v ));

		TEST( c != null );
		TEST( a1.i == c->i );
		TEST( a1.f == c->f );
		TEST( All( a1.v == c->v ));

		TEST( s1 == s2 );
		TEST( f1 == f2 );
		TEST( i1 == i2 );
	}


	static void  Serialization_Test2 ()
	{
		auto	stream = MakeRC<ArrayWStream>();

		const uint		u_val	= 0x12345678;
		const float		f_val	= 2.1345678f;
		const double	d_val	= 987.1234567;

		PackedBits<0, 8, uint>		u0	{u_val};
		PackedBits<2, 9, uint>		u1	{u_val};
		PackedFloatBits<0>			f0	{f_val};
		PackedFloatBits<13>			f1	{f_val};
		PackedFloatBits<20>			f2	{f_val};
		PackedDoubleBits<0>			d0	{d_val};
		PackedDoubleBits<20>		d1	{d_val};
		PackedDoubleBits<40>		d2	{d_val};

		PackedBits<0, 64, ulong>	u2	{0x080};
		PackedBits<0, 64, ulong>	u3	{0x00F};
		PackedBits<0, 64, ulong>	u4	{0xC00};

		{
			BitSerializer	ser{ stream };
			TEST( ser( u0, u1 ));
			TEST( ser( f0, f1, f2 ));
			TEST( ser( d0, d1, d2 ));
			TEST( ser( u2, u3, u4 ));

			DEBUG_ONLY( AE_LOGI( "UnusedBits: "s << ToString( ser.dbgUnusedBits ));)
		}
		{
			auto			rstream = MakeRC<MemRefRStream>( stream->GetData() );
			BitDeserializer	des{ rstream };

			TEST( des( u0, u1 ));
			TEST( des( f0, f1, f2 ));
			TEST( des( d0, d1, d2 ));
			TEST( des( u2, u3, u4 ));

			TEST( des.IsEnd() );
		}

		TEST( uint{u0} == (u_val & 0xFF) );
		TEST( uint{u1} == (u_val & (0x1FF << 2)) );

		TEST( BitEqual( float{f0}, f_val, EnabledBitCount(32) ));
		TEST( BitEqual( float{f1}, f_val, EnabledBitCount(32-13) ));
		TEST( BitEqual( float{f2}, f_val, EnabledBitCount(32-20) ));

		TEST( BitEqual( double{d0}, d_val, EnabledBitCount(64) ));
		TEST( BitEqual( double{d1}, d_val, EnabledBitCount(64-20) ));
		TEST( BitEqual( double{d2}, d_val, EnabledBitCount(64-40) ));

		TEST( u2.ToBits() == 0x080 );
		TEST( u3.ToBits() == 0x00F );
		TEST( u4.ToBits() == 0xC00 );
	}


	static void  Serialization_Test3 ()
	{
		auto	stream = MakeRC<ArrayWStream>();

		Array<uint>								a1	{6,2,8,92,54,84,95,11,73};
		ArrayView<uint>							a2	{ a1.data(), 5 };
		FixedArray<uint,8>						a3	{9,5,22,88,33};
		FixedTupleArray<8,float, uint, bool>	a4;
		Union<int, String, float>				a5 = "dgsdfvsdsdfgbfc";
		Optional<String>						a6 = "ainjaisncuksa pqikal";

		a4.push_back( 1.f, 2u, true );
		a4.push_back( 4.f, 5u, false );
		a4.push_back( 8.f, 1u, false );
		a4.push_back( 2.f, 9u, true );

		TupleArrayView<float, uint, bool>		a7 {a4};
		TupleArrayView<float, bool, uint, int>	a8 { a4.size(), a4.data<float>(), a4.data<bool>(), a4.data<uint>(), null };

		{
			Serializer	ser{ stream };
			TEST( ser( a1, a2, a3, a4, a5, a6, a7, a8 ));
		}

		Array<uint>								b1;
		ArrayView<uint>							b2;
		FixedArray<uint,8>						b3;
		FixedTupleArray<8,float, uint, bool>	b4;
		Union<int, String, float>				b5;
		Optional<String>						b6;
		TupleArrayView<float, uint, bool>		b7;
		TupleArrayView<float, bool, uint, int>	b8;
		{
			auto				rstream = MakeRC<MemRefRStream>( stream->GetData() );
			Deserializer		des{ rstream };
			LinearAllocator<>	alloc;
			des.allocator = &alloc;

			TEST( des( b1, b2, b3, b4, b5, b6, b7, b8 ));
			TEST( a1 == b1 );
			TEST( a2 == b2 );
			TEST( a3 == b3 );
			TEST( a4 == b4 );
			TEST( a5 == b5 );
			TEST( a6 == b6 );
			TEST( a7 == b7 );
		}
	}


	static void  Serialization_Test4 ()
	{
		struct CpuCluster final : ISerializable
		{
			String					name;
			CpuArchInfo::CoreBits_t	logicalCores;

			ND_ bool  operator == (const CpuCluster &rhs) const {
				return	name == rhs.name and
						logicalCores == rhs.logicalCores;
			}

			bool  Serialize (Serializer &ser) C_NE_OV {
				return ser( name, logicalCores );
			}

			bool  Deserialize (Deserializer &des) __NE_OV {
				return des( OUT name, OUT logicalCores );
			}
		};
		using CpuClusters_t	= FixedArray< CpuCluster, CpuArchInfo::MaxCoreTypes >;

		CpuClusters_t	clusters;
		{
			auto&	dst = clusters.emplace_back();
			dst.name	= "Cortex X1";
			dst.logicalCores.set( 7 );
		}{
			auto&	dst = clusters.emplace_back();
			dst.name	= "Cortex A78";
			dst.logicalCores.set( 5 ).set( 6 );
		}{
			auto&	dst = clusters.emplace_back();
			dst.name	= "Cortex A55";
			dst.logicalCores.set( 0 ).set( 1 ).set( 2 ).set( 3 );
		}

		auto	stream = MakeRC<ArrayWStream>();
		{
			Serializer	ser{ stream };
			TEST( ser( clusters ));
		}

		{
			auto				rstream = MakeRC<MemRefRStream>( stream->GetData() );
			Deserializer		des{ rstream };
			LinearAllocator<>	alloc;
			des.allocator = &alloc;

			CpuClusters_t	clusters2;
			TEST( des( OUT clusters2 ));

			TEST( clusters2 == clusters );
		}
	}


	static void  SerializationTraits ()
	{
		StaticAssert( IsTriviallySerializable< int >);
		StaticAssert( IsTriviallySerializable< float >);
		//StaticAssert( IsTriviallySerializable< Quat >);

		StaticAssert( not IsTriviallySerializable< String >);
	}
}


extern void UnitTest_Serialization ()
{
	SerializationTraits();

	Serialization_Test1();
	Serialization_Test2();
	Serialization_Test3();
	Serialization_Test4();

	TEST_PASSED();
}
