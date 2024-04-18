// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/DataSource/AsyncDataSource.h"

namespace AE::Threading
{

	//
	// Read-only Data Source (Sync on top of Async Stream)
	//
	class SyncRDataSource final : public RDataSource
	{
	// variables
	private:
		RC<AsyncRDataSource>	_ds;

	// methods
	public:
		explicit SyncRDataSource (RC<AsyncRDataSource> ds)					__NE___	: _ds{RVRef(ds)} {}

		// RDataSource //
		bool		IsOpen ()												C_NE_OV	{ return _ds and _ds->IsOpen(); }
		ESourceType	GetSourceType ()										C_NE_OV	{ return _ds->GetSourceType() & ~(ESourceType::Async | ESourceType::Buffered); }
		Bytes		Size ()													C_NE_OV	{ return _ds->Size(); }
		Bytes		ReadBlock (Bytes pos, OUT void* buffer, Bytes size)		__NE_OV;
	};



	//
	// Write-only Data Source (Sync on top of Async Stream)
	//
	class SyncWDataSource final : public WDataSource
	{
	// variables
	private:
		RC<AsyncWDataSource>	_ds;

	// methods
	public:
		explicit SyncWDataSource (RC<AsyncWDataSource> ds)					__NE___ : _ds{RVRef(ds)} {}

		// WDataSource //
		bool		IsOpen ()												C_NE_OV	{ return _ds and _ds->IsOpen(); }
		ESourceType	GetSourceType ()										C_NE_OV	{ return _ds->GetSourceType() & ~(ESourceType::Async | ESourceType::Buffered); }
		Bytes		WriteBlock (Bytes pos, const void* buffer, Bytes size)	__NE_OV;
		void		Flush ()												__NE_OV	{}
	};
//-----------------------------------------------------------------------------



	//
	// Read-only Stream (Sync on top of Async Stream)
	//
	class SyncRStream final : public RStream
	{
	// variables
	private:
		RC<AsyncRStream>	_stream;

	// methods
	public:
		explicit SyncRStream (RC<AsyncRStream> stream)							__NE___	: _stream{RVRef(stream)} {}

		// RStream //
		bool		IsOpen ()													C_NE_OV	{ return _stream and _stream->IsOpen(); }
		ESourceType	GetSourceType ()											C_NE_OV	{ return _stream->GetSourceType() & ~(ESourceType::Async | ESourceType::Buffered); }
		PosAndSize	PositionAndSize ()											C_NE_OV	{ return _stream->PositionAndSize(); }

		bool		SeekFwd (Bytes)												__NE_OV	{ return false; }
		Bytes		ReadSeq (OUT void* buffer, Bytes size)						__NE_OV;
		bool		SeekSet (Bytes)												__NE_OV	{ return false; }
	};



	//
	// Write-only Stream (Sync on top of Async Stream)
	//
	class SyncWStream final : public WStream
	{
	// variables
	private:
		RC<AsyncWStream>	_stream;

	// methods
	public:
		explicit SyncWStream (RC<AsyncWStream> stream)							__NE___ : _stream{RVRef(stream)} {}

		// WStream //
		bool		IsOpen ()													C_NE_OV	{ return _stream and _stream->IsOpen(); }
		ESourceType	GetSourceType ()											C_NE_OV	{ return _stream->GetSourceType() & ~(ESourceType::Async | ESourceType::Buffered); }

		Bytes		Position ()													C_NE_OV	{ return _stream->Position(); }
		bool		SeekFwd (Bytes)												__NE_OV	{ return false; }
		Bytes		WriteSeq (const void* buffer, Bytes size)					__NE_OV;
		void		Flush ()													__NE_OV	{}
	};
//-----------------------------------------------------------------------------



	//
	// Read-only Stream (Sync on top of Async Data Source)
	//
	class SyncRStreamOnAsyncDS final : public RStream
	{
	// variables
	private:
		AtomicByte< Bytes >		_pos;
		RC<AsyncRDataSource>	_ds;

	// methods
	public:
		explicit SyncRStreamOnAsyncDS (RC<AsyncRDataSource> ds, Bytes offset = 0_b)	__NE___	: _pos{offset}, _ds{RVRef(ds)} {}

		// RStream //
		bool		IsOpen ()														C_NE_OV	{ return _ds and _ds->IsOpen(); }
		ESourceType	GetSourceType ()												C_NE_OV	{ return _ds->GetSourceType() & ~(ESourceType::Async | ESourceType::Buffered); }
		PosAndSize	PositionAndSize ()												C_NE_OV	{ return PosAndSize{ _pos.load(), _ds->Size() }; }

		bool		SeekFwd (Bytes offset)											__NE_OV;
		Bytes		ReadSeq (OUT void* buffer, Bytes size)							__NE_OV;
		bool		SeekSet (Bytes newPos)											__NE_OV;
	};



	//
	// Write-only Stream (Sync on top of Async Data Source)
	//
	class SyncWStreamOnAsyncDS final : public WStream
	{
	// variables
	private:
		AtomicByte< Bytes >		_pos;
		RC<AsyncWDataSource>	_ds;

	// methods
	public:
		explicit SyncWStreamOnAsyncDS (RC<AsyncWDataSource> ds, Bytes offset = 0_b)	__NE___ : _pos{offset}, _ds{RVRef(ds)} {}

		// WStream //
		bool		IsOpen ()														C_NE_OV	{ return _ds and _ds->IsOpen(); }
		ESourceType	GetSourceType ()												C_NE_OV	{ return _ds->GetSourceType() & ~(ESourceType::Async | ESourceType::Buffered); }

		Bytes		Position ()														C_NE_OV	{ return _pos.load(); }
		bool		SeekFwd (Bytes offset)											__NE_OV;
		Bytes		WriteSeq (const void* buffer, Bytes size)						__NE_OV;
		void		Flush ()														__NE_OV	{}
	};
//-----------------------------------------------------------------------------



/*
=================================================
	SeekFwd
=================================================
*/
	inline bool  SyncRStreamOnAsyncDS::SeekFwd (Bytes offset) __NE___
	{
		Bytes	pos = _pos.fetch_add( offset );
		return pos <= _ds->Size();
	}

/*
=================================================
	SeekSet
=================================================
*/
	inline bool  SyncRStreamOnAsyncDS::SeekSet (Bytes newPos) __NE___
	{
		if ( newPos <= _ds->Size() )
		{
			_pos.store( newPos );
			return true;
		}
		return false;
	}

/*
=================================================
	SeekFwd
=================================================
*/
	inline bool  SyncWStreamOnAsyncDS::SeekFwd (Bytes offset) __NE___
	{
		_pos.fetch_add( offset );
		return true;
	}


} // AE::Threading
