// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "res_editor/Scripting/ScriptCommon.h"
#include "res_editor/Scripting/ScriptBuffer.h"
#include "res_editor/Scripting/ScriptImage.h"
#include "res_editor/Scripting/ScriptVideoImage.h"
#include "res_editor/Scripting/ScriptRTScene.h"
#include "res_editor/Passes/IPass.h"

namespace AE::ResEditor
{

	//
	// Pass Arguments
	//

	class ScriptPassArgs final : Noncopyable
	{
	// types
	public:
		using ResourceUnion_t = Union< NullUnion, ScriptBufferPtr, ScriptImagePtr, ScriptVideoImagePtr, ScriptRTScenePtr,
									   Array<ScriptImagePtr> >;

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

		void  ArgSceneIn (const String &name, const ScriptRTScenePtr &scene)								__Th___;

		void  ArgBufferIn (const String &name, const ScriptBufferPtr &buf)									__Th___;
		void  ArgBufferOut (const String &name, const ScriptBufferPtr &buf)									__Th___;
		void  ArgBufferInOut (const String &name, const ScriptBufferPtr &buf)								__Th___;

		void  ArgImageIn (const String &name, const ScriptImagePtr &img)									__Th___;
		void  ArgImageOut (const String &name, const ScriptImagePtr &img)									__Th___;
		void  ArgImageInOut (const String &name, const ScriptImagePtr &img)									__Th___;

		void  ArgTextureIn (const String &name, const ScriptImagePtr &tex, const String &samplerName)		__Th___;
		void  ArgVideoIn (const String &name, const ScriptVideoImagePtr &tex, const String &samplerName)	__Th___;

		void  ArgImageArrIn (const String &name, Array<ScriptImagePtr> arr)									__Th___;
		void  ArgImageArrOut (const String &name, Array<ScriptImagePtr> arr)								__Th___;
		void  ArgImageArrInOut (const String &name, Array<ScriptImagePtr> arr)								__Th___;

		template <typename DSL, typename AS, typename AT>
		void  ArgsToDescSet (EShaderStages stages, DSL &dsLayout, AS arraySize, AT accessType)				C_Th___;
		void  InitResources (OUT ResourceArray &resources, PipelinePackID packId)							C_Th___;
		void  ValidateArgs ()																				C_Th___;
		void  AddLayoutReflection ()																		C_Th___;

		void  CopyFrom (const ScriptPassArgs &)																__Th___;

	private:
		void  _AddArg (const String &name, const ScriptBufferPtr &buf, EResourceUsage usage)				__Th___;
		void  _AddArg (const String &name, const ScriptImagePtr &img, EResourceUsage usage)					__Th___;
		void  _AddArg (const String &name, Array<ScriptImagePtr> arr, EResourceUsage usage)					__Th___;
	};



/*
=================================================
	ArgsToDescSet
=================================================
*/
	template <typename DSL, typename AS, typename AT>
	void  ScriptPassArgs::ArgsToDescSet (const EShaderStages stages, DSL &dsLayout, AS, AT accessType) C_Th___
	{
		const AS	array_size {1};

		for (auto& arg : _args)
		{
			Visit( arg.res,
				[&] (ScriptBufferPtr buf) {
					if ( buf->HasLayout() ){
						dsLayout->AddStorageBuffer( stages, arg.name, array_size, buf->GetTypeName(), accessType, arg.state, False{} );
					}else{
					// TODO
					//	dsLayout->AddStorageTexelBuffer( stages, arg.name, array_size, PipelineCompiler::EImageType(buf->TexelBufferType()),
					//									 buf->GetViewFormat(), accessType, arg.state );
					}
				},
				[&] (ScriptImagePtr tex) {
					const auto	type = PipelineCompiler::EImageType(tex->ImageType());
					if ( arg.samplerName.empty() )
						dsLayout->AddStorageImage( stages, arg.name, array_size, type, tex->Description().format, accessType, arg.state );
					else
						dsLayout->AddCombinedImage_ImmutableSampler( stages, arg.name, type, arg.state, arg.samplerName );
				},
				[&] (ScriptVideoImagePtr video) {
					String	sampler = (video->HasYcbcrSampler() ? video->GetSamplerName() : arg.samplerName);
					dsLayout->AddCombinedImage_ImmutableSampler( stages, arg.name, PipelineCompiler::EImageType(video->ImageType()), arg.state, sampler );
				},
				[&] (ScriptRTScenePtr) {
					dsLayout->AddRayTracingScene( stages, arg.name, array_size );
				},
				[&] (const Array<ScriptImagePtr> &arr) {
					dsLayout->AddStorageImage(	stages, arg.name, AS{uint(arr.size())},
												PipelineCompiler::EImageType(arr[0]->ImageType()),
												arr[0]->Description().format, accessType, arg.state );
				},
				[] (NullUnion) {
					CHECK_THROW_MSG( false, "unsupported argument type" );
				}
			);
		}
	}


} // AE::ResEditor
