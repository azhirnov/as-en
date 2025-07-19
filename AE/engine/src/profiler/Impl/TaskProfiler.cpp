// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#ifdef AE_ENABLE_IMGUI
# include "imgui.h"
#endif
#include "base/Algorithms/StringUtils.h"
#include "profiler/Impl/TaskProfiler.h"

namespace AE::Profiler
{
	using namespace AE::Threading;


/*
=================================================
	constructor
=================================================
*/
	TaskProfiler::LfTaskCommands::LfTaskCommands () :
		_data{ Allocator_t::Allocate( SizeAndAlign{ _AllocSize, _MaxAlign })},
		_dataSize{ _AllocSize }
	{}

	TaskProfiler::LfTaskCommands::~LfTaskCommands ()
	{
		Allocator_t::Deallocate( _data, SizeAndAlign{ _AllocSize, _MaxAlign });
	}

/*
=================================================
	Insert
=================================================
*/
	template <typename CmdType, typename ArrayType>
	TaskProfiler::LfTaskCommands::Result<CmdType>
		TaskProfiler::LfTaskCommands::Insert (usize count)
	{
		StaticAssert( TaskCmdList_t::HasType<CmdType> );

		Bytes	size = SizeOf<CmdType>;

		if constexpr( not IsVoid<ArrayType> )
		{
			ASSERT( count > 0 );
			size = AlignUp( size, AlignOf<ArrayType> ) + SizeOf<ArrayType> * count;
		}

		size = AlignUp( size, _MaxAlign );

		const uint	pos		= _writePos.fetch_add( uint(size) );
		auto*		result	= Cast<CmdType>( _data + Bytes{pos} );

		if_unlikely( pos + uint(size) > _dataSize )
			return { null, UMax, *this };

		DEBUG_ONLY( result->_magicNumber = BaseCmd::MAGIC; )
		result->_size		= CheckCast<ushort>( size );
		result->_commandID	= TaskCmdList_t::Index<CmdType>;

		return { result, pos + uint(size), *this };
	}

/*
=================================================
	_Complete
=================================================
*/
	void  TaskProfiler::LfTaskCommands::_Complete (const void* ptr, uint endPos)
	{
		if ( ptr == null or endPos == UMax )
			return;	// not allocated

		const uint	start_pos = uint(Bytes{ptr} - Bytes{_data});
		ASSERT( start_pos < endPos );
		ASSERT( endPos <= _dataSize );

		for (;;)
		{
			uint	exp = start_pos;

			if_likely( _availPos.CAS( INOUT exp, endPos ))
				break;

			ASSERT( exp <= start_pos );
			ThreadUtils::Pause();
		}
	}

/*
=================================================
	Extract
=================================================
*/
	TaskProfiler::BaseCmd const*  TaskProfiler::LfTaskCommands::Extract ()
	{
		uint		pos = _readPos.load();
		BaseCmd*	cmd;

		for (;;)
		{
			if_unlikely( pos >= _availPos.load() )
				return null;

			cmd	= Cast<BaseCmd>( _data + Bytes{pos} );

			ASSERT( cmd->_magicNumber == BaseCmd::MAGIC );
			ASSERT( cmd->_commandID < TaskCmdList_t::Count );
			ASSERT( cmd->_size > 0 );

			if_likely( _readPos.CAS( INOUT pos, pos + cmd->_size ))
				break;

			ThreadUtils::Pause();
		}
		return cmd;
	}

/*
=================================================
	Reset
=================================================
*/
	void  TaskProfiler::LfTaskCommands::Reset ()
	{
		ASSERT( _writePos.load() == _availPos.load() );
		ASSERT( _readPos.load() == _availPos.load() );

		_writePos.store( 0 );
		_availPos.store( 0 );
		_readPos.store( 0 );
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	TaskProfiler::TaskProfiler (TimePoint_t startTime) __NE___ :
		ProfilerUtils{ startTime }
	{
		NOTHROW( _tmpTaskMap.reserve( 1024 );)
	}

/*
=================================================
	Begin
=================================================
*/
	void  TaskProfiler::Begin (const IAsyncTask &task) __NE___
	{
		BeginNonTaskWork( &task, task.DbgName() );
	}

/*
=================================================
	End
=================================================
*/
	void  TaskProfiler::End (const IAsyncTask &task) __NE___
	{
		EndNonTaskWork( &task, task.DbgName() );
	}

/*
=================================================
	BeginNonTaskWork
=================================================
*/
	void  TaskProfiler::BeginNonTaskWork (const void* id, StringView name) __NE___
	{
		const uint	idx = _frameIdx.load();
		if_likely( idx < _FirstFrameIdx or idx >= _LastFrameIdx )
			return;

		auto		cmd		= _taskCmds.Insert< TaskBeginCmd, char >( name.length()+1 );
		char*		cmd_txt	= Cast<char>(cmd.DynamicData());

		if_unlikely( not cmd )
			return;	// failed to allocate

		cmd->task		= id;
		cmd->time		= CurrentTimeNano();
		cmd->threadId	= ThreadID(this->CurrentThreadID());
		cmd->frameIdx	= idx;

		MemCopy( OUT cmd_txt, name.data(), Bytes{name.length()} );
		cmd_txt[ name.length() ] = '\0';
	}

/*
=================================================
	EndNonTaskWork
=================================================
*/
	void  TaskProfiler::EndNonTaskWork (const void* id, StringView name) __NE___
	{
		const uint	idx = _frameIdx.load();
		if_likely( idx < _FirstFrameIdx or idx >= _LastFrameIdx+2 )
			return;

		auto		cmd		= _taskCmds.Insert< TaskEndCmd, char >( name.length()+1 );
		char*		cmd_txt	= Cast<char>(cmd.DynamicData());

		if_unlikely( not cmd )
			return;	// failed to allocate

		cmd->task		= id;
		cmd->time		= CurrentTimeNano();
		cmd->threadId	= ThreadID(this->CurrentThreadID());
		cmd->frameIdx	= idx;

		MemCopy( OUT cmd_txt, name.data(), Bytes{name.length()} );
		cmd_txt[ name.length() ] = '\0';
	}

/*
=================================================
	Enqueue
=================================================
*/
	void  TaskProfiler::Enqueue (const IAsyncTask &) __NE___
	{
	}

/*
=================================================
	AddThread
=================================================
*/
	void  TaskProfiler::AddThread (ThreadPtr thread) __NE___
	{
		CHECK_ERRV( thread );
		EXLOCK( _threadInfosGuard );

		const auto	tid = ThreadID(thread->DbgID());

		if_unlikely( not _threadInfos.contains( tid ))
		{
			ThreadInfo	info;
			info.caption	= thread->GetProfilingInfo().threadName;
			info.thread		= RVRef(thread);

			_threadInfos.emplace( tid, RVRef(info) );
		}
	}

/*
=================================================
	DrawImGUI
=================================================
*/
#ifdef AE_ENABLE_IMGUI
	void  TaskProfiler::DrawImGUI ()
	{
		_frameIdx.Inc();
		_UpdateThreadFreq();

	  #ifdef AE_PLATFORM_ANDROID
		ImGui::SetNextWindowPos( ImVec2{0,300}, ImGuiCond_Once );
		ImGui::SetNextWindowSize( ImVec2{600,500}, ImGuiCond_Once );

		const ImGuiWindowFlags	flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
	  #else
		const ImGuiWindowFlags	flags = ImGuiWindowFlags_NoScrollbar;
	  #endif

		if ( ImGui::Begin( "TaskProfiler", null, flags ))
		{
			const ImVec2	wnd_size	= ImGui::GetContentRegionAvail();
			const ImVec2	wnd_pos		= ImGui::GetCursorScreenPos();
			const RectF		max_region	= RectF{float2{ wnd_size.x, Abs(wnd_size.y) }} + float2{wnd_pos.x, wnd_pos.y};

			RectF	region1 = max_region;
			_imTaskGraph.Draw( INOUT region1 );
		}
		ImGui::End();
	}
#endif

/*
=================================================
	Update
=================================================
*/
	void  TaskProfiler::Update (secondsf)
	{
	  #ifdef AE_ENABLE_IMGUI
		_imTaskGraph.Begin();
	  #endif

		_tmpTaskMap.clear();
		SHAREDLOCK( _threadInfosGuard );

		for (;;)
		{
			auto*	cmd = _taskCmds.Extract();
			if_unlikely( cmd == null )
				break;

			switch ( cmd->_commandID )
			{
				case TaskCmdList_t::Index<TaskBeginCmd> :
				{
					const auto&	begin	= *Cast<TaskBeginCmd>(cmd);
					const char*	name	= Cast<char>( &begin + 1 );
					TaskInfo	info;

					info.name		= name;
					info.begin		= begin.time;
					info.threadId	= begin.threadId;
					info.frameIdx	= begin.frameIdx;

					bool	inserted = _tmpTaskMap.emplace( begin.task, info ).second;
					ASSERT( inserted );
					Unused( inserted );
					break;
				}

				case TaskCmdList_t::Index<TaskEndCmd> :
				{
					const auto&	end	= *Cast<TaskEndCmd>(cmd);
					TaskInfo	info;

					info.name = Cast<char>( &end + 1 );

					if_likely( auto it = _tmpTaskMap.find( end.task );  it != _tmpTaskMap.end() )
					{
						ASSERT( info.name == it->second.name );
						info = it->second;
						_tmpTaskMap.erase( it );

						ASSERT( info.threadId == end.threadId );
						ASSERT( info.frameIdx <= end.frameIdx );
					}
					else
					{
						// init info
						info.begin		= nanosecondsd{NaN<double>()};	// before capture
						info.threadId	= end.threadId;
						info.frameIdx	= end.frameIdx;
					}

					StringView	tname = "main";
					if ( auto t_it = _threadInfos.find( info.threadId );  t_it != _threadInfos.end() )
						tname = t_it->second.caption;

					RGBA8u	color;
					StringToColor( OUT color, info.name );

				  #ifdef AE_ENABLE_IMGUI
					_imTaskGraph.Add( info.name, color, info.begin, end.time, usize(info.threadId), tname );
				  #endif
					break;
				}

				default :
					DBG_WARNING( "unknown command" );
			}
		}

		// tasks doesn't end in capture interval
		for (auto& [task, info] : _tmpTaskMap)
		{
			StringView	tname = "main";
			if ( auto t_it = _threadInfos.find( info.threadId );  t_it != _threadInfos.end() )
				tname = t_it->second.caption;

			RGBA8u	color;
			StringToColor( OUT color, info.name );

		  #ifdef AE_ENABLE_IMGUI
			_imTaskGraph.Add( info.name, color, info.begin, nanosecondsd{NaN<double>()}, usize(info.threadId), tname );
		  #endif
		}

		_taskCmds.Reset();
		_tmpTaskMap.clear();

		const uint	old_idx = _frameIdx.exchange( 0 );
		Unused( old_idx );
		//ASSERT( old_idx > _LastFrameIdx );

	  #ifdef AE_ENABLE_IMGUI
		_imTaskGraph.End();
	  #endif
	}

/*
=================================================
	_UpdateThreadFreq
=================================================
*/
	void  TaskProfiler::_UpdateThreadFreq ()
	{
		EXLOCK( _threadInfosGuard );

		String	str;

		for (auto& [tid, t] : _threadInfos)
		{
			const auto	info = t.thread->GetProfilingInfo();

			str.clear();
			str << info.threadName << " (" << ToString( uint(info.coreId) ) << ')';

			t.caption = StringView{str};
		}
	}


} // AE::Profiler
