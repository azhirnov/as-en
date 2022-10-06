// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/Common.h"
#include "graphics/Public/MultiSamples.h"
#include "graphics/Public/RenderStateEnums.h"

namespace AE::Graphics
{
	struct MDepthStencilState;
	struct MDynamicRenderState;


	//
	// Render State
	//
	struct RenderState final
	{
	// types
	public:

		//
		// Color Buffer
		//
		struct ColorBuffer
		{
		// types
			template <typename T>
			struct ColorPair
			{
				T	color;
				T	alpha;

				ColorPair () : color{T::Unknown}, alpha{T::Unknown} {}
				ColorPair (T rgba) : color{rgba}, alpha{rgba} {}
				ColorPair (T rgb, T a) : color{rgb}, alpha{a} {}

				ND_ bool  operator == (const ColorPair<T> &rhs) const {
					return color == rhs.color and alpha == rhs.alpha;
				}
			};

			struct ColorMask
			{
				bool	r : 1;
				bool	g : 1;
				bool	b : 1;
				bool	a : 1;

				ColorMask () : ColorMask{true} {}
				explicit ColorMask (bool val) : r{val}, g{val}, b{val}, a{val} {}

				ND_ bool  All ()	const	{ return r & g & b & a; }
				ND_ bool  Any ()	const	{ return r | g | b | a; }
				ND_ bool  None ()	const	{ return not Any(); }
			};


		// variables
			ColorPair< EBlendFactor >	srcBlendFactor	{ EBlendFactor::One,  EBlendFactor::One };
			ColorPair< EBlendFactor >	dstBlendFactor	{ EBlendFactor::Zero, EBlendFactor::Zero };
			ColorPair< EBlendOp >		blendOp			{ EBlendOp::Add, EBlendOp::Add };
			bool						blend			= false;
			ColorMask					colorMask;
		
		// methods
			ColorBuffer () {}

			ND_ bool	operator == (const ColorBuffer &rhs) const;
			ND_ HashVal	CalcHash () const;
		};
		STATIC_ASSERT( sizeof(ColorBuffer) == 8 );


		//
		// Color Buffers State
		//
		struct ColorBuffersState
		{
		// types
			using ColorBuffers_t	= StaticArray< ColorBuffer, GraphicsConfig::MaxColorAttachments >;

		// variables
			ColorBuffers_t		buffers;
			ELogicOp			logicOp		= ELogicOp::None;
			RGBA32f				blendColor	{ 1.0f };

		// methods
			ColorBuffersState () {}

			ND_ bool	operator == (const ColorBuffersState &rhs) const;
			ND_ HashVal	CalcHash () const;
		};
		STATIC_ASSERT( sizeof(ColorBuffersState) == 84 );
		

		//
		// Stencil Face State
		//
		struct StencilFaceState
		{
		// variables
			EStencilOp		failOp			= EStencilOp::Keep;		// stencil test failed
			EStencilOp		depthFailOp		= EStencilOp::Keep;		// depth and stencil tests are passed
			EStencilOp		passOp			= EStencilOp::Keep;		// stencil test passed and depth test failed
			ECompareOp		compareOp		= ECompareOp::Always;
			ubyte			reference		= 0;
			ubyte			writeMask		= UMax;
			ubyte			compareMask		= UMax;

		// methods
			StencilFaceState () {}

			ND_ bool	operator == (const StencilFaceState &rhs) const;
			ND_ HashVal	CalcHash () const;
		};
		STATIC_ASSERT( sizeof(StencilFaceState) == 7 );


		//
		// Stencil Buffer State
		//
		struct StencilBufferState
		{
		// variables
			StencilFaceState	front;
			StencilFaceState	back;
			bool				enabled	= false;	// stencil write/test

		// methods
			StencilBufferState () {}

			ND_ bool	operator == (const StencilBufferState &rhs) const;
			ND_ HashVal	CalcHash () const;
		};
		STATIC_ASSERT( sizeof(StencilBufferState) == 15 );


		//
		// Depth Buffer State
		//
		struct DepthBufferState
		{
		// variables
			float				minBounds		= 0.f;
			float				maxBounds		= 1.f;
			bool				boundsEnabled	= false;				// enable 'minBounds' and 'maxBounds'
			ECompareOp			compareOp		= ECompareOp::LEqual;	// if 'test' enabled
			bool				write			= false;				// depth write enabled
			bool				test			= true;					// depth test enabled

		// methods
			DepthBufferState () {}

			ND_ bool	operator == (const DepthBufferState &rhs) const;
			ND_ HashVal	CalcHash () const;
		};
		STATIC_ASSERT( sizeof(DepthBufferState) == 12 );


		//
		// Input Assembly State
		//
		struct InputAssemblyState
		{
		// variables
			EPrimitive		topology			= Default;
			bool			primitiveRestart	= false;	// if 'true' then index with -1 value will restarting the assembly of primitives

		// methods
			InputAssemblyState () {}

			ND_ bool	operator == (const InputAssemblyState &rhs) const;
			ND_ HashVal	CalcHash () const;
		};
		STATIC_ASSERT( sizeof(InputAssemblyState) == 2 );


		//
		// Rasterization State
		//
		struct RasterizationState
		{
		// variables
			float			depthBiasConstFactor	= 0.f;
			float			depthBiasClamp			= 0.f;
			float			depthBiasSlopeFactor	= 0.f;
			bool			depthBias				= 0.f;

			EPolygonMode	polygonMode				= EPolygonMode::Fill;

			bool			depthClamp				= false;
			bool			rasterizerDiscard		= false;

			bool			frontFaceCCW			= true;
			ECullMode		cullMode				= ECullMode::None;

		// methods
			RasterizationState () {}

			ND_ bool	operator == (const RasterizationState &rhs) const;
			ND_ HashVal	CalcHash () const;
		};
		STATIC_ASSERT( sizeof(RasterizationState) == 20 );


		//
		// Multisample State
		//
		struct MultisampleState
		{
		// types
			//using SampleMask	= StaticArray< uint, GraphicsConfig::MaxSamples / CT_SizeOfInBits<uint> >;
			STATIC_ASSERT( GraphicsConfig::MaxSamples / CT_SizeOfInBits<uint> == 1 );

		// variables
			uint				sampleMask			= UMax;
			float				minSampleShading	= 0.f;
			MultiSamples		samples;
			bool				sampleShading		= false;

			bool				alphaToCoverage		= false;
			bool				alphaToOne			= false;

		// methods
			MultisampleState () {}

			ND_ bool	operator == (const MultisampleState &rhs) const;
			ND_ HashVal	CalcHash () const;
		};
		STATIC_ASSERT( sizeof(MultisampleState) == 12 );


	// variables
	public:
		ColorBuffersState		color;
		DepthBufferState		depth;
		StencilBufferState		stencil;
		InputAssemblyState		inputAssembly;
		RasterizationState		rasterization;
		MultisampleState		multisample;


	// methods
	public:
		RenderState () {}

		ND_ bool	operator == (const RenderState &rhs) const;
		ND_ HashVal	CalcHash () const;

			void  Set (const MDepthStencilState &ds, const MDynamicRenderState &rs);
			void  SetDefault (EPipelineDynamicState);
	};
	STATIC_ASSERT( sizeof(RenderState) == 148 );
	
	
	
	
	//
	// Metal Depth Stencil State
	//
	struct MDepthStencilState
	{
	// variables
	public:
		RenderState::DepthBufferState		depth;
		RenderState::StencilBufferState		stencil;


	// methods
	public:
		MDepthStencilState () {}

		ND_ bool	operator == (const MDepthStencilState &rhs) const;
		ND_ HashVal	CalcHash () const;

			void  Set (const RenderState &rs);
	};



	//
	// Metal Dynamic Render State
	//
	struct MDynamicRenderState
	{
	// variables
	public:
		EPrimitive		topology				= Default;
		EPolygonMode	polygonMode				= EPolygonMode::Fill;		// setTriangleFillMode
		bool			frontFaceCCW			= true;						// setFrontFacingWinding
		ECullMode		cullMode				= ECullMode::None;			// setCullMode
		
		bool			depthClamp				= false;					// setDepthClipMode

		float			depthBiasConstFactor	= 0.f;						// setDepthBias:depthBias
		float			depthBiasClamp			= 0.f;						// setDepthBias:clamp
		float			depthBiasSlopeFactor	= 0.f;						// setDepthBias:slopeScale


	// methods
	public:
		MDynamicRenderState () {}

		ND_ bool	operator == (const MDynamicRenderState &rhs) const;
		ND_ HashVal	CalcHash () const;

			void  Set (const RenderState &rs);
	};


} // AE::Graphics


namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::RenderState::ColorBuffer >				{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::RenderState::ColorBuffer >			{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::RenderState::ColorBuffersState >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::RenderState::ColorBuffersState >	{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::RenderState::StencilFaceState >			{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::RenderState::StencilFaceState >		{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::RenderState::StencilBufferState >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::RenderState::StencilBufferState >	{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::RenderState::InputAssemblyState >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::RenderState::InputAssemblyState >	{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::RenderState::RasterizationState >		{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::RenderState::RasterizationState >	{ static constexpr bool  value = true; };
	
	template <> struct TMemCopyAvailable< AE::Graphics::RenderState::MultisampleState >			{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::RenderState::MultisampleState >		{ static constexpr bool  value = true; };

	template <> struct TMemCopyAvailable< AE::Graphics::RenderState >							{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::RenderState >						{ static constexpr bool  value = true; };

	
	template <> struct TMemCopyAvailable< AE::Graphics::MDepthStencilState >					{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::MDepthStencilState >				{ static constexpr bool  value = true; };

	template <> struct TMemCopyAvailable< AE::Graphics::MDynamicRenderState >					{ static constexpr bool  value = true; };
	template <> struct TTrivialySerializable< AE::Graphics::MDynamicRenderState >				{ static constexpr bool  value = true; };

} // AE::Base


namespace std
{
	template <> struct hash< AE::Graphics::RenderState >			: AE::Base::DefaultHasher_CalcHash< AE::Graphics::RenderState >			{};
	template <> struct hash< AE::Graphics::MDepthStencilState >		: AE::Base::DefaultHasher_CalcHash< AE::Graphics::MDepthStencilState >	{};
	template <> struct hash< AE::Graphics::MDynamicRenderState >	: AE::Base::DefaultHasher_CalcHash< AE::Graphics::MDynamicRenderState >	{};

} // std