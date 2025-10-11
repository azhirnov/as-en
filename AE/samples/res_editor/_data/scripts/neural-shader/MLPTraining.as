/*
	copied form slang examples
	https://github.com/shader-slang/slang/tree/master/examples/mlp-training (Apache-2.0 license)

	Problems and solutions with porting slang to ResEditor:
	 * can not pass slang class as input argument - this class is not known by engine reflection system so it is break everything.
	 * slang doesn't allow to cast pointers - I can't pass DeviceAddress to slang and convert it to 'MyNetwork' class.
	 * used GLSL 'INIT' pass to fill 'ArgBuffer' with DeviceAddress, then same buffer passed to slang with typed pointers, and then it initialize slang classes.
	 * shader can be much simpler when slang will support [Differentiable] for GLSL.
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
		//	cbuf.Uint(	"size",		large_buf_size );
		}{
			array<float>	rnd_values;
			Random			rnd;

			rnd_values.resize( 1024 );
			for (uint i = 0; i < rnd_values.size(); ++i) {
				rnd_values[i] = rnd.Uniform( 0.0f, 1.0f );
			}

			cbuf.FloatArray( "rndValues",	rnd_values );
		}


		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "INIT" );
			pass.AddFlag( EPassFlags::Enable_ShaderTrace );
			pass.ArgInOut(	"un_ArgBuf",	arg_buf );
			pass.ArgIn(		"un_CBuf",		cbuf );
			pass.LocalSize( 1 );
			pass.DispatchGroups( 1 );
			pass.Slider(	"iMaxIter",		0,	1000,	0 );
			pass.Slider(	"iNoLimits",	0,	1,		0 );
		}{
			RC<ComputePass>		pass = ComputePass( "", "LEARN_GRADIENT" );
			pass.AddFlag( EPassFlags::UseSLang );
			pass.ArgInOut(	"un_ArgBuf",	arg_buf );
			pass.LocalSize( wg_size );
			pass.DispatchGroups( wg_count );
		}{
			RC<ComputePass>		pass = ComputePass( "", "ADJUST_PARAMETERS" );
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
			pass.Slider( "iCmp",		0,		3,		2 );
			pass.Slider( "iSlider",		0.f,	1.f,	0.5f );
			pass.Slider( "iCmpScale",	0,		10,		3 );
		}

		RC<DynamicUInt>	loss = DynamicUInt();
		ReadBuffer( loss, arg_buf, "loss" );
		
		RC<DynamicUInt>	iteration = DynamicUInt();
		ReadBuffer( iteration, arg_buf, "iteration" );

		Label( loss.AsFloat(),	"loss" );
		Label( iteration,		"iteration" );

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#if defined(LEARN_GRADIENT) or defined(ADJUST_PARAMETERS) or defined(VIEW)

	//-----------------------------------------------------------------------------
	// common.slang

	public typealias NFloat = half;


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


	//-----------------------------------------------------------------------------
	// mlp_sw.slang
	
	public struct FeedForwardLayer<int InputSize, int OutputSize>
	{
		public NFloat* weights;
		public NFloat* weightsGrad;
		public NFloat* biases;
		public NFloat* biasesGrad;

		[BackwardDerivative(evalBwd)]
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
					resultGrad.data[i] *= 0.01h;
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
		public static const NFloat epsilon = 1e-7h;
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
		network.layer1.weights		= arg.layer1_weights;
		network.layer1.weightsGrad	= arg.layer1_weightsGrad;
		network.layer1.biases		= arg.layer1_biases;
		network.layer1.biasesGrad	= arg.layer1_biasesGrad;
		
		network.layer2.weights		= arg.layer2_weights;
		network.layer2.weightsGrad	= arg.layer2_weightsGrad;
		network.layer2.biases		= arg.layer2_biases;
		network.layer2.biasesGrad	= arg.layer2_biasesGrad;

		if (tid >= arg.inputCount)
			return;

		var input = (half2)arg.inputs[tid];
		bwd_diff(loss)(network, input.x, input.y, 1.0h);
		let thisLoss = (float)loss(network, input.x, input.y);
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

		AdamOptimizer::step(state, arg.params[tid], arg.gradients[tid]);
		
		arg.adamState_mean[tid]	= state.mean;
		arg.adamState_variance[tid] = state.variance;
		arg.adamState_iteration[tid] = state.iteration;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef VIEW

	half4  Inference (half x, half y)
	{
		let arg = un_ArgBuf[0];

		MyNetwork network;
		network.layer1.weights	= arg.layer1_weights;
		network.layer1.biases	= arg.layer1_biases;
		
		network.layer2.weights	= arg.layer2_weights;
		network.layer2.biases	= arg.layer2_biases;

		return network.eval(x, y);
	}
	
	SLANG_ENTRY_OUT  Main (SLANG_ENTRY_IN)
	{
		float2	size		= float2(pc.wgCount_dispatchIndex.xy * hl_WorkGroupSize.xy);
		float2	uv			= (float2(hl_DispatchThreadID.xy) + 0.5) / size;

		float4	ref_color	= float4(groundtruth( half(uv.x), half(uv.y) ));
		float4	mlp_color	= float4(Inference( half(uv.x), half(uv.y) ));
		float4	color;

		switch ( iCmp )
		{
			case 0 :	color = ref_color;								break;
			case 1 :	color = mlp_color;								break;
			case 2 :	color = uv.x < iSlider ? ref_color : mlp_color;	break;
			case 3 :	color = abs( ref_color - mlp_color ) * exp10( float(iCmpScale) );  break;
		}

		un_Image[hl_DispatchThreadID.xy] = color;
	}

#endif
//-----------------------------------------------------------------------------
#ifdef INIT

	// based on mlp-training.cpp (allocateNetworkParameterStorage)

	const uint NFloatSize		= 2;	// half
	const uint kLayerSizes[]	= {4, 16, 4};

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


	void  Main ()
	{
		if ( un_PerPass.frame > 0 )
		{
			un_ArgBuf.enable = 0;

			if ( iNoLimits == 1 )
			{
				if ( (un_PerPass.frame % 20) == 0 )
					un_ArgBuf.enable = 1;
			}
			else
			if ( un_ArgBuf.iteration < iMaxIter )
				un_ArgBuf.enable = 1;

			if ( un_ArgBuf.enable == 1 )
			{
				++un_ArgBuf.iteration;
				un_ArgBuf.loss	= 0; // clear loss buffer
			}
			return;
		}

		const uint	input_count		= 16;
		const uint	sizeof_float	= 4;
		const uint	sizeof_int		= 4;
		const uint	sizeof_NFloat	= 2;

		ulong		ptr				= un_CBuf.address;
		const ulong	params_ptr		= ptr;

		// layer1
		{
			uint	weights_size	= MatrixStorageSize( getNetworkLayerWeightCount( 0 ) * NFloatSize );	// 128
			uint	bias_size		= MatrixStorageSize( getNetworkLayerBiasCount( 0 ) * NFloatSize );		// 32 aligned to 64

			un_ArgBuf.layer1_weights	= half_AEPtr( ptr );	ptr += weights_size;
			un_ArgBuf.layer1_biases		= half_AEPtr( ptr );	ptr += bias_size;
		}

		// layer2
		{
			uint	weights_size	= MatrixStorageSize( getNetworkLayerWeightCount( 1 ) * NFloatSize );	// 128
			uint	bias_size		= MatrixStorageSize( getNetworkLayerBiasCount( 1 ) * NFloatSize );		// 8 aligned to 64

			un_ArgBuf.layer2_weights	= half_AEPtr( ptr );	ptr += weights_size;
			un_ArgBuf.layer2_biases		= half_AEPtr( ptr );	ptr += bias_size;
		}
		//-------------------------------------------------


		const uint	grad_offset		= uint(ptr - params_ptr);	// 384
		const ulong	gradients_ptr	= ptr;
		
		// layer1 gradients
		{
			uint	weights_size	= MatrixStorageSize( getNetworkLayerWeightCount( 0 ) * NFloatSize );	// 128
			uint	bias_size		= MatrixStorageSize( getNetworkLayerBiasCount( 0 ) * NFloatSize );		// 32 aligned to 64

			un_ArgBuf.layer1_weightsGrad	= half_AEPtr( ptr );	ptr += weights_size;
			un_ArgBuf.layer1_biasesGrad		= half_AEPtr( ptr );	ptr += bias_size;
		}

		// layer2 gradients
		{
			uint	weights_size	= MatrixStorageSize( getNetworkLayerWeightCount( 1 ) * NFloatSize );	// 128
			uint	bias_size		= MatrixStorageSize( getNetworkLayerBiasCount( 1 ) * NFloatSize );		// 8 aligned to 64

			un_ArgBuf.layer2_weightsGrad	= half_AEPtr( ptr );	ptr += weights_size;
			un_ArgBuf.layer2_biasesGrad		= half_AEPtr( ptr );	ptr += bias_size;
		}

		const uint	param_buf_size = uint(ptr - params_ptr);	// 768
		//-------------------------------------------------


		// other
		{
			un_ArgBuf.inputs				= float2_AEPtr( ptr );	ptr += input_count * 2 * sizeof_float;
			un_ArgBuf.inputCount			= input_count;
			un_ArgBuf.loss					= 0; // clear loss buffer
		}

		// adam state, see 'AdamState'
		{
			const uint	init_params_count	= param_buf_size / sizeof_NFloat;	// 384

			un_ArgBuf.adamState_mean		= half_AEPtr( ptr );	ptr += init_params_count * sizeof_NFloat;
			un_ArgBuf.adamState_variance	= half_AEPtr( ptr );	ptr += init_params_count * sizeof_NFloat;
			un_ArgBuf.adamState_iteration	= int_AEPtr( ptr );		ptr += init_params_count * sizeof_int;
		}

		// gradients
		{
			un_ArgBuf.params				= half_AEPtr( params_ptr );
			un_ArgBuf.gradients				= half_AEPtr( gradients_ptr );
			un_ArgBuf.gradientsCount		= (param_buf_size - grad_offset) / sizeof_NFloat;	// 192, must be <= 256
		}
		//-------------------------------------------------


		uint	j = 0;

		// set inputs
		for (uint i = 0; i < un_ArgBuf.inputCount; ++i, j += 2)
		{
			un_ArgBuf.inputs.data[i] = float2( un_CBuf.rndValues[j+0], un_CBuf.rndValues[j+1] );
		}

		// set init params
		for (uint i = 0; i < grad_offset / sizeof_NFloat; ++i, ++j)
		{
			un_ArgBuf.params.data[i] = half( un_CBuf.rndValues[j] * 2.0 - 1.0 );
		}

		// j must be < 1024
	}

#endif
//-----------------------------------------------------------------------------
