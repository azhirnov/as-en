// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Random.h"
#include "base/DataSource/DataStream.h"
#include "base/FileSystem/Path.h"

namespace AE::Base
{

#ifdef AE_COMPILER_MSVC

	//
	// Visual Studio Log output
	//
	class VisualStudioLogOutput final : public ILogger, public NothrowAllocatable
	{
	public:
		EResult  Process (const MessageInfo &info) __Th_OV;
	};

#endif


#ifdef AE_PLATFORM_ANDROID

	//
	// Android Log output
	//
	class AndroidLogOutput final : public ILogger, public NothrowAllocatable
	{
	private:
		const String	_tag;

	public:
		explicit AndroidLogOutput (StringView tag)	__NE___	: _tag{ tag } {}

		EResult  Process (const MessageInfo &info)	__Th_OV;
	};

#else

	//
	// Console Log output
	//
	class ConsoleLogOutput final : public ILogger, public NothrowAllocatable
	{
	private:
		Mutex		_guard;

	public:
		EResult  Process (const MessageInfo &info) __Th_OV;
	};

#endif


#if defined(AE_PLATFORM_WINDOWS) or defined(AE_PLATFORM_APPLE)

	//
	// Dialog Log output
	//
	class DialogLogOutput final : public ILogger, public NothrowAllocatable
	{
	private:
		Mutex				_guard;
		const LevelBits		_levelBits;
		const ScopeBits		_scopeBits;
		const ThreadID		_mainThread;			// for Apple
		bool				_enabled	= true;		// can be disabled in runtime

	public:
		DialogLogOutput (LevelBits levelBits, ScopeBits scopeBits)			__NE___ :
			_levelBits{ levelBits }, _scopeBits{ scopeBits },
			_mainThread{ std::this_thread::get_id() }
		{}

		EResult  Process (const MessageInfo &info)							__Th_OV;

	private:
		EResult  _ProcessImpl (const String &caption, const String &msg, ELevel	level)	__Th___;
	};

#endif


#ifdef AE_PLATFORM_EMSCRIPTEN

	//
	// Dialog Log output
	//
	class DialogLogOutputEms final : public ILogger, public NothrowAllocatable
	{
	private:
		const LevelBits		_levelBits;
		const ScopeBits		_scopeBits;

	public:
		DialogLogOutputEms (LevelBits levelBits, ScopeBits scopeBits)	__NE___ :
			_levelBits{ levelBits }, _scopeBits{ scopeBits }
		{}

		EResult  Process (const MessageInfo &info)						__Th_OV;
	};

#endif


	//
	// File Log output
	//
	class FileLogOutput final : public ILogger, public NothrowAllocatable
	{
	private:
		Mutex							_guard;
		RC< WStream >					_file;
		FlatHashMap< usize, String >	_threadNames;

	public:
		explicit FileLogOutput (RC<WStream> file)		__NE___;

		EResult	Process (const MessageInfo &info)		__Th_OV;
		void	SetCurrentThreadName (StringView name)	__NE_OV;
	};



	//
	// HTML Log output
	//
	class HtmlLogOutput final : public ILogger, public NothrowAllocatable
	{
	// types
	private:
		enum class EColor : uint
		{
			Unknown		= 0,

			Blue		= 0x0000FF,
			Navy		= 0x0000AB,
			Violet		= 0x8A2BE2,

			Lime		= 0x32CD32,
			Green		= 0x688E23,
			DarkGreen	= 0x007F00,

			Orange		= 0xFF7F00,
			Red			= 0xFF0000,
			DarkRed		= 0xA00000,

			Silver		= 0x909090,

			Black		= 0x000000,
			White		= 0xFFFFFF,
		};

		struct ThreadInfo
		{
			EColor	bgColor		= Default;
			String	name;
		};
		using ThreadInfoMap_t = FlatHashMap< usize, ThreadInfo >;


	// variables
	private:
		Mutex				_guard;

		RC< WStream >		_file;
		uint				_txtColor;
		uint				_bgColor;
		Random				_random;

		ThreadInfoMap_t		_threadInfos;
		const bool			_enableThreadNames;


	// methods
	public:
		HtmlLogOutput (RC<WStream> file, bool tnames)					__NE___;
		~HtmlLogOutput ()												__NE___;

		EResult	Process (const MessageInfo &info)						__Th_OV;
		void	SetCurrentThreadName (StringView name)					__NE_OV;

	private:
		void	_SetColor (EColor col, EColor bg, INOUT String &str);
		void	_Flush (StringView str)									C_NE___;
	};



	//
	// per thread HTML Log output
	//
	class HtmlLogOutputPerThread final : public ILogger, public NothrowAllocatable
	{
	// types
	private:
		using ThreadMap_t = FlatHashMap< usize, Unique<HtmlLogOutput> >;


	// variables
	private:
		SharedMutex		_guard;
		ThreadMap_t		_perThread;
		const Path		_filenamePrefix;


	// methods
	public:
		HtmlLogOutputPerThread (StringView prefix)						__NE___;
		~HtmlLogOutputPerThread ()										__NE___;

		EResult	Process (const MessageInfo &info)						__Th_OV;
	};



	//
	// Break on error Logger
	//
	class BreakOnErrorLogger final : public ILogger
	{
	public:
		EResult  Process (const MessageInfo &info) __Th_OV;
	};


} // AE::Base
