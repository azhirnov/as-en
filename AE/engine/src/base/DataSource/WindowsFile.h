// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Memory/UntypedStorage.h"
#include "base/DataSource/DataStream.h"
#include "base/Containers/NtStringView.h"

#ifdef AE_PLATFORM_WINDOWS

namespace AE::Base
{

	//
	// Windows Read-only File Stream
	//
	class WinFileRStream final : public RStream
	{
	// types
	public:
		enum class EMode : uint
		{
			RandomAccess	= 1 << 0,	// access is intended to be random.
			SequentialScan	= 1 << 1,	// access is intended to be sequential from beginning to end.
			Win_NoBuffering	= 1 << 2,	// file or device is being opened with no system caching for data reads and writes,
										// read/write size must be multiple of 'DirectAccessAlign()'.
			Direct			= Win_NoBuffering,

			Unknown			= 0,
			_BITOPS_
		};

	private:
		using Handle_t	= UntypedStorage< sizeof(void*), alignof(void*) >;

		static constexpr EMode	c_DefaultMode = EMode::SequentialScan;


	// variables
	private:
		Handle_t		_file;
		const Bytes		_fileSize;
		const ReqAlign	_align;

		DEBUG_ONLY(
			const Path  _dbgFilename;
			const EMode	_dbgMode;
		)


	// methods
	private:
		WinFileRStream (const Handle_t &file, EMode mode DEBUG_ONLY(, Path filename))	__NE___;

	public:
		explicit WinFileRStream (const char* filename, EMode mode = c_DefaultMode)		__NE___;
		explicit WinFileRStream (NtStringView filename, EMode mode = c_DefaultMode)		__NE___;
		explicit WinFileRStream (const String &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit WinFileRStream (NtWStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinFileRStream (const wchar_t* filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinFileRStream (const WString &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit WinFileRStream (const Path &path, EMode mode = c_DefaultMode)			__NE___;

		~WinFileRStream ()																__NE_OV;


		// RStream //
		bool		IsOpen ()															C_NE_OV;
		ESourceType	GetSourceType ()													C_NE_OV;
		PosAndSize	PositionAndSize ()													C_NE_OV	{ return { _Position(), _fileSize }; }

		bool		SeekFwd (Bytes offset)												__NE_OV;
		bool		SeekSet (Bytes newPos)												__NE_OV;

		Bytes		ReadSeq (OUT void*, Bytes)											__NE_OV;

		ReqAlign	DirectAccessAlign ()												C_NE_OV	{ return _align; }

	private:
		ND_ Bytes  _Position ()															C_NE___;
	};



	//
	// Windows Write-only File Stream
	//
	class WinFileWStream final : public WStream
	{
	// types
	public:
		enum class EMode : uint
		{
			Win_NoBuffering	= 1 << 0,	// file or device is being opened with no system caching for data reads and writes,
										// read/write size must be multiple of 'DirectAccessAlign()'.
			Win_NoCaching	= 1 << 1,	// write operations will not go through any intermediate cache, they will go directly to disk.
			Direct			= Win_NoBuffering | Win_NoCaching,

			OpenRewrite		= 0,		// create new or discard previous file.						// default
			OpenUpdate		= 1 << 3,	// keep previous content and update some parts in the file.
			OpenAppend		= 1 << 4,	// write operations will not overwrite existing data.

			SharedRead		= 1 << 5,	// other process can read file.

			Unknown			= 0,
			_BITOPS_
		};

	private:
		using Handle_t	= UntypedStorage< sizeof(void*), alignof(void*) >;

		static constexpr EMode	c_DefaultMode = EMode::SharedRead;


	// variables
	private:
		Handle_t		_file;
		const ReqAlign	_align;

		DEBUG_ONLY(
			const Path  _dbgFilename;
			const EMode	_dbgMode;
		)


	// methods
	private:
		WinFileWStream (const Handle_t &file, EMode mode DEBUG_ONLY(, Path filename))	__NE___;

	public:
		explicit WinFileWStream (const char*  filename, EMode mode = c_DefaultMode)		__NE___;
		explicit WinFileWStream (NtStringView filename, EMode mode = c_DefaultMode)		__NE___;
		explicit WinFileWStream (const String &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit WinFileWStream (NtWStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinFileWStream (const wchar_t* filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinFileWStream (const WString &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit WinFileWStream (const Path &path, EMode mode = c_DefaultMode)			__NE___;

		~WinFileWStream ()																__NE_OV;


		// WStream //
		bool		IsOpen ()															C_NE_OV;
		Bytes		Position ()															C_NE_OV;
		ESourceType	GetSourceType ()													C_NE_OV;

		bool		SeekFwd (Bytes offset)												__NE_OV;
		bool		UpdateAt (Bytes pos)												__NE_OV;
		Bytes		Reserve (Bytes additionalSize)										__NE_OV;

		Bytes		WriteSeq (const void*, Bytes)										__NE_OV;
		void		Flush ()															__NE_OV;

		ReqAlign	DirectAccessAlign ()												C_NE_OV	{ return _align; }
	};
//-----------------------------------------------------------------------------



	//
	// Windows Read-only File Data Source
	//
	class WinFileRDataSource final : public RDataSource
	{
	// types
	public:
		using EMode = WinFileRStream::EMode;

	private:
		using Handle_t	= UntypedStorage< sizeof(void*), alignof(void*) >;

		static constexpr EMode	c_DefaultMode = EMode::RandomAccess;


	// variables
	private:
		Handle_t		_file;
		const Bytes 	_fileSize;
		const ReqAlign	_align;

		DEBUG_ONLY(
			const Path  _dbgFilename;
			const EMode	_dbgMode;
		)


	// methods
	private:
		WinFileRDataSource (const Handle_t &file, EMode mode DEBUG_ONLY(, Path filename))	__NE___;

	public:
		explicit WinFileRDataSource (const char* filename, EMode mode = c_DefaultMode)		__NE___;
		explicit WinFileRDataSource (NtStringView filename, EMode mode = c_DefaultMode)		__NE___;
		explicit WinFileRDataSource (const String &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit WinFileRDataSource (NtWStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinFileRDataSource (const wchar_t* filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinFileRDataSource (const WString &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit WinFileRDataSource (const Path &path, EMode mode = c_DefaultMode)			__NE___;

		~WinFileRDataSource ()																__NE_OV;


		// RDataSource //
		bool		IsOpen ()																C_NE_OV;
		ESourceType	GetSourceType ()														C_NE_OV;
		Bytes		Size ()																	C_NE_OV	{ return _fileSize; }

		Bytes		ReadBlock (Bytes, OUT void*, Bytes)										__NE_OV;

		ReqAlign	DirectAccessAlign ()													C_NE_OV	{ return _align; }
	};



	//
	// Windows Write-only File Data Source
	//
	class WinFileWDataSource final : public WDataSource
	{
	// types
	public:
		using EMode = WinFileWStream::EMode;

	private:
		using Handle_t	= UntypedStorage< sizeof(void*), alignof(void*) >;

		static constexpr EMode	c_DefaultMode = EMode::SharedRead;


	// variables
	private:
		Handle_t		_file;
		const ReqAlign	_align;

		DEBUG_ONLY(
			const Path  _dbgFilename;
			const EMode	_dbgMode;
		)


	// methods
	private:
		WinFileWDataSource (const Handle_t &file, EMode mode DEBUG_ONLY(, Path filename))	__NE___;

	public:
		explicit WinFileWDataSource (const char* filename, EMode mode = c_DefaultMode)		__NE___;
		explicit WinFileWDataSource (NtStringView filename, EMode mode = c_DefaultMode)		__NE___;
		explicit WinFileWDataSource (const String &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit WinFileWDataSource (NtWStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinFileWDataSource (const wchar_t* filename, EMode mode = c_DefaultMode)	__NE___;
		explicit WinFileWDataSource (const WString &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit WinFileWDataSource (const Path &path, EMode mode = c_DefaultMode)			__NE___;

		~WinFileWDataSource ()																__NE_OV;


		// WStream //
		bool		IsOpen ()																C_NE_OV;
		ESourceType	GetSourceType ()														C_NE_OV;
		Bytes		Capacity ()																C_NE_OV;

		Bytes		Reserve (Bytes)															__NE_OV;

		Bytes		WriteBlock (Bytes, const void*, Bytes)									__NE_OV;
		void		Flush ()																__NE_OV;

		ReqAlign	DirectAccessAlign ()													C_NE_OV	{ return _align; }
	};


} // AE::Base

#endif // AE_PLATFORM_WINDOWS
