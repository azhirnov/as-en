// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "networking/Utils/NetLog.h"

namespace AE::Networking
{

/*
=================================================
	Process
=================================================
*/
	ILogger::EResult  NetworkLogger::Process (const MessageInfo &info) __Th___
	{
		const usize	max_size	= usize(NetConfig::TCP_MaxMsgSize - 32);
		usize		offset		= 0;
		StringView	short_path	= FileSystem::ToShortPath( info.file );

		// send first
		{
			Bytes	extra_size	= Min( Bytes{max_size}, StringSizeOf(short_path) + StringSizeOf(info.message) );
			auto	msg			= _msgProducer->CreateMsg< CSMsg_Log >( extra_size );
			if ( msg )
			{
				MemCopy( OUT msg.Extra<char>(), extra_size, short_path.data(), StringSizeOf(short_path) );
				extra_size -= Min( extra_size, StringSizeOf(short_path) );

				MemCopy( OUT msg.Extra<char>() + StringSizeOf(short_path), extra_size,
						 info.message.data(), StringSizeOf(info.message) );

				msg->msg		= StringView{ msg.Extra<char>() + StringSizeOf(short_path), usize(extra_size) };
				msg->loc.file	= StringView{ msg.Extra<char>(), short_path.size() };
				msg->loc.line	= info.line;
				msg->level		= info.level;
				msg->scope		= info.scope;

				if ( not _msgProducer->AddMessage( msg ))
					return EResult::Unknown;
			}
		}

		const ELevel	log_level = info.level > ELevel::SilentError ? ELevel::SilentError : info.level;

		// send other parts
		for (; offset < info.message.size();)
		{
			const usize	max = Min( offset + max_size, info.message.size() );
			usize		end = Clamp( info.message.rfind( '\n', max ), offset, max );
						end = (max - end < 5 ? max : ((float(end - offset) / float(max - offset)) < 0.7f ? max : end));

			StringView	text = info.message.substr( offset, end-offset );
			auto		msg  = _msgProducer->CreateMsg< CSMsg_Log >( StringSizeOf(text) );
			if ( msg )
			{
				msg->msg	= msg.PutExtra( text );
				msg->level	= log_level;
				msg->scope	= info.scope;

				if ( not _msgProducer->AddMessage( msg ))
					break;
			}

			offset = end;
		}
		return EResult::Unknown;
	}


} // AE::Networking
