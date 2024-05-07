// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "profiler/Impl/ProfilerUtils.h"

namespace AE::Profiler
{

	//
	// Task Profiler
	//

	class TaskProfiler final : public Threading::ITaskProfiler, public ProfilerUtils
	{
	// types
	private:
		using ThreadPtr		= RC<Threading::IThread>;
		using Allocator_t	= UntypedAllocator;
		enum class ThreadID : usize {};

		struct BaseCmd
		{
			DEBUG_ONLY(
				static constexpr uint	MAGIC = 0x762a3cf0;
				uint	_magicNumber	= MAGIC;
			)
			ushort		_commandID		= 0;
			ushort		_size			= 0;
		};

		struct TaskBeginCmd : BaseCmd
		{
			const void*		task;
			nanosecondsd	time;
			ThreadID		threadId;
			uint			frameIdx;
			//char			name []
		};

		struct TaskEndCmd : BaseCmd
		{
			const void*		task;
			nanosecondsd	time;
			ThreadID		threadId;
			uint			frameIdx;
			//char			name []
		};

		using TaskCmdList_t = TypeList< TaskBeginCmd, TaskEndCmd >;


		struct LfTaskCommands
		{
		// types
		public:
			template <typename CmdType>
			struct Result
			{
				friend struct LfTaskCommands;
			private:
				CmdType *			_cmd;
				uint				_endPos;
				LfTaskCommands &	_self;

				Result (CmdType* cmd, uint endPos, LfTaskCommands& self) : _cmd{cmd}, _endPos{endPos}, _self{self} {}

			public:
				~Result ()	{ _self._Complete( _cmd, _endPos ); }

				ND_ CmdType*	operator -> ()		{ return _cmd; }
				ND_ void*		Begin ()			{ return _cmd; }
				ND_ void*		DynamicData ()		{ return _cmd + SizeOf<CmdType>; }

				ND_ explicit operator bool () const	{ return _cmd != null; }
			};


		// variables
		private:
			void*			_data		= null;
			Bytes32u		_dataSize;

			Atomic<uint>	_writePos	{0};
			Atomic<uint>	_availPos	{0};
			Atomic<uint>	_readPos	{0};


		// methods
		public:
			LfTaskCommands ();
			~LfTaskCommands ();

			template <typename CmdType>
			ND_ Result<CmdType>	Insert ()	{ return Insert<CmdType, void>( 0 ); }

			template <typename CmdType, typename ArrayType>
			ND_ Result<CmdType>	Insert (usize count);

			ND_ BaseCmd const*	Extract ();

				void			Reset ();
				void			_Complete (const void* ptr, uint pos);
		};

		static constexpr uint	_FirstFrameIdx	= 2;
		static constexpr uint	_LastFrameIdx	= _FirstFrameIdx + 2;

		static constexpr Bytes	_MaxAlign		{TaskCmdList_t::ForEach_Max< TypeListUtils::GetTypeAlign >()};
		static constexpr Bytes	_MaxSize		{TaskCmdList_t::ForEach_Max< TypeListUtils::GetTypeSize >()};
		static constexpr uint	_MaxTasks		= 1u << 20;
		static constexpr Bytes	_AllocSize		= _MaxSize * _MaxTasks;


	private:
		struct TaskInfo
		{
			StringView		name;
			nanosecondsd	begin;
			ThreadID		threadId;
			uint			frameIdx;
		};

		using TaskMap_t		= FlatHashMap< const void*, TaskInfo >;
		using Caption_t		= FixedString<64>;

		struct ThreadInfo
		{
			Caption_t		caption;
			ThreadPtr		thread;
		};
		using ThreadInfos_t	= HashMap< ThreadID, ThreadInfo >;	// keep ref to 'caption'


	// variables
	private:
		Atomic<uint>	_frameIdx {0};

		LfTaskCommands	_taskCmds;

		SharedMutex		_threadInfosGuard;
		ThreadInfos_t	_threadInfos;

		TaskMap_t		_tmpTaskMap;

	  #ifdef AE_ENABLE_IMGUI
		ImTaskRangeHorDiagram	_imTaskGraph;
	  #endif


	// methods
	public:
		explicit TaskProfiler (TimePoint_t startTime)			__NE___;

		void  DrawImGUI ();
		void  Draw (Canvas &) {}
		void  Update (secondsf dt);


	  // ITaskProfiler //
		void  Begin (const Threading::IAsyncTask &)				__NE_OV;
		void  End (const Threading::IAsyncTask &)				__NE_OV;
		void  Enqueue (const Threading::IAsyncTask &)			__NE_OV;
		void  AddThread (ThreadPtr)								__NE_OV;

		void  BeginNonTaskWork (const void* id, StringView name)__NE_OV;
		void  EndNonTaskWork (const void* id, StringView name)	__NE_OV;

	private:
		void  _UpdateThreadFreq ();
	};


} // AE::Profiler
