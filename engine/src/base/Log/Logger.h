// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"
#include "base/Stream/Stream.h"
#include "base/Math/Random.h"

#include <mutex>

namespace AE::Base
{

#ifdef AE_COMPILER_MSVC

	//
	// Visual Studio Log output
	//

	class VisualStudioLogOutput final : public ILogger
	{
	public:
		EResult  Process (const MessageInfo &info) override;
	};

#endif


#ifdef AE_PLATFORM_ANDROID

	//
	// Android Log output
	//
	
	class AndroidLogOutput final : public ILogger
	{
	private:
		const String	_tag;

	public:
		explicit AndroidLogOutput (StringView tag) : _tag{ tag } {}

		EResult  Process (const MessageInfo &info) override;
	};

#else
	
	//
	// Console Log output
	//
	
	class ConsoleLogOutput final : public ILogger
	{
	public:
		EResult  Process (const MessageInfo &info) override;
	};

#endif

	
#if defined(AE_PLATFORM_WINDOWS) and not defined(AE_CI_BUILD)

	//
	// Dialog Log output
	//
	
	class DialogLogOutput final : public ILogger
	{
	private:
		const LevelBits		_levelBits;
		const ScopeBits		_scopeBits;

	public:
		DialogLogOutput (LevelBits levelBits, ScopeBits scopeBits) :
			_levelBits{ levelBits }, _scopeBits{ scopeBits }
		{}

		EResult  Process (const MessageInfo &info) override;
	};

#endif
	

	//
	// File Log output
	//

	class FileLogOutput final : public ILogger
	{
	private:
		std::mutex						_guard;
		RC< WStream >					_file;
		FlatHashMap< usize, String >	_threadNames;

	public:
		explicit FileLogOutput (RC<WStream> file);

		EResult	Process (const MessageInfo &info) override;
		void	SetCurrentThreadName (StringView name) override;
	};
	


	//
	// HTML Log output
	//

	class HtmlLogOutput final : public ILogger
	{
	// types
	private:
		enum class EColor : uint
		{
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
			EColor	bgColor;
			String	name;
		};
		using ThreadInfoMap_t = FlatHashMap< usize, ThreadInfo >;


	// variables
	private:
		std::mutex			_guard;

		RC< WStream >		_file;
		uint				_txtColor;
		uint				_bgColor;
		Random				_random;

		ThreadInfoMap_t		_threadInfos;


	// methods
	public:
		explicit HtmlLogOutput (RC<WStream> file);
		~HtmlLogOutput ();

		EResult	Process (const MessageInfo &info) override;
		void	SetCurrentThreadName (StringView name) override;

	private:
		void  _SetColor (EColor col, EColor bg, INOUT String &str);
		void  _Flush (StringView str) const;
	};

} // AE::Base
