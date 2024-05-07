// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Containers/UntypedStorage.h"
#include "base/DataSource/DataStream.h"
#include "base/Containers/NtStringView.h"
#include "base/FileSystem/Path.h"

#ifdef AE_PLATFORM_UNIX_BASED

namespace AE::Base
{

	//
	// Unix Read-only File Stream
	//
	class UnixFileRStream final : public RStream
	{
	// types
	public:
		enum class EMode : uint
		{
			Unknown			= 0,
			RandomAccess	= 1 << 0,	// access is intended to be random
			SequentialScan	= 1 << 1,	// access is intended to be sequential from beginning to end
			Direct			= 1 << 2,	// Try to minimize cache effects of the I/O to and from this file.

			Unix_LargeFile	= 1 << 16,	// 64 bit address
		};

	private:
		using Handle_t	= int;	// fd

		static constexpr EMode	c_DefaultMode = EMode::SequentialScan;


	// variables
	private:
		Handle_t		_file		= -1;
		const Bytes		_fileSize;

		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		UnixFileRStream (Handle_t file DEBUG_ONLY(, Path filename))						__NE___;

	public:
		explicit UnixFileRStream (const char* filename, EMode mode = c_DefaultMode)		__NE___;
		explicit UnixFileRStream (NtStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixFileRStream (const String &filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixFileRStream (const Path &path, EMode mode = c_DefaultMode)			__NE___;

		~UnixFileRStream ()																__NE_OV;


		// RStream //
		bool		IsOpen ()															C_NE_OV	{ return _file >= 0; }
		ESourceType	GetSourceType ()													C_NE_OV;
		PosAndSize	PositionAndSize ()													C_NE_OV	{ return { _Position(), _fileSize }; }

		bool		SeekFwd (Bytes offset)												__NE_OV;
		bool		SeekSet (Bytes newPos)												__NE_OV;

		Bytes		ReadSeq (OUT void*, Bytes)											__NE_OV;
		bool		Prefetch (Bytes offset, Bytes size)									__NE_OV;

	private:
		ND_ Bytes  _Position ()															C_NE___;
	};

	AE_BIT_OPERATORS( UnixFileRStream::EMode );



	//
	// Unix Write-only File Stream
	//
	class UnixFileWStream final : public WStream
	{
	// types
	public:
		enum class EMode : uint
		{
			Unknown			= 0,
			Direct			= 1 << 0,	// Try to minimize cache effects of the I/O to and from this file.

			OpenRewrite		= 0,		// create new or discard previous file	// default
			OpenUpdate		= 1 << 3,	// keep previous content and update some parts in the file
			OpenAppend		= 1 << 4,	// write operations will not overwrite existing data

			SharedRead		= 1 << 5,	// other process can read file

			Unix_LargeFile	= 1 << 16,	// 64 bit address
		};

	private:
		using Handle_t	= int;	// fd

		static constexpr EMode	c_DefaultMode = EMode::Unknown;


	// variables
	private:
		Handle_t		_file	= -1;

		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		UnixFileWStream (Handle_t file DEBUG_ONLY(, Path filename))						__NE___;

	public:
		explicit UnixFileWStream (const char*  filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixFileWStream (NtStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixFileWStream (const String &filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixFileWStream (const Path &path, EMode mode = c_DefaultMode)			__NE___;

		~UnixFileWStream ()																__NE_OV;


		// WStream //
		bool		IsOpen ()															C_NE_OV	{ return _file >= 0; }
		Bytes		Position ()															C_NE_OV;
		ESourceType	GetSourceType ()													C_NE_OV;

		bool		SeekFwd (Bytes offset)												__NE_OV;
		Bytes		Reserve (Bytes additionalSize)										__NE_OV;

		Bytes		WriteSeq (const void*, Bytes)										__NE_OV;
		void		Flush ()															__NE_OV;
	};

	AE_BIT_OPERATORS( UnixFileWStream::EMode );
//-----------------------------------------------------------------------------



	//
	// Unix Read-only File Data Source
	//
	class UnixFileRDataSource final : public RDataSource
	{
	// types
	public:
		using EMode = UnixFileRStream::EMode;

	private:
		using Handle_t	= int;	// fd

		static constexpr EMode	c_DefaultMode = EMode::RandomAccess;


	// variables
	private:
		Handle_t		_file		= -1;
		const Bytes 	_fileSize;

		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		UnixFileRDataSource (Handle_t file DEBUG_ONLY(, Path filename))						__NE___;

	public:
		explicit UnixFileRDataSource (const char* filename, EMode mode = c_DefaultMode)		__NE___;
		explicit UnixFileRDataSource (NtStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixFileRDataSource (const String &filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixFileRDataSource (const Path &path, EMode mode = c_DefaultMode)			__NE___;

		~UnixFileRDataSource ()																__NE_OV;


		// RDataSource //
		bool		IsOpen ()																C_NE_OV	{ return _file >= 0; }
		ESourceType	GetSourceType ()														C_NE_OV;
		Bytes		Size ()																	C_NE_OV	{ return _fileSize; }

		Bytes		ReadBlock (Bytes, OUT void*, Bytes)										__NE_OV;
	};



	//
	// Unix Write-only File Data Source
	//
	class UnixFileWDataSource final : public WDataSource
	{
	// types
	public:
		using EMode = UnixFileWStream::EMode;

	private:
		using Handle_t	= int;	// fd

		static constexpr EMode	c_DefaultMode = EMode::Unknown;


	// variables
	private:
		Handle_t		_file	= -1;

		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		UnixFileWDataSource (Handle_t file DEBUG_ONLY(, Path filename))						__NE___;

	public:
		explicit UnixFileWDataSource (const char* filename, EMode mode = c_DefaultMode)		__NE___;
		explicit UnixFileWDataSource (NtStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixFileWDataSource (const String &filename, EMode mode = c_DefaultMode)	__NE___;
		explicit UnixFileWDataSource (const Path &path, EMode mode = c_DefaultMode)			__NE___;

		~UnixFileWDataSource ()																__NE_OV;


		// WStream //
		bool		IsOpen ()																C_NE_OV	{ return _file >= 0; }
		ESourceType	GetSourceType ()														C_NE_OV;
		Bytes		Capacity ()																C_NE_OV;

		Bytes		Reserve (Bytes)															__NE_OV;

		Bytes		WriteBlock (Bytes, const void*, Bytes)									__NE_OV;
		void		Flush ()																__NE_OV;
	};


} // AE::Base

#endif // AE_PLATFORM_UNIX_BASED
