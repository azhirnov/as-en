// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "CPP_VM/VirtualMachine.h"

namespace LFAS::CPP
{
	using namespace AE::Base;


	//
	// Script
	//

	class VirtualMachine::Script
	{
	// variables
	private:
		ScriptFn				_fn;
		const String			_name;
		std::atomic<ulong>		_invocations {0};


	// methods
	public:
		explicit Script (String &&name, ScriptFn &&fn) : _fn{RVRef(fn)}, _name{name} {}
		~Script () {}

		void  Run ()
		{
			_fn();
			++_invocations;
		}

		ND_ ulong  GetInvocations ()
		{
			return _invocations;
		}

		ND_ StringView  GetName () const
		{
			return _name;
		}
	};



//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	VirtualMachine::AtomicGlobalLock::AtomicGlobalLock (std::mutex* lock) :
		_lock{ lock }
	{
		if ( _lock ) {
			_lock->lock();
			_locked = true;
		}
	}

	VirtualMachine::AtomicGlobalLock::AtomicGlobalLock (AtomicGlobalLock &&other) :
		_lock{other._lock}, _locked{other._locked}
	{
		other._lock		= null;
		other._locked	= false;
	}

/*
=================================================
	destructor
=================================================
*/
	VirtualMachine::AtomicGlobalLock::~AtomicGlobalLock ()
	{
		if ( _lock and _locked )
			_lock->unlock();
	}
//-----------------------------------------------------------------------------



/*
=================================================
	constructor
=================================================
*/
	VirtualMachine::VirtualMachine ()
	{
	}

/*
=================================================
	destructor
=================================================
*/
	VirtualMachine::~VirtualMachine ()
	{
		EXLOCK( _atomicGlobalGuard );

		{
			EXLOCK( _atomicMapGuard );

			for (auto& at : _atomicMap) {
				CHECK( at.second.isDestroyed );
			}
			_atomicMap.clear();
		}
		{
			EXLOCK( _storageMapGuard );

			for (auto& st : _storageMap) {
				CHECK( st.second.isDestroyed );
			}
			_storageMap.clear();
		}
	}

/*
=================================================
	ThreadFenceAcquire
=================================================
*/
	void  VirtualMachine::ThreadFenceAcquire ()
	{
		EXLOCK( _storageMapGuard );

		const auto	tid = std::this_thread::get_id();

		for (auto& storage : _storageMap)
		{
			storage.second.ranges.Acquire( tid );
		}
	}

/*
=================================================
	ThreadFenceRelease
=================================================
*/
	void  VirtualMachine::ThreadFenceRelease ()
	{
		EXLOCK( _storageMapGuard );

		const auto	tid = std::this_thread::get_id();

		for (auto& storage : _storageMap)
		{
			storage.second.ranges.Release( tid );
		}
	}

/*
=================================================
	ThreadFenceAcquireRelease
=================================================
*/
	void  VirtualMachine::ThreadFenceAcquireRelease ()
	{
		EXLOCK( _storageMapGuard );

		const auto	tid = std::this_thread::get_id();

		for (auto& storage : _storageMap)
		{
			storage.second.ranges.AcquireRelease( tid );
		}
	}

/*
=================================================
	ThreadFenceRelaxed
=================================================
*/
	void  VirtualMachine::ThreadFenceRelaxed ()
	{
		// do nothing
	}

/*
=================================================
	Yield
=================================================
*/
	void  VirtualMachine::Yield ()
	{
		uint	delay;
		{
			EXLOCK( _randomGuard );
			delay = _random.Uniform( 0, 20 );
		}

		if ( delay > 4 )
			return;

		std::this_thread::sleep_for( milliseconds{delay} );	// TODO
	}

/*
=================================================
	CheckForUncommittedChanges
----
	find uncommitted changes in current thread
=================================================
*/
	void  VirtualMachine::CheckForUncommittedChanges () const
	{
		EXLOCK( _storageMapGuard );

		const auto	tid = std::this_thread::get_id();

		for (auto& storage : _storageMap)
		{
			storage.second.ranges.CheckUnreleased( tid );
		}
	}

/*
=================================================
	AtomicCreate
=================================================
*/
	void  VirtualMachine::AtomicCreate (const void* ptr)
	{
		EXLOCK( _atomicMapGuard );

		auto[iter, inserted] = _atomicMap.insert({ ptr, AtomicInfo{} });

		if ( not inserted )
		{
			CHECK( iter->second.isDestroyed );
		}

		iter->second.isDestroyed = false;
	}

/*
=================================================
	AtomicDestroy
=================================================
*/
	void  VirtualMachine::AtomicDestroy (const void* ptr)
	{
		EXLOCK( _atomicMapGuard );

		auto	iter = _atomicMap.find( ptr );
		CHECK_ERRV( iter != _atomicMap.end() );

		iter->second.isDestroyed = true;
	}

/*
=================================================
	AtomicCompareExchangeWeakFalsePositive
=================================================
*/
	bool  VirtualMachine::AtomicCompareExchangeWeakFalsePositive (const void *)
	{
		EXLOCK( _randomGuard );
		return _random.Bernoulli( 0.125 );
	}

/*
=================================================
	GetAtomicGlobalLock
=================================================
*/
	VirtualMachine::AtomicGlobalLock  VirtualMachine::GetAtomicGlobalLock (bool isSequentiallyConsistent)
	{
		return AtomicGlobalLock{ isSequentiallyConsistent ? &_atomicGlobalGuard : null };
	}

/*
=================================================
	StorageCreate
=================================================
*/
	void  VirtualMachine::StorageCreate (const void* ptr, Bytes size)
	{
		EXLOCK( _storageMapGuard );

		auto[iter, insert] = _storageMap.insert({ ptr, StorageInfo{size} });

		if ( not insert )
		{
			CHECK( iter->second.isDestroyed );
			CHECK( iter->second.ranges.Read( 0_b, size, std::this_thread::get_id() ));
		}

		iter->second.isDestroyed = false;
		CHECK( iter->second.ranges.Init( size, std::this_thread::get_id() ));
	}

/*
=================================================
	StorageDestroy
=================================================
*/
	void  VirtualMachine::StorageDestroy (const void* ptr)
	{
		EXLOCK( _storageMapGuard );

		auto	iter = _storageMap.find( ptr );
		CHECK_ERRV( iter != _storageMap.end() );

		CHECK( not iter->second.isDestroyed );
		iter->second.isDestroyed = true;

		// you must invalidate cache before calling destructor
		CHECK( iter->second.ranges.Read( 0_b, iter->second.size, std::this_thread::get_id() ));

		// remove temporary variables (that used only in one thread)
		if ( iter->second.ranges.UniqueThreadCount() <= 1 )
		{
			_storageMap.erase( iter );
		}
		else
		{
			for (auto& r : iter->second.ranges._ranges)
			{
				CHECK( r.unavailable.size() == 0 );
			}
		}

		// TODO: write ?
	}

/*
=================================================
	StorageReadAccess
=================================================
*/
	void  VirtualMachine::StorageReadAccess (const void* ptr, Bytes offset, Bytes size)
	{
		EXLOCK( _storageMapGuard );

		auto	iter = _storageMap.find( ptr );
		CHECK_ERRV( iter != _storageMap.end() );

		ASSERT( offset + size <= iter->second.size );

		CHECK( iter->second.ranges.Read( offset, offset + size, std::this_thread::get_id() ));
	}

/*
=================================================
	StorageWriteAccess
=================================================
*/
	void  VirtualMachine::StorageWriteAccess (const void* ptr, Bytes offset, Bytes size)
	{
		EXLOCK( _storageMapGuard );

		auto	iter = _storageMap.find( ptr );
		CHECK_ERRV( iter != _storageMap.end() );

		ASSERT( offset + size <= iter->second.size );

		CHECK( iter->second.ranges.Write( offset, offset + size, std::this_thread::get_id() ));
	}

/*
=================================================
	CreateScript
=================================================
*/
	VirtualMachine::ScriptPtr  VirtualMachine::CreateScript (StringView inName, ScriptFn &&fn)
	{
		CHECK_ERR( fn );

		String	name {inName};

		if ( name.empty() )
			name = "Script_"s << ToString( ++_scriptCounter );

		return MakeShared<Script>( RVRef(name), RVRef(fn) );
	}

/*
=================================================
	RunParallel
=================================================
*/
	void  VirtualMachine::RunParallel (ArrayView<ScriptPtr> scripts, secondsf timeout)
	{
		CHECK_ERRV( not scripts.empty() );

		const usize	max_threads	= Max( 8u, std::thread::hardware_concurrency() );	// 1.5x of hw threads for more chaos

		std::atomic<bool>	break_all {false};
		Array<std::thread>	threads;

		AE_LOGI( "RunParallel - start" );

		threads.reserve( max_threads );
		{
			EXLOCK( _randomGuard );
			for (usize i = 0; i < max_threads; ++i)
			{
				threads.emplace_back(
					[&scripts, &break_all, rnd = i + _random.Uniform( 1u, 9u )] ()
					{
						for (usize j = rnd; not break_all; ++j)
						{
							auto&	sc = scripts[ j % scripts.size() ];

							sc->Run();
						}
					});
			}
		}

		std::this_thread::sleep_for( timeout );		// TODO: random sleep time Nano/Micor/Milli

		break_all.store( true );
		AE_LOGI( "RunParallel - wait" );

		for (auto& t : threads) {
			t.join();
		}

		AE_LOGI( "RunParallel - end" );

		for (auto& sc : scripts)
		{
			AE_LOGI( String(sc->GetName()) << " invocations: " << ToString(sc->GetInvocations()) );
			CHECK( sc->GetInvocations() > 0 );
		}
	}

/*
=================================================
	GetVirtualMachineInstance
=================================================
*/
namespace {
	ND_ static VirtualMachine*&  GetVirtualMachineInstance ()
	{
		static VirtualMachine*	inst = null;
		return inst;
	}
}

/*
=================================================
	CreateInstance
=================================================
*/
	bool  VirtualMachine::CreateInstance ()
	{
		if ( GetVirtualMachineInstance() )
			return false;

		GetVirtualMachineInstance() = new VirtualMachine{};
		return true;
	}

/*
=================================================
	DestroyInstance
=================================================
*/
	bool  VirtualMachine::DestroyInstance ()
	{
		if ( not GetVirtualMachineInstance() )
			return false;

		delete GetVirtualMachineInstance();
		GetVirtualMachineInstance() = null;
		return true;
	}

/*
=================================================
	Instance
=================================================
*/
	VirtualMachine&  VirtualMachine::Instance ()
	{
		return *GetVirtualMachineInstance();
	}


} // LFAS::CPP
