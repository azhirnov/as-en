// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "graphics/Canvas/Canvas.h"
#include "graphics/Canvas/Primitives.h"

#include "graphics/Resources/FormattedText.h"

#include "graphics/UI/LayoutEnums.h"
#include "graphics/UI/StyleCollection.h"

namespace AE::UI
{
	using AE::Graphics::Canvas;
	using AE::Graphics::SurfaceDimensions;
	using TransferContext_t	= Graphics::DirectCtx::Transfer;
	using DrawContext_t		= Graphics::DirectCtx::Draw;



	//
	// Drawable interface
	//
	class IDrawable :
		public EnableRC<IDrawable>,
		public NonAllocatable
	{
	// types
	public:
		struct Material;
		struct GlobalMaterial;
		struct PreDrawParams;
		struct DrawParams;

		using PreDrawFn_t	= void (*) (INOUT void* &data, TransferContext_t &ctx)											__Th___;
		using DrawFn_t		= void (*) (INOUT void* &data, const DrawParams &params, Canvas &canvas, DrawContext_t &ctx)	__Th___;

		enum class EType : ubyte
		{
			Rectangle,
			Image,
			NinePatch,
			Text,
			_Count,
			Unknown		= 0xFF
		};

	protected:
		using IStylePtr		= Ptr< const StyleCollection::IStyle >;


	// interface
	public:
		//virtual void  Init () = 0;	// TODO

		virtual void  PreDraw (const PreDrawParams &params, TransferContext_t &ctx)			__Th___	{ Unused( params, ctx ); }
		virtual void  Draw (const DrawParams &params, Canvas &canvas, DrawContext_t &ctx)	__Th___	= 0;

		ND_ virtual EType  Type ()															C_NE___ = 0;

		// serializing
		ND_ virtual bool  Serialize (Serializing::Serializer &)								C_NE___ = 0;
		ND_ virtual bool  Deserialize (Serializing::Deserializer &)							__NE___ = 0;

		// utils
		ND_ static bool  RegisterDrawables (Serializing::ObjectFactory &)					__NE___;

		ND_ static void*  operator new (usize, void* where)									__NE___	{ return where; }
		//	static void   operator delete (void*, void*)									__NE___	{}

	protected:
		ND_ float  _GetColorAnimSpeed ()													C_NE___;

		ND_ static Serializing::SerializedID  _GetDrawableID (EType type)					__NE___;
	};


	//
	// Material
	//
	struct IDrawable::Material
	{
	// variables
		uint					stencilRef		= 0;
		uint					mtrDynOffset	= 0;
		GraphicsPipelineID		ppln;

		// all UI pipelines must:
		//	- use same descriptor sets
		//	- have dynamic stencil reference

	// methods
		ND_ bool  operator == (const Material &rhs)		C_NE___;
		ND_ bool  operator != (const Material &rhs)		C_NE___	{ return not (*this == rhs); }

		ND_ bool  IsDefined ()							C_NE___	{ return ppln != Default; }
	};


	//
	// Global Material
	//
	struct IDrawable::GlobalMaterial
	{
	// variables
		mutable Material				mtr;			// inout
		DescriptorSetID					ds;
		uint							globalDynOffset	= 0;

		static constexpr DescSetBinding	dsIndex			{0};
		static constexpr EPrimitive		topology		= EPrimitive::TriangleList;

	// methods
			void  operator =  (const Material &rhs)		C_NE___	{ mtr = rhs; }

		ND_ bool  operator == (const Material &rhs)		C_NE___	{ return mtr == rhs; }
		ND_ bool  operator != (const Material &rhs)		C_NE___	{ return mtr != rhs; }

		ND_ bool  IsDefined ()							C_NE___	{ return mtr.IsDefined(); }

		ND_ Material*  operator -> ()					C_NE___	{ return &mtr; }
	};


	//
	// Draw Params
	//
	struct IDrawable::DrawParams
	{
		RectF			globalRect;
		RectF			clipRect;
		EStyleIndex		style		= EStyleIndex::Enabled;
		secondsf		dt;
		GlobalMaterial	mtr;		// inout
	};


	//
	// Pre Draw Params
	//
	struct IDrawable::PreDrawParams
	{
		secondsf		dt;
	};



	//
	// Rectangle Shape Drawable
	//
	class RectangleDrawable final : public IDrawable
	{
	// types
	public:
		struct Data
		{
			Material		mtr;
			EStyleIndex		styleIdx		= EStyleIndex::Enabled;
			RGBA8u			currColor;
			float			currScale		= 1.f;
			float			factor			= 2.0f;

			// accessed in cold branch
			RGBA8u			prevColor;
			float			prevScale		= 1.f;
			IStylePtr		stylePtr;
		};

	// variables
	private:
		StyleName::Optimized_t	_style;
		Data					_data;

	// methods
	public:
		explicit RectangleDrawable (Ptr<IAllocator>)								__NE___	{}

		ND_ bool  SetStyle (StyleName::Ref name)									__NE___;
			void  SetStyleName (StyleName::Ref name)								__NE___	{ _style = name; }

		// IDrawable //
		void	Draw (const DrawParams &params, Canvas &canvas, DrawContext_t &ctx)	__Th_OV;
		EType	Type ()																C_NE_OV	{ return EType::Rectangle; }

		// serializing
		bool	Serialize (Serializing::Serializer &)								C_NE_OV;
		bool	Deserialize (Serializing::Deserializer &)							__NE_OV;
	};



	//
	// Image Drawable
	//
	class ImageDrawable final : public IDrawable
	{
	// types
	public:
		struct Data
		{
			Material		mtr;
			EStyleIndex		styleIdx		= EStyleIndex::Enabled;
			RGBA8u			currColor;
			float			currScale		= 1.f;
			float			factor			= 2.0f;
			RectF			uv;

			// accessed in cold branch
			RGBA8u			prevColor;
			float			prevScale		= 1.f;
			IStylePtr		stylePtr;
		};

	// variables
	private:
		StyleName::Optimized_t	_style;
		Data					_data;

	// methods
	public:
		explicit ImageDrawable (Ptr<IAllocator>)									__NE___	{}

		ND_ bool  SetStyle (StyleName::Ref name)									__NE___;
			void  SetStyleName (StyleName::Ref name)								__NE___	{ _style = name; }

		// IDrawable //
		void	Draw (const DrawParams &params, Canvas &canvas, DrawContext_t &ctx)	__Th_OV;
		EType	Type ()																C_NE_OV	{ return EType::Image; }

		// serializing
		bool	Serialize (Serializing::Serializer &)								C_NE_OV;
		bool	Deserialize (Serializing::Deserializer &)							__NE_OV;
	};



	//
	// Nine Patch Image Drawable
	//
	class NinePatchDrawable final : public IDrawable
	{
	// types
	public:
		struct Data
		{
			Material		mtr;
		};

	// variables
	private:
		StyleName::Optimized_t	_style;
		Data					_data;

	// methods
	public:
		explicit NinePatchDrawable (Ptr<IAllocator>)								__NE___	{}

		ND_ bool  SetStyle (StyleName::Ref name)									__NE___;
			void  SetStyleName (StyleName::Ref name)								__NE___	{ _style = name; }

		// IDrawable //
		void	Draw (const DrawParams &params, Canvas &canvas, DrawContext_t &ctx)	__Th_OV;
		EType	Type ()																C_NE_OV	{ return EType::NinePatch; }

		// serializing
		bool	Serialize (Serializing::Serializer &)								C_NE_OV;
		bool	Deserialize (Serializing::Deserializer &)							__NE_OV;
	};



	//
	// Text Drawable
	//
	class TextDrawable final : public IDrawable
	{
	// types
	public:
		using FontStylePtr	= Ptr< const StyleCollection::FontStyle >;

		struct Data
		{
			Material		mtr;
			EStyleIndex		styleIdx		= EStyleIndex::Enabled;
			RGBA8u			currColor;
			float			currScale		= 1.f;
			float			factor			= 2.0f;

			// accessed in cold branch
			RGBA8u			prevColor;
			float			prevScale		= 1.f;
			FontStylePtr	stylePtr;
		};


	// variables
	private:
		StyleName::Optimized_t		_style;
		Data						_data;
	//	Graphics::FormattedText		_text;
		U8String					_text;


	// methods
	public:
		explicit TextDrawable (Ptr<IAllocator>)										__NE___	{}

		ND_ bool  SetStyle (StyleName::Ref name)									__NE___;
			void  SetStyleName (StyleName::Ref name)								__NE___	{ _style = name; }

			void  SetText (U8String text)											__NE___	{ _text = RVRef(text); }

		// IDrawable //
		void	Draw (const DrawParams &params, Canvas &canvas, DrawContext_t &ctx)	__Th_OV;
		EType	Type ()																C_NE_OV	{ return EType::Text; }

		// serializing
		bool	Serialize (Serializing::Serializer &)								C_NE_OV;
		bool	Deserialize (Serializing::Deserializer &)							__NE_OV;
	};


} // AE::UI
