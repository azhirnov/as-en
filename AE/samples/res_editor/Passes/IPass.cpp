// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Resources/Buffer.h"
#include "Resources/BufferView.h"
#include "Resources/RTScene.h"
#include "Resources/Image.h"
#include "Resources/VideoImage.h"
#include "Resources/VideoImage2.h"
#include "Controllers/IController.h"
#include "Core/EditorUI.h"
#include "_data/cpp/types.h"

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
					if constexpr( IsSame< T, DynamicDim >)
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
	ResourceArray
=================================================
*/
	ResourceArray::ResourceArray ()
	{}

	ResourceArray::~ResourceArray ()
	{}
//-----------------------------------------------------------------------------


/*
=================================================
	Debugger::IsEnabled
=================================================
*/
	bool  IPass::Debugger::IsEnabled (const IPass* pass) const
	{
		return	IsEnabled()		and
				(pass == target or target == IPass::c_DebugAllTargets);
	}
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

/*
=================================================
	_ReadTimeQuery
=================================================
*/
	void  IPass::_ReadTimeQuery (FrameUID frameId)
	{
		if ( not _passTime )
			return;

		auto&	qm		= GraphicsScheduler().GetQueryManager();
		auto&	query	= _timeQuery[ frameId.Index() ];
		uint	cnt		= _GetRepeatCount();

		if ( not query )
			return;

		StaticArray< nanosecondsd, 2 >	time = {};
		qm.GetTimestamp( query, OUT time.data(), Sizeof(time) );

		query = Default;

		_passTime->Set( float( secondsd{ time[1] - time[0] }.count() / double(cnt) ));
	}

/*
=================================================
	_UpdateComputeUB
=================================================
*/
	void  IPass::_UpdateComputeUB (INOUT AnyTypeRef ub, const UpdatePassData &pd) C_NE___
	{
		CHECK_ERRV( ub.Is< ShaderTypes::ComputePassUB >());

		auto&	ub_data		= ub.As<ShaderTypes::ComputePassUB>();
		ub_data.time		= pd.totalTime.count();
		ub_data.timeDelta	= pd.frameTime.count();
		ub_data.frame		= pd.frameId;
		ub_data.passFrameId	= _dynData.frame;
		ub_data.seed		= pd.seed;
		ub_data.mouse		= float4{ pd.unormCursorPos.x, pd.unormCursorPos.y, float(pd.pressed), 0.f };
		ub_data.customKeys	= float2{ pd.customKeys[0], pd.customKeys[1] };
		ub_data.pixPerMm	= pd.pixPerMm;
		ub_data.mmPerPix	= pd.mmPerPix;

		if ( _controller )
			_controller->CopyTo( OUT ub_data.camera );

		_CopySliders( OUT ub_data.floatSliders, OUT ub_data.intSliders, OUT ub_data.colors );
		_CopyConstants( _shConst, OUT ub_data.floatConst, OUT ub_data.intConst );

		if ( _dynData.prevFrame != pd.frameId )
		{
			++_dynData.frame;
			_dynData.prevFrame = pd.frameId;
		}
	}

} // AE::ResEditor
