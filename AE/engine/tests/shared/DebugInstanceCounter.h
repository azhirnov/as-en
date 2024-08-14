// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "UnitTest_Shared.h"


template <typename T, usize UID>
struct DebugInstanceCounter
{
	inline static slong		_counter		= 0;
	inline static slong		_emptyCtorCnt	= 0;
	inline static slong		_copyCtorCnt	= 0;
	inline static slong		_moveCtorCnt	= 0;
	inline static slong		_dtorCnt		= 0;

	slong					_magicNumber;
	T						value;

	DebugInstanceCounter () __NE___ :
		_magicNumber( _counter + (1ll << 62)), value{}
	{
		TEST( _magicNumber != 0 );
		++_counter;
		++_emptyCtorCnt;
	}

	explicit DebugInstanceCounter (const T &val) __NE___ :
		DebugInstanceCounter()
	{
		value = val;
	}

	DebugInstanceCounter (const DebugInstanceCounter &other) __NE___ :
		_magicNumber{ other._magicNumber }, value{ other.value }
	{
		++_counter;
		++_copyCtorCnt;
	}

	DebugInstanceCounter (DebugInstanceCounter &&other) __NE___ :
		_magicNumber{ other._magicNumber }, value{ RVRef(other.value) }
	{
		++_counter;
		++_moveCtorCnt;
	}

	~DebugInstanceCounter () __NE___
	{
		TEST( _magicNumber != 0 );	// assert on second dtor call
		--_counter;
		++_dtorCnt;
		_magicNumber = 0;
	}

	DebugInstanceCounter& operator = (const DebugInstanceCounter &rhs) __NE___
	{
		_magicNumber = rhs._magicNumber;
		value = rhs.value;
		return *this;
	}

	DebugInstanceCounter& operator = (DebugInstanceCounter &&rhs) __NE___
	{
		_magicNumber = rhs._magicNumber;
		value = RVRef( rhs.value );
		return *this;
	}

	bool operator == (const DebugInstanceCounter &rhs) C_NE___
	{
		return	value == rhs.value		and
				(_magicNumber != 0)		and
				(rhs._magicNumber != 0);
	}

	bool operator != (const DebugInstanceCounter &rhs) C_NE___
	{
		return	value != rhs.value		and
				(_magicNumber != 0)		and
				(rhs._magicNumber != 0);
	}

	bool operator < (const DebugInstanceCounter &rhs) C_NE___
	{
		return	value < rhs.value		and
				(_magicNumber != 0)		and
				(rhs._magicNumber != 0);
	}

	bool operator > (const DebugInstanceCounter &rhs) C_NE___
	{
		return	value > rhs.value		and
				(_magicNumber != 0)		and
				(rhs._magicNumber != 0);
	}


	static bool CheckStatistic () __NE___
	{
		String	str;
		str << "\nObjectID:    " << ToString( UID )
			<< "\nObj counter: " << ToString( _counter )		// objects which is not destroyed
			<< "\nEmpty ctors: " << ToString( _emptyCtorCnt )
			<< "\nCopy ctors:  " << ToString( _copyCtorCnt )
			<< "\nMove ctors:  " << ToString( _moveCtorCnt )
			<< "\nDtors:       " << ToString( _dtorCnt );

		AE_LOGI( str );
		return _counter == 0;
	}

	static void ClearStatistic () __NE___
	{
		_counter		= 0;
		_emptyCtorCnt	= 0;
		_copyCtorCnt	= 0;
		_moveCtorCnt	= 0;
		_dtorCnt		= 0;
	}

private:
	void*  operator & () const;
	void*  operator & ();
};


namespace std
{

	template <typename T, size_t UID>
	struct hash< DebugInstanceCounter<T,UID> >
	{
		ND_ size_t  operator () (const DebugInstanceCounter<T,UID> &value) C_NE___ {
			return hash<T>{}( value.value );
		}
	};

}	// std
