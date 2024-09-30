// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Vulkan render pass compatible if they are equal.
	Can be changed:
		- Initial and final image layout in attachment descriptions
		- Load and store operations in attachment descriptions
		- Image layout in attachment references
		- if two render passes have a single subpass, the resolve attachment reference and depth/stencil resolve mode compatibility requirements are ignored.
*/

#pragma once

#include "ScriptObjects/ScriptFeatureSet.h"
#include "Packer/Enums.h"

namespace AE::PipelineCompiler
{
	struct CompatibleRenderPassDesc;


	//
	// Render Pass Attachment
	//

	struct RPAttachment final : public EnableScriptRC
	{
	// types
		struct ShaderIO
		{
			ShaderIOName	name;
			EShaderIO		type	= Default;
			uint			index	= UMax;		// color/input attachment index

			ShaderIO () {}
			ShaderIO (const String &name, EShaderIO type, uint idx) __Th___;

			ND_ bool  IsDefined () const	{ return name.IsDefined(); }
		};

		struct Usage
		{
			EAttachment		type		= Default;
			ShaderIO		input;		// input attachment
			ShaderIO		output;		// color attachment
			uint2			texelSize;

			explicit Usage (EAttachment type) : type{type} {}
		};
		using UsageMap_t = HashMap< SubpassName, Usage, SubpassName::Hasher_t, SubpassName::EqualTo_t >;


	// variables
		UsageMap_t		usageMap;
		EPixelFormat	format		= Default;
		MultiSamples	samples;
		const uint		index		= UMax;

		const AttachmentName			_name;
		const CompatibleRenderPassDesc*	_compat	= null;


	// methods
		RPAttachment () {}
		RPAttachment (AttachmentName::Ref name, const CompatibleRenderPassDesc* compat, uint idx) :
			index{idx}, _name{name}, _compat{compat} {}

		void  AddUsage (const String &subpassName, EAttachment type)												__Th___;
		void  AddUsage2 (const String &subpassName, EAttachment type, const ShaderIO &inOrOut)						__Th___;
		void  AddUsage3 (const String &subpassName, EAttachment type, const ShaderIO &in, const ShaderIO &out)		__Th___;
		void  AddUsage4 (const String &subpassName, EAttachment type, const packed_uint2 &texelSize)				__Th___;

		ND_ bool	Validate ()																						const;

			void	Print ()																						const;
		ND_ String	ToString (StringView padding)																	const;

		ND_ bool	IsColor ()																						const;

	private:
		void  _AddUsage (const String &subpassName, EAttachment type, Optional<ShaderIO> in, Optional<ShaderIO> out,
						 const packed_uint2 &texelSize)																__Th___;
	};
	using RPAttachmentPtr = ScriptRC< RPAttachment >;



	//
	// Attachment Specialization
	//

	struct RPAttachmentSpec final : public EnableScriptRC
	{
	// types
		struct Layout
		{
			EResourceState	state	= Default;
		};
		using LayoutMap_t = HashMap< SubpassName, Layout, SubpassName::Hasher_t, SubpassName::EqualTo_t >;


	// variables
		LayoutMap_t				layouts;
		EAttachmentLoadOp		loadOp		= EAttachmentLoadOp::Load;
		EAttachmentStoreOp		storeOp		= EAttachmentStoreOp::Store;

		const CompatibleRenderPassDesc*	_compat	= null;
		const AttachmentName			_name;


	// methods
		RPAttachmentSpec () {}
		RPAttachmentSpec (AttachmentName::Ref name, const CompatibleRenderPassDesc* compat) : _compat{compat}, _name{name}  {}

		void  AddLayout (const String &subpassName, EResourceState state)					__Th___;
		void  AddLayout2 (const String &subpassName, uint state)							__Th___;

		void  GenOptimalLayouts  ()															__Th___;
		void  GenOptimalLayouts2 (EResourceState initialState, EResourceState finalState)	__Th___;
		void  GenOptimalLayouts3 (uint initialState, uint finalState)						__Th___;

		ND_ bool	Validate (RenderPassName::Ref rpName);

			void	Print ()																const;
		ND_ String	ToString (StringView padding)											const;

	private:
		ND_ bool	_ValidatePass1 (RenderPassName::Ref rpName)								const;
		ND_ bool	_ValidatePass2 (RenderPassName::Ref rpName);
	};
	using RPAttachmentSpecPtr = ScriptRC< RPAttachmentSpec >;



	//
	// Render Pass Specialization
	//

	struct RenderPassSpec final : public EnableScriptRC
	{
	// types
		using Attachments_t	= HashMap< AttachmentName, RPAttachmentSpecPtr, AttachmentName::Hasher_t, AttachmentName::EqualTo_t >;


	// variables
		Attachments_t					_attachments;
		const RenderPassName			_name;
		const CompatibleRenderPassDesc*	_compat	= null;


	// methods
		RenderPassSpec () {}
		RenderPassSpec (RenderPassName::Ref name, const CompatibleRenderPassDesc* compat) : _name{name}, _compat{compat} {}

		ND_ RPAttachmentSpec*	AddAttachment (const String &attachmentName)	__Th___;
		ND_ RPAttachmentSpecPtr	AddAttachment2 (const String &attachmentName)	__Th___;

			void	GenOptimalLayouts ()										__Th___;

		ND_ bool	Validate ()													const;

			void	Print ()													const;
		ND_ String	ToString (StringView padding)								const;
	};
	using RenderPassSpecPtr = ScriptRC< RenderPassSpec >;



	//
	// Compatible Render Pass description
	//

	struct CompatibleRenderPassDesc final : public EnableScriptRC
	{
	// types
		using AttachBits_t = BitSet< GraphicsConfig::MaxAttachments >;

		struct SubpassInfo
		{
			SubpassName				name;
			mutable AttachBits_t	assignedColorAttachment	{0};
			mutable AttachBits_t	assignedInputAttachment	{0};
		};

		using Attachments_t		= HashMap< AttachmentName, RPAttachmentPtr, AttachmentName::Hasher_t, AttachmentName::EqualTo_t >;
		using Subpasses_t		= Array< SubpassInfo >;
		using SubpassesMap_t	= HashMap< SubpassName, uint, SubpassName::Hasher_t, SubpassName::EqualTo_t >;
		using Specializations_t	= HashMap< RenderPassName, RenderPassSpecPtr, RenderPassName::Hasher_t, RenderPassName::EqualTo_t >;

		struct ScriptConstants
		{
			static constexpr char	c_Subpass_ExternalIn[]	= "ExternalIn";
			static constexpr char	c_Subpass_ExternalOut[]	= "ExternalOut";

			const String	subpass_ExternalIn		{ c_Subpass_ExternalIn };
			const String	subpass_ExternalOut		{ c_Subpass_ExternalOut };
			const String	subpass_Main			{ "Main" };

			const String	attachment_Color		{ "out_Color" };
			const String	attachment_Depth		{ "Depth" };
			const String	attachment_DepthStencil	{ "DepthStencil" };
		};

		enum class EState
		{
			Initial,
			AddSubpasses,
			AddAttachments,
			AddSpecializations,
		};


	// variables
		const CompatRenderPassName		_name;
		EState							_state			= EState::Initial;
		Attachments_t					_attachments;
		Subpasses_t						_subpasses;
		SubpassesMap_t					_subpassMap;
		Specializations_t				_specializations;
		Array< ScriptFeatureSetPtr >	_features;


	// methods
		CompatibleRenderPassDesc ();
		explicit CompatibleRenderPassDesc (const String &name)					__Th___;

			void				AddFeatureSet (const String &name)				__Th___;

		ND_ RenderPassSpec*		AddSpecialization (const String &rpName)		__Th___;
		ND_ RenderPassSpecPtr	AddSpecialization2 (const String &rpName)		__Th___;

		ND_ RPAttachment*		AddAttachment (const String &attachmentName)	__Th___;
		ND_ RPAttachmentPtr		AddAttachment2 (const String &attachmentName)	__Th___;

			void				AddSubpass (const String &subpassName)			__Th___;

		ND_ RenderPassSpecPtr	GetRenderPass (RenderPassName::Ref name)		const;
		ND_ bool				IsFirstSubpass (SubpassName::Ref name)			const;

		ND_ bool	Validate ();

			void	Print ()													const;
		ND_ String	ToString (StringView padding)								const;

		static void  Bind (const ScriptEnginePtr &se)							__Th___;
	};
	using CompatibleRenderPassDescPtr = ScriptRC< CompatibleRenderPassDesc >;


} // AE::PipelineCompiler
