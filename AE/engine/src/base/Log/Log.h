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
		enum class EResult : unsigned int
		{
			Continue,
			Break,
			Abort,
			Unknown	= Continue,
		};

		enum class ELevel : unsigned char
		{
			Debug,
			Info,
			Warning,
			Error,
			Fatal,
			_Count
		};

		enum class EScope : unsigned char
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
		ILogger ()													__NE___	{}
		virtual ~ILogger ()											__NE___	{}

		ND_ virtual EResult	Process (const MessageInfo &info)		__Th___ = 0;
			virtual void	SetCurrentThreadName (std::string_view)	__NE___ {}


	// default loggers
	public:
		using LoggerPtr	= std::unique_ptr< ILogger >;

		ND_ static LevelBits	GetDialogLevelBits ()				__NE___;
		ND_ static ScopeBits	GetDialogScopeBits ()				__NE___;

		ND_ static LoggerPtr	CreateIDEOutput ()												__NE___;	// VS only
		ND_ static LoggerPtr	CreateConsoleOutput (std::string_view tag = {})					__NE___;	// cross platform
		ND_ static LoggerPtr	CreateFileOutput (std::string_view fileName)					__NE___;
		ND_ static LoggerPtr	CreateHtmlOutput (std::string_view fileName)					__NE___;
		ND_ static LoggerPtr	CreateHtmlOutputPerThread (std::string_view prefix)				__NE___;
		ND_ static LoggerPtr	CreateDialogOutput (LevelBits levelBits = GetDialogLevelBits(),
													ScopeBits scopeBits = GetDialogScopeBits())	__NE___;
		ND_ static LoggerPtr	CreateBreakOnError ()											__NE___;
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
		ND_ static EResult  Process (std::string_view msg, std::string_view func, std::string_view file, unsigned int line, ILogger::ELevel level, ILogger::EScope scope)	__Th___;

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
			_LoggerScope (int)	__NE___ { Initialize(); }
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

#ifdef AE_ENABLE_LOGS
# define AE_PRIVATE_LOGX( /*ELogLevel*/_level_, /*ELogScope*/ _scope_, _msg_, _file_, _line_ )							\
	if ( not IsConstEvaluated() ) {																						\
		TRY{																											\
			{switch_enum( AE::Base::StaticLogger::Process(	std::string_view{_msg_}, (AE_FUNCTION_NAME),				\
															std::string_view{_file_}, (_line_), (_level_), (_scope_) ))	\
			{																											\
				case_likely	AE::Base::StaticLogger::EResult::Continue :		break;										\
				case		AE::Base::StaticLogger::EResult::Break :		AE_PRIVATE_BREAK_POINT();	break;			\
				case		AE::Base::StaticLogger::EResult::Abort :		AE_PRIVATE_EXIT();			break;			\
			}}																											\
			switch_end																									\
		}CATCH_ALL();	/* to catch exceptions in string formatting */													\
	}

#else
# define AE_PRIVATE_LOGX( ... )	{}
#endif


#define AE_PRIVATE_LOG_I(  _msg_, _file_, _line_ )	AE_PRIVATE_LOGX( AE::ELogLevel::Info,	 AE::ELogScope::Unknown, (_msg_), (_file_), (_line_) )
#define AE_PRIVATE_LOG_E(  _msg_, _file_, _line_ )	AE_PRIVATE_LOGX( AE::ELogLevel::Error,	 AE::ELogScope::Unknown, (_msg_), (_file_), (_line_) )
#define AE_PRIVATE_LOG_W( _msg_, _file_, _line_ )	AE_PRIVATE_LOGX( AE::ELogLevel::Warning, AE::ELogScope::Unknown, (_msg_), (_file_), (_line_) )
