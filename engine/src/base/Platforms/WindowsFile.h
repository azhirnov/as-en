// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"
#include "base/Containers/UntypedStorage.h"
#include "base/Stream/Stream.h"
#include "base/Containers/NtStringView.h"
#include "base/Utils/FileSystem.h"

#ifdef AE_PLATFORM_WINDOWS

namespace AE::Base
{

	//
	// Windows Read-only File
	//
	class WindowsRFile final : public RStream
	{
	// types
	public:
		enum class EFlags
		{
			Unknown			= 0,
			RandomAccess	= 1 << 0,	// access is intended to be random
			SequentialScan	= 1 << 1,	// access is intended to be sequential from beginning to end
			NoBuffering		= 1 << 2,	// file or device is being opened with no system caching for data reads and writes
			Async			= 1 << 3,	// 
		};

		using ReadFinishedFn_t	= void (*) (void* userData, Bytes readn, Bytes offset, const void* buffer);

	private:
		using Handle_t		= UntypedStorage< sizeof(void*), alignof(void*) >;
		using Overlapped_t	= UntypedStorage< sizeof(void*)*4, alignof(void*) >;

		struct _AsyncReadHelper;

		struct PendingRead
		{
			Overlapped_t		overlapped;
			const void *		buffer		= null;
			void *				userData	= null;
			ReadFinishedFn_t	fn			= null;
		};


	// variables
	private:
		Handle_t		_file;
		Bytes			_fileSize;
		const EFlags	_flags;

		std::list<PendingRead>	_pending;


	// methods
	public:
		explicit WindowsRFile (NtStringView filename, EFlags flags = Default);
		explicit WindowsRFile (const char *filename, EFlags flags = Default);
		explicit WindowsRFile (const String &filename, EFlags flags = Default);
		
		explicit WindowsRFile (NtWStringView filename, EFlags flags = Default);
		explicit WindowsRFile (const wchar_t *filename, EFlags flags = Default);
		explicit WindowsRFile (const WString &filename, EFlags flags = Default);

		explicit WindowsRFile (const Path &path, EFlags flags = Default);

		~WindowsRFile () override;
		
		bool	SeekSet (Bytes pos) override;
		Bytes	ReadSeq (OUT void* buffer, Bytes size) override;

		bool	IsOpen ()	const override;
		Bytes	Position ()	const override;
		Bytes	Size ()		const override			{ return _fileSize; }
		
		EStreamType	GetStreamType () const override	{ return EStreamType::SequentialAccess | EStreamType::RandomAccess | EStreamType::FixedSize; }	// TODO

		ND_ bool	IsAsync ()			const		{ return AllBits( _flags, EFlags::Async ); }
		ND_ bool	HasPendingRead ()	const		{ ASSERT(not IsAsync());  return not _pending.empty(); }

			bool	ReadAsync (Bytes offset, OUT void* buffer, Bytes size, void* userData, ReadFinishedFn_t fn);

	private:
		ND_ Bytes  _GetSize () const;
	};

	AE_BIT_OPERATORS( WindowsRFile::EFlags );

	

	//
	// Windows Write-only File
	//
	class WindowsWFile final : public WStream
	{
	// types
	public:
		enum class EFlags
		{
			Unknown			= 0,
			NoBuffering		= 1 << 0,	// file or device is being opened with no system caching for data reads and writes
			Async			= 1 << 1,	// 
		};
		
		using WriteFinishedFn_t	= void (*) (void* userData, Bytes readn, Bytes offset);

	private:
		using Handle_t		= UntypedStorage< sizeof(void*), alignof(void*) >;
		using Overlapped_t	= UntypedStorage< sizeof(void*)*4, alignof(void*) >;

		struct _AsyncWriteHelper;

		struct PendingWrite
		{
			Overlapped_t		overlapped;
			void *				userData	= null;
			WriteFinishedFn_t	fn			= null;
		};
		

	// variables
	private:
		Handle_t		_file;
		const EFlags	_flags;
		
		std::list<PendingWrite>	_pending;


	// methods
	public:
		explicit WindowsWFile (NtStringView filename, EFlags flags = Default);
		explicit WindowsWFile (const char *filename, EFlags flags = Default);
		explicit WindowsWFile (const String &filename, EFlags flags = Default);

		explicit WindowsWFile (NtWStringView filename, EFlags flags = Default);
		explicit WindowsWFile (const wchar_t *filename, EFlags flags = Default);
		explicit WindowsWFile (const WString &filename, EFlags flags = Default);

		explicit WindowsWFile (const Path &path, EFlags flags = Default);

		~WindowsWFile () override;

		bool	IsOpen ()	const override;
		Bytes	Position ()	const override;
		Bytes	Size ()		const override;
		
		bool	SeekSet (Bytes pos) override;
		Bytes	Write2 (const void *buffer, Bytes size) override;
		void	Flush () override;
			
		EStreamType	GetStreamType () const override	{ return EStreamType::SequentialAccess | EStreamType::RandomAccess; }	// TODO

		ND_ bool	IsAsync ()			const		{ return AllBits( _flags, EFlags::Async ); }
		ND_ bool	HasPendingRead ()	const		{ ASSERT(not IsAsync());  return not _pending.empty(); }

			bool	WriteAsync (const void* buffer, Bytes size, void* userData, WriteFinishedFn_t fn);
	};

	AE_BIT_OPERATORS( WindowsWFile::EFlags );

} // AE::Base

#endif // AE_PLATFORM_WINDOWS
