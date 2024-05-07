// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics/Public/RenderPassDesc.h"
#include "Packer/PackCommon.h"

#ifdef AE_BUILD_PIPELINE_COMPILER
# include "ScriptObjects/ScriptRenderPass.h"
#endif

namespace AE::PipelineCompiler
{
	using namespace AE::Graphics;

	//
	// Serializable Render Pass
	//

	class SerializableRenderPass : public Serializing::ISerializable
	{
	// types
	public:
		struct AttachmentState
		{
			EResourceState	initial	= Default;
			EResourceState	final	= Default;
		};
		using AttachmentStates_t = StaticArray< AttachmentState, GraphicsConfig::MaxAttachments >;


	// variables
	protected:
		RenderPassName			_name;
		AttachmentStates_t		_states			= {};


	// methods
	public:
		ND_ RenderPassName const&			Name ()				const	{ return _name; }
		ND_ AttachmentStates_t const&		AttachmentStates ()	const	{ return _states; }
	};


} // AE::PipelineCompiler

#ifdef AE_ENABLE_VULKAN
# if not defined(VK_NO_PROTOTYPES) and defined(VULKAN_CORE_H_)
#	error invalid configuration, include vulkan.h after this file.
# endif

# ifndef VK_NO_PROTOTYPES
#	define VK_NO_PROTOTYPES
# endif

# define VK_ENABLE_BETA_EXTENSIONS
# ifdef AE_COMPILER_MSVC
#	pragma warning (push, 0)
#	include <vulkan/vulkan.h>
#	pragma warning (pop)
# else
#	include <vulkan/vulkan.h>
# endif

namespace AE::PipelineCompiler
{
	//
	// Serializable Vulkan Render Pass
	//

	class SerializableVkRenderPass final : public SerializableRenderPass
	{
	// variables
	private:
		LinearAllocator<>		_allocator;
		VkRenderPassCreateInfo2	_ci				= {};
		bool					_isCompatible	= false;	// 'true' after 'MakeCompatible()'


	// methods
	public:
		SerializableVkRenderPass () {}

		#ifdef AE_BUILD_PIPELINE_COMPILER
		ND_ bool  Create (const CompatibleRenderPassDesc &compat);
		ND_ bool  Create (const CompatibleRenderPassDesc &compat, const RenderPassSpec &rp);
		ND_ static bool  MakeCompatible (INOUT Array<SerializableVkRenderPass> &);
		#endif
		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		ND_ VkRenderPassCreateInfo2 *		operator -> ()				{ return &_ci; }
		ND_ VkRenderPassCreateInfo2 const*	operator -> ()		const	{ return &_ci; }

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;

		ND_ bool  Deserialize (EPixelFormat fmt, Serializing::Deserializer &) __NE___;

	private:
		#ifdef AE_BUILD_PIPELINE_COMPILER
		bool  _Create (const CompatibleRenderPassDesc &compat, const RenderPassSpec &rp, bool withDeps);
		bool  _ConvertAttachments (const CompatibleRenderPassDesc &compat, const RenderPassSpec &spec);
		bool  _ConvertSubpasses (const CompatibleRenderPassDesc &compat, const RenderPassSpec &spec);
		bool  _ConvertDependencies (const CompatibleRenderPassDesc &compat, const RenderPassSpec &spec);
		#endif
	};

} // AE::PipelineCompiler

#endif // AE_ENABLE_VULKAN
//-----------------------------------------------------------------------------


#include "Packer/MetalEnums.h"

namespace AE::PipelineCompiler
{
	//
	// Serializable Metal Render Pass
	//

	class SerializableMtlRenderPass final : public SerializableRenderPass
	{
	// types
	public:
		struct MtlAttachment
		{
			MtlPixelFormat			format			= MtlPixelFormat::Invalid;
			MtlLoadAction			loadAction		= MtlLoadAction::DontCare;
			MtlStoreAction			storeAction		= MtlStoreAction::DontCare;
			MtlStoreActionOptions	storeActionOpt	= MtlStoreActionOptions::None;
			MtlAttachmentFlags		flags			= Default;

			ND_ bool  IsDefined () const	{ return format != MtlPixelFormat::Invalid and flags != Default; }
		};
		using MtlAttachments_t = StaticArray< MtlAttachment, GraphicsConfig::MaxAttachments >;


	// variables
	private:
		MtlAttachments_t					_mtlAtt;
		//ubyte								_attCount		= 0;
		MtlMultisampleDepthResolveFilter	_depthResolve	= MtlMultisampleDepthResolveFilter::Sample0;
		MtlMultisampleStencilResolveFilter	_stencilResolve	= MtlMultisampleStencilResolveFilter::Sample0;


	// methods
	public:
		SerializableMtlRenderPass () {}

		#ifdef AE_BUILD_PIPELINE_COMPILER
		ND_ bool  Create (const CompatibleRenderPassDesc &compat);
		ND_ bool  Create (const CompatibleRenderPassDesc &compat, const RenderPassSpec &rp);
		#endif
		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		ND_ MtlAttachments_t const&		MtlAttachments ()	const	{ return _mtlAtt; }

		ND_ MtlMultisampleDepthResolveFilter	DepthResolveFilter ()	const	{ return _depthResolve; }
		ND_ MtlMultisampleStencilResolveFilter	StencilResolveFilter ()	const	{ return _stencilResolve; }


		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;

		ND_ bool  Deserialize (EPixelFormat fmt, Serializing::Deserializer &) __NE___;

	private:
		#ifdef AE_BUILD_PIPELINE_COMPILER
		bool  _ConvertAttachments (const CompatibleRenderPassDesc &compat, const RenderPassSpec &spec);
		bool  _ValidateSubpasses (const CompatibleRenderPassDesc &compat, const RenderPassSpec &spec);
		#endif
	};

} // AE::PipelineCompiler
//-----------------------------------------------------------------------------



namespace AE::PipelineCompiler
{
	using namespace AE::Graphics;


	//
	// Subpass Shader IO
	//

	struct SubpassShaderIO : public Serializing::ISerializable
	{
	// types
		using ShaderIOArr_t = FixedTupleArray< GraphicsConfig::MaxColorAttachments, ShaderIOName::Optimized_t, EShaderIO >;
		using Hasher_t		= DefaultHasher_CalcHash< SubpassShaderIO >;


	// variables
		ShaderIOArr_t	colorAttachments;
		ShaderIOArr_t	inputAttachments;


	// methods
		ND_ bool	operator == (const SubpassShaderIO &rhs)C_NE___;
		ND_ HashVal	CalcHash ()								C_NE___;

		// ISerializable
		bool  Serialize (Serializing::Serializer &)			C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &)		__NE_OV;
	};
	StaticAssert( sizeof(SubpassShaderIO::ShaderIOArr_t) == 44 );



	//
	// Serializable Render Pass Info
	//

	class SerializableRenderPassInfo final : public Serializing::ISerializable
	{
	// types
	public:
		struct Subpass final : SubpassShaderIO
		{
			ubyte	subpassIndex	= UMax;
			bool	hasDepth		= false;
			bool	hasStencil		= false;

			// ISerializable
			bool  Serialize (Serializing::Serializer &)		C_NE_OV;
			bool  Deserialize (Serializing::Deserializer &) __NE_OV;
		};

	public:
		static constexpr uint	MaxCompatCount	= 1 << 16;
		static constexpr uint	MaxSpecCount	= 1 << 8;


	private:
		using AttachmentNameToIdx_t	= FixedMap< AttachmentName::Optimized_t, ubyte, GraphicsConfig::MaxAttachments >;
		using SubpassShaderIO_t		= FixedMap< SubpassName::Optimized_t, Subpass, GraphicsConfig::MaxSubpasses >;
		using FeatureSets_t			= FixedSet< FeatureSetName::Optimized_t, 8 >;


	// variables
	public:
		CompatRenderPassName::Optimized_t	name;
		AttachmentNameToIdx_t				attachments;
		SubpassShaderIO_t					subpasses;
		FeatureSets_t						features;


	// methods
	public:
		SerializableRenderPassInfo () {}

		#ifdef AE_BUILD_PIPELINE_COMPILER
		ND_ bool  Create (const CompatibleRenderPassDesc &compat);
		#endif
		#ifdef AE_TEST_PIPELINE_COMPILER
		ND_ String  ToString (const HashToName &) const;
		#endif

		// ISerializable
		bool  Serialize (Serializing::Serializer &)		C_NE_OV;
		bool  Deserialize (Serializing::Deserializer &) __NE_OV;
	};



#ifdef AE_BUILD_PIPELINE_COMPILER

	//
	// Render Pass Packer
	//

	class RenderPassPacker
	{
	// methods
	public:
		ND_ static bool  Serialize (Serializing::Serializer &ser) __NE___;
	};

#endif


} // AE::PipelineCompiler


template <>
struct std::hash< AE::PipelineCompiler::SubpassShaderIO > final :
	AE::Base::DefaultHasher_CalcHash< AE::PipelineCompiler::SubpassShaderIO >
{};


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

#  ifdef AE_HAS_VULKAN_HEADERS
#	pragma detect_mismatch( "AE_HAS_VULKAN_HEADERS", "1" )
#  else
#	pragma detect_mismatch( "AE_HAS_VULKAN_HEADERS", "0" )
#  endif

#endif // AE_CPP_DETECT_MISMATCH
