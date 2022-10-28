// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Stream/Stream.h"
#include "threading/Common.h"

namespace AE::Threading
{
	
	//
	// Thread safe Read-only Stream
	//
	
	template <typename T>
	class ThreadSafeRStream final : public RStream
	{
	// variables
	private:
		mutable Mutex	_guard;
		T				_stream;


	// methods
	public:
		template <typename ...Args>
		explicit ThreadSafeRStream (Args && ...args) : _stream{FwdArg<Args>(args)...} {}

		~ThreadSafeRStream () override {}

		bool		IsOpen ()		const override	{ EXLOCK( _guard );  return _stream->IsOpen(); }
		Bytes		Size ()			const override	{ EXLOCK( _guard );  return _stream->Size(); }
		
		EStreamType	GetStreamType ()const override	{ EXLOCK( _guard );  return EStreamType::RandomAccess | EStreamType::ThreadSafe; }
		
		Bytes  Position () const override
		{
			DBG_WARNING( "not thread-safe, use ReadRnd() instead" );
			EXLOCK( _guard );
			return _stream->Position();
		}

		bool  SeekSet (Bytes pos) override
		{
			DBG_WARNING( "not thread-safe, use ReadRnd() instead" );
			EXLOCK( _guard );
			return _stream->SeekSet( pos );
		}

		Bytes  ReadSeq (OUT void *buffer, Bytes size) override
		{
			DBG_WARNING( "not thread-safe, use ReadRnd() instead" );
			EXLOCK( _guard );
			return _stream->ReadSeq( OUT buffer, size );
		}
		
		Bytes  ReadRnd (Bytes offset, OUT void *buffer, Bytes size) override
		{
			EXLOCK( _guard );
			return _stream->ReadRnd( offset, OUT buffer, size );
		}
	};

	

	//
	// Thread safe Write-only Stream
	//
	
	template <typename T>
	class ThreadSafeWStream : public WStream
	{
	// variables
	private:
		mutable Mutex	_guard;
		T				_stream;


	// methods
	public:
		explicit ThreadSafeWStream (T &stream) : _stream{stream} {}
		~ThreadSafeWStream () override {}

		bool		IsOpen ()		const override	{ EXLOCK( _guard );  return _stream->IsOpen(); }
		Bytes		Position ()		const override	{ EXLOCK( _guard );  return _stream->Position(); }
		Bytes		Size ()			const override	{ EXLOCK( _guard );  return _stream->Size(); }

		EStreamType	GetStreamType ()const override	{ EXLOCK( _guard );  return EStreamType::RandomAccess | EStreamType::ThreadSafe; }

		bool	SeekSet (Bytes pos) override
		{
			EXLOCK( _guard );
			return _stream->SeekSet( pos );
		}

		Bytes	Write2 (const void *buffer, Bytes size) override
		{
			EXLOCK( _guard );
			return _stream->Write2( buffer, size );
		}

		void	Flush () override
		{
			EXLOCK( _guard );
			return _stream->Flush();
		}
	};


} // AE::Threading
