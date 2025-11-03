// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Executor.h"

namespace
{

	static void  VecMatMulAdd (MutableArrayView<float> output, ArrayView<float> input, ArrayView<float> weights, ArrayView<float> bias, const bool columnMajor)
	{
		const uint	rowA	= uint(input.size());
		const uint	rowC	= uint(output.size());

		CHECK( rowC == bias.size() );
		CHECK( rowC * rowA == weights.size() );
		
		if ( columnMajor )
		{
			for (uint i = 0; i < rowC; ++i)
			{
				float	sum = float(bias[i]);
				
				for (uint j = 0; j < rowA; ++j)
				{
					sum += float(weights[ i + j * rowC ]) * float(input[j]);
				}
				output[i] = sum;
			}
		}
		else
		{
			for (uint i = 0; i < rowC; ++i)
			{
				float	sum = float(bias[i]);
				
				for (uint j = 0; j < rowA; ++j)
				{
					sum += float(weights[ i * rowA + j ]) * float(input[j]);
				}
				output[i] = sum;
			}
		}
	}


	static void  VecMatMulAdd (MutableArrayView<sint> output, ArrayView<ubyte> input, ArrayView<ubyte> weights, ArrayView<sint> bias, const bool columnMajor)
	{
		const uint	rowA	= uint(input.size());
		const uint	rowC	= uint(output.size());

		CHECK( rowC == bias.size() );
		CHECK( rowC * rowA == weights.size() );
		
		if ( columnMajor )
		{
			for (uint i = 0; i < rowC; ++i)
			{
				slong	sum = bias[i];
				
				for (uint j = 0; j < rowA; ++j)
				{
					int	wg = weights[ i + j * rowC ];
					int in = input[j];
					int	d  = wg * in;
					sum += d;
				}
				output[i] = CheckCast{ sum };
			}
		}
		else
		{
			for (uint i = 0; i < rowC; ++i)
			{
				slong	sum = bias[i];
				
				for (uint j = 0; j < rowA; ++j)
				{
					int	wg = weights[ i * rowA + j ];
					int	in = input[j];
					int d  = wg * in;
					sum += d;
				}
				output[i] = CheckCast{ sum };
			}
		}
	}


	static const float	c_ReLU = 0.001f;

	static void  ReLU (INOUT MutableArrayView<float> output)
	{
		for (usize i = 0; i < output.size(); ++i)
		{
			output[i] = Max( output[i], output[i] * c_ReLU );
		}
	}


	static float4  groundtruth (float2 v)
	{
		return float4{
			(v.x + v.y) / (1.0f + v.y * v.y),
			2.0f * v.x + v.y,
			0.5f * v.x * v.x + 1.2f * v.y,
			v.x + 0.5f * v.y * v.y
		};
	}


	static const float	layer1_weights[] = {
		-0.0928f,  0.0139f,  0.4922f, -0.0628f,
		-0.7881f,  0.8369f, -0.0995f, -0.5659f,
		-0.6118f, -0.0479f,  0.4529f,  1.0029f,
		 0.1842f,  0.1786f,  0.3518f,  0.4033f,
		-0.1718f,  0.7925f,  0.0148f,  0.6353f,
		 0.0883f, -0.1793f,  0.3452f,  0.7446f,
		-0.4482f, -0.8584f, -0.9048f, -0.3621f,
		-0.1393f,  0.9072f, -0.4216f,  0.5576f,
		-0.5093f,  0.7363f,  0.1106f,  0.0964f,
		 0.4783f,  0.1877f, -0.2881f, -0.6841f,
		-0.6191f, -0.2073f,  0.0620f, -0.2289f,
		 0.4561f,  0.4419f,  1.0430f,  0.5889f,
		 0.3206f,  0.6357f, -0.4717f,  0.4082f,
		 0.3484f, -0.7026f,  0.2219f,  0.3301f,
		 0.0861f, -1.1680f, -0.1084f, -0.7729f,
		 0.8970f,  0.8696f, -0.5347f,  0.2196f
	};
	static const float	layer1_bias[] = {
		 0.7134f,
		 0.9053f,
		-0.2942f,
		-1.0752f,
		-0.2935f,
		 0.5703f,
		-0.1776f,
		-0.2017f,
		 0.8032f,
		 0.0580f,
		 0.3833f,
		 0.3315f,
		 0.5352f,
		 0.6489f,
		 0.5229f,
		-0.0424f
	};
		
	static const float	layer2_weights[] = {
		 0.3367f,  0.3921f, -0.4937f, -0.9429f,
		 0.1268f, -0.4663f,  0.1044f,  0.2329f,
		-0.2917f,  0.9966f, -0.2367f,  0.4331f,
		-0.1051f,  0.3770f, -0.3362f,  0.2964f,
		 0.4985f, -0.8521f, -0.2500f,  0.1519f,
		-0.1794f,  0.0366f, -0.4055f, -0.0549f,
		-0.3408f,  0.0943f, -0.1816f,  0.3987f,
		 0.4573f, -0.0176f, -0.3452f,  0.7578f,
		-0.4170f, -0.2264f, -0.1010f, -0.0543f,
		 0.0832f, -0.0303f, -0.6582f, -0.0494f,
		 0.8413f,  0.0787f, -0.0286f,  0.5908f,
		-0.5493f, -0.4963f, -0.1473f,  0.2625f,
		-0.1842f, -1.1016f, -0.3655f,  0.2754f,
		 0.4307f, -0.5361f, -0.2419f, -0.1438f,
		 0.4321f,  0.5103f,  0.1064f,  0.2695f,
		 0.8521f,  0.5054f, -0.3196f, -0.3792f
	};
	static const float	layer2_bias[] = {
		-0.1865f,
		 0.5977f,
		 0.3621f,
		 0.3047f
	};


	static void  MLP_Test1 ()
	{
		const bool	columnMajor = false;
		float		output1 [16];
		float		output2 [4];
		float		max_diff	= 0.f;

		for (float y = 0.f; y < 1.0f; y += 0.1f)
		for (float x = 0.f; x < 1.0f; x += 0.1f)
		{
			float	input[] = { x, y, x*x, y*y };

			VecMatMulAdd( OUT output1, input, layer1_weights, layer1_bias, columnMajor );
			ReLU( INOUT output1 );

			VecMatMulAdd( OUT output2, output1, layer2_weights, layer2_bias, columnMajor );
			ReLU( INOUT output2 );

			float4	ref = groundtruth( float2{ input[0], input[1] });

			float	diff = 0.f;
			diff += Abs( ref[0] - output2[0] );
			diff += Abs( ref[1] - output2[1] );
			diff += Abs( ref[2] - output2[2] );
			diff += Abs( ref[3] - output2[3] );

			max_diff = Max( max_diff, diff );
			CHECK( diff < 0.1f );
		}
		AE_LOGI( "Max diff: "s << ToString( max_diff, 2, True{"exp"} ));
	}

	
	static void  MLP_Test2 ()
	{
		float	layer1_weights2 [CountOf( layer1_weights )];
		float	layer1_bias2	[CountOf( layer1_bias )];
		float	layer2_weights2	[CountOf( layer2_weights )];
		float	layer2_bias2	[CountOf( layer2_bias )];
		float	output1			[16];
		float	output2			[4];
		float	q_output1		[16];
		float	q_output2		[4];
		float	max_diff		= 0.f;
		float2	scale_bias;
		float2	inv_scale_bias;

		const bool	columnMajor = false;

		const auto	CalcMinMax = [](ArrayView<float> arr, INOUT float &min, INOUT float &max)
		{{
			for (usize i = 0; i < arr.size(); ++i)
			{
				min = Min( min, arr[i] );
				max = Max( max, arr[i] );
			}
		}};
		const auto	Convert = [](ArrayView<float> input, OUT MutableArrayView<float> output, float2 scaleBias)
		{{
			CHECK( input.size() == output.size() );
			for (usize i = 0; i < input.size(); ++i)
			{
				output[i] = (input[i] + scaleBias.y) * scaleBias.x;

				ASSERT( output[i] >= 0.f );
				ASSERT( output[i] <= 255.f );
			}
		}};

		{
			float	min		= MaxValue<float>();
			float	max		= 0.f;

			CalcMinMax( layer1_weights, INOUT min, INOUT max );
			CalcMinMax( layer1_bias,	INOUT min, INOUT max );
			CalcMinMax( layer2_weights, INOUT min, INOUT max );
			CalcMinMax( layer2_bias,	INOUT min, INOUT max );

			scale_bias = float2{ 255.f / (max - min), -min };

			Convert( layer1_weights, OUT layer1_weights2, scale_bias );
			Convert( layer2_weights, OUT layer2_weights2, scale_bias );
			Convert( layer1_bias,    OUT layer1_bias2,    scale_bias );
			Convert( layer2_bias,    OUT layer2_bias2,    scale_bias );

			for (float& a : layer1_bias2) a *= 255.f;

			inv_scale_bias = float2{ (max - min) / 255.f, min };
		}
		
		for (float y = 0.f; y < 1.0f; y += 0.1f)
		for (float x = 0.f; x < 1.0f; x += 0.1f)
		{
			float	input[]		= { x, y, x*x, y*y };
			float	q_input[]	= { x, y, x*x, y*y };

			for (float& a : q_input) a *= 255.f;

			VecMatMulAdd( OUT q_output1, q_input, layer1_weights2, layer1_bias2, columnMajor );
			VecMatMulAdd( OUT output1,   input,   layer1_weights,  layer1_bias,  columnMajor );
			ReLU( INOUT output1 );

			float	sum_in1 = 1.f;
			float	sum_in2 = 1.f;
			for (float in : input) {
				sum_in1 += in;
			}
			
			for (usize i = 0; i < CountOf(output1); ++i)
			{
				q_output1[i] = q_output1[i] * inv_scale_bias.x / 255.f + inv_scale_bias.y * sum_in1;
				q_output1[i] = Max( q_output1[i], q_output1[i] * c_ReLU );

				float diff = Abs( q_output1[i] - output1[i] );
				ASSERT( diff < 0.001f );

				sum_in2 += q_output1[i];
			}
			
			VecMatMulAdd( OUT q_output2, q_output1, layer2_weights2, layer2_bias2, columnMajor );
			VecMatMulAdd( OUT output2,   output1,   layer2_weights,  layer2_bias,  columnMajor );
			ReLU( INOUT output2 );

			
			for (usize i = 0; i < CountOf(output2); ++i)
			{
				q_output2[i] = q_output2[i] * inv_scale_bias.x + inv_scale_bias.y * sum_in2;
				q_output2[i] = Max( q_output2[i], q_output2[i] * c_ReLU );

				float diff = Abs( q_output2[i] - output2[i] );
				ASSERT( diff < 0.001f );
			}
			
			float4	ref = groundtruth( float2{ input[0], input[1] });

			float	diff = 0.f;
			diff += Abs( ref[0] - output2[0] );
			diff += Abs( ref[1] - output2[1] );
			diff += Abs( ref[2] - output2[2] );
			diff += Abs( ref[3] - output2[3] );

			max_diff = Max( max_diff, diff );
			CHECK( diff < 0.1f );
		}
		AE_LOGI( "Max diff: "s << ToString( max_diff, 2, True{"exp"} ));
	}

	
	static void  MLP_Test3 ()
	{
		ubyte	q_layer1_weights	[CountOf( layer1_weights )];
		sint	q_layer1_bias		[CountOf( layer1_bias )];
		ubyte	q_layer2_weights	[CountOf( layer2_weights )];
		sint	q_layer2_bias		[CountOf( layer2_bias )];
		float	max_diff			= 0.f;
		float2	scale_bias;
		float2	inv_scale_bias;

		const bool	columnMajor = false;

		const auto	CalcMinMax = [](ArrayView<float> arr, INOUT float &min, INOUT float &max)
		{{
			for (usize i = 0; i < arr.size(); ++i)
			{
				min = Min( min, arr[i] );
				max = Max( max, arr[i] );
			}
		}};
		const auto	Convert = []<typename T> (ArrayView<float> input, OUT MutableArrayView<T> output, float2 scaleBias)
		{{
			CHECK( input.size() == output.size() );
			float	md = 0.f;

			for (usize i = 0; i < input.size(); ++i)
			{
				output[i] = T( (input[i] + scaleBias.y) * scaleBias.x + 0.5f );

				float	a = float(output[i]);
				a = a / scaleBias.x - scaleBias.y;

				float	d = Abs( a - input[i] );
				md = Max( d, md );
				ASSERT( d < 0.1f );
			}
			AE_LOGI( "Quantization error: "s << ToString( md, 2, True{"exp"} ));
		}};

		{
			float	min		= MaxValue<float>();
			float	max		= 0.f;

			CalcMinMax( layer1_weights, INOUT min, INOUT max );
			CalcMinMax( layer1_bias,	INOUT min, INOUT max );
			CalcMinMax( layer2_weights, INOUT min, INOUT max );
			CalcMinMax( layer2_bias,	INOUT min, INOUT max );

			scale_bias = float2{ 255.f / (max - min), -min };

			Convert( layer1_weights, OUT MutableArrayView<ubyte>{ q_layer1_weights }, scale_bias );
			Convert( layer2_weights, OUT MutableArrayView<ubyte>{ q_layer2_weights }, scale_bias );
			Convert( layer1_bias,    OUT MutableArrayView<sint >{ q_layer1_bias },    scale_bias );
			Convert( layer2_bias,    OUT MutableArrayView<sint >{ q_layer2_bias },    scale_bias );
			
			for (auto& a : q_layer1_bias) a *= 255;
			for (auto& a : q_layer2_bias) a *= 255;

			inv_scale_bias = float2{ (max - min) / (255.f * 255.f), min };
		}
		
		for (float y = 0.f; y < 1.0f; y += 0.1f)
		for (float x = 0.f; x < 1.0f; x += 0.1f)
		{
			float	input[]		= { x, y, x*x, y*y };
			float	output1		[16];
			ubyte	q_input1	[CountOf( input )];
			sint	q_output1	[CountOf( output1 )];
			ubyte	q_input2	[CountOf( q_output1 )];
			float	sum_in1		= 1.f;
			float	sum_in2		= 1.f;

			for (usize i = 0; i < CountOf(input); ++i)
			{
				sum_in1 += input[i];
				q_input1[i] = ubyte( input[i] * 255.f + 0.5f );
			}

			VecMatMulAdd( OUT q_output1, q_input1, q_layer1_weights, q_layer1_bias, columnMajor );
			VecMatMulAdd( OUT output1,   input,    layer1_weights,   layer1_bias,   columnMajor );
			ReLU( INOUT output1 );
			
			for (usize i = 0; i < CountOf(output1); ++i)
			{
				float	a = float(q_output1[i]) * inv_scale_bias.x + inv_scale_bias.y * sum_in1;
				a = Max( a, a * c_ReLU );

				float diff = Abs( a - output1[i] );
				ASSERT( diff < 0.04f );

				output1[i] = Saturate( output1[i] );
				a = Saturate( a );

				sum_in2 += a;

				//ASSERT( a >= -0.1f );
				//ASSERT( a <= 1.2f );

				q_input2[i] = ubyte( a * 255.f + 0.5f );
			}
			
			float	output2		[4];
			sint	q_output2	[CountOf( output2 )];

			VecMatMulAdd( OUT q_output2, q_input2, q_layer2_weights, q_layer2_bias, columnMajor );
			VecMatMulAdd( OUT output2,   output1,  layer2_weights,   layer2_bias,   columnMajor );
			ReLU( INOUT output2 );

			for (usize i = 0; i < CountOf(output2); ++i)
			{
				float	a = float(q_output2[i]) * inv_scale_bias.x + inv_scale_bias.y * sum_in2;
				a = Max( a, a * c_ReLU );

				float diff = Abs( a - output2[i] );
				ASSERT( diff < 0.04f );
			}
			/*
			float4	ref = groundtruth( float2{ input[0], input[1] });

			float	diff = 0.f;
			diff += Abs( ref[0] - output2[0] );
			diff += Abs( ref[1] - output2[1] );
			diff += Abs( ref[2] - output2[2] );
			diff += Abs( ref[3] - output2[3] );

			max_diff = Max( max_diff, diff );
			CHECK( diff < 0.1f );*/
		}
		AE_LOGI( "Max diff: "s << ToString( max_diff, 2, True{"exp"} ));
	}

	
	static void  MLP_Test4 ()
	{
		const float	layer1_weights2 [] = {
			1.0f, 1.0f,
			1.0f, 1.0f
		};
		const float	layer1_bias2 [] = {
			0.0f,
			0.0f
		};
		float	q_layer1_weights	[CountOf( layer1_weights2 )];
		float	q_layer1_bias		[CountOf( layer1_bias2 )];
		float	output1 [2];
		float	output2 [2];

		const float		scale		= 3.0f;
		const float		inv_scale	= 1.0f / scale;
		const float		bias		= 0.2f;

		for (usize i = 0; i < CountOf(layer1_weights2); ++i) {
			q_layer1_weights[i] = (layer1_weights2[i] + bias) * scale;
		}
		for (usize i = 0; i < CountOf(layer1_bias2); ++i) {
			q_layer1_bias[i] = (layer1_bias2[i] + bias) * scale * 255.f;
		}
		
		for (float y = 0.f; y < 1.0f; y += 0.1f)
		for (float x = 0.f; x < 1.0f; x += 0.1f)
		{
			float	input[]		= { x, y };
			float	q_input[]	= { x * 255.f, y * 255.f };

			VecMatMulAdd( OUT output1, input,   layer1_weights2,  layer1_bias2,  false );
			VecMatMulAdd( OUT output2, q_input, q_layer1_weights, q_layer1_bias, false );

			float	sum_in = 0.f;
			for (float in : input) {
				sum_in += in;
			}

			for (usize i = 0; i < CountOf(output1); ++i)
			{
				float	b = output2[i] * inv_scale / 255.f - bias * (sum_in + 1.f);
				float	diff = Abs( b - output1[i] );
				ASSERT( diff < 0.01f );
			}
		}
	}
}


extern void Test_MLPTraining (Executor &)
{
//	MLP_Test1();
	MLP_Test2();
	MLP_Test3();
//	MLP_Test4();

	TEST_PASSED();
}
