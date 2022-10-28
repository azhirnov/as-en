// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

namespace AE::Base
{


	//
	// Logger interface
	//

	class ILogger
	{
	// types
	public:
		enum class EResult : unsigned short
		{
			Continue,
			Break,
			Abort,
			Unknown	= Continue,
		};

		enum class ELevel : unsigned short
		{
			Debug,
			Info,
			Warning,
			Error,
			Fatal,
			_Count
		};

		enum class EScope : unsigned short
		{
			Unknown,
			GraphicsDriver,
			Network,
			Engine,
			System,		// OS, ...
			Client,		// user code
			// TODO
			_Count
		};

		struct MessageInfo
		{
			std::string_view	message;
			std::string_view	func;
			std::string_view	file;
			unsigned int		line		= 0;
			size_t				threadId	= 0;
			ELevel				level		= ELevel::Debug;
			EScope				scope		= EScope::Unknown;
		};

		using LevelBits		= std::bitset< size_t(ELevel::_Count) >;
		using ScopeBits		= std::bitset< size_t(EScope::_Count) >;


	// interface
	public:
		virtual ~ILogger () {}

		ND_ virtual EResult	Process (const MessageInfo &info) = 0;
			virtual void	SetCurrentThreadName (std::string_view) {}


	// default loggers
	public:
		using LoggerPtr	= std::unique_ptr< ILogger >;
		
		ND_ static LevelBits	GetDialogLevelBits ();
		ND_ static ScopeBits	GetDialogScopeBits ();

		ND_ static LoggerPtr	CreateIDEOutput ();												// VS only
		ND_ static LoggerPtr	CreateConsoleOutput (std::string_view tag = AE_ENGINE_NAME);	// cross platfrom
		ND_ static LoggerPtr	CreateFileOutput (std::string_view fileName);
		ND_ static LoggerPtr	CreateHtmlOutput (std::string_view fileName);
		ND_ static LoggerPtr	CreateDialogOutput (LevelBits levelBits = GetDialogLevelBits(), ScopeBits scopeBits = GetDialogScopeBits());
	};



	//
	// Static Logger
	//

	struct StaticLogger
	{
		using EResult		= ILogger::EResult;
		using LevelBits		= ILogger::LevelBits;
		using ScopeBits		= ILogger::ScopeBits;
		
		ND_ static EResult  Process (const char *msg, const char *func, const char *file, unsigned int line, ILogger::ELevel level, ILogger::EScope scope);
		ND_ static EResult  Process (std::string_view msg, std::string_view func, std::string_view file, unsigned int line, ILogger::ELevel level, ILogger::EScope scope);

			static void		SetFilter (LevelBits levelBits, ScopeBits scopeBits);

			static void		ClearLoggers ();
			static void		AddLogger (std::unique_ptr<ILogger> ptr);

			static void		InitDefault ();
			static void		Initialize ();
			static void		Deinitialize (bool checkMemLeaks = false);

			static void		SetCurrentThreadName (std::string_view name);

		template <bool checkMemLeaks>
		struct _LoggerScope
		{
			_LoggerScope ()		{ InitDefault(); }
			~_LoggerScope ()	{ Deinitialize( checkMemLeaks ); }
		};
		using LoggerDbgScope	= _LoggerScope<true>;
		using LoggerScope		= _LoggerScope<false>;
	};

} // AE::Base

namespace AE
{
	using ELogLevel	= AE::Base::ILogger::ELevel;
	using ELogScope	= AE::Base::ILogger::EScope;

} // AE

#define AE_PRIVATE_LOGX( /*ELogLevel*/_level_, /*ELogScope*/ _scope_, _msg_, _file_, _line_ ) \
	BEGIN_ENUM_CHECKS() \
	{switch ( ::AE::Base::StaticLogger::Process( (_msg_), (AE_FUNCTION_NAME), (_file_), (_line_), (_level_), (_scope_) )) \
	{ \
		case_likely	::AE::Base::StaticLogger::EResult::Continue :	break; \
		case		::AE::Base::StaticLogger::EResult::Break :		AE_PRIVATE_BREAK_POINT();	break; \
		case		::AE::Base::StaticLogger::EResult::Abort :		AE_PRIVATE_EXIT();			break; \
	}} \
	END_ENUM_CHECKS()

#define AE_PRIVATE_LOGI( _msg_, _file_, _line_ )	AE_PRIVATE_LOGX( AE::ELogLevel::Info,  AE::ELogScope::Unknown, (_msg_), (_file_), (_line_) )
#define AE_PRIVATE_LOGE( _msg_, _file_, _line_ )	AE_PRIVATE_LOGX( AE::ELogLevel::Error, AE::ELogScope::Unknown, (_msg_), (_file_), (_line_) )
