// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "res_editor/Resources/Buffer.h"
#include "res_editor/Resources/RTScene.h"
#include "res_editor/Resources/Image.h"
#include "res_editor/Resources/VideoImage.h"
#include "res_editor/Resources/VideoImage2.h"
#include "res_editor/Core/EditorUI.h"

namespace AE::ResEditor
{
namespace
{
	template <typename B>
	struct CopyConstant
	{
		B&		_dst;
		RC<>	_rc;

		CopyConstant (B& dst, RC<> rc) : _dst{dst}, _rc{RVRef(rc)}
		{}

		template <typename T, usize I>
		void  operator () () __NE___
		{
			if ( auto* dyn_val = DynCast<T>( _rc.get() ))
			{
				const auto	GetValue = [dyn_val] ()
				{{
					if constexpr( IsSameTypes< T, DynamicDim >)
						return dyn_val->Dimension3_NonZero();
					else
						return dyn_val->Get();
				}};

				auto	val = GetValue();

				StaticAssert( sizeof(val) <= sizeof(B) );
				std::memcpy( OUT &_dst, &val, sizeof(val) );
			}
		}
	};

} // namespace
//-----------------------------------------------------------------------------



/*
=================================================
	constructor / destructor
=================================================
*/
	ResourceArray::ResourceArray ()
	{}

	ResourceArray::~ResourceArray ()
	{}
//-----------------------------------------------------------------------------


/*
=================================================
	constructor
=================================================
*/
	IPass::IPass ()
	{}

	IPass::IPass (StringView dbgName, RGBA8u dbgColor) :
		_dbgName{dbgName}, _dbgColor{dbgColor}
	{}

/*
=================================================
	destructor
=================================================
*/
	IPass::~IPass () __NE___
	{
		UIInteraction::Instance().RemovePass( this );
	}

/*
=================================================
	_CopySliders
=================================================
*/
	void  IPass::_CopySliders (OUT StaticArray<float4, 8>	&dstFloats,
							   OUT StaticArray<int4, 8>		&dstInts,
							   OUT StaticArray<float4, 8>	&dstColors) const
	{
		if ( auto p_sliders = UIInteraction::Instance().GetSliders( this ))
		{
			auto	sliders = p_sliders->ReadLock();

			StaticAssert( sizeof(dstFloats)	== sizeof(sliders->floatSliders) );
			StaticAssert( sizeof(dstInts)	== sizeof(sliders->intSliders) );
			StaticAssert( sizeof(dstColors)	== sizeof(sliders->colors) );

			MemCopy( OUT dstFloats.data(),	sliders->floatSliders.data(),	Sizeof(dstFloats) );
			MemCopy( OUT dstInts.data(),	sliders->intSliders.data(),		Sizeof(dstInts) );
			MemCopy( OUT dstColors.data(),	sliders->colors.data(),			Sizeof(dstColors) );
		}
	}

/*
=================================================
	_CopyConstants
=================================================
*/
	void  IPass::_CopyConstants (const Constants			&c,
								 OUT StaticArray<float4, 8>	&dstFloats,
								 OUT StaticArray<int4, 8>	&dstInts) const
	{
		StaticAssert( sizeof(dstFloats)	== sizeof(float4) * Constants::MaxCount );
		StaticAssert( sizeof(dstInts)	== sizeof(int4) * Constants::MaxCount );

		for (usize i = 0; i < c.f.size(); ++i)
		{
			if ( c.f[i] )
				DynamicFloatTypes_t::Visit( CopyConstant{ OUT dstFloats[i], c.f[i] });
		}
		for (usize i = 0; i < c.i.size(); ++i)
		{
			if ( c.i[i] )
				DynamicIntTypes_t::Visit( CopyConstant{ OUT dstInts[i], c.i[i] });
		}
	}

/*
=================================================
	_IsEnabled
=================================================
*/
	bool  IPass::_IsEnabled () const
	{
		if_likely( not _enablePass.dynamic )
			return true;

		const uint	lhs = _enablePass.dynamic->Get();
		const uint	rhs = _enablePass.ref;

		switch_enum( _enablePass.op )
		{
			case ECompare::Less :		return lhs <  rhs;
			case ECompare::Equal :		return lhs == rhs;
			case ECompare::Greater :	return lhs >  rhs;
			case ECompare::AnyBit :		return AnyBits( lhs, rhs );

			case ECompare::Unknown :	break;
		}
		switch_end
		return true;
	}

/*
=================================================
	_GetRepeatCount
=================================================
*/
	uint  IPass::_GetRepeatCount () const
	{
		if ( not _repeatCount )
			return 1;

		return _repeatCount->Get();
	}


} // AE::ResEditor
