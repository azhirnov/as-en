// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	copied form slang examples
	https://github.com/shader-slang/slang/tree/master/examples/mlp-training (Apache-2.0 license)

	Problems and solutions with porting slang to ResEditor:
	 * can not pass slang class as input argument - this class is not known by engine reflection system so it is break everything.
	 * slang doesn't allow to cast pointers - I can't pass DeviceAddress to slang and convert it to 'MyNetwork' class.
	 * used GLSL 'INIT' pass to fill 'ArgBuffer' with DeviceAddress, then same buffer passed to slang with typed pointers, and then it initialize slang classes.
	 * shader can be much simpler when slang will support [Differentiable] for GLSL.

	video:
	[Neural Shading Course: Part 6 - How to Generate Derivatives for Complex Code Automatically](https://youtu.be/B9-YyYuNIfI)
	[Neural Shading Course: Part 7 - How to Get the Best Hardware Acceleration using Cooperative Vectors](https://youtu.be/o-m5hP_9yLE)

	TODO: image generation
	https://www.youtube.com/watch?v=1LaTljisOiM
	https://youtu.be/inZ4mkTbd2c
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <slang-shader.h>
#	define LEARN_GRADIENT
#	define ADJUST_PARAMETERS
#	define INIT
#	define VIEW
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		Assert( GetFeatureSet().hasShaderAtomicPackedFp16(), "required 'ShaderAtomicPackedFp16' feature" );

		// initialize
		RC<Image>	rt				= Image( EPixelFormat::RGBA16F, SurfaceSize() );
		const uint	large_buf_size	= 128 << 20;
		RC<Buffer>	large_buf		= Buffer( large_buf_size );		// initialized to zero
		RC<Buffer>	arg_buf			= Buffer();
		RC<Buffer>	cbuf			= Buffer();
		const uint	wg_size			= 256;
		const uint	wg_count		= 1;

		large_buf.Name( "LargeBuf" );
		arg_buf.Name( "ArgBuffer" );
		cbuf.Name( "ConstBuf" );

		{
			arg_buf.UseLayout(
				"ArgBuffer",

				"	uint		enable;"s +
				"	uint		iteration;" +
				"	uint		iteration2;" +

				// LEARN_GRADIENT
				"	half *		layer1_weights;" +
				"	half *		layer1_weightsGrad;" +
				"	half *		layer1_biases;" +
				"	half *		layer1_biasesGrad;" +

				"	half *		layer2_weights;" +
				"	half *		layer2_weightsGrad;" +
				"	half *		layer2_biases;" +
				"	half *		layer2_biasesGrad;" +

				"	float2 *	inputs;" +
				"	uint		inputCount;" +
				"	Atomic<uint>	loss;" +

				// ADJUST_PARAMETERS
				"	uint		gradientsCount;" +

				"	half *		adamState_mean;" +
				"	half *		adamState_variance;" +
				"	int *		adamState_iteration;" +

				"	half *		params;" +
				"	half *		gradients;"
			);
			arg_buf.AddReference( large_buf );

			cbuf.ULong( "address",	large_buf.DeviceAddress() );
		}{
			array<float>	rnd_values;
			Random			rnd;

			rnd_values.resize( 1024 );
			for (uint i = 0; i < rnd_values.size(); ++i) {
				rnd_values[i] = rnd.Uniform( 0.0f, 1.0f );
			}

			cbuf.FloatArray( "rndValues",	rnd_values );
		}

		string	def;
		if ( not GetFeatureSet().hasCooperativeVectorTraining() )
			def += "; SOFTWARE;";

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT" );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
			pass.ArgInOut(	"un_ArgBuf",	arg_buf );
			pass.ArgIn(		"un_CBuf",		cbuf );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
			pass.Slider(	"iMaxIter",			0,	1000,	0 );
			pass.Slider(	"iNoLimits",		0,	1,		0 );
			pass.Slider(	"iRndInput",		0,	4,		0 );	// randomise input for each step
			pass.Slider(	"iInUpdInterval",	1,	10,		8 );
		}{
			RC<ComputePass>		pass = ComputePass( "", "LEARN_GRADIENT"+def );
			pass.AddFlag( EPassFlags::UseSLang );
			pass.ArgInOut(	"un_ArgBuf",	arg_buf );
			pass.LocalSize( wg_size );
			pass.DispatchGroups( wg_count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "ADJUST_PARAMETERS"+def );
			pass.AddFlag( EPassFlags::UseSLang );
			pass.ArgIn(		"un_ArgBuf",	arg_buf );
			pass.LocalSize( wg_size );
			pass.DispatchGroups( wg_count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "VIEW" );
			pass.AddFlag( EPassFlags::UseSLang );
			pass.ArgOut(	"un_Image",		rt );
			pass.ArgIn(		"un_ArgBuf",	arg_buf );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
			pass.Slider(	"iCmp",			0,		3,		3 );
			pass.Slider(	"iSlider",		0.f,	1.f,	0.5f );
			pass.Slider(	"iCmpScale",	-2,		10,		-2 );
			pass.Slider(	"iShowInput",	0,		1,		1 );
		}

		RC<DynamicUInt>	loss = DynamicUInt();
		ReadBuffer( loss, arg_buf, "loss" );

		RC<DynamicUInt>	iteration = DynamicUInt();
		ReadBuffer( iteration, arg_buf, "iteration" );

		Label( loss.AsFloat(),	"loss" );
		Label( iteration,		"iteration" );

		Present( rt );

		const uint weights_size = 128 + 64 + 128 + 64;

		Export( large_buf, "mlp-4x16-fp16-.bin", 0, weights_size );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(LEARN_GRADIENT) or defined(ADJUST_PARAMETERS) or defined(VIEW)

	#define SOFTWARE
	#define INOUT

	//-----------------------------------------------------------------------------
	// common.slang

	public typealias NFloat = half;


#ifdef SOFTWARE
	//-----------------------------------------------------------------------------
	// mlvec_sw.slang

	public struct MLVec<int N> : IDifferentiable
	{
		public NFloat data[N];

		[Differentiable]
		public NFloat[N] toArray()
		{
			return data;
		}

		[Differentiable]
		public static MLVec<N> fromArray(NFloat[N] values)
		{
			MLVec<N> result;
			[ForceUnroll]
			for (int i = 0; i < N; i++)
				result.data[i] = values[i];
			return result;
		}
	}

	MLVec<OutputSize> matMulAdd<int OutputSize, int InputSize>(MLVec<InputSize> input, NFloat* matrix, NFloat* bias)
	{
		let getMatElem = (int row, int col) => matrix[row*InputSize + col];
		let getBias = (int idx) => bias[idx];
		MLVec<OutputSize> result = {};
		for (int i = 0; i < OutputSize; i++)
		{
			NFloat r = getBias(i);
			for (int j = 0; j < InputSize; j++)
				r += getMatElem(i, j) * input.data[j];
			result.data[i] = r;
		}
		return result;
	}

	MLVec<OutputSize> matMulTransposed<int OutputSize, int InputSize>(MLVec<InputSize> input, NFloat* matrix)
	{
		let getMatElem = (int row, int col) => matrix[col*OutputSize + row];
		MLVec<OutputSize> result = {};
		for (int i = 0; i < OutputSize; i++)
		{
			NFloat r = {};
			for (int j = 0; j < InputSize; j++)
				r += getMatElem(i, j) * input.data[j];
			result.data[i] = r;
		}
		return result;
	}

	void outerProductAccumulate<int M, int N>(MLVec<M> v0, MLVec<N> v1, NFloat* matrix)
	{
		for (int i = 0; i < M; i++)
		{
			for (int j = 0; j < N; j++)
			{
				let elem = v0.data[i] * v1.data[j];
				half original;
				InterlockedAddF16Emulated(matrix + (i*N + j), elem, original);	// VK_NV_shader_atomic_float16_vector
			}
		}
	}

#else
	//-----------------------------------------------------------------------------
	// mlvec.slang

	public struct MLVec<int N> : IDifferentiable
	{
		public CoopVec<NFloat, N> data;
		public typealias Differential = MLVec<N>;

		public static MLVec<N> fromArray(NFloat[N] values)
		{
			MLVec<N> result;
			[ForceUnroll]
			for (int i = 0; i < N; i++)
				result.data[i] = values[i];
			return result;
		}

		internal static NFloat[N] coopVecToArray(CoopVec<NFloat, N> v)
		{
			NFloat[N] arr;
			[ForceUnroll]
			for (int i = 0; i < N; i++)
				arr[i] = v[i];
			return arr;
		}

		[BackwardDerivativeOf(fromArray)]
		internal static void fromArrayBwd(inout DifferentialPair<NFloat[N]> values, MLVec<N> dResult)
		{
			values = diffPair(values.p, coopVecToArray(dResult.data));
		}

		internal static NFloat[N] toArray(MLVec<N> vec)
		{
			return coopVecToArray(vec.data);
		}

		[BackwardDerivativeOf(toArray)]
		internal static void toArrayBwd(inout DifferentialPair<MLVec<N>> vec, NFloat[N] dResult)
		{
			vec = diffPair(vec.p, MLVec<N>.fromArray(dResult));
		}

		[Differentiable]
		public NFloat[N] toArray()
		{
			return toArray(this);
		}

		public override static Differential dadd(Differential d0, Differential d1)
		{
			return {d0.data + d1.data};
		}
		public override static Differential dmul<U:__BuiltinRealType>(U s, Differential d)
		{
			return {d.data * __realCast<NFloat>(s)};
		}
		public override static Differential dzero()
		{
			return {};
		}
	}
#endif

#ifdef SOFTWARE
	//-----------------------------------------------------------------------------
	// mlp_sw.slang

	public struct FeedForwardLayer<int InputSize, int OutputSize>
	{
		public NFloat* weights;
		public NFloat* weightsGrad;
		public NFloat* biases;
		public NFloat* biasesGrad;

		[BackwardDerivative(evalBwd)]  // will use 'evalBwd' instead of auto-generated derivatives
		public MLVec<OutputSize> eval(MLVec<InputSize> input)
		{
			var output = matMulAdd<OutputSize>(
				input,
				weights,
				biases);
			// ReLU activation
			for (int i = 0; i < OutputSize; i++)
				if (output.data[i] < 0.0)
					output.data[i] *= 0.001h;
			return output;
		}

		public void evalBwd(
			inout DifferentialPair<MLVec<InputSize>> input,
			MLVec<OutputSize> resultGrad)
		{
			let fwd = eval(input.p);

			// Back-prop resultGrad through activation.
			for (int i = 0; i < OutputSize; i++)
			{
				if (fwd.data[i] < 0.0)
					resultGrad.data[i] *= 0.001h;
			}

			// Back-prop gradients to the weights matrix.
			outerProductAccumulate(
				resultGrad,
				input.p,
				weightsGrad);

			// Back-prop gradients to the biases vector.
			for (int i = 0; i < OutputSize; i++)
			{
				NFloat originalValue;
				InterlockedAddF16Emulated(biasesGrad + i, resultGrad.data[i], originalValue);	// VK_NV_shader_atomic_float16_vector
			}

			// Back-prop gradients to the input vector.
			let dInput = matMulTransposed<InputSize>(resultGrad, weights);

			input = {input.p, dInput};
		}
	}

#else
	//-----------------------------------------------------------------------------
	// mlp.slang

	static const CoopVecComponentType kComponentType = CoopVecComponentType.Float16;

	public struct FeedForwardLayer<int InputSize, int OutputSize>
	{
		internal void* weights;
		internal void* weightsGrad;
		internal void* biases;
		internal void* biasesGrad;

		public MLVec<OutputSize> eval(MLVec<InputSize> input)
		{
			// Compute mul(weights, inputVec) + biases.
			// `weights` is treated as an OutputSize(row) x InputSize(col) matrix.
			var output = coopVecMatMulAdd<NFloat, OutputSize>(
				input.data, kComponentType, // input and format
				weights, kComponentType, // weights and format
				biases, kComponentType, // biases and format
				CoopVecMatrixLayout.RowMajor, // matrix layout
				false, // transpose matrix? must be `false` since we specified RowMajor.
				InputSize * sizeof(NFloat)); // matrix stride
			output = max(output, output * 0.001h); // Leaky ReLU activation
			return {output};
		}

		[BackwardDerivativeOf(eval)]
		public void evalBwd(
			inout DifferentialPair<MLVec<InputSize>> input,
			MLVec<OutputSize> resultGrad)
		{
			let fwd = eval(input.p);

			// Back-prop resultGrad through activation.
			[ForceUnroll]
			for (int i = 0; i < OutputSize; i++)
			{
				if (fwd.data[i] < 0.0)
					resultGrad.data[i] *= 0.01h;
			}

			// Back-prop gradients to the weights matrix.
			coopVecOuterProductAccumulate(
				resultGrad.data,
				input.p.data,
				weightsGrad,
				0, // matrixStride, ignored since layout is TrainingOptimal
				CoopVecMatrixLayout.TrainingOptimal, // matrix layout, must be TrainingOptimal.
				kComponentType);

			// Back-prop gradients to the biases vector.
			coopVecReduceSumAccumulate(resultGrad.data, (void*)biasesGrad);

			// Back-prop gradients to the input vector by computing
			// mul(transpose(weights), resultGrad).
			// By specifying the matrix layout as ColumnMajor, we can
			// achieve the effect of transposing the weights matrix.
			let dInput = coopVecMatMul<NFloat, InputSize>(
				resultGrad.data, kComponentType,
				weights, kComponentType,
				CoopVecMatrixLayout.ColumnMajor,
				false,  // transpose, must be `false` since we specified ColumnMajor.
				InputSize * sizeof(NFloat));

			input = {input.p, {dInput}};
		}
	}
#endif


	//-----------------------------------------------------------------------------
	// adam.slang

	public struct AdamState
	{
		internal NFloat mean;
		internal NFloat variance;
		internal int iteration;
	}

	public struct AdamOptimizer
	{
		// Adam parameters
		public static const NFloat beta1 = 0.9h;
		public static const NFloat beta2 = 0.999h;
		public static const NFloat epsilon = 1.0e-5h;
		public static const NFloat learningRate = 0.01h;

		public static void step(inout AdamState state, inout NFloat param, inout NFloat grad)
		{
			state.iteration++;
			if (isinf(grad))
			{
				if (grad > 0)
					grad = 10000.0h;
				else
					grad = -10000.0h;
			}
			state.mean = beta1 * state.mean + (NFloat(1.f) - beta1) * grad;
			state.variance = beta2 * state.variance + (NFloat(1.f) - beta2) * grad * grad;
			NFloat meanHat = state.mean / (NFloat(1.f) - pow(beta1, NFloat(state.iteration)));
			NFloat varianceHat = state.variance / (NFloat(1.f) - pow(beta2, NFloat(state.iteration)));
			param -= learningRate * meanHat / (sqrt(max(NFloat(0.f), varianceHat) + epsilon));

			// clear gradient
			grad = NFloat(0.f);
		}
	}


	//-----------------------------------------------------------------------------
	// network.slang

	public struct MyNetwork
	{
		public FeedForwardLayer<4, 16> layer1;
		public FeedForwardLayer<16, 4> layer2;

		[Differentiable]
		internal MLVec<4> encodeInput(NFloat x, NFloat y)
		{
			return MLVec<4>.fromArray({
					x,
					y,
					x*x,
					y*y,
				});
		}

		[Differentiable]
		internal MLVec<4> _eval(NFloat x, NFloat y)
		{
			let encoding = encodeInput(x, y);
			let layer1Output = layer1.eval(encoding);		// 4D -> 16D
			let layer2Output = layer2.eval(layer1Output);	// 16D -> 4D
			return layer2Output;
		}

		[Differentiable]
		public half4 eval(no_diff NFloat x, no_diff NFloat y)
		{
			let mlv = _eval(x, y);
			let arr = mlv.toArray();
			return half4(arr[0], arr[1], arr[2], arr[3]); // MLVec to half4
		}
	}

	[Differentiable]
	public half loss(inout MyNetwork network, no_diff half x, no_diff half y)
	{
		let networkResult = network.eval(x, y);
		let gt = no_diff groundtruth(x, y);
		let diff = networkResult - gt;

		return dot(diff, diff); // Mean‑Squared Error
	}

	// function which will be approximated
	public half4 groundtruth(half x, half y)
	{
		return {
			(x + y) / (1 + y * y),
			2 * x + y,
			0.5 * x * x + 1.2 * y,
			x + 0.5 * y * y,
		};
	}

#endif
//-----------------------------------------------------------------------------
#ifdef LEARN_GRADIENT

	// from kernels.slang
	SLANG_ENTRY_OUT  Main (SLANG_ENTRY_IN)
	{
		const uint tid = hl_DispatchThreadID.x;

		let arg = un_ArgBuf[0];

		if ( arg.enable == 0 )
			return;

		MyNetwork network;

	#ifdef SOFTWARE
		network.layer1.weights		= arg.layer1_weights;
		network.layer1.weightsGrad	= arg.layer1_weightsGrad;
		network.layer1.biases		= arg.layer1_biases;
		network.layer1.biasesGrad	= arg.layer1_biasesGrad;

		network.layer2.weights		= arg.layer2_weights;
		network.layer2.weightsGrad	= arg.layer2_weightsGrad;
		network.layer2.biases		= arg.layer2_biases;
		network.layer2.biasesGrad	= arg.layer2_biasesGrad;
	#else
		network.layer1.weights		= (void*)arg.layer1_weights;
		network.layer1.weightsGrad	= (void*)arg.layer1_weightsGrad;
		network.layer1.biases		= (void*)arg.layer1_biases;
		network.layer1.biasesGrad	= (void*)arg.layer1_biasesGrad;

		network.layer2.weights		= (void*)arg.layer2_weights;
		network.layer2.weightsGrad	= (void*)arg.layer2_weightsGrad;
		network.layer2.biases		= (void*)arg.layer2_biases;
		network.layer2.biasesGrad	= (void*)arg.layer2_biasesGrad;
	#endif

		if (tid >= arg.inputCount)
			return;

		const half	initialGradient = 1.0h;
		const half2	input = (half2)arg.inputs[tid];

		bwd_diff(loss)( INOUT network, input.x, input.y, initialGradient );

		let thisLoss = (float)loss( INOUT network, input.x, input.y );
		let maxLoss = WaveActiveMax(thisLoss);
		if (WaveIsFirstLane())
		{
			un_ArgBuf[0].loss.max(bit_cast<uint32_t>(maxLoss));
		}
	}

#endif
//-----------------------------------------------------------------------------
#ifdef ADJUST_PARAMETERS

	// from kernels.slang
	SLANG_ENTRY_OUT  Main (SLANG_ENTRY_IN)
	{
		const uint tid = hl_DispatchThreadID.x;

		let arg = un_ArgBuf[0];

		if ( arg.enable == 0 )
			return;

		if ( tid >= arg.gradientsCount )
			return;

		if ( isnan(arg.gradients[tid]) )
		{
			arg.gradients[tid] = 0.0h;
			return;
		}

		AdamState state;
		state.mean		= arg.adamState_mean[tid];
		state.variance	= arg.adamState_variance[tid];
		state.iteration	= arg.adamState_iteration[tid];

		AdamOptimizer::step( INOUT state, INOUT arg.params[tid], INOUT arg.gradients[tid] );

		arg.adamState_mean[tid]	= state.mean;
		arg.adamState_variance[tid] = state.variance;
		arg.adamState_iteration[tid] = state.iteration;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW

	half4  Eval (half x, half y)
	{
		let arg = un_ArgBuf[0];

		MyNetwork network;

	#ifdef SOFTWARE
		network.layer1.weights		= arg.layer1_weights;
		network.layer1.biases		= arg.layer1_biases;

		network.layer2.weights		= arg.layer2_weights;
		network.layer2.biases		= arg.layer2_biases;
	#else
		network.layer1.weights		= (void*)arg.layer1_weights;
		network.layer1.biases		= (void*)arg.layer1_biases;

		network.layer2.weights		= (void*)arg.layer2_weights;
		network.layer2.biases		= (void*)arg.layer2_biases;
	#endif

		return network.eval(x, y);
	}

	SLANG_ENTRY_OUT  Main (SLANG_ENTRY_IN)
	{
		float2	size		= float2(pc.wgCount_dispatchIndex.xy * hl_WorkGroupSize.xy);
		float2	uv			= (float2(hl_DispatchThreadID.xy) + 0.5) / size;
		float2	ratio		= float2( 1.0, size.y / size.x );

		float4	ref_color	= float4(groundtruth( half(uv.x), half(uv.y) ));
		float4	mlp_color	= float4(Eval( half(uv.x), half(uv.y) ));
		float4	color;

		switch ( iCmp )
		{
			case 0 :	color = ref_color;									break;
			case 1 :	color = mlp_color;									break;
			case 2 :	color = (uv.x < iSlider ? ref_color : mlp_color);	break;
			case 3 :	color = abs( ref_color - mlp_color ) * exp2( float(iCmpScale) );break;
		}

		// tonemapping / color clamp
		if ( iCmp == 3 )
		{
			float m = max(max(color.r, color.g), max(color.b, color.a));
			if ( m > 1.0 )
				color = float4(1.0);
		}

		if ( iShowInput == 1 && iCmp == 3 )
		{
			float		md		= 1.0e+20;
			const uint	count	= un_ArgBuf[0].inputCount;

			for (uint i = 0; i < count; ++i)
			{
				float d = distance( uv * ratio, un_ArgBuf[0].inputs[i] * ratio );
				md = min(d, md);
			}

			md *= 2000.0;

			color.rgb	*= smoothstep( 7.0, 10.0, md );			// black border
			color.r		+= 1.0 - smoothstep( 5.0, 7.0, md );	// red dot
		}
		un_Image[hl_DispatchThreadID.xy] = half4(color);
	}

#endif
//-----------------------------------------------------------------------------
#ifdef INIT
	#include "Hash.glsl"

	// based on mlp-training.cpp (allocateNetworkParameterStorage)

	const uint	kLayerSizes[]	= {4, 16, 4};
	const uint	kInputDim		= 4;


	uint  getNetworkLayerBiasCount (int i)
	{
		return kLayerSizes[i + 1];
	}

	uint  getNetworkLayerWeightCount (int i)
	{
		return kLayerSizes[i] * kLayerSizes[i + 1];
	}

	uint  MatrixStorageSize (uint size)
	{
		return ((size + 63) / 64) * 64;
	}


	void  Initialize ()
	{
		const uint	input_count		= kLayerSizes[1];
		const uint	sizeof_float	= 4;
		const uint	sizeof_int		= 4;
		const uint	sizeof_NFloat	= 2;

		ulong		ptr				= un_CBuf.address;
		const ulong	params_ptr		= ptr;

		// layer1
		{
			uint	weights_size	= MatrixStorageSize( getNetworkLayerWeightCount( 0 ) * sizeof_NFloat );		// 128 (half4x16)
			uint	bias_size		= MatrixStorageSize( getNetworkLayerBiasCount( 0 ) * sizeof_NFloat );		// 32 (half16) aligned to 64

			un_ArgBuf.layer1_weights	= half_AEPtr( ptr );	ptr += weights_size;
			un_ArgBuf.layer1_biases		= half_AEPtr( ptr );	ptr += bias_size;
		}

		// layer2
		{
			uint	weights_size	= MatrixStorageSize( getNetworkLayerWeightCount( 1 ) * sizeof_NFloat );		// 128 (half16x4)
			uint	bias_size		= MatrixStorageSize( getNetworkLayerBiasCount( 1 ) * sizeof_NFloat );		// 8 (half4) aligned to 64

			un_ArgBuf.layer2_weights	= half_AEPtr( ptr );	ptr += weights_size;
			un_ArgBuf.layer2_biases		= half_AEPtr( ptr );	ptr += bias_size;
		}
		//-------------------------------------------------


		const uint	grad_offset		= uint(ptr - params_ptr);		// 384
		const ulong	grad_ptr		= ptr;
		const uint	param_count		= grad_offset / sizeof_NFloat;	// 192

		// layer1 gradients
		{
			uint	weights_size	= MatrixStorageSize( getNetworkLayerWeightCount( 0 ) * sizeof_NFloat );		// 128
			uint	bias_size		= MatrixStorageSize( getNetworkLayerBiasCount( 0 ) * sizeof_NFloat );		// 32 aligned to 64

			un_ArgBuf.layer1_weightsGrad	= half_AEPtr( ptr );	ptr += weights_size;
			un_ArgBuf.layer1_biasesGrad		= half_AEPtr( ptr );	ptr += bias_size;
		}

		// layer2 gradients
		{
			uint	weights_size	= MatrixStorageSize( getNetworkLayerWeightCount( 1 ) * sizeof_NFloat );		// 128
			uint	bias_size		= MatrixStorageSize( getNetworkLayerBiasCount( 1 ) * sizeof_NFloat );		// 8 aligned to 64

			un_ArgBuf.layer2_weightsGrad	= half_AEPtr( ptr );	ptr += weights_size;
			un_ArgBuf.layer2_biasesGrad		= half_AEPtr( ptr );	ptr += bias_size;
		}

		const uint	grad_count = uint(ptr - grad_ptr) / sizeof_NFloat;	// 192
		//-------------------------------------------------


		// other
		{
			un_ArgBuf.inputs				= float2_AEPtr( ptr );	ptr += input_count * 2 * sizeof_float;
			un_ArgBuf.inputCount			= input_count;
			un_ArgBuf.loss					= 0; // clear loss buffer
		}

		// adam state, see 'AdamState'
		{
			const uint	adam_count	= param_count;

			un_ArgBuf.adamState_mean		= half_AEPtr( ptr );	ptr += adam_count * sizeof_NFloat;
			un_ArgBuf.adamState_variance	= half_AEPtr( ptr );	ptr += adam_count * sizeof_NFloat;
			un_ArgBuf.adamState_iteration	= int_AEPtr( ptr );		ptr += adam_count * sizeof_int;
		}

		// gradients
		{
			un_ArgBuf.params				= half_AEPtr( params_ptr );
			un_ArgBuf.gradients				= half_AEPtr( grad_ptr );
			un_ArgBuf.gradientsCount		= grad_count;				// 192, must be <= 256
		}
		//-------------------------------------------------


		uint	j = 0;

		// set inputs
		for (uint i = 0; i < un_ArgBuf.inputCount; ++i, j += 2)
		{
			un_ArgBuf.inputs.data[i] = float2( un_CBuf.rndValues[j+0], un_CBuf.rndValues[j+1] );	// [0, 1]
		}

		// set init params
		for (uint i = 0; i < grad_offset / sizeof_NFloat; ++i, ++j)
		{
			un_ArgBuf.params.data[i] = half( un_CBuf.rndValues[j] * 2.0 - 1.0 );	// [-1, +1]
		}

		// j must be < 1024
	}


	void  SetRndInput ()
	{
		switch ( iRndInput )
		{
			case 1 :
			{
				for (uint i = 0; i < un_ArgBuf.inputCount; ++i) {
					un_ArgBuf.inputs.data[i] = DHash22( float2( float(un_ArgBuf.iteration * 16 + i), un_PerPass.time ) * 0.1 );
				}
				break;
			}
			case 2 :
			{
				// voronoise
				uint	i		= 0;
				float	scale	= 1.0 / float(kInputDim);

				for (int y = 0; y < kInputDim; ++y)
				for (int x = 0; x < kInputDim; ++x, ++i)
				{
					float2	pos = float2( x, y ) + 0.5;
					float2	off = DHash22(float2( float(i), un_PerPass.time * 0.1 )) - 0.5;		// [-0.5, +0.5]
					un_ArgBuf.inputs.data[i] = (pos + off) * scale;
				}
				break;
			}
			case 3 :
			{
				uint	i		= 0;
				float	scale	= 1.0 / (float(kInputDim-1) + 0.2);

				for (int y = 0; y < kInputDim; ++y)
				for (int x = 0; x < kInputDim; ++x, ++i)
				{
					float2	pos = float2( x, y ) + 0.1;
					float2	off = DHash22(float2( float(i), un_PerPass.time * 0.1 )) - 0.5;		// [-0.5, +0.5]
					un_ArgBuf.inputs.data[i] = Saturate( (pos + off) * scale );
				}
				break;
			}
			case 4 :
			{
				// stable grid
				uint		i		= 0;
				const uint	j		= (un_ArgBuf.iteration2 += kInputDim-1);
				const uint	mask	= kInputDim * kInputDim - 1;
				uint2		off		= uint2( j % kInputDim, j / kInputDim );

				for (uint y = 0; y < kInputDim; ++y)
				for (uint x = 0; x < kInputDim; ++x, ++i)
				{
					un_ArgBuf.inputs.data[i] = (float2( (uint2(x,y) * kInputDim + off) & mask ) + 0.5) / float(mask+1);
				}
				break;
			}
		}
	}


	void  Update ()
	{
		un_ArgBuf.enable = 0;

		if ( iNoLimits == 1 )
		{
			un_ArgBuf.enable = 1;
		}
		else
		{
			if ( un_ArgBuf.iteration < iMaxIter )
				un_ArgBuf.enable = 1;
		}

		if ( un_ArgBuf.enable == 1 )
		{
			++un_ArgBuf.iteration;
			un_ArgBuf.loss	= 0; // clear loss buffer
		}

		if ( iRndInput > 0 and (un_PerPass.frame % (1<<iInUpdInterval)) == 0 )
		{
			SetRndInput();
		}
	}


	void  Main ()
	{
		if ( un_PerPass.frame > 0 )
		{
			Update();
			return;
		}

		Initialize();
	}

#endif
//-----------------------------------------------------------------------------
