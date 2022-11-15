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
			SilentError,
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

		ND_ virtual EResult	Process (const MessageInfo &info)		__TH___ = 0;
			virtual void	SetCurrentThreadName (std::string_view)	__NE___ {}


	// default loggers
	public:
		using LoggerPtr	= std::unique_ptr< ILogger >;
		
		ND_ static LevelBits	GetDialogLevelBits () __NE___;
		ND_ static ScopeBits	GetDialogScopeBits () __NE___;

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
	// types
		using EResult		= ILogger::EResult;
		using LevelBits		= ILogger::LevelBits;
		using ScopeBits		= ILogger::ScopeBits;


	// methods
		ND_ static EResult  Process (const char *msg, const char *func, const char *file, unsigned int line, ILogger::ELevel level, ILogger::EScope scope)					__TH___;
		ND_ static EResult  Process (std::string_view msg, std::string_view func, std::string_view file, unsigned int line, ILogger::ELevel level, ILogger::EScope scope)	__TH___;

			static void		SetFilter (LevelBits levelBits, ScopeBits scopeBits)__NE___;

			static void		ClearLoggers ()										__NE___;
			static void		AddLogger (std::unique_ptr<ILogger> ptr)			__NE___;

			static void		InitDefault ()										__NE___;
			static void		Initialize ()										__NE___;
			static void		Deinitialize (bool checkMemLeaks = false)			__NE___;

			static void		SetCurrentThreadName (std::string_view name)		__NE___;

		template <bool checkMemLeaks>
		struct _LoggerScope
		{
			_LoggerScope ()		__NE___ { InitDefault(); }
			~_LoggerScope ()	__NE___ { Deinitialize( checkMemLeaks ); }
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
	try { \
		BEGIN_ENUM_CHECKS() \
		{switch ( AE::Base::StaticLogger::Process( (_msg_), (AE_FUNCTION_NAME), (_file_), (_line_), (_level_), (_scope_) )) \
		{ \
			case_likely	AE::Base::StaticLogger::EResult::Continue :		break; \
			case		AE::Base::StaticLogger::EResult::Break :		AE_PRIVATE_BREAK_POINT();	break; \
			case		AE::Base::StaticLogger::EResult::Abort :		AE_PRIVATE_EXIT();			break; \
		}} \
		END_ENUM_CHECKS() \
	} catch(...) {} // to catch exceptions in string formating


#define AE_PRIVATE_LOG_I(  _msg_, _file_, _line_ )	AE_PRIVATE_LOGX( AE::ELogLevel::Info,		 AE::ELogScope::Unknown, (_msg_), (_file_), (_line_) )
#define AE_PRIVATE_LOG_E(  _msg_, _file_, _line_ )	AE_PRIVATE_LOGX( AE::ELogLevel::Error,		 AE::ELogScope::Unknown, (_msg_), (_file_), (_line_) )
#define AE_PRIVATE_LOG_SE( _msg_, _file_, _line_ )	AE_PRIVATE_LOGX( AE::ELogLevel::SilentError, AE::ELogScope::Unknown, (_msg_), (_file_), (_line_) )
