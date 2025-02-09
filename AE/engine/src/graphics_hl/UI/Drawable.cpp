// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "graphics_hl/UI/Drawable.h"

#ifndef AE_UI_NO_GRAPHICS
# include "graphics_hl/UI/System.h"
#endif

namespace AE::UI
{
	using namespace AE::Graphics;
	using namespace AE::Serializing;

/*
=================================================
	_GetColorAnimSpeed
=================================================
*/
#ifndef AE_UI_NO_GRAPHICS
# ifdef AE_CFG_RELEASE
	inline float  IDrawable::_GetColorAnimSpeed () C_NE___
	{
		return 10.f;
	}
# else
	inline float  IDrawable::_GetColorAnimSpeed () C_NE___
	{
		return UIStyleCollection().GetSettings().colorAnimSpeed;
	}
# endif
#endif

/*
=================================================
	Material::operator ==
=================================================
*/
	bool  IDrawable::Material::operator == (const Material &rhs) C_NE___
	{
		return	//stencilRef		== rhs.stencilRef		and
			//	topology		== rhs.topology			and
			//	dynamicOffset	== rhs.dynamicOffset	and
				ppln			== rhs.ppln;
	}
//-----------------------------------------------------------------------------



/*
=================================================
	SetStyle
=================================================
*/
#ifndef AE_UI_NO_GRAPHICS
	bool  RectangleDrawable::SetStyle (StyleName::Ref name) __NE___
	{
		_data.stylePtr	= UIStyleCollection().GetStyle( name );
		CHECK_ERR( _data.stylePtr );

		_style			= name;
		_data.mtr.ppln	= _data.stylePtr->pipeline;

		auto	style	= _data.stylePtr->Get( _data.styleIdx );
		ASSERT_MSG( style.uv.IsEmpty(), "UV defined for color-only drawable, use ImageDrawable instead" );

		_data.currColor	= style.color;
		_data.currScale	= style.scale;

		_data.prevColor	= _data.currColor;
		_data.prevScale	= _data.currScale;
		_data.factor	= 1.f;

		return true;
	}
#else
	bool  RectangleDrawable::SetStyle (StyleName::Ref name) __NE___
	{
		_style = name;
		return true;
	}
#endif

/*
=================================================
	Draw
=================================================
*/
#ifndef AE_UI_NO_GRAPHICS
	void  RectangleDrawable::Draw (const DrawParams &params, Canvas &canvas, DrawContext_t &ctx) __Th___
	{
		// flush canvas if different materials
		if_unlikely( params.mtr != _data.mtr and not canvas.IsEmpty() )
		{
			ctx.BindPipeline( params.mtr->ppln );
			ctx.BindDescriptorSet( params.mtr.dsIndex, params.mtr.ds, {&params.mtr.globalDynOffset, 1} );	// TODO
		//	ctx.SetStencilReference( params.mtr->stencilRef );
			canvas.Flush( ctx, params.mtr.topology );
		}
		params.mtr = _data.mtr;

		// change state
		if_unlikely( _data.styleIdx != params.style )
		{
			_data.styleIdx	= params.style;
			_data.prevColor	= (_data.factor < 1.0f ? Lerp( _data.prevColor, _data.currColor, _data.factor ) : _data.currColor);
			_data.prevScale	= (_data.factor < 1.0f ? Lerp( _data.prevScale, _data.currScale, _data.factor ) : _data.currScale);

			auto	style = _data.stylePtr->Get( _data.styleIdx );
			_data.currColor	= style.color;
			_data.currScale	= style.scale;
			_data.factor	= 0.0f;
		}

		RGBA8u	color	= _data.currColor;
		float	scale	= _data.currScale;

		// color animation
		if_unlikely( _data.factor < 1.0f )
		{
			_data.factor	= Min( 1.0f, _data.factor + params.dt.count() * _GetColorAnimSpeed() );
			color			= Lerp( _data.prevColor, _data.currColor, _data.factor );
			scale			= Lerp( _data.prevScale, _data.currScale, _data.factor );
		}

		canvas.Draw( Rectangle2D{ params.clipRect.Scale( scale ), RectF{}, color });
	}
#else
	void  RectangleDrawable::Draw (const DrawParams &, Canvas &, DrawContext_t &) __Th___
	{}
#endif

/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  RectangleDrawable::Serialize (Serializer &ser) C_NE___
	{
		return ser( _GetDrawableID(Type()), _style );
	}

	bool  RectangleDrawable::Deserialize (Deserializer &des) __NE___
	{
		if_unlikely( not des( OUT _style ))
			return false;

		return SetStyle( StyleName{_style} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	SetStyle
=================================================
*/
#ifndef AE_UI_NO_GRAPHICS
	bool  ImageDrawable::SetStyle (StyleName::Ref name) __NE___
	{
		_data.stylePtr	= UIStyleCollection().GetStyle( name );
		CHECK_ERR( _data.stylePtr );

		_style			= name;
		_data.mtr.ppln	= _data.stylePtr->pipeline;

		auto	style	= _data.stylePtr->Get( _data.styleIdx );
		ASSERT_MSG( not style.uv.IsEmpty(), "UV must be defined for image drawable" );

		_data.currColor	= style.color;
		_data.currScale	= style.scale;
		_data.uv		= style.UV();

		_data.prevColor	= _data.currColor;
		_data.prevScale	= _data.currScale;
		_data.factor	= 1.f;

		return true;
	}
#else
	bool  ImageDrawable::SetStyle (StyleName::Ref name) __NE___
	{
		_style = name;
		return true;
	}
#endif

/*
=================================================
	Draw
=================================================
*/
#ifndef AE_UI_NO_GRAPHICS
	void  ImageDrawable::Draw (const DrawParams &params, Canvas &canvas, DrawContext_t &ctx) __Th___
	{
		// flush canvas if different materials
		if_unlikely( params.mtr != _data.mtr and not canvas.IsEmpty() )
		{
			ctx.BindPipeline( params.mtr->ppln );
			ctx.BindDescriptorSet( params.mtr.dsIndex, params.mtr.ds, {&params.mtr.globalDynOffset, 1} );	// TODO
		//	ctx.SetStencilReference( params.mtr->stencilRef );
			canvas.Flush( ctx, params.mtr.topology );
		}
		params.mtr = _data.mtr;

		// change state
		if_unlikely( _data.styleIdx != params.style )
		{
			_data.styleIdx	= params.style;
			_data.prevColor	= (_data.factor < 1.0f ? Lerp( _data.prevColor, _data.currColor, _data.factor ) : _data.currColor);
			_data.prevScale	= (_data.factor < 1.0f ? Lerp( _data.prevScale, _data.currScale, _data.factor ) : _data.currScale);

			auto	style = _data.stylePtr->Get( _data.styleIdx );
			_data.currColor	= style.color;
			_data.currScale	= style.scale;
			_data.factor	= 0.0f;
		}

		RGBA8u	color	= _data.currColor;
		float	scale	= _data.currScale;

		// color animation
		if_unlikely( _data.factor < 1.0f )
		{
			_data.factor	= Min( 1.0f, _data.factor + params.dt.count() * _GetColorAnimSpeed() );
			color			= Lerp( _data.prevColor, _data.currColor, _data.factor );
			scale			= Lerp( _data.prevScale, _data.currScale, _data.factor );
		}

		canvas.Draw( Rectangle2D{ params.clipRect.Scale( scale ), _data.uv, color });
	}
#else
	void  ImageDrawable::Draw (const DrawParams &, Canvas &, DrawContext_t &) __Th___
	{}
#endif

/*
=================================================
	Serialize / Deserialize
=================================================
*/
	bool  ImageDrawable::Serialize (Serializer &ser) C_NE___
	{
		return ser( _GetDrawableID(Type()), _style );
	}

	bool  ImageDrawable::Deserialize (Deserializer &des) __NE___
	{
		if_unlikely( not des( OUT _style ))
			return false;

		return SetStyle( StyleName{_style} );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	DrawableSerializer
=================================================
*/
namespace
{
	template <typename T>
	struct DrawableSerializer
	{
		static bool  Serialize (Serializer &, const void*) __NE___ { return false; }

		static bool  Deserialize (Deserializer &des, INOUT void* &ptr, Ptr<IAllocator> alloc) __NE___
		{
			CHECK_ERR( ptr == null and alloc != null );

			ptr = alloc->Allocate<T>();
			CHECK_ERR( ptr != null );

			return PlacementNew<T>( OUT ptr, alloc )->Deserialize( des );
		}
	};
}
/*
=================================================
	RegisterDrawables
=================================================
*/
	bool  IDrawable::RegisterDrawables (ObjectFactory &factory) __NE___
	{
		switch_enum( EType::Unknown )
		{
			case EType::Unknown :
			case EType::NinePatch :
			case EType::Text :
			case EType::_Count :
			#define REG( _name_ )															\
				case EType::_name_ :														\
					CHECK_ERR( factory.Register< _name_##Drawable >(						\
									SerializedID{ #_name_ "Drawable" },						\
									&DrawableSerializer< _name_##Drawable >::Serialize,		\
									&DrawableSerializer< _name_##Drawable >::Deserialize ));
			REG( Rectangle )
			REG( Image )
		//	REG( NinePatch )
		//	REG( Text )
			#undef REG

			default : break;
		}
		switch_end
		return true;
	}

/*
=================================================
	_GetDrawableID
=================================================
*/
	SerializedID  IDrawable::_GetDrawableID (EType type) __NE___
	{
		switch_enum( type )
		{
			#define REG( _name_ )	case EType::_name_ : return SerializedID{ #_name_ "Drawable" };
			REG( Rectangle )
			REG( Image )
			REG( NinePatch )
			REG( Text )
			#undef REG
			case EType::_Count :
			case EType::Unknown :	break;
		}
		switch_end
		return Default;
	}


} // AE::UI
