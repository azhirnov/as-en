
#ifdef AE_LICENSE_MIT

#if 1
	// from https://github.com/Jam3/glsl-fast-gaussian-blur
	// MIT license

	float4  Blur5 (gl::CombinedTex2D<float> image, float2 uv, float2 invImageDim, float2 direction)
	{
		const float2  off1  = float2(1.3333333333333333) * direction * invImageDim;

		float4  color = float4(0.0);
		color += gl.texture.Sample( image, uv ) * 0.29411764705882354;
		color += gl.texture.Sample( image, uv + off1 ) * 0.35294117647058826;
		color += gl.texture.Sample( image, uv - off1 ) * 0.35294117647058826;
		return color;
	}

	float4  Blur9 (gl::CombinedTex2D<float> image, float2 uv, float2 invImageDim, float2 direction)
	{
		const float2  off1  = float2(1.3846153846) * direction * invImageDim;
		const float2  off2  = float2(3.2307692308) * direction * invImageDim;

		float4  color = float4(0.0);
		color += gl.texture.Sample( image, uv ) * 0.2270270270;
		color += gl.texture.Sample( image, uv + off1 ) * 0.3162162162;
		color += gl.texture.Sample( image, uv - off1 ) * 0.3162162162;
		color += gl.texture.Sample( image, uv + off2 ) * 0.0702702703;
		color += gl.texture.Sample( image, uv - off2 ) * 0.0702702703;
		return color;
	}

	float4  Blur13 (gl::CombinedTex2D<float> image, float2 uv, float2 invImageDim, float2 direction)
	{
		const float2  off1  = float2(1.411764705882353) * direction * invImageDim;
		const float2  off2  = float2(3.2941176470588234) * direction * invImageDim;
		const float2  off3  = float2(5.176470588235294) * direction * invImageDim;

		float4  color = float4(0.0);
		color += gl.texture.Sample( image, uv ) * 0.1964825501511404;
		color += gl.texture.Sample( image, uv + off1 ) * 0.2969069646728344;
		color += gl.texture.Sample( image, uv - off1 ) * 0.2969069646728344;
		color += gl.texture.Sample( image, uv + off2 ) * 0.09447039785044732;
		color += gl.texture.Sample( image, uv - off2 ) * 0.09447039785044732;
		color += gl.texture.Sample( image, uv + off3 ) * 0.010381362401148057;
		color += gl.texture.Sample( image, uv - off3 ) * 0.010381362401148057;
		return color;
	}
#endif

#if 0
	// https://lisyarus.github.io/blog/posts/blur-coefficients-generator.html

	// radius: 3, sigma: 3
	float4  Blur5 (gl::CombinedTex2D<float> image, float2 uv, float2 invImageDim, float2 direction)
	{
		const int SAMPLE_COUNT = 4;
		const float OFFSETS[4] = float[4](
			-2.431625915613778,
			-0.4862426846689484,
			1.4588111840004858,
			3
		);
		const float WEIGHTS[4] = float[4](
			0.24696196374528634,
			0.34050702333458593,
			0.30593582919679174,
			0.10659518372333592
		);
		float4  color = float4(0.0);
		for (int i = 0; i < SAMPLE_COUNT; ++i)
		{
			float2 off = OFFSETS[i] * invImageDim * direction;
			color += gl.texture.Sample( image, uv + off ) * WEIGHTS[i];
		}
		return color;
	}

	// radius: 5, sigma: 3
	float4  Blur9 (gl::CombinedTex2D<float> image, float2 uv, float2 invImageDim, float2 direction)
	{
		const int SAMPLE_COUNT = 6;
		const float OFFSETS[6] = float[6](
			-4.268941421369995,
			-2.364576440741639,
			-0.4722507649454868,
			1.4174297935376854,
			3.3147990233346842,
			5
		);
		const float WEIGHTS[6] = float[6](
			0.043867558300718715,
			0.1914659874907833,
			0.3595399106052396,
			0.2914549970600666,
			0.1018737430617653,
			0.011797803481426415
		);
		float4  color = float4(0.0);
		for (int i = 0; i < SAMPLE_COUNT; ++i)
		{
			float2 off = OFFSETS[i] * invImageDim * direction;
			color += gl.texture.Sample( image, uv + off ) * WEIGHTS[i];
		}
		return color;
	}

	// radius: 8, sigma: 3
	float4  Blur13 (gl::CombinedTex2D<float> image, float2 uv, float2 invImageDim, float2 direction)
	{
		const int SAMPLE_COUNT = 9;
		const float OFFSETS[9] = float[9](
			-7.158869104880915,
			-5.227545617192816,
			-3.3147990233346842,
			-1.4174297935376854,
			0.47225076494548685,
			2.364576440741639,
			4.268941421369995,
			6.190858044244866,
			8
		);
		const float WEIGHTS[9] = float[9](
			0.000965986154627156,
			0.015138410013340544,
			0.10097508115497955,
			0.2888839763482681,
			0.3563682904024343,
			0.18977700282965465,
			0.04348058809226511,
			0.004257199648717665,
			0.00015346535571297352
		);
		float4  color = float4(0.0);
		for (int i = 0; i < SAMPLE_COUNT; ++i)
		{
			float2 off = OFFSETS[i] * invImageDim * direction;
			color += gl.texture.Sample( image, uv + off ) * WEIGHTS[i];
		}
		return color;
	}

#endif

#endif // AE_LICENSE_MIT
