// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "profiler/Utils/PowerVRProfiler.h"

#ifdef AE_ENABLE_REMOTE_GRAPHICS

namespace AE::Profiler
{
	using namespace AE::RemoteGraphics;

/*
=================================================
	Impl
=================================================
*/
	struct PowerVRProfiler::Impl
	{
		ECounterSet		supported;
		ECounterSet		enabled;

		RDevice const&	dev;

		Impl (RDevice const& dev) __NE___ : dev{dev} {}
	};

	PowerVRProfiler::PowerVRProfiler ()										__NE___	{}
	PowerVRProfiler::~PowerVRProfiler ()									__NE___	{}

	bool  PowerVRProfiler::IsInitialized ()									C_NE___	{ return bool{_impl}; }
	void  PowerVRProfiler::Deinitialize ()									__NE___	{ _impl.reset( null ); }

	PowerVRProfiler::ECounterSet  PowerVRProfiler::SupportedCounterSet ()	C_NE___	{ return _impl ? _impl->supported : Default; }
	PowerVRProfiler::ECounterSet  PowerVRProfiler::EnabledCounterSet ()		C_NE___	{ return _impl ? _impl->enabled : Default; }

/*
=================================================
	Initialize
=================================================
*/
	bool  PowerVRProfiler::Initialize (const ECounterSet &cs) __NE___
	{
		CHECK_ERR( not IsInitialized() );

		Msg::ProfPVR_Initialize					msg;
		RC<Msg::ProfPVR_Initialize_Response>	res;

		msg.required = cs;

		auto&	dev = GraphicsScheduler().GetDevice();
		CHECK_ERR( dev.SendAndWait( msg, OUT res ));

		if ( res->ok )
		{
			_impl = MakeUnique<Impl>( dev );
			_impl->supported	= res->supported;
			_impl->enabled		= res->enabled;
		}
		return res->ok;
	}

/*
=================================================
	Tick
=================================================
*/
	void  PowerVRProfiler::Tick () C_NE___
	{
		if ( not IsInitialized() ) return;

		Msg::ProfPVR_Tick			msg;
		RC<Msg::DefaultResponse>	res;

		Unused( _impl->dev.SendAndWait( msg, OUT res ));
	}

/*
=================================================
	Sample
=================================================
*/
	void  PowerVRProfiler::Sample (OUT Counters_t &result) C_NE___
	{
		result.clear();

		if ( not IsInitialized() ) return;

		Msg::ProfPVR_Sample					msg;
		RC<Msg::ProfPVR_Sample_Response>	res;

		CHECK_ERRV( _impl->dev.SendAndWait( msg, OUT res ));

		result = RVRef(res->counters);
	}

} // AE::Profiler
//-----------------------------------------------------------------------------

#elif defined(AE_ENABLE_PVRCOUNTER)

#include "PVRScopeStats.h"

namespace AE::Profiler
{
namespace
{
/*
=================================================
	BXMCounterToName
=================================================
*/
	ND_ static const char*  BXMCounterToName (const PowerVRProfiler::ECounter counter) __NE___
	{
		using ECounter = PowerVRProfiler::ECounter;

		static const char*	names [] = {
			"Geometry  active",
			"Geometry  time per frame",
			"Geometry  time",
			"GPU clock speed",
			"GPU memory interface load",
			"GPU memory read bytes per second",
			"GPU memory total bytes per second",
			"GPU memory write bytes per second",
			"Renderer active",
			"Renderer time per frame",
			"Renderer time",
			"SPM active",
			"TDM active",
			"TDM time per frame",
			"TDM time",
			"Tiler/Triangle ratio",
			"Tiler/Triangles input per frame",
			"Tiler/Triangles input per second",
			"Tiler/Triangles output per frame",
			"Tiler/Triangles output per second",
			"Tiler/Vertices per triangle",
			"Renderer/HSR efficiency",
			"Renderer/ISP pixel load",
			"Renderer/ISP tiles in flight",
			"Shader/Compute kernels per frame",
			"Shader/Compute kernels per second",
			"Shader/Cycles per compute kernel",
			"Shader/Cycles per pixel",
			"Shader/Cycles per vertex",
			"Shader/Pipelines starved",
			"Shader/Primary ALU Pipeline starved",
			"Shader/Processing load: compute",
			"Shader/Processing load: pixel",
			"Shader/Processing load: vertex",
			"Shader/Register overload: pixel",
			"Shader/Register overload: vertex",
			"Shader/Shaded pixels per frame",
			"Shader/Shaded pixels per second",
			"Shader/Shaded vertices per frame",
			"Shader/Shaded vertices per second",
			"Shader/Shader processing load",
			"Texturing/Texture fetches per pixel",
			"Texturing/Texture filter cycles per fetch",
			"Texturing/Texture filter input load",
			"Texturing/Texture filter load",
			"Texturing/Texture read cycles per fetch",
			"Texturing/Texture read stall"
		};
		StaticAssert( CountOf(names) == (uint(ECounter::_BXM_End) - uint(ECounter::_BXM_Begin)) );

		if ( counter >= ECounter::_BXM_Begin and counter < ECounter::_BXM_End )
			return names[ uint(counter) - uint(ECounter::_BXM_Begin) ];

		return null;
	}

} // namespace


/*
=================================================
	Impl
=================================================
*/
	struct PowerVRProfiler::Impl
	{
	// types
		using Index_t		= ushort;
		using TypeToData_t	= StaticArray< Index_t, uint(ECounter::_Count) >;
		using BXMCounters_t	= FlatHashMap< StringView, ECounter >;
		using LastTimeArr_t	= StaticArray< double, uint(ePVRScopeEventSHGEnd)/2+1 >;


	// variables
		SPVRScopeCounterReading		_reading;
		uint						_numCounter				= 0;
		SPVRScopeImplData*			_scopeData				= null;
		SPVRScopeCounterDef*		_counters				= null;

		bool						_isActiveGroupChanged	= true;
		uint						_activeGroupSelect		= 0;
		uint						_activeGroup			= UMax;

		TypeToData_t				_typeToData;
		ECounterSet					_supportedCounters;
		const ECounterSet			_enabledCounters;

		LastTimeArr_t				_lastTime				= {};

		BXMCounters_t				_bxmCountersMap;


	// methods
		Impl (const ECounterSet &enabled)	__NE___;
		~Impl ()							__NE___;

		void  Print ();

		void  Tick ()						__NE___;
		void  Read ()						__NE___;
		bool  SetActiveGroup (uint group)	__NE___;
		void  UpdateCounters ()				__NE___;
		void  ReadTimingData ()				__NE___;
	};


/*
=================================================
	Impl::ctor
=================================================
*/
	PowerVRProfiler::Impl::Impl (const ECounterSet &enabled) __NE___ :
		_enabledCounters{ enabled }
	{
		_reading.pfValueBuf				= null;
		_reading.nValueCnt				= 0;
		_reading.nReadingActiveGroup	= 99;

		_bxmCountersMap.reserve( uint(ECounter::_BXM_End) - uint(ECounter::_BXM_Begin) );
		for (uint i = uint(ECounter::_BXM_Begin); i < uint(ECounter::_BXM_End); ++i) {
			_bxmCountersMap.emplace( BXMCounterToName( ECounter(i) ), ECounter(i) );
		}
	}

/*
=================================================
	Impl::dtor
=================================================
*/
	PowerVRProfiler::Impl::~Impl () __NE___
	{
		if ( _scopeData != null )
			::PVRScopeDeInitialise( &_scopeData, &_counters, &_reading );
	}

/*
=================================================
	Impl::Tick
=================================================
*/
	void  PowerVRProfiler::Impl::Tick () __NE___
	{
		::PVRScopeReadCounters( _scopeData, null );
	}

/*
=================================================
	Impl::Read
=================================================
*/
	void  PowerVRProfiler::Impl::Read () __NE___
	{
		if_likely( _isActiveGroupChanged )
		{
			::PVRScopeSetGroup( _scopeData, _activeGroupSelect );
			_isActiveGroupChanged = false;
		}

		if ( ::PVRScopeReadCounters( _scopeData, INOUT &_reading ))
		{
		}
	}

/*
=================================================
	Impl::UpdateCounters
=================================================
*/
	void  PowerVRProfiler::Impl::UpdateCounters () __NE___
	{
		const uint	prev_count	= _numCounter;
		bool		changed		= ::PVRScopeGetCounters( _scopeData, OUT &_numCounter, OUT &_counters, INOUT &_reading );

		changed = changed and (prev_count != _numCounter or _activeGroup != _reading.nReadingActiveGroup);
		if ( not changed )
			return;

		_activeGroup = _reading.nReadingActiveGroup;
		_supportedCounters.clear();
		std::memset( OUT _typeToData.data(), 0xFF, sizeof(_typeToData) );

		for (uint i = 0; i < _numCounter; ++i)
		{
			auto&	c = _counters[i];

			if ( c.nGroup == _activeGroup )
			{
				StringView	name {c.pszName};
				usize		pos = name.find( '/' );

				if ( pos != StringView::npos )
					name = name.substr( pos+1 );

				auto	it = _bxmCountersMap.find( name );
				if ( it != _bxmCountersMap.end() and _enabledCounters.contains( it->second ))
				{
					_typeToData[ uint(it->second) ] = Index_t(i);
					_supportedCounters.insert( it->second );
				}
			}
		}
	}

/*
=================================================
	Impl::Print
=================================================
*/
	void  PowerVRProfiler::Impl::Print ()
	{
		if ( _counters == null or _numCounter == 0 )
			return;

		String	str = "PowerVR";
		str << "\n  active group: " << ToString(_activeGroup)
			<< "\n  counters (" << ToString(_numCounter) << "):";

		for (uint i = 0; i < _numCounter; ++i)
		{
			auto&	c = _counters[i];
			str << "\n  " << c.pszName << " (group: " << ToString(c.nGroup) << (c.nBoolPercentage ? ", %" : "") << ")";
		}

		AE_LOGI( str );
	}

/*
=================================================
	Impl::SetActiveGroup
=================================================
*/
	bool  PowerVRProfiler::Impl::SetActiveGroup (const uint group) __NE___
	{
		if ( _activeGroupSelect == group )
			return true;

		for (uint i = 0; i < _numCounter; ++i)
		{
			if ( _counters[i].nGroup != UMax and _counters[i].nGroup >= group )
			{
				_activeGroupSelect		= group;
				_isActiveGroupChanged	= true;
				return true;
			}
		}
		return false;
	}

/*
=================================================
	Impl::ReadTimingData
=================================================
*/
	void  PowerVRProfiler::Impl::ReadTimingData () __NE___
	{
		uint		count	= 0;
		const auto*	packets = ::PVRScopeReadTimingData( _scopeData, OUT &count );

		if ( count > 0 and packets != null )
		{
			String	str;
			for (uint i = 0; i < count; ++i)
			{
				auto&	packet	= packets[i];
				bool	is_end	= false;

				switch_enum( packet.eEventType )
				{
					case ePVRScopeEventComputeBegin:
					case ePVRScopeEventTABegin :
					case ePVRScopeEvent3DBegin :
					case ePVRScopeEvent2DBegin :
					case ePVRScopeEventRTUBegin :	// RTU time refers to any time taken to carry out frame processing, ray processing,
													// fixed function ray traversal tests and queries, and frame buffer accumulation.
					case ePVRScopeEventSHGBegin :	// The SHG unit takes the output of the SHF and is responsible for generating a scene
													// hierarchy acceleration structure for the provided components which can later be used by the RTU.
						_lastTime[ uint(packet.eEventType)/2 ] = packet.dTime;
						break;

					case ePVRScopeEventComputeEnd :		str << "\nCompute";	is_end = true;	break;
					case ePVRScopeEventTAEnd :			str << "\nTA";		is_end = true;	break;
					case ePVRScopeEvent3DEnd :			str << "\n3D";		is_end = true;	break;
					case ePVRScopeEvent2DEnd :			str << "\n2D";		is_end = true;	break;
					case ePVRScopeEventRTUEnd :			str << "\nRTU";		is_end = true;	break;
					case ePVRScopeEventSHGEnd :			str << "\nSHG";		is_end = true;	break;
				}
				switch_end;

				if ( is_end )
					str << ": " << ToString( secondsd{packet.dTime - _lastTime[uint(packet.eEventType)/2]}, 2 );
			}

			if ( not str.empty() )
				AE_LOGI( str );
		}
	}
//-----------------------------------------------------------------------------


/*
=================================================
	constructor / destructor
=================================================
*/
	PowerVRProfiler::PowerVRProfiler () __NE___
	{}

	PowerVRProfiler::~PowerVRProfiler () __NE___
	{}

/*
=================================================
	Initialize
=================================================
*/
	bool  PowerVRProfiler::Initialize (const ECounterSet &counterSet) __NE___
	{
		CHECK_ERR( not IsInitialized() );

		auto	impl = MakeUnique<Impl>( counterSet );

		EPVRScopeInitCode	err = ::PVRScopeInitialise( OUT &impl->_scopeData );
		if ( err != ePVRScopeInitCodeOk )
            return false;

		impl->SetActiveGroup( 0 );
		impl->UpdateCounters();
		//impl->ReadTimingData();	// initialize

		_impl = RVRef(impl);
		return true;
	}

/*
=================================================
	Deinitialize
=================================================
*/
	void  PowerVRProfiler::Deinitialize () __NE___
	{
		_impl.reset( null );
	}

/*
=================================================
	IsInitialized
=================================================
*/
	bool  PowerVRProfiler::IsInitialized () C_NE___
	{
		return _impl != null;
	}

/*
=================================================
	Tick
----
	Such an application should call this function at least once per frame in order to gather new counter values.
=================================================
*/
	void  PowerVRProfiler::Tick () C_NE___
	{
		if ( not _impl )
			return;  // not initialized

		_impl->Tick();
	}

/*
=================================================
	Sample
=================================================
*/
	void  PowerVRProfiler::Sample (OUT Counters_t &outCounters) C_NE___
	{
		outCounters.clear();

		if ( not _impl )
			return;  // not initialized

		_impl->UpdateCounters();
		_impl->Read();
        _impl->ReadTimingData();

		const auto*		idx_map		= _impl->_typeToData.data();
		const auto*		values		= _impl->_reading.pfValueBuf;
		const uint		count		= _impl->_reading.nValueCnt;	Unused( count );

		for (ECounter c : _impl->_supportedCounters)
		{
			ASSERT( idx_map[uint(c)] != UMax );
			ASSERT( idx_map[uint(c)] < count );

			outCounters.emplace( c, values[idx_map[uint(c)]] );
		}
	}

/*
=================================================
	SupportedCounterSet / EnabledCounterSet
=================================================
*/
	PowerVRProfiler::ECounterSet  PowerVRProfiler::SupportedCounterSet () C_NE___
	{
		CHECK_ERR( IsInitialized() );
		return _impl->_supportedCounters;
	}

	PowerVRProfiler::ECounterSet  PowerVRProfiler::EnabledCounterSet () C_NE___
	{
		CHECK_ERR( IsInitialized() );
		return _impl->_enabledCounters;
	}

} // AE::Profiler
//-----------------------------------------------------------------------------

#else // not AE_ENABLE_PVRCOUNTER and not AE_ENABLE_REMOTE_GRAPHICS

# include "profiler/Remote/RemotePowerVRProfiler.h"

namespace AE::Profiler
{
	struct PowerVRProfiler::Impl
	{
		RC<PowerVRProfilerClient>	client;

		Impl (RC<PowerVRProfilerClient> c) __NE___ : client{RVRef(c)} {}
	};

	PowerVRProfiler::PowerVRProfiler ()										__NE___	{}
	PowerVRProfiler::~PowerVRProfiler ()									__NE___	{}

	bool  PowerVRProfiler::Initialize (const ECounterSet &cs)				__NE___	{ return _impl and _impl->client->Initialize( cs ); }
	bool  PowerVRProfiler::IsInitialized ()									C_NE___	{ return _impl and _impl->client->IsInitialized(); }

	PowerVRProfiler::ECounterSet  PowerVRProfiler::SupportedCounterSet ()	C_NE___	{ return _impl ? _impl->client->SupportedCounterSet() : Default; }
	PowerVRProfiler::ECounterSet  PowerVRProfiler::EnabledCounterSet ()		C_NE___	{ return _impl ? _impl->client->EnabledCounterSet() : Default; }

	void  PowerVRProfiler::Tick ()											C_NE___	{ if (_impl) return _impl->client->Tick(); }
	void  PowerVRProfiler::Sample (OUT Counters_t &result)					C_NE___	{ if (_impl) return _impl->client->Sample( OUT result ); }


	bool  PowerVRProfiler::InitClient (RC<PowerVRProfilerClient> client)	__NE___
	{
		CHECK_ERR( client );

		_impl = MakeUnique<Impl>( RVRef(client) );
		return true;
	}

	void  PowerVRProfiler::Deinitialize () __NE___
	{
		_impl.reset( null );
	}

} // AE::Profiler

#endif // AE_ENABLE_PVRCOUNTER
//-----------------------------------------------------------------------------
