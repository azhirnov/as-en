// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
#endif
#include "profiler/Impl/LogWindow.h"

namespace AE::Profiler
{
namespace
{
	static constexpr uint	c_LevelBits		= 3;
	static constexpr uint	c_LevelMask		= (1u << c_LevelBits) - 1;
	static constexpr usize	c_MaxMsgLen		= usize{4_KiB};
}

/*
=================================================
	WeakPtrObj::ReleaseRef
=================================================
*/
	void  LogWindow::WeakPtrObj::ReleaseRef ()
	{
		const auto	cnt = _refCount.Dec();
		ASSERT_GE( cnt, 1 );

		if_unlikely( cnt == 1 )
		{
			ASSERT( DeferExLock{_guard}.try_lock() );

			delete this;
		}
	}

/*
=================================================
	WeakPtrObj::LockShared
=================================================
*/
	LogWindow::LogWindowRef  LogWindow::WeakPtrObj::LockShared ()
	{
		if_likely( _guard.try_lock_shared() )
		{
			return LogWindowRef{ _ptr, _guard };
		}
		return LogWindowRef{};
	}

/*
=================================================
	constructor
=================================================
*/
	LogWindow::LogWindow (OUT WeakPtr &ref) __NE___
	{
		_strongRef	= new WeakPtrObj{ *this };
		ref			= *_strongRef;

		ZeroMem( _lines );
		_lines[0] = uint(ELevel::_Count);
	}

/*
=================================================
	destructor
=================================================
*/
	LogWindow::~LogWindow () __NE___
	{
		if ( _strongRef )
			_strongRef->ReleaseRef();
	}

/*
=================================================
	Process
=================================================
*/
	ILogger::EResult  LogWindow::Process (const MessageInfo &info) __Th___
	{
		constexpr StringView	after_line = "): "sv;

		if ( info.level >= _minLevel and _strongRef )
		{
			EXLOCK( _strongRef->Guard() );

			CHECK_Lt( _lastLine, _maxLines );

			const auto		path	= FileSystem::ToShortPath( info.file );
			const auto		line	= ToString( info.line );
			const usize		len		= Min( path.length() + line.length() + 1 + after_line.length() + info.message.length(), c_MaxMsgLen );
			const usize		begin	= _lines[_lastLine] >> c_LevelBits;
			usize			offset	= 0;

			CHECK_Lt( begin, _bufSize );
			CHECK_Lt( begin + len, _bufSize );

			_lastLine = (_lastLine + 1) % _maxLines;

			// copy file path
			MemCopy( OUT &_buffer[begin], path.data(), Bytes{Min( len, path.length() )} );
			offset = Min( len, offset + path.length() );

			// copy line number
			if ( offset+1 < len ) {
				CHECK_Lt( begin + len, _bufSize );
				_buffer[begin + offset] = '(';
				offset += 1;
				MemCopy( OUT &_buffer[begin + offset], line.data(), Bytes{Min( len - offset, line.length() )} );
				offset = Min( len, offset + line.length() );
			}

			// copy delimiter between file+line and message body
			if ( offset < len ) {
				CHECK_Lt( begin + len, _bufSize );
				MemCopy( OUT &_buffer[begin + offset], after_line.data(), Bytes{Min( len - offset, after_line.length() )} );
				offset = Min( len, offset + after_line.length() );
			}

			// copy message body
			if ( offset < len ) {
				CHECK_Lt( begin + len, _bufSize );
				MemCopy( OUT &_buffer[begin + offset], info.message.data(), Bytes{Min( len - offset, info.message.length() )} );
				offset = Min( len, offset + info.message.length() );
			}
			ASSERT( offset == len );

			_lines[_lastLine] = (uint(info.level) & c_LevelMask) | (uint(begin + len) << c_LevelBits);

			if ( info.level >= ELevel::Error )
				_hasError.store( true );
		}
		return EResult::Unknown;
	}

/*
=================================================
	DrawImGUI
=================================================
*/
#ifdef AE_ENABLE_IMGUI
	void  LogWindow::DrawImGUI ()
	{
		ASSERT( _strongRef );
		ASSERT( not DeferExLock{ _strongRef->Guard() }.try_lock() );
		ASSERT_Lt( _lastLine, _maxLines );

		const ImGuiWindowFlags	wnd_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar;

		ImGui::SetNextWindowSize( ImVec2{650,300}, ImGuiCond_Once );

		if_unlikely( _hasError.exchange(false))
			ImGui::SetNextWindowCollapsed( false );

		// open log on error
		if ( ImGui::Begin( "Log", null, wnd_flags ))
		{
			uint i = 1;
			for (; i < _maxLines; ++i)
			{
				uint	j = (_lastLine + i) % _maxLines;
				if_unlikely( _lines[j] != 0 )
					break;
			}

			for (; i < _maxLines; ++i)
			{
				uint	j			= (_lastLine + i) % _maxLines;
				uint	j1			= (_lastLine + i+1) % _maxLines;
				uint	line		= _lines[j];
				uint	next_line	= _lines[j1];

				uint	begin		= line >> c_LevelBits;
				uint	end			= next_line >> c_LevelBits;

				if ( begin > end or end > _bufSize )
					break;

				RGBA8u	col;
				switch_enum( ELevel(next_line & c_LevelMask) )
				{
					case ELevel::Debug :		col = HtmlColor::Gray;			break;
					case ELevel::Info :			col = HtmlColor::WhiteSmoke;	break;
					case ELevel::Warning :		col = HtmlColor::Yellow;		break;
					case ELevel::Error :		col = HtmlColor::Red;			break;
					case ELevel::Fatal :		col = HtmlColor::DarkRed;		break;
					case ELevel::_Count :
					default :					DBG_WARNING( "unknown log level" ); break;
				}
				switch_end

				ImGui::PushStyleColor( ImGuiCol_Text, BitCast<ImVec4>( RGBA32f{col} ));
				ImGui::TextUnformatted( &_buffer[begin], &_buffer[end] );
				ImGui::PopStyleColor();
			}

			if ( ImGui::GetScrollY() >= ImGui::GetScrollMaxY() )
				ImGui::SetScrollHereY( 1.0f );
		}
		ImGui::End();
	}
#endif

} // AE::Profiler
