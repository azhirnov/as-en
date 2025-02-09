// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_hl/UI/Screen.h"
#include "graphics_hl/UI/StyleCollection.h"

namespace AE::UI { class SystemImpl; }
namespace AE { UI::SystemImpl&  UISystem () __NE___; }

namespace AE::UI
{

	//
	// UI System
	//

	class SystemImpl final
	{
		friend struct InPlace<SystemImpl>;

	// types
	public:
		using StyleCollectionCI			= StyleCollection::CreateInfo;
		using StyleCollectionAsyncCI	= StyleCollection::CreateInfoAsync;

		class ScreenApi
		{
			friend class Screen;
			static void  Recycle (uint indexInPool) __NE___;
		};

		class InstanceCtor {
		public:
			ND_ static bool			Create (const StyleCollectionCI &)	__NE___;
			ND_ static AsyncTask	Create (StyleCollectionAsyncCI &)	__NE___;
				static void			Destroy ()							__NE___;
		};

	private:
		static constexpr uint	_MaxScreens	= StyleCollection::MaxScreens;
		using ScreenPool_t		= Threading::LfStaticIndexedPool< Screen, uint, _MaxScreens >;


	// variables
	private:
		StyleCollection			_styleCollection;
		ScreenPool_t			_screenPool;

		DRC_ONLY(
			RWDataRaceCheck		_drCheck;
		)


	// methods
	public:
		ND_ RC<Screen>  CreateScreen ()								__NE___;

		ND_ StyleCollection const&			GetStyleCollection ()	C_NE___	{ return _styleCollection; }
		ND_ IRenderTechPipelines&			GetRenderTech ()		C_NE___	{ return *_styleCollection._rtech; }


	private:
		SystemImpl ()												__NE___;
		~SystemImpl ()												__NE___;

		ND_ bool		_Initialize (const StyleCollectionCI &ci)	__NE___;
		ND_ AsyncTask	_Initialize (StyleCollectionAsyncCI &ci)	__NE___;

		friend SystemImpl&		AE::UISystem ()						__NE___;
		ND_ static SystemImpl&  _Instance ()						__NE___;
	};

/*
=================================================
	UIStyleCollection
=================================================
*/
	Nd__In StyleCollection const&  UIStyleCollection () __NE___
	{
		return UISystem().GetStyleCollection();
	}
//-----------------------------------------------------------------------------



	//
	// UI System Scope (helper)
	//
	struct SystemScope
	{
	private:
		bool	_isCreated = false;

	public:
		~SystemScope ()									__NE___
		{
			Destroy();
		}

		ND_ bool  Create (const StyleCollection::CreateInfo &ci) __NE___
		{
			CHECK_ERR( not _isCreated );
			_isCreated = true;
			return SystemImpl::InstanceCtor::Create( ci );
		}

		ND_ AsyncTask  Create (StyleCollection::CreateInfoAsync &ci) __NE___
		{
			CHECK_ERR( not _isCreated );
			_isCreated = true;
			return SystemImpl::InstanceCtor::Create( ci );
		}

		void  Destroy ()								__NE___
		{
			if ( _isCreated )
			{
				_isCreated = false;
				SystemImpl::InstanceCtor::Destroy();
			}
		}

		ND_ SystemImpl*			operator -> ()			__NE___	{ ASSERT( _isCreated );  return &UISystem(); }
		ND_ SystemImpl const*	operator -> ()			C_NE___	{ ASSERT( _isCreated );  return &UISystem(); }
		ND_ bool				IsCreated ()			C_NE___	{ return _isCreated; }
	};


} // AE::UI


namespace AE
{
/*
=================================================
	UISystem
=================================================
*/
	inline UI::SystemImpl&  UISystem () __NE___
	{
		return UI::SystemImpl::_Instance();
	}

} // AE
