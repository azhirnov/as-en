// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "Scripting/ScriptCommon.h"
#include "Scripting/ScriptBuffer.h"
#include "Scripting/ScriptBufferView.h"
#include "Scripting/ScriptImage.h"
#include "Scripting/ScriptVideoImage.h"
#include "Scripting/ScriptRTScene.h"
#include "Passes/IPass.h"

namespace AE::ResEditor
{

	//
	// Pass Arguments
	//

	class ScriptPassArgs final : Noncopyable
	{
	// types
	public:
		using ResourceUnion_t = Union< NullUnion, ScriptBufferPtr, ScriptBufferViewPtr, ScriptImagePtr,
									   ScriptVideoImagePtr, ScriptRTScenePtr, Array<ScriptImagePtr> >;

		struct Argument
		{
			String				name;
			ResourceUnion_t		res;
			EResourceState		state			= Default;
			String				samplerName;
		};
		using Arguments_t	= Array< Argument >;


	// variables
	private:
		Arguments_t					_args;
		FlatHashSet< String >		_uniqueNames;
		Function<void(Argument &)>	_onAddArg;


	// methods
	public:
		explicit ScriptPassArgs (Function<void(Argument &)> fn)												: _onAddArg{RVRef(fn)} {}

		ND_ bool				Empty ()																	C_NE___	{ return _args.empty(); }
		ND_ Arguments_t const&	Args ()																		C_NE___	{ return _args; }
		ND_ Arguments_t&		Args ()																		__NE___	{ return _args; }

		void  ArgSceneIn (const String &name, const ScriptRTScenePtr &scene)								__Th___;

		void  ArgBufferIn (const String &name, const ScriptBufferPtr &buf)									__Th___;
		void  ArgBufferOut (const String &name, const ScriptBufferPtr &buf)									__Th___;
		void  ArgBufferInOut (const String &name, const ScriptBufferPtr &buf)								__Th___;

		void  ArgBufferViewIn (const String &name, const ScriptBufferViewPtr &buf)							__Th___;
		void  ArgBufferViewOut (const String &name, const ScriptBufferViewPtr &buf)							__Th___;
		void  ArgBufferViewInOut (const String &name, const ScriptBufferViewPtr &buf)						__Th___;

		void  ArgImageIn (const String &name, const ScriptImagePtr &img)									__Th___;
		void  ArgImageOut (const String &name, const ScriptImagePtr &img)									__Th___;
		void  ArgImageInOut (const String &name, const ScriptImagePtr &img)									__Th___;

		void  ArgTextureIn (const String &name, const ScriptImagePtr &tex)									__Th___;
		void  ArgTextureIn2 (const String &name, const ScriptImagePtr &tex, const String &samplerName)		__Th___;
		void  ArgTextureArrIn (const String &name, Array<ScriptImagePtr> arr)								__Th___;
		void  ArgTextureArrIn2 (const String &name, Array<ScriptImagePtr> arr, const String &samplerName)	__Th___;

		void  ArgVideoIn (const String &name, const ScriptVideoImagePtr &tex, const String &samplerName)	__Th___;

		void  ArgImageArrIn (const String &name, Array<ScriptImagePtr> arr)									__Th___;
		void  ArgImageArrOut (const String &name, Array<ScriptImagePtr> arr)								__Th___;
		void  ArgImageArrInOut (const String &name, Array<ScriptImagePtr> arr)								__Th___;

		template <typename DSL, typename AS>
		void  ArgsToDescSet (EShaderStages stages, DSL &dsLayout, AS arraySize)								C_Th___;
		void  InitResources (OUT ResourceArray &resources, PipelinePackID packId)							C_Th___;
		void  ValidateArgs ()																				C_Th___;
		void  AddLayoutReflection ()																		C_Th___;

		void  CopyFrom (const ScriptPassArgs &)																__Th___;
		void  MoveTo (OUT ScriptPassArgs &dst)																__NE___;

	private:
		void  _AddArg (const String &name, const ScriptBufferPtr &buf, EResourceUsage usage)				__Th___;
		void  _AddArg (const String &name, const ScriptBufferViewPtr &buf, EResourceUsage usage)			__Th___;
		void  _AddArg (const String &name, const ScriptImagePtr &img, EResourceUsage usage)					__Th___;
		void  _AddArg (const String &name, Array<ScriptImagePtr> arr, EResourceUsage usage)					__Th___;

		void  _AddTexture (const String &name, const ScriptImagePtr &tex, const String &)					__Th___;
		void  _AddTexture (const String &name, Array<ScriptImagePtr> arr, const String &)					__Th___;
	};



/*
=================================================
	ArgsToDescSet
=================================================
*/
	template <typename DSL, typename AS>
	void  ScriptPassArgs::ArgsToDescSet (const EShaderStages stages, DSL &dsLayout, const AS arraySize) C_Th___
	{
		ASSERT( arraySize.value == 1 );

		for (auto& arg : _args)
		{
			Visit( arg.res,
				[&] (ScriptBufferPtr buf) {
					CHECK_THROW( buf->HasLayout() );
					dsLayout->AddStorageBuffer( stages, arg.name, arraySize, buf->GetTypeName(), Default, arg.state, False{} );
				},
				[&] (ScriptBufferViewPtr buf) {
					if ( AllBits( arg.state, EResourceState::ShaderSample ))
						dsLayout->AddUniformTexelBuffer( stages, arg.name, arraySize, buf->TexelBufferType(), arg.state );
					else
						dsLayout->AddStorageTexelBuffer( stages, arg.name, arraySize, buf->TexelBufferType(), buf->Format(), Default, arg.state );
				},
				[&] (ScriptImagePtr tex) {
					const auto	type = tex->ImageType();
					if ( not arg.samplerName.empty() )
						dsLayout->AddCombinedImage_ImmutableSampler( stages, arg.name, type, arg.state, arg.samplerName );
					else
					if ( AllBits( arg.state, EResourceState::ShaderSample ))
						dsLayout->AddSampledImage( stages, arg.name, arraySize, type, arg.state );
					else
						dsLayout->AddStorageImage( stages, arg.name, arraySize, type, tex->PixelFormat(), Default, arg.state );
				},
				[&] (ScriptVideoImagePtr video) {
					String	sampler = (video->HasYcbcrSampler() ? video->GetSamplerName() : arg.samplerName);
					dsLayout->AddCombinedImage_ImmutableSampler( stages, arg.name, video->ImageType(), arg.state, sampler );
				},
				[&] (ScriptRTScenePtr) {
					dsLayout->AddRayTracingScene( stages, arg.name, arraySize );
				},
				[&] (const Array<ScriptImagePtr> &arr)
				{
					const auto	type = arr[0]->ImageType();
					if ( not arg.samplerName.empty() )
					{
						Array<String>	samplers;  samplers.resize( arr.size(), arg.samplerName );
						dsLayout->AddCombinedImage_ImmutableSampler( stages, arg.name, type, arg.state, arraySize, samplers );
					}else
					if ( AllBits( arg.state, EResourceState::ShaderSample )) {
						dsLayout->AddSampledImage( stages, arg.name, AS{uint(arr.size())}, type, arg.state );
					}else{
						dsLayout->AddStorageImage( stages, arg.name, AS{uint(arr.size())}, type, arr[0]->PixelFormat(), Default, arg.state );
					}
				},
				[] (NullUnion) {
					CHECK_THROW_MSG( false, "unsupported argument type" );
				}
			);
		}
	}


} // AE::ResEditor
