// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "threading/TaskSystem/TaskScheduler.h"

namespace AE::Threading
{

	//
	// Lock-free Task Queue
	//
	class LfTaskQueue
	{
	// types
	private:
		static constexpr uint	TasksPerChunk		= (1u << 7) - 2;	// 2 pointers are reserved
		static constexpr uint	MaxChunks			= 2;
		static constexpr uint	MaxDepth			= 128*4;
		static constexpr uint	SpinlockWaitCount	= 8;

		using TimePoint_t	= std::chrono::high_resolution_clock::time_point;
		using TaskArr_t		= StaticArray< AsyncTask, TasksPerChunk >;
		using EStatus		= IAsyncTask::EStatus;

		union PackedBits
		{
			struct {
				usize	locked	: 1;	// 1 - spin-lock is locked
				usize	pos		: 8;	// last iterator position in 'Chunk::array'
				usize	count	: 8;	// elements count in 'Chunk::array'
			}		pack;
			usize	bits;

			PackedBits ()									__NE___ : bits{0} {}

			ND_ bool		IsLocked ()						C_NE___	{ return pack.locked == 1; }
			ND_ PackedBits	Lock ()							C_NE___	{ PackedBits tmp{*this};  tmp.pack.locked = 1;  return tmp; }
			ND_ bool		operator == (PackedBits rhs)	C_NE___	{ return bits == rhs.bits; }
		};

		struct alignas(AE_CACHE_LINE) Chunk
		{
			StructAtomic< PackedBits >	packed	{PackedBits{}};
			Atomic< Chunk *>			next	{null};
			TaskArr_t					array	{};

			Chunk () __NE___ {}
		};

		StaticAssert64( sizeof(Chunk) == 1_Kb );
		StaticAssert( TasksPerChunk * MaxChunks * MaxDepth < 1'000'000 );

		using ChunkArray_t	= StaticArray< Chunk *, MaxChunks >;


	// variables
	private:
		ChunkArray_t	_chunks		{};

		//POTValue		_seedMask;

		DEBUG_ONLY(
			ETaskQueue		_queueType;
			String			_name;
			Atomic<ulong>	_searchTime		{0};	// Nanoseconds	// task search time
			Atomic<ulong>	_workTime		{0};	// Nanoseconds
			Atomic<ulong>	_insertionTime	{0};	// Nanoseconds
			Atomic<ulong>	_maxTasks		{0};
			Atomic<slong>	_taskCount		{0};
			Atomic<ulong>	_totalProcessed	{0};
		)


	// methods
	public:
		LfTaskQueue (POTValue seedMask, StringView name, ETaskQueue type)	__Th___;
		~LfTaskQueue ()														__NE___	{ Release(); }

		ND_ AsyncTask	Pull (EThreadSeed seed)								__NE___;
			bool		Process (EThreadSeed seed)							__NE___;
			void		Add (AsyncTask task, EThreadSeed seed)				__NE___;

			void		WriteProfilerStat ()								__NE___;

		ND_ Bytes		MaxAllocationSize ()								C_NE___;
		ND_ Bytes		AllocatedSize ()									C_NE___;

			void		Release ()											__NE___;
			void		CancelAll ()										__NE___;


	  // debugging //
	  #ifdef AE_DEBUG
			void	DbgDetectDeadlock (const Function<void (AsyncTask)> &fn)__NE___;
		ND_ ulong	GetTotalProcessedTasks ()								C_NE___	{ return _totalProcessed.load(); }
	  #endif

			AE_GLOBALLY_ALLOC

	private:
		ND_ static bool  _RemoveTask (TaskArr_t& arr, INOUT usize& pos, INOUT usize& count, OUT AsyncTask& task) __NE___;
	};


} // AE::Threading
