// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "UnitTest_Common.h"

namespace
{
	enum class Event
	{
		Lock,
		Unlock,

		LockShared,
		UnlockShared,

		ObjCopyCtor,
		ObjSet,
		ObjGet,
	};

	static InPlace< Array<Event> >	s_Events;


	struct DbgMutex
	{
	private:
		bool	_locked = false;

	public:
			void  lock ()				{ TEST( not _locked );	_locked = true;		s_Events->push_back( Event::Lock ); }
			void  unlock ()				{ TEST( _locked );		_locked = false;	s_Events->push_back( Event::Unlock ); }
		ND_ bool  try_lock ()			{ if ( not _locked ) { lock(); return true; }  return false; }

			void  lock_shared ()		{ TEST( not _locked );	_locked = true;		s_Events->push_back( Event::LockShared ); }
			void  unlock_shared ()		{ TEST( _locked );		_locked = false;	s_Events->push_back( Event::UnlockShared ); }
		ND_ bool  try_lock_shared ()	{ if ( not _locked ) { lock_shared(); return true; }  return false; }
	};


	struct ProtectedObject
	{
	private:
		int		_value	= 0;

	public:
		ProtectedObject ()										__NE___	{}
		explicit ProtectedObject (int val)						__NE___ : _value{val}	{}

		ProtectedObject (const ProtectedObject &other)			__NE___	: _value{other._value}	{ s_Events->push_back( Event::ObjCopyCtor ); }

		ProtectedObject&  operator = (const ProtectedObject &)			= delete;
		ProtectedObject&  operator = (ProtectedObject &&)				= delete;

			void	Set (int val)								__NE___	{ _value = val;  s_Events->push_back( Event::ObjSet ); }
		ND_ int		Get ()										C_NE___	{ s_Events->push_back( Event::ObjGet );  return _value; }
	};


	static void  CmpEvents (ArrayView<Event> ref)
	{
		TEST( ref == *s_Events );
		s_Events->clear();
	}
//-----------------------------------------------------------------------------



	static void  Synchronized_Test1 ()
	{
		s_Events.Create();
		{
			Synchronized< DbgMutex, ProtectedObject >	obj;

			{
				auto	w = obj.WriteNoLock();
				EXLOCK( w );
			}
			CmpEvents({ Event::Lock, Event::Unlock });

			{
				auto	r = obj.ReadNoLock();
				SHAREDLOCK( r );
			}
			CmpEvents({ Event::LockShared, Event::UnlockShared });

			obj.Write( ProtectedObject{ 1 });
			CmpEvents({ Event::Lock, Event::ObjCopyCtor, Event::Unlock });

			{
				auto	o = obj.Read();
				TEST( o.Get() == 1 );
				CmpEvents({ Event::LockShared, Event::ObjCopyCtor, Event::UnlockShared, Event::ObjGet });
			}

			obj->Set( 2 );
			CmpEvents({ Event::Lock, Event::ObjSet, Event::Unlock });

			TEST( obj->Get() == 2 );
			CmpEvents({ Event::Lock, Event::ObjGet, Event::Unlock });
		}
		s_Events.Destroy();
	}
}


extern void UnitTest_Synchronized ()
{
	Synchronized_Test1();

	TEST_PASSED();
}
