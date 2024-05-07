// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/DataSource/DataStream.h"
#include "base/Containers/NtStringView.h"
#include "base/FileSystem/Path.h"

namespace AE::Base
{

	//
	// Read-only STL File Stream
	//

	class StdFileRStream final : public RStream
	{
	// variables
	private:
		FILE*			_file		= null;
		Bytes const		_fileSize;
		Bytes			_position;

		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		explicit StdFileRStream (FILE* file DEBUG_ONLY(, Path filename)) __NE___;

	public:
		explicit StdFileRStream (const char* filename)		__NE___;
		explicit StdFileRStream (NtStringView filename)		__NE___;
		explicit StdFileRStream (const String &filename)	__NE___;
		explicit StdFileRStream (const Path &path)			__NE___;

	  #ifdef AE_PLATFORM_WINDOWS
		explicit StdFileRStream (NtWStringView filename)	__NE___;
		explicit StdFileRStream (const wchar_t* filename)	__NE___;
		explicit StdFileRStream (const WString &filename)	__NE___;
	  #endif

		~StdFileRStream ()									__NE_OV;

		// control file buffering
		bool  SetBufferSize (Bytes size)					__NE___;
		bool  SetBuffer (char* buf, Bytes size)				__NE___;
		bool  DisableBuffering ()							__NE___;


		// RStream //
		bool		IsOpen ()								C_NE_OV	{ return _file != null; }
		PosAndSize	PositionAndSize ()						C_NE_OV	{ return { _position, _fileSize }; }
		ESourceType	GetSourceType ()						C_NE_OV;

		bool		SeekSet (Bytes pos)						__NE_OV;	// require ESourceType::RandomAccess
		bool		SeekFwd (Bytes offset)					__NE_OV;

		Bytes		ReadSeq (OUT void*, Bytes)				__NE_OV;
	};



	//
	// Write-only STL File Stream
	//

	class StdFileWStream final : public WStream
	{
	// types
	public:
		enum class EMode
		{
			OpenRewrite,	// create new or discard previous file
			OpenUpdate,		// keep previous content and update some parts in the file
			OpenAppend,		// write data to the end of the file
		};
	private:
		static constexpr EMode	c_DefaultMode = EMode::OpenRewrite;


	// variables
	private:
		FILE*		_file	= null;

		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		explicit StdFileWStream (FILE* file DEBUG_ONLY(, Path filename))				__NE___;

	public:
		explicit StdFileWStream (const char* filename, EMode mode = c_DefaultMode)		__NE___;
		explicit StdFileWStream (NtStringView filename, EMode mode = c_DefaultMode)		__NE___;
		explicit StdFileWStream (const String &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit StdFileWStream (const Path &path, EMode mode = c_DefaultMode)			__NE___;

	  #ifdef AE_PLATFORM_WINDOWS
		explicit StdFileWStream (NtWStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit StdFileWStream (const wchar_t* filename, EMode mode = c_DefaultMode)	__NE___;
		explicit StdFileWStream (const WString &filename, EMode mode = c_DefaultMode)	__NE___;
	  #endif

		StdFileWStream ()																__NE___;	// temp file

		~StdFileWStream ()																__NE_OV;


		// WStream //
		bool		IsOpen ()															C_NE_OV	{ return _file != null; }
		Bytes		Position ()															C_NE_OV;
		ESourceType	GetSourceType ()													C_NE_OV;

		bool		SeekFwd (Bytes offset)												__NE_OV;

		Bytes		WriteSeq (const void* buffer, Bytes size)							__NE_OV;
		void		Flush ()															__NE_OV;
	};
//-----------------------------------------------------------------------------



	//
	// Read-only STL File Data Source
	//

	class StdFileRDataSource final : public RDataSource
	{
	// variables
	private:
		FILE*			_file		= null;
		Bytes const		_fileSize;
		Bytes			_lastPos;

		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		explicit StdFileRDataSource (FILE* file DEBUG_ONLY(, Path filename))		__NE___;

	public:
		explicit StdFileRDataSource (const char* filename)							__NE___;
		explicit StdFileRDataSource (NtStringView filename)							__NE___;
		explicit StdFileRDataSource (const String &filename)						__NE___;

		explicit StdFileRDataSource (const Path &path)								__NE___;

	  #ifdef AE_PLATFORM_WINDOWS
		explicit StdFileRDataSource (NtWStringView filename)						__NE___;
		explicit StdFileRDataSource (const wchar_t* filename)						__NE___;
		explicit StdFileRDataSource (const WString &filename)						__NE___;
	  #endif

		~StdFileRDataSource ()														__NE_OV;


		// RDataSource //
		bool		IsOpen ()														C_NE_OV	{ return _file != null; }
		ESourceType	GetSourceType ()												C_NE_OV;
		Bytes		Size ()															C_NE_OV	{ return _fileSize; }

		Bytes		ReadBlock (Bytes pos, OUT void* buffer, Bytes size)				__NE_OV;
	};



	//
	// Write-only STL File Data Source
	//

	class StdFileWDataSource final : public WDataSource
	{
	// types
	public:
		using EMode = StdFileWStream::EMode;
	private:
		static constexpr EMode	c_DefaultMode = EMode::OpenRewrite;


	// variables
	private:
		FILE* 		_file		= null;
		Bytes		_lastPos;

		DEBUG_ONLY( const Path  _filename;)


	// methods
	private:
		explicit StdFileWDataSource (FILE* file DEBUG_ONLY(, Path filename))				__NE___;

	public:
		explicit StdFileWDataSource (const char* filename, EMode mode = c_DefaultMode)		__NE___;
		explicit StdFileWDataSource (NtStringView filename, EMode mode = c_DefaultMode)		__NE___;
		explicit StdFileWDataSource (const String &filename, EMode mode = c_DefaultMode)	__NE___;

		explicit StdFileWDataSource (const Path &path, EMode mode = c_DefaultMode)			__NE___;

	  #ifdef AE_PLATFORM_WINDOWS
		explicit StdFileWDataSource (NtWStringView filename, EMode mode = c_DefaultMode)	__NE___;
		explicit StdFileWDataSource (const wchar_t* filename, EMode mode = c_DefaultMode)	__NE___;
		explicit StdFileWDataSource (const WString &filename, EMode mode = c_DefaultMode)	__NE___;
	  #endif

		StdFileWDataSource ()																__NE___;	// temp file

		~StdFileWDataSource ()																__NE_OV;


		// WStream //
		bool		IsOpen ()																C_NE_OV	{ return _file != null; }
		ESourceType	GetSourceType ()														C_NE_OV;
		Bytes		Capacity ()																C_NE_OV;

		Bytes		WriteBlock (Bytes pos, const void* buffer, Bytes size)					__NE_OV;
		void		Flush ()																__NE_OV;
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
