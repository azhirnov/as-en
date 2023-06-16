// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Utils/FileSystem.h"

#include "graphics/Public/ResourceEnums.h"
#include "graphics/Public/MipmapLevel.h"
#include "graphics/Public/ImageLayer.h"
#include "graphics/Public/ImageDesc.h"
#include "graphics/Public/RenderStateEnums.h"

#include "scripting/Impl/ScriptTypes.h"
#include "scripting/Bindings/CoreBindings.h"

#ifndef AE_ENABLE_SCRIPTING
# error requires scripting
#endif

namespace AE::ResEditor
{
	using namespace AE::Base;

	using AE::Graphics::EPixelFormat;
	using AE::Graphics::ImageLayer;
	using AE::Graphics::MipmapLevel;
	using AE::Graphics::EBlendFactor;
	using AE::Graphics::EBlendOp;

	using AE::Scripting::ScriptEnginePtr;

	using EnableScriptRC	= AE::Scripting::AngelScriptHelper::SimpleRefCounter;

	template <typename T>
	using ScriptRC			= AE::Scripting::AngelScriptHelper::SharedPtr<T>;

	template <typename T>
	using ScriptArray		= AE::Scripting::ScriptArray<T>;
	

	class IController;
	class IPass;

	class ScriptSceneGraphicsPass;
	class ScriptSceneRayTracingPass;

	using ScriptBufferPtr			= ScriptRC< class ScriptBuffer >;
	using ScriptImagePtr			= ScriptRC< class ScriptImage >;
	using ScriptVideoImagePtr		= ScriptRC< class ScriptVideoImage >;
	using ScriptRTGeometryPtr		= ScriptRC< class ScriptRTGeometry >;
	using ScriptRTScenePtr			= ScriptRC< class ScriptRTScene >;

	using ScriptPostprocessPtr		= ScriptRC< class ScriptPostprocess >;
	using ScriptComputePassPtr		= ScriptRC< class ScriptComputePass >;
	using ScriptBasePassPtr			= ScriptRC< class ScriptBasePass >;
	using ScriptDynamicDimPtr		= ScriptRC< class ScriptDynamicDim >;
	using ScriptDynamicUInt3Ptr		= ScriptRC< class ScriptDynamicUInt3 >;
	using ScriptDynamicInt4Ptr		= ScriptRC< class ScriptDynamicInt4 >;
	using ScriptDynamicFloat4Ptr	= ScriptRC< class ScriptDynamicFloat4 >;
	using ScriptDynamicUIntPtr		= ScriptRC< class ScriptDynamicUInt >;
	using ScriptDynamicULongPtr		= ScriptRC< class ScriptDynamicULong >;
	using ScriptDynamicFloatPtr		= ScriptRC< class ScriptDynamicFloat >;
	using ScriptCollectionPtr		= ScriptRC< class ScriptCollection >;
	
	using ScriptBaseControllerPtr	= ScriptRC< class ScriptBaseController >;
	using ScriptGeomSourcePtr		= ScriptRC< class ScriptGeomSource >;
	
	using ScriptScenePtr			 = ScriptRC< class ScriptScene >;


	enum class EResourceUsage : uint
	{
		Unknown			= 0,

		ComputeRead		= 1 << 0,
		ComputeWrite	= 1 << 1,
		ComputeRW		= ComputeRead | ComputeWrite,

		ColorAttachment	= 1 << 2,
		DepthStencil	= 1 << 3,

		UploadedData	= 1 << 4,
		WillReadback	= 1 << 5,

		Sampled			= 1 << 6,
		GenMipmaps		= 1 << 7,
		Present			= 1 << 8,

		VertexInput		= 1 << 9,
		IndirectBuffer	= 1 << 10,
		ASBuild			= 1 << 11,
	};
	AE_BIT_OPERATORS( EResourceUsage );


} // AE::ResEditor
