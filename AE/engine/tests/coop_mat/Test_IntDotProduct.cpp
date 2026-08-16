// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"

namespace
{
	using ByteBuffer = Executor::ByteBuffer;

	template <typename T>
	ND_ ByteBuffer  BufCast (Array<T> &arr)
	{
		return ByteBuffer{ Cast<ubyte>(arr.data()), arr.size() * sizeof(T) };
	}


	template <typename T>
	static void  Test_4x8BitPacked (ArrayView<T> inputA, ArrayView<T> inputB, ArrayView<T> refOutput)
	{
		StaticAssert( IsSame<T, uint> or IsSame<T, int> );

		CHECK( inputA.size() == inputB.size() );
		CHECK( inputA.size() == refOutput.size() );

		using Packed = Conditional< IsSame<T, uint>, ubyte4, sbyte4 >;

		Array<T>	output;
		output.resize( refOutput.size() );

		for (usize i = 0; i < refOutput.size(); ++i)
		{
			Packed	a, b;
			memcpy( &a, &inputA[i], sizeof(a) );
			memcpy( &b, &inputB[i], sizeof(b) );

			T	sum = 0;
			for (uint j = 0; j < 4; ++j)
			{
				sum += T(a[j]) * T(b[j]);
			}
			output[i] = sum;
		}

		String	str		= "\n| expected | shader output | error % |\n";
		float	max_err	= 0.f;
		float	avr_err	= 0.f;

		for (usize i = 0; i < refOutput.size(); ++i)
		{
			float	err	= Abs(float(output[i]) - float(refOutput[i])) * 100.f / Max( Abs(float(output[i])), 1.0e-5f );
			max_err = Max( max_err, err );
			avr_err += err;

			str << "| " << ToString( output[i] ) << " | " << ToString( refOutput[i] )
				<< " | " << ToString( err, 2 ) << "% |\n";
		}
		avr_err /= float(refOutput.size());
		str << "----------------------------------------\n";

		if ( max_err < 0.01f )
			str.clear();

		str << "max error: " << ToString( max_err, 2 ) << "%, avr: " << ToString( avr_err, 2 ) << "%";
		AE_LOGI( str );
		CHECK( max_err < 1.f );
	}


	template <typename T>
	static void  Test_4x8BitPacked_AccSat (ArrayView<T> inputA, ArrayView<T> inputB, ArrayView<T> inputC, ArrayView<T> refOutput)
	{
		StaticAssert( IsSame<T, uint> or IsSame<T, int> );

		CHECK( inputA.size() == inputB.size() );
		CHECK( inputA.size() == inputC.size() );
		CHECK( inputA.size() == refOutput.size() );

		using Packed = Conditional< IsSame<T, uint>, ubyte4, sbyte4 >;

		Array<T>	output;
		output.resize( refOutput.size() );

		for (usize i = 0; i < refOutput.size(); ++i)
		{
			Packed	a, b;
			memcpy( &a, &inputA[i], sizeof(a) );
			memcpy( &b, &inputB[i], sizeof(b) );

			T	sum = inputC[i];
			for (uint j = 0; j < 4; ++j)
			{
				sum += T(a[j]) * T(b[j]);
			}
			output[i] = sum;
		}

		String	str		= "\n| expected | shader output | error % |\n";
		float	max_err	= 0.f;
		float	avr_err	= 0.f;

		for (usize i = 0; i < refOutput.size(); ++i)
		{
			float	err	= Abs(float(output[i]) - float(refOutput[i])) * 100.f / Max( Abs(float(output[i])), 1.0e-5f );
			max_err = Max( max_err, err );
			avr_err += err;

			str << "| " << ToString( output[i] ) << " | " << ToString( refOutput[i] )
				<< " | " << ToString( err, 2 ) << "% |\n";
		}
		avr_err /= float(refOutput.size());
		str << "----------------------------------------\n";

		if ( max_err < 0.01f )
			str.clear();

		str << "max error: " << ToString( max_err, 2 ) << "%, avr: " << ToString( avr_err, 2 ) << "%";
		AE_LOGI( str );
		CHECK( max_err < 1.f );
	}


	template <typename T>
	void  FillWithLinearData (INOUT Array<T> &arr, T scale, T bias)
	{
		for (usize i = 0; i < arr.size(); ++i) {
			arr[i] = T( i * scale + bias );
		}
	}


	static void  IntegerDotProduct_4x8BitPacked_UnsignedAccelerated_Test1 (Executor &ex)
	{
		Array<uint>		input_a;
		Array<uint>		input_b;
		Array<uint>		output;
		String			src;
		const uint		warp_size = ex.GetDevice().GetDeviceProperties().compute.subgroupSize;

		input_a.resize( warp_size );
		input_b.resize( warp_size );
		output .resize( warp_size );

		FillWithLinearData( INOUT input_a, 0x03'02'03'01u, 0x01'02'03'04u );
		FillWithLinearData( INOUT input_b, 0x01'04'02'05u, 0x10'08'06'09u );

		src << R"(
			void Main ()
			{
				uint	i = gl.subgroup.Index;
				uint	a = un_InputA.data[i];
				uint	b = un_InputB.data[i];

				uint	res = IntDot4x8( a, b );

				un_Output.data[i] = res;
			}
		)";
		CHECK_FATAL( ex.Run( src, BufCast(input_a), BufCast(input_b), BufCast(input_b), BufCast(output), 4 ));

		Test_4x8BitPacked<uint>( input_a, input_b, output );
	}


	static void  IntegerDotProduct_4x8BitPacked_SignedAccelerated_Test1 (Executor &ex)
	{
		Array<int>		input_a;
		Array<int>		input_b;
		Array<int>		output;
		String			src;
		const uint		warp_size = ex.GetDevice().GetDeviceProperties().compute.subgroupSize;

		input_a.resize( warp_size );
		input_b.resize( warp_size );
		output .resize( warp_size );

		FillWithLinearData( INOUT input_a, 0x03'02'83'01, 0x01'02'03'04 );
		FillWithLinearData( INOUT input_b, 0x01'84'02'05, 0x10'88'06'09 );

		src << R"(
			void Main ()
			{
				uint	i = gl.subgroup.Index;
				int		a = int(un_InputA.data[i]);		// bitcast
				int		b = int(un_InputB.data[i]);

				int		res = IntDot4x8( a, b );

				un_Output.data[i] = uint(res);
			}
		)";
		CHECK_FATAL( ex.Run( src, BufCast(input_a), BufCast(input_b), BufCast(input_b), BufCast(output), 4 ));

		Test_4x8BitPacked<int>( input_a, input_b, output );
	}


	static void  IntegerDotProduct_4x8BitPacked_MixedSignednessAccelerated_Test1 (Executor &)
	{
		// TODO
	}


	static void  IntegerDotProduct_AccumulatingSaturating_4x8BitPacked_UnsignedAccelerated_Test1 (Executor &ex)
	{
		Array<uint>		input_a, input_b, input_c;
		Array<uint>		output;
		String			src;
		const uint		warp_size = ex.GetDevice().GetDeviceProperties().compute.subgroupSize;

		input_a.resize( warp_size );
		input_b.resize( warp_size );
		input_c.resize( warp_size );
		output .resize( warp_size );

		FillWithLinearData( INOUT input_a, 0x03'02'03'01u,	0x01'02'03'04u );
		FillWithLinearData( INOUT input_b, 0x01'04'02'05u,	0x10'08'06'09u );
		FillWithLinearData( INOUT input_c, 6232354u,		0x100u );

		src << R"(
			void Main ()
			{
				uint	i = gl.subgroup.Index;
				uint	a = un_InputA.data[i];  // packed
				uint	b = un_InputB.data[i];  // packed
				uint	c = un_InputC.data[i];  // scalar

				uint	res = IntDotAccSat4x8( a, b, c );

				un_Output.data[i] = res;
			}
		)";
		CHECK_FATAL( ex.Run( src, BufCast(input_a), BufCast(input_b), BufCast(input_c), BufCast(output), 4 ));

		Test_4x8BitPacked_AccSat<uint>( input_a, input_b, input_c, output );
	}


	static void  IntegerDotProduct_AccumulatingSaturating_4x8BitPacked_SignedAccelerated_Test1 (Executor &ex)
	{
		Array<int>		input_a, input_b, input_c;
		Array<int>		output;
		String			src;
		const uint		warp_size = ex.GetDevice().GetDeviceProperties().compute.subgroupSize;

		input_a.resize( warp_size );
		input_b.resize( warp_size );
		input_c.resize( warp_size );
		output .resize( warp_size );

		FillWithLinearData( INOUT input_a, 0x03'02'83'01,	0x01'02'03'04 );
		FillWithLinearData( INOUT input_b, 0x01'84'02'05,	0x10'88'06'09 );
		FillWithLinearData( INOUT input_c, 92384283,		-1000 );

		src << R"(
			void Main ()
			{
				uint	i = gl.subgroup.Index;
				int		a = int(un_InputA.data[i]);  // packed
				int		b = int(un_InputB.data[i]);  // packed
				int		c = int(un_InputC.data[i]);  // scalar

				int		res = IntDotAccSat4x8( a, b, c );

				un_Output.data[i] = uint(res);
			}
		)";
		CHECK_FATAL( ex.Run( src, BufCast(input_a), BufCast(input_b), BufCast(input_c), BufCast(output), 4 ));

		Test_4x8BitPacked_AccSat<int>( input_a, input_b, input_c, output );
	}


	static void  IntegerDotProduct_AccumulatingSaturating_4x8BitPacked_MixedSignednessAccelerated_Test1 (Executor &)
	{
		// TODO
	}

} // namespace


extern void Test_IntDotProduct (Executor &ex)
{
	if ( not ex.SupportsIntDotProduct() )
		return;

	auto&	props = ex.GetDevice().GetVProperties().shaderIntegerDotProductProps;

	if ( props.integerDotProduct8BitUnsignedAccelerated )
	{} // TODO

	if ( props.integerDotProduct8BitSignedAccelerated )
	{} // TODO

	if ( props.integerDotProduct8BitMixedSignednessAccelerated )
	{} // TODO

	if ( props.integerDotProduct4x8BitPackedUnsignedAccelerated )
		IntegerDotProduct_4x8BitPacked_UnsignedAccelerated_Test1( ex );

	if ( props.integerDotProduct4x8BitPackedSignedAccelerated )
		IntegerDotProduct_4x8BitPacked_SignedAccelerated_Test1( ex );

	if ( props.integerDotProduct4x8BitPackedMixedSignednessAccelerated )
		IntegerDotProduct_4x8BitPacked_MixedSignednessAccelerated_Test1( ex );

	if ( props.integerDotProduct16BitUnsignedAccelerated )
	{} // TODO

	if ( props.integerDotProduct16BitSignedAccelerated )
	{} // TODO

	if ( props.integerDotProduct16BitMixedSignednessAccelerated )
	{} // TODO

	if ( props.integerDotProduct32BitUnsignedAccelerated )
	{} // TODO

	if ( props.integerDotProduct32BitSignedAccelerated )
	{} // TODO

	if ( props.integerDotProduct32BitMixedSignednessAccelerated )
	{} // TODO

	if ( props.integerDotProduct64BitUnsignedAccelerated )
	{} // TODO

	if ( props.integerDotProduct64BitSignedAccelerated )
	{} // TODO

	if ( props.integerDotProduct64BitMixedSignednessAccelerated )
	{} // TODO

	if ( props.integerDotProductAccumulatingSaturating8BitUnsignedAccelerated )
	{} // TODO

	if ( props.integerDotProductAccumulatingSaturating8BitSignedAccelerated )
	{} // TODO

	if ( props.integerDotProductAccumulatingSaturating8BitMixedSignednessAccelerated )
	{} // TODO

	if ( props.integerDotProductAccumulatingSaturating4x8BitPackedUnsignedAccelerated )
		IntegerDotProduct_AccumulatingSaturating_4x8BitPacked_UnsignedAccelerated_Test1( ex );

	if ( props.integerDotProductAccumulatingSaturating4x8BitPackedSignedAccelerated )
		IntegerDotProduct_AccumulatingSaturating_4x8BitPacked_SignedAccelerated_Test1( ex );

	if ( props.integerDotProductAccumulatingSaturating4x8BitPackedMixedSignednessAccelerated )
		IntegerDotProduct_AccumulatingSaturating_4x8BitPacked_MixedSignednessAccelerated_Test1( ex );

	if ( props.integerDotProductAccumulatingSaturating16BitUnsignedAccelerated )
	{} // TODO

	if ( props.integerDotProductAccumulatingSaturating16BitSignedAccelerated )
	{} // TODO

	if ( props.integerDotProductAccumulatingSaturating16BitMixedSignednessAccelerated )
	{} // TODO

	if ( props.integerDotProductAccumulatingSaturating32BitUnsignedAccelerated )
	{} // TODO

	if ( props.integerDotProductAccumulatingSaturating32BitSignedAccelerated )
	{} // TODO

	if ( props.integerDotProductAccumulatingSaturating32BitMixedSignednessAccelerated )
	{} // TODO

	if ( props.integerDotProductAccumulatingSaturating64BitUnsignedAccelerated )
	{} // TODO

	if ( props.integerDotProductAccumulatingSaturating64BitSignedAccelerated )
	{} // TODO

	if ( props.integerDotProductAccumulatingSaturating64BitMixedSignednessAccelerated )
	{} // TODO

	TEST_PASSED();
}
