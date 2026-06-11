// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "lang_model/Public/Common.h"
#include "graphics_rhi/Public/ResourceEnums.h"

namespace AE::LangModel
{
	using Graphics::EPixelFormat;


	enum class EModelFormat : ubyte
	{
		GGUF,				// for LLama
		SafeTensors,		// for OpenVINO
		_Count,
		Unknown		= _Count
	};
	using EModelFormatSet = EnumSet< EModelFormat >;


	enum class EFlashAttention : ubyte
	{
		Auto,
		Disabled,
		Enabled,

		Unknown		= Auto,
	};


	enum class EImplementation : ubyte
	{
		LLama,
		StableDiffusion,
		_Count,
	};


	enum class ERole : ubyte
	{
		User,		// from user (prompt)
		Assistant,	// from LLM
		System,		// setup LLM
		_Count,

		Prompt		= User,
	};


	enum class EBackend : ubyte
	{
		CPU,		// will use CPU only backend which is slower, other backend will combine CPU with GPU or NPU
		Auto,
		Vulkan,		// may return out-of-memory error for small VRAM
		CUDA,		// can use RAM to increase memory size, but performance limited to PCI bandwidth
		Metal,
	//	SYCL,
	//	HIP,
		_Count
	};


	//
	// Language Model Open Params
	//
	struct OpenParams
	{
		RC<ILoadingListener>	listener;
		RC<ILogListener>		logger;
		bool					enableLogger	= true;
		EBackend				backend			= EBackend::Auto;
	};



	//
	// Language Model Context Params
	//
	struct ContextParams
	{
	protected:
		const EImplementation	_type;

		explicit ContextParams (EImplementation type)	__NE___ : _type{type} {}

	public:
		ND_ EImplementation  Type ()					C_NE___	{ return _type; }
	};



	//
	// Language Model Info
	//
	struct ModelInfo
	{
		uint				maxContextSize			= 0;		// context size the model was trained on
		uint				layerCount				= 0;
		float				RoPE_freqScaleTrain		= 0.f;
		ulong				paramsCount				= 0;
		Bytes				modelSize;
		bool				recurrentModel			= false;
		bool				diffusionModel			= false;
		EModelFormat		format					= Default;
	};



	//
	// Language Model Loading Listener
	//
	class ILoadingListener : public EnableRC<ILoadingListener>
	{
	// interface
	public:
		// return 'false' to stop loading
		virtual bool  Progress (Percent)	__NE___ = 0;
	};



	//
	// Language Model Log Listener
	//
	class ILogListener : public EnableRC<ILogListener>
	{
	// interface
	public:
		virtual void  Log (ELogLevel, StringView)	__NE___ = 0;
	};



	//
	// Language Model Response Listener
	//
	class IResponseListener : public EnableRC<IResponseListener>
	{
	// types
	public:
		enum class ErrorCode
		{
			None,
			FailedToApplyChatTemplate,
			FailedToTokenizePrompt,
			ContextSizeExceeded,
			UnknownError,
			InterruptedByUser,
			OutOfMemory,
		};

	// interface
	public:

		// Return 'false' to stop generation.
		// User may ignore input, complete response will be passed to 'OnComplete()'.
		//
		virtual bool  AppendResponse (U8StringView piece, uint tokens)	__NE___ = 0;


		// Called once when response is complete.
		// Pass full response in utf8 format and total token count.
		//
		virtual void  OnComplete (U8StringView response, uint tokens)	__NE___ = 0;

		// Called once before tokenizing prompt.
		//
		virtual void  RequiredPromptTokens (uint tokens)				__NE___ = 0;

		virtual void  OnError (ErrorCode)								__NE___ = 0;
	};



	//
	// Diffusion Model Input Image
	//
	struct ImageRef
	{
		uint2				dim;
		EPixelFormat		format		= Default;	// expected 'RGB8_UNorm'
		void*				pixels		= null;
		Bytes				rowPitch;				// most implementations expects to get packed data, so alignment may cause additional copy
		Bytes				size;

		ND_ bool	IsDefined ()			C_NE___	{ return dim.x > 0 and dim.y > 0 and format != Default and pixels != null and size > 0; }
	};

	struct ImageStorage : ImageRef
	{
	private:
		void *		_ptr	= null;		// may not match with 'pixels'
		Bytes		_size;

	public:
		ImageStorage ()						__NE___ {}
		ImageStorage (ImageStorage &&other)	__NE___;
		~ImageStorage ()					__NE___;

		ND_ bool  Allocate ()				__NE___;
			void  Deallocate ()				__NE___;
		ND_ bool  IsValid ()				C_NE___;
	};



	//
	// Diffusion Model Image Generation Params
	//
	struct ImageGenParams
	{
	public:
		U8String				prompt;							// text to image mode
		U8String				negativePrompt;

		uint2					dim;
		uint					seed				= 0;
		uint					resultCount			= 1;

		// image to image mode
		// init image is being noised and transformed
		ImageRef				initImage;
		float					initImageStrength	= 1.f;		// 0 - tries to preserve the input almost entirely,
																// 1 - behaves close to pure text-to-image
		// define which part of 'initImage' will be modified
		ImageRef				maskImage;

		// use cases:
		// * pose transfer  - prompt contains character, control image contains pose
		// * edge-guided generation  - control image define edges of objects
		// * depth-guided generation  - bright texels - near, dark - far, used to define object placement, perspective, camera feel.
		// loaded model must support control image, otherwise it is ignored.
		ImageRef				controlImage;
		float					controlStrength		= 1.f;		// 0 - losely follows control image
																// 1 - closely follows control image

		// images that generation can use as references/guidance, instead of using only text prompt or single init image.
		// purposes:
		//  * style reference
		//  * combine several reference images
		//  * keep a character/object closer to reference images across generations
		Array<ImageRef>			refImages;

		// automatically resize reference images to match expected dimensions
		bool					autoResizeRefImage	= true;

		// 'false' - use the same reference images for every generated image
		// 'true'  - move to the next reference image as generation proceeds, so 'refImages' should equal to 'resultCount'
		bool					increaseRefIndex	= false;

	protected:
		const EImplementation	_type;
		explicit ImageGenParams (EImplementation type)	__NE___ : _type{type} {}
	public:
		ND_ EImplementation  Type ()					C_NE___	{ return _type; }

	};


	//
	// Diffusion Model Video Generation Params
	//
	struct VideoGenParams
	{
	public:

	protected:
		const EImplementation	_type;
		explicit VideoGenParams (EImplementation type)	__NE___ : _type{type} {}
	public:
		ND_ EImplementation  Type ()					C_NE___	{ return _type; }
	};

} // AE::LangModel


#ifdef AE_ENABLE_LOGS
namespace AE::Base
{

/*
=================================================
	ToString (EBackend)
=================================================
*/
	Nd__In StringView  ToString (LangModel::EBackend type) __NE___
	{
		switch_enum( type )
		{
			using enum LangModel::EBackend;
			case CPU :		return "CPU";
			case Auto :		return "Auto";
			case Vulkan :	return "Vulkan";
			case CUDA :		return "CUDA";
			case Metal :	return "Metal";
			case _Count :	break;
		}
		switch_end
		return Default;
	}

} // AE::Base
#endif // AE_ENABLE_LOGS
