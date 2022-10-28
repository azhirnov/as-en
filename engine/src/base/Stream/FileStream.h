// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Stream/Stream.h"
#include "base/Containers/NtStringView.h"
#include "base/Utils/FileSystem.h"

namespace AE::Base
{

	//
	// Read-only File Stream
	//

	class FileRStream final : public RStream
	{
	// variables
	private:
		FILE*		_file	= null;
		Bytes		_fileSize;
		Bytes		_position;

		DEBUG_ONLY( const Path  _filename;)


	// methods
	public:
		explicit FileRStream (NtStringView filename);
		explicit FileRStream (const char* filename);
		explicit FileRStream (const String &filename);
		explicit FileRStream (const Path &path);
		
	#ifdef AE_PLATFORM_WINDOWS
		explicit FileRStream (NtWStringView filename);
		explicit FileRStream (const wchar_t *filename);
		explicit FileRStream (const WString &filename);
	#endif
		
		FileRStream () {}
		~FileRStream () override;

		bool	IsOpen ()	const override				{ return _file != null; }
		Bytes	Position ()	const override				{ return _position; }
		Bytes	Size ()		const override				{ return _fileSize; }
		
		EStreamType	GetStreamType () const override		{ return EStreamType::SequentialAccess | EStreamType::RandomAccess | EStreamType::FixedSize; }

		bool	SeekSet (Bytes pos) override;
		Bytes	ReadSeq (OUT void *buffer, Bytes size) override;

	private:
		ND_ Bytes  _GetSize () const;
	};



	//
	// Write-only File Stream
	//

	class FileWStream final : public WStream
	{
	// types
	public:
		enum class EMode
		{
			Rewrite,	// create new or discard previous file
			Update,		// keep previous content and update some parts in the file
			Append,		// write data to the end of the file
		};


	// variables
	private:
		FILE*		_file	= null;
		
		DEBUG_ONLY( const Path  _filename;)


	// methods
	public:
		explicit FileWStream (NtStringView filename, EMode mode = EMode::Rewrite);
		explicit FileWStream (const char* filename, EMode mode = EMode::Rewrite);
		explicit FileWStream (const String &filename, EMode mode = EMode::Rewrite);
		explicit FileWStream (const Path &path, EMode mode = EMode::Rewrite);

	#ifdef AE_PLATFORM_WINDOWS
		explicit FileWStream (NtWStringView filename, EMode mode = EMode::Rewrite);
		explicit FileWStream (const wchar_t *filename, EMode mode = EMode::Rewrite);
		explicit FileWStream (const WString &filename, EMode mode = EMode::Rewrite);
	#endif

		FileWStream () {}
		~FileWStream () override;
		
		bool	IsOpen ()	const override				{ return _file != null; }
		Bytes	Position ()	const override;
		Bytes	Size ()		const override;
		
		EStreamType	GetStreamType () const override		{ return EStreamType::SequentialAccess | EStreamType::RandomAccess; }

		bool	SeekSet (Bytes pos) override;
		Bytes	Write2 (const void *buffer, Bytes size) override;
		void	Flush () override;
	};

} // AE::Base


// check definitions
#ifdef AE_CPP_DETECT_MISMATCH

# ifdef _FILE_OFFSET_BITS
#  if _FILE_OFFSET_BITS == 64
#	pragma detect_mismatch( "_FILE_OFFSET_BITS", "64" )
#  else
#	pragma detect_mismatch( "_FILE_OFFSET_BITS", "32" )
#  endif
# endif

#endif // AE_CPP_DETECT_MISMATCH
